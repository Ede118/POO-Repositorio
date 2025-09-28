#include "App.h"

#include <algorithm>
#include <cctype>
#include <sstream>

App::App(File& file, Verificador& verificador)
    : file_(file), verificador_(verificador) {}

void App::processWrite(std::istream& input, bool appendMode) {
    accepted_.clear();
    rejected_.clear();

    Event ev = appendMode ? Event::Append : Event::Write;
    std::string rawLine;
    while (std::getline(input, rawLine)) {
        const std::string line = trim(rawLine);
        if (line.empty()) {
            continue;
        }

        auto parts = splitCSV(line);
        if (parts.size() != 6) {
            throw AppError("Se esperaban 6 campos: id, device, c1..c4");
        }

        std::vector<std::string> cols{parts.begin() + 2, parts.end()};
        NodoID nodo(parts[0], parts[1], cols);

        const bool allowed = ev == Event::Append
                                 ? verificador_.canAppend(nodo.id(), nodo.device())
                                 : verificador_.canWrite(nodo.id(), nodo.device());
        if (!allowed) {
            rejected_.push_back(line);
            continue;
        }

        file_.writeEventRow(nodo.id(), ev, nodo.cols());
        accepted_.push_back(line);
    }
}

const std::vector<std::string>& App::accepted() const noexcept { return accepted_; }
const std::vector<std::string>& App::rejected() const noexcept { return rejected_; }

std::vector<std::string> App::splitCSV(const std::string& line) {
    std::vector<std::string> parts;
    std::string current;
    std::istringstream iss(line);
    while (std::getline(iss, current, ',')) {
        parts.push_back(trim(current));
    }
    return parts;
}

std::string App::trim(const std::string& s) {
    auto begin = std::find_if_not(s.begin(), s.end(), [](unsigned char c) { return std::isspace(c); });
    auto end = std::find_if_not(s.rbegin(), s.rend(), [](unsigned char c) { return std::isspace(c); }).base();
    if (begin >= end) {
        return {};
    }
    return std::string(begin, end);
}
