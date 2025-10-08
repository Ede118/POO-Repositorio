#include "File.h"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace {
constexpr char kLineSep = '\n';
}

// ============================================================================================ //

File::File(std::filesystem::path path) : path_(std::move(path)) {}

// ============================================================================================ //

bool File::isEmpty() const {
    if (!std::filesystem::exists(path_)) {
        return true;
    }
    std::error_code ec;
    const auto size = std::filesystem::file_size(path_, ec);
    return ec ? true : size == 0;
}

// ============================================================================================ //

void File::writeHeader(const std::vector<std::string>& columns) {
    ensureParentDir();
    std::ofstream out(path_, std::ios::app);
    if (!out) {
        throw std::runtime_error("No se pudo abrir el archivo para escribir la cabecera");
    }
    bool needsNewline = false;
    if (!isEmpty()) {
        std::ifstream in(path_);
        if (in) {
            std::string firstLine;
            if (std::getline(in, firstLine)) {
                std::ostringstream expected;
                for (size_t i = 0; i < columns.size(); ++i) {
                    if (i != 0) {
                        expected << ',';
                    }
                    expected << columns[i];
                }
                if (firstLine == expected.str()) {
                    return;
                }
            }
            needsNewline = true;
        }
    }
    if (needsNewline) {
        out << kLineSep;
    }
    for (size_t i = 0; i < columns.size(); ++i) {
        if (i != 0) {
            out << ',';
        }
        out << columns[i];
    }
    out << kLineSep;
    if (!out) {
        throw std::runtime_error("Fallo la escritura de la cabecera");
    }
}

// ============================================================================================ //

void File::appendRow(const std::vector<std::string>& columns) {
    ensureParentDir();
    std::ofstream out(path_, std::ios::app);
    if (!out) {
        throw std::runtime_error("No se pudo abrir el archivo para escribir la fila");
    }
    for (size_t i = 0; i < columns.size(); ++i) {
        if (i != 0) {
            out << ',';
        }
        out << escapeCsv(columns[i]);
    }
    out << kLineSep;
    if (!out) {
        throw std::runtime_error("Fallo la escritura de la fila");
    }
}

// ============================================================================================ //

std::vector<std::vector<std::string>> File::readAllRows() const {
    std::vector<std::vector<std::string>> rows;
    if (!std::filesystem::exists(path_)) {
        return rows;
    }
    std::ifstream in(path_);
    if (!in) {
        throw std::runtime_error("No se pudo abrir el archivo para lectura");
    }
    std::string line;
    bool headerSkipped = false;
    while (std::getline(in, line)) {
        if (!headerSkipped) {
            headerSkipped = true;
            continue;
        }
        if (line.empty()) {
            continue;
        }
        rows.emplace_back(parseCsvLine(line));
    }
    return rows;
}

// ============================================================================================ //

void File::ensureParentDir() const {
    const auto parent = path_.parent_path();
    if (!parent.empty()) {
        std::filesystem::create_directories(parent);
    }
}

// ============================================================================================ //

std::string File::escapeCsv(const std::string& field) {
    bool needsQuotes = false;
    std::string escaped;
    escaped.reserve(field.size());
    for (char c : field) {
        if (c == '"' || c == ',' || c == '\n' || c == '\r') {
            needsQuotes = true;
        }
        if (c == '"') {
            escaped.push_back('"');
        }
        escaped.push_back(c);
    }
    if (!needsQuotes) {
        return escaped.empty() ? field : escaped;
    }
    std::ostringstream oss;
    oss << '"' << (escaped.empty() ? field : escaped) << '"';
    return oss.str();
}

// ============================================================================================ //

std::vector<std::string> File::parseCsvLine(const std::string& line) {
    std::vector<std::string> result;
    std::string current;
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (inQuotes) {
            if (c == '"') {
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    current.push_back('"');
                    ++i;
                } else {
                    inQuotes = false;
                }
            } else {
                current.push_back(c);
            }
        } else {
            if (c == '"') {
                inQuotes = true;
            } else if (c == ',') {
                result.emplace_back(current);
                current.clear();
            } else {
                current.push_back(c);
            }
        }
    }
    result.emplace_back(current);
    return result;
}
