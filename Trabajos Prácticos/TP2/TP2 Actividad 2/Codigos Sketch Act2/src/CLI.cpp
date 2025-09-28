#include "CLI.h"

#include <cstdlib>
#include <iostream>
#include <sstream>

void CLI::parse(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-m" && i + 1 < argc) {
            const char modeChar = argv[++i][0];
            if (modeChar == 'r' || modeChar == 'R') {
                mode_ = Mode::Read;
            } else if (modeChar == 'w' || modeChar == 'W') {
                mode_ = Mode::Write;
            } else {
                throw AppError("Valor inválido para -m (use r|w)");
            }
        } else if ((arg == "-i" || arg == "--input") && i + 1 < argc) {
            inputPath_ = std::filesystem::path(argv[++i]);
        } else if ((arg == "-o" || arg == "--output") && i + 1 < argc) {
            outputPath_ = std::filesystem::path(argv[++i]);
        } else if (arg == "-a" || arg == "--append") {
            append_ = true;
        } else if (arg == "--allow" && i + 1 < argc) {
            parseAllow(argv[++i]);
        } else if (arg == "-h" || arg == "--help") {
            printHelp();
            std::exit(0);
        } else {
            std::ostringstream oss;
            oss << "Opción no reconocida: " << arg;
            throw AppError(oss.str());
        }
    }

    if (mode_ != Mode::Write) {
        throw AppError("Solo se admite Mode::Write en esta etapa");
    }
}

void CLI::printHelp() {
    std::cout << "Uso:\n"
                 "  -m w             Modo escritura (único soportado)\n"
                 "  -i, --input P    Ruta del archivo CSV de entrada (default: stdin)\n"
                 "  -o, --output P   Ruta de salida para el CSV consolidado\n"
                 "  -a, --append     Registra eventos como append ('a')\n"
                 "  --allow SPEC     Regla de permisos id:device:perms (ej: u001:dev-alpha:wa)\n"
                 "  -h, --help       Muestra esta ayuda\n";
}

void CLI::parseAllow(const std::string& spec) {
    const auto first = spec.find(':');
    const auto second = spec.find(':', first == std::string::npos ? first : first + 1);
    if (first == std::string::npos || second == std::string::npos || second + 1 >= spec.size()) {
        throw AppError("Formato de --allow inválido. Use id:device:perms");
    }
    AllowRule rule;
    rule.id = spec.substr(0, first);
    rule.device = spec.substr(first + 1, second - first - 1);
    rule.perms = spec.substr(second + 1);
    if (rule.id.empty() || rule.device.empty() || rule.perms.empty()) {
        throw AppError("--allow requiere id, device y permisos (r|w|a)");
    }
    rules_.push_back(std::move(rule));
}
