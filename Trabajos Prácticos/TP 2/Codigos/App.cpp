#include "App.h"
#include "File.h"
#include "Types.h"

#include <iostream>
#include <fstream>
#include <thread>
#include <chrono>

#ifdef __linux__
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include <sys/select.h>
#include <sys/time.h>

using std::string;
using std::vector;

char App::fmtByte(InputFormat f) {
    return f==InputFormat::CSV ? 'c' : (f==InputFormat::JSON ? 'j' : 'x');
}

// Mapea entero a constante termios
#ifdef __linux__
static speed_t map_baud(int b){
    switch(b){
        case 9600: return B9600; case 19200: return B19200; case 38400: return B38400;
        case 57600: return B57600; case 115200: return B115200;
        default: return B9600;
    }
}

static void set_serial_raw_8N1(const std::string& dev, int baud){
    int fd = ::open(dev.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) throw AppError("No se pudo abrir TTY para configurar.");
    termios tio{};
    if (tcgetattr(fd, &tio) != 0){ ::close(fd); throw AppError("tcgetattr falló"); }
    cfmakeraw(&tio);
    tio.c_cflag |= CLOCAL | CREAD;          // habilitar receptor, ignorar modem
    tio.c_cflag &= ~PARENB;                 // sin paridad
    tio.c_cflag &= ~CSTOPB;                 // 1 stop bit
    tio.c_cflag &= ~CSIZE;  tio.c_cflag |= CS8; // 8 bits
    tio.c_iflag &= ~(IXON | IXOFF | IXANY); // sin control de flujo por soft
    tio.c_cc[VMIN]  = 1;
    tio.c_cc[VTIME] = 0;
    speed_t sp = map_baud(baud);
    cfsetispeed(&tio, sp);
    cfsetospeed(&tio, sp);
    if (tcsetattr(fd, TCSANOW, &tio) != 0){ ::close(fd); throw AppError("tcsetattr falló"); }
    ::close(fd);
}
#endif

// parser mínimo: si el µC devuelve CSV lo partimos; si devuelve JSON/XML, acá podrías
// transformarlo a columnas; como la persistencia es SIEMPRE CSV, esperamos columnas.
vector<string> App::parseLine(InputFormat fmt, const string& line) {
    if (fmt == InputFormat::CSV) return File::splitCSV(line);
    // si viniera JSON o XML del µC, acá deberías convertirlo a columnas reales.
    // Por ahora, lo guardamos como una sola columna 'raw' para no perder nada.
    return vector<string>{ line };
}

std::string App::requestLineFromSerial(const std::string& dev, char reqByte) {
    // Apertura del dispositivo como fstream simple. Para un control fino, usar termios.
    std::fstream ser(dev, std::ios::in | std::ios::out | std::ios::binary);
    if (!ser) throw AppError("No se pudo abrir el dispositivo serie: " + dev);
    ser.rdbuf()->pubsetbuf(nullptr, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(250));   // por si resetea el µC

    ser.put(reqByte);
    ser.flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    std::string line;
    if (!std::getline(ser, line)) return {};
    if (!line.empty() && line.back()=='\r') line.pop_back();
    return line;
}

#ifdef __linux__
static speed_t map_baud(int b){
    switch(b){
        case 9600: return B9600; case 19200: return B19200; case 38400: return B38400;
        case 57600: return B57600; case 115200: return B115200;
        default: return B9600;
    }
}

int App::openSerialRaw8N1(const std::string& dev, int baud){
    int fd = ::open(dev.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fd < 0) throw AppError("No se pudo abrir TTY.");
    termios tio{};
    if (tcgetattr(fd, &tio) != 0){ ::close(fd); throw AppError("tcgetattr falló"); }
    cfmakeraw(&tio);
    tio.c_cflag |= CLOCAL | CREAD;     // habilitar receptor, ignorar módem
    tio.c_cflag &= ~PARENB;            // sin paridad
    tio.c_cflag &= ~CSTOPB;            // 1 stop bit
    tio.c_cflag &= ~CSIZE; tio.c_cflag |= CS8; // 8 bits
    tio.c_iflag &= ~(IXON | IXOFF | IXANY);    // sin flujo por soft
    tio.c_cc[VMIN]  = 1;
    tio.c_cc[VTIME] = 0;
    speed_t sp = map_baud(baud);
    cfsetispeed(&tio, sp);
    cfsetospeed(&tio, sp);
    if (tcsetattr(fd, TCSANOW, &tio) != 0){ ::close(fd); throw AppError("tcsetattr falló"); }
    // Volver a bloqueante
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
    return fd;
}

bool App::writeByte(int fd, char b){
    ssize_t n = ::write(fd, &b, 1);
    return n == 1;
}

std::string App::readLine(int fd, int timeout_ms){
    std::string out;
    char ch;
    while(true){
        fd_set rfds; FD_ZERO(&rfds); FD_SET(fd, &rfds);
        timeval tv;
        tv.tv_sec  = timeout_ms/1000;
        tv.tv_usec = (timeout_ms%1000)*1000;
        int r = ::select(fd1, &rfds, nullptr, nullptr, &tv);
        if (r == 0) return "";           // timeout
        if (r < 0)  return "";           // error -> tratamos como timeout
        ssize_t n = ::read(fd, &ch, 1);
        if (n == 1){
            if (ch == '\n') break;
            if (ch != '\r') out.push_back(ch);
        }
    }
    return out;
}
#endif

int App::run(int argc, char** argv) {
    try {
        CLI cli;
        cli.parse(argc, argv);

        // CSV es la verdad: header estándar si no existe
        // Si tu µC manda campos fijos, ponelos aquí:
        // Ejemplo común Id,V,LevelQ. Si no, usa un "raw" para JSON/XML de entrada.
        vector<string> header = (cli.inFormat()==InputFormat::CSV)
            ? vector<string>{"Id","V","LevelQ"}
            : vector<string>{"raw"};

        File csv(cli.fileName());
        if (cli.mode() == Mode::Write) {
            csv.ensureHeader(header);

            #ifdef __linux__
            if (cli.useSerial()) {
                // Configurar la TTY antes de empezar a hacer I/O con fstream
                set_serial_raw_8N1(cli.serialDev(), cli.baud());
                // after config, la primera apertura puede resetear el Arduino: espera breve
                std::this_thread::sleep_for(std::chrono::milliseconds(400));
            }
            #endif

            for (int i=0; i<cli.readCount(); ++i) {
                std::string line;
                if (cli.useSerial()) {
                    line = requestLineFromSerial(cli.serialDev(), fmtByte(cli.inFormat()));
                    if (line.empty()) continue;
                } else {
                    if (!std::getline(std::cin, line)) break;
                }
                auto cols = parseLine(cli.inFormat(), line);
                // ajusta tamaño a header
                if (cols.size() < header.size()) cols.resize(header.size());
                csv.appendCSVLine(cols);
            }
            return 0;
        }

        // Mode::Read — leemos CSV y emitimos al formato pedido
        auto lines = csv.readAllLinesWithHeader();   // incluye cabecera
        switch (cli.outFormat()) {
            case OutputFormat::CSV:  std::cout << File::toCSV(lines);  break;
            case OutputFormat::JSON: std::cout << File::toJSON(lines); break;
            case OutputFormat::XML:  std::cout << File::toXML(lines);  break;
        }
        return 0;

    } catch (const AppError& e) {
        std::cerr << "[FATAL] " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "[FATAL] " << e.what() << "\n";
        return 2;
    }
}
