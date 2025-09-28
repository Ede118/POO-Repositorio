#include "Types.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>

IOFormat charToFormat(char c) {
    switch (c) {
        case 'c': case 'C': return IOFormat::CSV;
        case 'j': case 'J': return IOFormat::JSON;
        case 'x': case 'X': return IOFormat::XML;
        default: throw std::invalid_argument("Formato inválido (use c|j|x).");
    }
}

const char* fmtName(IOFormat f) {
    switch (f) {
        case IOFormat::CSV: return "CSV";
        case IOFormat::JSON: return "JSON";
        case IOFormat::XML: return "XML";
    }
    return "?";
}

namespace {
std::vector<std::string> splitCSVsimple(const std::string& line) {
    std::vector<std::string> out;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, ',')) {
        out.push_back(token);
    }
    return out;
}

std::string trim(const std::string& value) {
    auto begin = std::find_if_not(value.begin(), value.end(), [](unsigned char ch) { return std::isspace(ch); });
    auto end = std::find_if_not(value.rbegin(), value.rend(), [](unsigned char ch) { return std::isspace(ch); }).base();
    if (begin >= end) {
        return {};
    }
    return std::string(begin, end);
}
}

std::vector<std::string> parseLine(IOFormat inFmt, const std::string& line,
                                   std::vector<std::string>* headersOpt) {
    if (inFmt == IOFormat::CSV) {
        auto values = splitCSVsimple(line);
        for (auto& value : values) {
            value = trim(value);
        }
        return values;
    }

    if (inFmt == IOFormat::JSON) {
        std::vector<std::string> values;
        if (headersOpt && !headersOpt->empty()) {
            for (const auto& key : *headersOpt) {
                const auto keyPos = line.find('"' + key + '"');
                if (keyPos == std::string::npos) {
                    values.emplace_back();
                    continue;
                }
                auto colon = line.find(':', keyPos);
                if (colon == std::string::npos) {
                    values.emplace_back();
                    continue;
                }
                auto end = line.find_first_of(",}", colon + 1);
                std::string raw = line.substr(colon + 1, end - colon - 1);
                raw.erase(std::remove_if(raw.begin(), raw.end(), [](unsigned char ch) {
                    return ch == '"' || std::isspace(ch);
                }), raw.end());
                values.push_back(std::move(raw));
            }
            return values;
        }

        size_t pos = 0;
        while ((pos = line.find(':', pos)) != std::string::npos) {
            auto end = line.find_first_of(",}", pos + 1);
            std::string raw = line.substr(pos + 1, end - pos - 1);
            raw.erase(std::remove_if(raw.begin(), raw.end(), [](unsigned char ch) {
                return ch == '"' || std::isspace(ch);
            }), raw.end());
            values.push_back(std::move(raw));
            pos = end;
        }
        return values;
    }

    std::vector<std::string> values;
    if (headersOpt && !headersOpt->empty()) {
        for (const auto& key : *headersOpt) {
            const std::string open = '<' + key + '>';
            const std::string close = "</" + key + '>';
            auto begin = line.find(open);
            auto end = line.find(close);
            if (begin == std::string::npos || end == std::string::npos || end < begin) {
                values.emplace_back();
                continue;
            }
            values.push_back(line.substr(begin + open.size(), end - (begin + open.size())));
        }
        return values;
    }

    size_t pos = 0;
    while (true) {
        auto open = line.find('<', pos);
        if (open == std::string::npos) {
            break;
        }
        if (line[open + 1] == '/') {
            pos = open + 1;
            continue;
        }
        auto close = line.find('>', open + 1);
        auto closeTag = line.find("</", close + 1);
        if (close == std::string::npos || closeTag == std::string::npos) {
            break;
        }
        values.push_back(line.substr(close + 1, closeTag - (close + 1)));
        pos = closeTag + 2;
    }
    return values;
}

bool tryParseHeaderTag(const std::string& line, std::vector<std::string>& headers) {
    if (line.rfind("#h:", 0) != 0) {
        return false;
    }
    std::string rest = line.substr(3);
    std::stringstream ss(rest);
    std::string item;
    headers.clear();
    while (std::getline(ss, item, ',')) {
        headers.push_back(trim(item));
    }
    return true;
}
