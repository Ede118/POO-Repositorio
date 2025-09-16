#include "File.h"
#include "Types.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

using std::string;
using std::vector;

File::File(std::string path) : path_(std::move(path)) {}

bool File::exists() const {
    namespace fs = std::filesystem;
    std::error_code ec;
    return fs::exists(path_, ec) && !fs::is_directory(path_, ec);
}

bool File::hasHeader() const {
    std::ifstream in(path_);
    string first;
    if (!in.good()) return false;
    if (!std::getline(in, first)) return false;
    // Heurística mínima: si tiene comas y no arranca con dígito, lo tomamos como cabecera
    return !first.empty() && first.find(',')!=string::npos && !std::isdigit(static_cast<unsigned char>(first[0]));
}

void File::ensureHeader(const vector<string>& header) {
    if (exists() && hasHeader()) return;
    std::ofstream out(path_, std::ios::app);
    if (!out) throw AppError("No se pudo abrir CSV para cabecera: " + path_);
    out << joinCSV(header) << '\n';
}

void File::appendCSVLine(const vector<string>& cols) {
    std::ofstream out(path_, std::ios::app);
    if (!out) throw AppError("No se pudo abrir CSV para escribir: " + path_);
    out << joinCSV(cols) << '\n';
}

vector<string> File::readAllLines() const {
    std::ifstream in(path_);
    if (!in) throw AppError("No se pudo abrir CSV para leer: " + path_);
    vector<string> lines;
    string line;
    // saltar cabecera si existe
    if (hasHeader()) std::getline(in, line);
    while (std::getline(in, line)) {
        if (!line.empty() && line.back()=='\r') line.pop_back();
        if (!line.empty()) lines.push_back(line);
    }
    return lines;
}

vector<string> File::readAllLinesWithHeader() const {
    std::ifstream in(path_);
    if (!in) throw AppError("No se pudo abrir CSV para leer: " + path_);
    vector<string> lines;
    string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back()=='\r') line.pop_back();
        lines.push_back(line);
    }
    return lines;
}

string File::joinCSV(const vector<string>& cols) {
    std::ostringstream o;
    for (size_t i=0;i<cols.size();++i) {
        if (i) o << ',';
        // no escapo comillas para mantenerlo simple; tu micro ya no manda comas en números
        o << cols[i];
    }
    return o.str();
}

vector<string> File::splitCSV(const string& line) {
    vector<string> out;
    std::istringstream is(line);
    string field;
    while (std::getline(is, field, ',')) out.push_back(field);
    return out;
}

static string jsonEscape(const string& s){
    string r; r.reserve(s.size());
    for(char c: s){
        switch(c){
            case '\"': r += "\\\""; break;
            case '\\': r += "\\\\"; break;
            case '\b': r += "\\b";  break;
            case '\f': r += "\\f";  break;
            case '\n': r += "\\n";  break;
            case '\r': r += "\\r";  break;
            case '\t': r += "\\t";  break;
            default:   r += c;      break;
        }
    }
    return r;
}

std::string File::toCSV(const vector<string>& lines){
    std::ostringstream o;
    for (auto& ln: lines) o << ln << '\n';
    return o.str();
}

std::string File::toJSON(const vector<string>& lines){
    if (lines.empty()) return "[]";
    auto header = splitCSV(lines.front());
    std::ostringstream o;
    o << "[";
    for (size_t i=1;i<lines.size();++i){
        auto cols = splitCSV(lines[i]);
        o << "{";
        for (size_t j=0;j<header.size();++j){
            if (j) o << ",";
            std::string key = header[j];
            std::string val = j < cols.size() ? cols[j] : "";
            o << "\"" << jsonEscape(key) << "\":";
            // si parece número, no va entre comillas
            bool numeric = !val.empty() && std::find_if(val.begin(), val.end(), [](unsigned char ch){
                return !(std::isdigit(ch) || ch=='.' || ch=='-' || ch=='+');
            })==val.end();
            if (numeric) o << val;
            else o << "\"" << jsonEscape(val) << "\"";
        }
        o << "}";
        if (i+1<lines.size()) o << ",";
    }
    o << "]";
    return o.str();
}

std::string File::toXML(const vector<string>& lines){
    if (lines.empty()) return "<rows/>";
    auto header = splitCSV(lines.front());
    std::ostringstream o;
    o << "<rows>";
    for (size_t i=1;i<lines.size();++i){
        auto cols = splitCSV(lines[i]);
        o << "<row>";
        for (size_t j=0;j<header.size();++j){
            std::string key = header[j];
            std::string val = j < cols.size() ? cols[j] : "";
            o << "<" << key << ">" << val << "</" << key << ">";
        }
        o << "</row>";
    }
    o << "</rows>";
    return o.str();
}
