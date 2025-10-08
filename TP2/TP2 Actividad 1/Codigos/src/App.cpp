#include "App.h"
#include "File.h"
#include "Types.h"
#include "CLI.h"
#include "serial.h"
#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <algorithm>
#include <memory>


namespace {
enum class WireFmt { CSV, JSON, XML, Unknown };

WireFmt sniff_wirefmt(const std::string& s) {
    size_t p = s.find_first_not_of(" \t\r\n");
    if (p == std::string::npos) return WireFmt::Unknown;
    char c = s[p];
    if (c == '{') return WireFmt::JSON;
    if (c == '<') return WireFmt::XML;
    return WireFmt::CSV;
}

void trimCRLF(std::string& s) {
    while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' ')) {
        s.pop_back();
    }
}

std::string buildCsvPath(const CLI& cli) {
    std::filesystem::create_directories(cli.baseDir());
    return (std::filesystem::path(cli.baseDir()) / (cli.fileName() + ".csv")).string();
}
} // namespace


int App::run(int argc, char** argv) {
    CLI cli;
    cli.parse(argc, argv);

    if (cli.mode() != Mode::Write) {
        throw AppError("Modo lectura no implementado con File V2.");
    }

    const std::string path = buildCsvPath(cli);
    File file(path);
    Event ev = cli.append() ? Event::Append : Event::Write;

    std::unique_ptr<SerialPort> sp;
    if (cli.useSerial()) {
        sp = std::make_unique<SerialPort>(cli.serialDev(), B115200);
        sp->flushIn();
    }

    std::vector<std::string> headers;
    const int target = std::max(0, cli.readCount());
    const bool fixedN = target > 0;
    int done = 0;
    int miss = 0;
    const int max_miss = std::max(30, 5 * std::max(1, target));

    const char reqChar = (cli.inputFmt() == IOFormat::CSV ? 'c'
                        : (cli.inputFmt() == IOFormat::JSON ? 'j' : 'x'));

    const std::string userId = cli.fileName().empty() ? std::string("sensor") : cli.fileName();

    while (!fixedN || done < target) {
        if (sp) {
            sp->writeByte(reqChar);
        }

        std::string line;
        bool got = sp ? sp->readLine(line, 2000)
                      : static_cast<bool>(std::getline(std::cin, line));
        if (!got || line.empty()) {
            if (++miss > max_miss) {
                break;
            }
            continue;
        }
        trimCRLF(line);

        WireFmt wf = sniff_wirefmt(line);
        IOFormat parseFmt =
            (wf == WireFmt::CSV)  ? IOFormat::CSV :
            (wf == WireFmt::JSON) ? IOFormat::JSON :
                                    IOFormat::XML;

        if ((parseFmt == IOFormat::JSON || parseFmt == IOFormat::XML) && headers.empty()) {
            headers = {"dispositivo_id", "porcentaje_valvula", "estado_nivel", "caudal"};
        }

        auto fields = parseLine(parseFmt, line, headers.empty() ? nullptr : &headers);
        if (fields.size() != 4) {
            if (++miss > max_miss) {
                break;
            }
            continue;
        }

        file.writeEventRow(userId, ev, fields);
        ++done;

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }

    std::cout << "Lecturas guardadas : " << done << "\n";
    std::cout << "Archivo: " << file.path() << "\n";
    return 0;
}
