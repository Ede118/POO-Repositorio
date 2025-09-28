#pragma once

#include <stdexcept>
#include <string>
#include <vector>

class AppError : public std::runtime_error {
public:
    explicit AppError(const std::string& msg) : std::runtime_error(msg) {}
};

enum class Mode { Read, Write };

enum class Event { Read, Write, Append };

inline char eventToChar(Event e) {
    switch (e) {
        case Event::Read: return 'r';
        case Event::Write: return 'w';
        case Event::Append: return 'a';
        default: return '?';
    }
}

enum class IOFormat { CSV, JSON, XML };

IOFormat charToFormat(char c);
const char* fmtName(IOFormat f);

std::vector<std::string> parseLine(IOFormat inFmt, const std::string& line,
                                   std::vector<std::string>* headersOpt = nullptr);

bool tryParseHeaderTag(const std::string& line, std::vector<std::string>& headers);
