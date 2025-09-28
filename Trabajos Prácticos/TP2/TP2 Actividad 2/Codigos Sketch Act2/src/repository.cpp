#include "repository.h"

#include <filesystem>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace {
Bytes readAll(const std::filesystem::path& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        throw std::runtime_error("Unable to open file for reading: " + path.string());
    }
    std::vector<char> buffer((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    Bytes bytes;
    bytes.reserve(buffer.size());
    for (unsigned char c : buffer) {
        bytes.push_back(static_cast<std::byte>(c));
    }
    return bytes;
}
}

void FileRepository::save(const Message& m, const std::string& path) {
    auto bytes = ser_.toBytes(m);
    std::filesystem::path outPath(path);
    if (outPath.has_parent_path()) {
        std::filesystem::create_directories(outPath.parent_path());
    }
    std::ofstream out(outPath, std::ios::binary | std::ios::trunc);
    if (!out) {
        throw std::runtime_error("Unable to open file for writing: " + outPath.string());
    }
    out.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
    if (!out) {
        throw std::runtime_error("Failed while writing file: " + outPath.string());
    }
}

Message FileRepository::load(const std::string& path) {
    auto bytes = readAll(path);
    return ser_.fromBytes(bytes);
}
