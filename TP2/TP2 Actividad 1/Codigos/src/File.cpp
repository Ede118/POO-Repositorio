#include "File.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <utility>

namespace {
std::string trimTrailing(const std::string& s) {
    std::string out = s;
    while (!out.empty() && (out.back() == '\r' || out.back() == '\n')) {
        out.pop_back();
    }
    return out;
}
}

File::File(std::filesystem::path path) : path_(std::move(path)) {}

const std::filesystem::path& File::path() const noexcept { return path_; }

void File::writeHeaderV2() {
    if (headerWritten_) {
        return;
    }
    if (hasHeaderOnDisk()) {
        headerWritten_ = true;
        return;
    }

    ensureParentDir();
    std::ofstream out(path_, std::ios::app);
    if (!out) {
        throw AppError("No se pudo abrir el archivo para escribir la cabecera");
    }
    if (std::filesystem::exists(path_) && std::filesystem::file_size(path_) > 0) {
        out << '\n';
    }
    out << kHeader << '\n';
    if (!out) {
        throw AppError("Fallo la escritura de la cabecera");
    }
    headerWritten_ = true;
}

void File::writeEventRow(const std::string& id, Event ev, const std::vector<std::string>& cols4) {
    if (cols4.size() != 4) {
        throw AppError("Se requieren exactamente 4 columnas de datos");
    }
    writeHeaderV2();
    ensureParentDir();
    std::ofstream out(path_, std::ios::app);
    if (!out) {
        throw AppError("No se pudo abrir el archivo de eventos");
    }
    out << buildRow(id, ev, cols4) << '\n';
    if (!out) {
        throw AppError("Fallo la escritura de la fila de evento");
    }
}

std::string File::nowStr() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void File::ensureParentDir() const {
    const auto parent = path_.parent_path();
    if (!parent.empty()) {
        std::filesystem::create_directories(parent);
    }
}

bool File::hasHeaderOnDisk() const {
    if (!std::filesystem::exists(path_)) {
        return false;
    }
    std::ifstream in(path_);
    if (!in) {
        return false;
    }
    std::string firstLine;
    if (!std::getline(in, firstLine)) {
        return false;
    }
    return trimTrailing(firstLine) == kHeader;
}

std::string File::buildRow(const std::string& id, Event ev, const std::vector<std::string>& cols4) {
    std::ostringstream oss;
    oss << id << ", " << nowStr() << ", " << eventToChar(ev);
    for (const auto& col : cols4) {
        oss << ", " << col;
    }
    return oss.str();
}
