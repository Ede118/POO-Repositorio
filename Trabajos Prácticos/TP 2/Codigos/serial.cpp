#include "serial.h"
#include <fcntl.h>
#include <stdexcept>
#include <sys/select.h>
#include <cstring>

static speed_t to_speed(int baud) {
    switch (baud) {
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        default: return B9600;
    }
}

SerialPort::SerialPort(const std::string& dev, int baud) {
    fd_ = ::open(dev.c_str(), O_RDWR | O_NOCTTY | O_SYNC);
    if (fd_ < 0) throw std::runtime_error("No se pudo abrir " + dev + ": " + std::strerror(errno));

    termios tio{};
    if (tcgetattr(fd_, &old_) != 0) throw std::runtime_error("tcgetattr falló");

    cfmakeraw(&tio);
    tio.c_cflag |= CLOCAL | CREAD;
    tio.c_cflag &= ~CRTSCTS;        // sin HW flow
    tio.c_cflag &= ~PARENB;         // 8N1
    tio.c_cflag &= ~CSTOPB;
    tio.c_cflag &= ~CSIZE;
    tio.c_cflag |= CS8;

    cfsetispeed(&tio, to_speed(baud));
    cfsetospeed(&tio, to_speed(baud));

    // VMIN/VTIME: lectura con timeout (decenas de ms)
    tio.c_cc[VMIN]  = 0;
    tio.c_cc[VTIME] = 1; // 100 ms

    if (tcsetattr(fd_, TCSANOW, &tio) != 0) throw std::runtime_error("tcsetattr falló");
    tcflush(fd_, TCIOFLUSH);
}

SerialPort::~SerialPort() {
    if (fd_ >= 0) {
        tcsetattr(fd_, TCSANOW, &old_);
        ::close(fd_);
    }
}

void SerialPort::writeByte(char c) {
    writeData(&c, 1);
}

void SerialPort::writeData(const void* data, size_t n) {
    const uint8_t* p = static_cast<const uint8_t*>(data);
    while (n) {
        ssize_t w = ::write(fd_, p, n);
        if (w < 0) throw std::runtime_error("write fallo");
        p += w; n -= w;
    }
    tcdrain(fd_);
}

bool SerialPort::readLine(std::string& out, int timeout_ms) {
    out.clear();
    const int step = 50; // ms
    int waited = 0;
    char ch;

    while (true) {
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(fd_, &rfds);
        timeval tv{0, step*1000};
        int r = select(fd_+1, &rfds, nullptr, nullptr, &tv);
        if (r > 0 && FD_ISSET(fd_, &rfds)) {
            ssize_t n = ::read(fd_, &ch, 1);
            if (n == 1) {
                if (ch == '\n' || ch == '\r') return !out.empty();
                out.push_back(ch);
            }
        } else {
            waited += step;
            if (waited >= timeout_ms) return !out.empty();
        }
    }
}
