#include "App.h"
#include "File.h"
#include "Types.h"
#include "CLI.h"
#include "serial.h"
#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <fstream>
#include <algorithm>


// Detecta formato real de la línea
enum class WireFmt { CSV, JSON, XML, Unknown };
static inline WireFmt sniff_wirefmt(const std::string& s) {
    size_t p = s.find_first_not_of(" \t\r\n");
    if (p == std::string::npos) return WireFmt::Unknown;
    char c = s[p];
    if (c == '{') return WireFmt::JSON;
    if (c == '<') return WireFmt::XML;
    return WireFmt::CSV;
}

static inline void trimCRLF(std::string& s) {
    while (!s.empty() && (s.back()=='\r' || s.back()=='\n' || s.back()==' ')) s.pop_back();
}

static bool file_has_header(const std::string& path) {
    std::ifstream is(path);
    std::string l;
    if (!std::getline(is, l)) return false;
    return l.rfind("#h", 0) == 0;
}

static std::string buildCsvPath(const CLI& cli) {
    std::filesystem::create_directories(cli.baseDir());
    return (std::filesystem::path(cli.baseDir()) / (cli.fileName() + ".csv")).string();
}


int App::run(int argc, char** argv) {
    CLI cli;
    cli.parse(argc, argv);

    // ====================================================================================================================== //
    /*                                          Mode::Write                                                                   */
    // ====================================================================================================================== //

    if (cli.mode() == Mode::Write) {
        namespace fs = std::filesystem;

        // Ruta CSV (si ya la hacías, podés usar tu versión)
        fs::create_directories(cli.baseDir());
        const std::string path = (fs::path(cli.baseDir()) / (cli.fileName() + ".csv")).string();

        File f(path);
        f.open(cli.append() ? 'a' : 'w');

        // Serie
        SerialPort* sp = nullptr;
        if (cli.useSerial()) sp = new SerialPort(cli.serialDev(), B115200);
        if (sp) sp->flushIn(); // limpia restos previos

        // ¿ya tiene cabecera?
        bool has_header = file_has_header(path);
        std::vector<std::string> headers; // para JSON/XML

        const int target  = std::max(0, cli.readCount()); // N
        const bool fixedN = target > 0;
        int done = 0, miss = 0;
        const int max_miss = std::max(30, 5*std::max(1, target));

        // Byte de pedido (da igual si el µC contesta otra cosa: ahora “olfateamos”)
        const char reqChar = (cli.inputFmt()==IOFormat::CSV ? 'c'
                            : (cli.inputFmt()==IOFormat::JSON ? 'j' : 'x'));

        while (!fixedN || done < target) {
            // 1) pedir
            if (sp) sp->writeByte(reqChar);

            // 2) leer
            std::string line;
            bool got = sp ? sp->readLine(line, 2000)
                        : static_cast<bool>(std::getline(std::cin, line));
            if (!got || line.empty()) {
                if (++miss > max_miss) break;
                continue;
            }
            trimCRLF(line);

            // 3) detectar formato real y parsear
            WireFmt wf = sniff_wirefmt(line);
            IOFormat parseFmt =
                (wf == WireFmt::CSV)  ? IOFormat::CSV :
                (wf == WireFmt::JSON) ? IOFormat::JSON :
                                        IOFormat::XML;

            if ((parseFmt == IOFormat::JSON || parseFmt == IOFormat::XML) && headers.empty()) {
                headers = {"dispositivo_id","porcentaje_valvula","estado_nivel","caudal"};
            }

            auto fields = parseLine(parseFmt, line, headers.empty()?nullptr:&headers);
            if (fields.size() != 4) {          // fila inválida → no cuentes
                if (++miss > max_miss) break;
                continue;
            }

            // 4) cabecera única
            if (!has_header) {
                f.writeHeaderTag({"Id","porcentaje_valvula","estado_nivel","caudal"});
                has_header = true;
            }

            // 5) persistir SIEMPRE PARSEADO (nada de writeRaw)
            f.writeParsed(fields);
            ++done;

            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        f.close();
        delete sp;
        std::cout << "Lecturas guardadas : " << done << "\n";
        std::cout << f.getInfoTable();
        return 0;
    }

    // ====================================================================================================================== //
    /*                                          Mode::Read                                                                   */
    // ====================================================================================================================== //
    
    else if (cli.mode() == Mode::Read) {
        // READ: mostrar info + datos en formato pedido
        const std::string path = buildCsvPath(cli);
        File f(path);
        f.open('r');

        std::cout << f.getInfoTable();

        // Intentar headers de primera línea con "#h:"
        std::ifstream is(path);
        std::vector<std::string> headers;
        std::string firstLine;
        if (std::getline(is, firstLine) && tryParseHeaderTag(firstLine, headers)) {
            tryParseHeaderTag(firstLine, headers);
        }

        switch (cli.outputFmt()) {
            case IOFormat::CSV:
                std::cout << f.toCSV();
                break;
            case IOFormat::JSON: {
                if (headers.empty()) {
                    auto rows = f.readAllRows();
                    size_t cols = rows.empty()?0:rows[0].size();
                    for (size_t i=0;i<cols;++i) headers.push_back("c"+std::to_string(i+1));
                }
                std::cout << f.toJSON(headers);
                break;
            }
            case IOFormat::XML: {
                if (headers.empty()) {
                    auto rows = f.readAllRows();
                    size_t cols = rows.empty()?0:rows[0].size();
                    for (size_t i=0;i<cols;++i) headers.push_back("c"+std::to_string(i+1));
                }
                std::cout << f.toXML(headers);
                break;
            }
        }

        f.close();
        return 0;
    } else {
        throw AppError("Modo no soportado.");
        return 1;
    }
}
