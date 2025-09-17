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

using std::string;
namespace fs = std::filesystem;

static string buildCsvPath(const CLI& cli) {
    fs::create_directories(cli.baseDir());
    return (fs::path(cli.baseDir()) / (cli.fileName() + ".csv")).string();
}

int App::run(int argc, char** argv) {
    CLI cli;
    cli.parse(argc, argv);

    if (cli.mode() == Mode::Write) {
        const string path = buildCsvPath(cli);

        // ¿existe y tiene cabecera?
        auto has_header = [&](){
            std::ifstream chk(path);
            std::string l;
            if (std::getline(chk,l)) return l.rfind("#h:",0)==0;
            return false;
        }();

        File f(path);
        f.open(cli.append() ? 'a' : 'w');

        // SERIAL
        SerialPort* sp = nullptr;
        if (cli.useSerial()) {
            sp = new SerialPort(cli.serialDev(), B9600); // cambia a B115200 si tu firmware usa eso
        }

        // calcular nextId leyendo la última línea (si existe y estamos en append)
        long nextId = 1;
        if (cli.append()) {
            std::ifstream is(path);
            std::string line, last;
            while (std::getline(is,line)) {
                if (!line.empty() && line[0] != '#') last = line;
            }
            if (!last.empty()) {
                auto p = last.find(',');
                std::string first = (p==std::string::npos)? last : last.substr(0,p);
                try { nextId = std::stol(first) + 1; } catch (...) { nextId = 1; }
            }
        }

        std::vector<std::string> headers;   // headers de datos (sin Id)
        bool header_written = has_header;   // si ya hay "#h:" no escribimos otra

        const bool fixedN = cli.readCount() > 0;
        const int target  = fixedN ? cli.readCount() : 5;
        const int rx_timeout_ms = 2000;     // ajustá si tu µC es lerdo
        int done = 0;
        int timeouts = 0;
        const int max_timeouts = (fixedN ? std::max(30, 5*target) : 1000);

        auto send_request = [&](char req){
            if (sp) sp->writeByte(req);
        };
        auto reqChar = (cli.inputFmt()==IOFormat::CSV ? 'x' :
                    (cli.inputFmt()==IOFormat::JSON? 'j' : 'c'));

        while (done < target && timeouts < max_timeouts) {
            send_request(reqChar);

            string line;
            bool got = false;
            if (sp) got = sp->readLine(line, rx_timeout_ms);
            else    got = static_cast<bool>(std::getline(std::cin, line));

            if (!got) { ++timeouts; continue; }
            if (line.empty()) continue;

            // ¿El µC mandó cabecera?
            if (headers.empty()) {
                std::vector<std::string> tmp;
                if (tryParseHeaderTag(line, tmp)) { headers = tmp; continue; }
            }

            // Parsear la línea en valores
            auto fields = parseLine(cli.inputFmt(), line, headers.empty()?nullptr:&headers);
            if (fields.empty()) continue;

            // Si aún no tenemos headers, infiéralos por cantidad de columnas y escribilos como "#h:"
            if (!header_written) {
                if (headers.empty()) {
                    headers.reserve(fields.size());
                    for (size_t i=0;i<fields.size();++i) headers.push_back("c"+std::to_string(i+1));
                }
                std::vector<std::string> hdr; hdr.reserve(headers.size()+1);
                hdr.push_back("Id");
                hdr.insert(hdr.end(), headers.begin(), headers.end());
                f.writeHeaderTag(hdr);
                header_written = true;
            }

            // anteponé Id y persistí como CSV
            fields.insert(fields.begin(), std::to_string(nextId++));
            f.writeParsed(fields);

            ++done;
            // micro-respiro para no saturar
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }

        f.close();
        delete sp; sp=nullptr;

        std::cout << f.getInfoTable();
        return 0;
    } else if (cli.mode() == Mode::Read) {
        // READ: mostrar info + datos en formato pedido
        const string path = buildCsvPath(cli);
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