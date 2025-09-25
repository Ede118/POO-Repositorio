#ifndef TYPES_H
#define TYPES_H

#include <stdexcept>
#include <string>
#include <string>
#include <vector>


class AppError : public std::runtime_error {
public:
    explicit AppError(const std::string& msg) : std::runtime_error(msg) {}
};

enum class Mode { Read, Write };
enum class IOFormat { CSV, JSON, XML };

inline IOFormat charToFormat(char c) {
    switch (c) {
        case 'c': case 'C': return IOFormat::CSV;
        case 'j': case 'J': return IOFormat::JSON;
        case 'x': case 'X': return IOFormat::XML;
        default: throw std::invalid_argument("Formato inválido (use c|j|x).");
    }
}
inline const char* fmtName(IOFormat f) {
    switch (f) { case IOFormat::CSV: return "CSV"; case IOFormat::JSON: return "JSON"; default: return "XML"; }
}

// Convierte una línea recibida en formato c/j/x a un vector de strings.
// c => CSV; j => JSON plano tipo {"a":1,"b":2}; x => XML plano <row><a>1</a>...</row>
std::vector<std::string> parseLine(IOFormat inFmt, const std::string& line,
                                   std::vector<std::string>* headersOpt = nullptr);

// Encabezados: para CSV se asumen si vienen en la primera fila con '#h: a,b,c'
// Para JSON/XML se infieren del primer objeto si no se proveen.
bool tryParseHeaderTag(const std::string& line, std::vector<std::string>& headers);


#endif
