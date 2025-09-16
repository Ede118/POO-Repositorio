#include "CLI.h"
#include <cstring>
#include <cstdlib>

static InputFormat parseIn(char c) {
    if (c=='c') return InputFormat::CSV;
    if (c=='j') return InputFormat::JSON;
    if (c=='x') return InputFormat::XML;
    throw AppError("Formato de entrada inválido (use c|j|x).");
}
static OutputFormat parseOut(char c) {
    if (c=='c') return OutputFormat::CSV;
    if (c=='j') return OutputFormat::JSON;
    if (c=='x') return OutputFormat::XML;
    throw AppError("Formato de salida inválido (use c|j|x).");
}

void CLI::parse(int argc, char** argv) {
    for (int i=1; i<argc; ++i) {
        if (!std::strcmp(argv[i], "-m")) {
            if (++i>=argc) throw AppError("Falta valor para -m {w|r}.");
            mode_ = (argv[i][0]=='w') ? Mode::Write : Mode::Read;
        } else if (!std::strcmp(argv[i], "-i")) {
            if (++i>=argc) throw AppError("Falta valor para -i {c|j|x}.");
            inFormat_ = parseIn(argv[i][0]);
        } else if (!std::strcmp(argv[i], "-o")) {
            if (++i>=argc) throw AppError("Falta valor para -o {c|j|x}.");
            outFormat_ = parseOut(argv[i][0]);
        } else if (!std::strcmp(argv[i], "-f")) {
            if (++i>=argc) throw AppError("Falta valor para -f <archivo.csv>.");
            fileName_ = argv[i];
        } else if (!std::strcmp(argv[i], "-n")) {
            if (++i>=argc) throw AppError("Falta valor para -n <cantidad>.");
            readCount_ = std::atoi(argv[i]);
        } else if (!std::strcmp(argv[i], "-A")) {
            append_ = true;
        } else if (!std::strcmp(argv[i], "-s")) {
            if (++i>=argc) throw AppError("Falta valor para -s </dev/ttyACM0>.");
            useSerial_ = true;
            serialDev_ = argv[i];
        } else if (!std::strcmp(argv[i], "-b")) {
            if (++i>=argc) throw AppError("Falta valor para -b <baud>.");
            baud_ = std::atoi(argv[i]);
            if (baud_ <= 0) throw AppError("Baud inválido.");
        } else if (!std::strcmp(argv[i], "-d")) {
            if (++i>=argc) throw AppError("Falta valor para -d <baseDir>.");
            baseDir_ = argv[i];
        } else {
            throw AppError(std::string("Flag desconocida: ")+argv[i]);
        }
    }
    if (fileName_.empty()) throw AppError("Falta nombre de archivo base (-f). Ej: -f sensores.csv");
    if (mode_ == Mode::Write && readCount_ <= 0) readCount_ = 1;
}
