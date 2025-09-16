#include "Files_101.h"

#include <sstream>
#include <iomanip>
#include <cctype>
#include <cstdlib>
#include <chrono>
#include <filesystem>
#include <fstream>

using namespace std;

// --------- utilidades ----------
static inline bool ends_with_csv(const std::string& s){
    return s.size() >= 4 && s.rfind(".csv") == s.size() - 4;
}

std::string Files_101::normalize_path_arg(const std::string& base){
    // recorta espacios y agrega .csv si falta; respeta subcarpetas si las hay
    std::string n = base;
    auto l = n.find_first_not_of(" \t\r\n");
    auto r = n.find_last_not_of(" \t\r\n");
    if (l == std::string::npos) n.clear();
    else n = n.substr(l, r - l + 1);
    if (!ends_with_csv(n)) n += ".csv";
    return n;
}

std::string Files_101::now_iso(){
    using clock = std::chrono::system_clock;
    auto t  = clock::to_time_t(clock::now());
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}

// ---------------- ctor ----------------
Files_101::Files_101(const std::string& base){
    path_ = normalize_path_arg(base);
}

// ------------- abrir/cerrar -----------
void Files_101::open(char mode){
    close();
    mode_ = mode;

    namespace fsys = std::filesystem;

    if (mode == 'w'){
        std::error_code ec;
        fsys::create_directories(fsys::path(path_).parent_path(), ec);
        fs_.open(path_, std::ios::out | std::ios::trunc);
        if (!fs_) throw std::runtime_error("No se pudo abrir " + path_ + " para escritura");
        dimension_ = 0;
        return;
    }
    if (mode == 'A'){
        std::error_code ec;
        fsys::create_directories(fsys::path(path_).parent_path(), ec);
        fs_.open(path_, std::ios::out | std::ios::app);
        if (!fs_) throw std::runtime_error("No se pudo abrir " + path_ + " para append");
        return;
    }
    if (mode == 'r'){
        fs_.open(path_, std::ios::in);            // SOLO lectura
        if (!fs_) throw std::runtime_error("No se pudo abrir " + path_ + " para lectura");
        fs_.clear();
        recount();                                 // cuenta filas de datos
        return;
    }
    throw std::runtime_error("Modo de apertura invalido");
}

void Files_101::close(){
    if (fs_.is_open()) fs_.close();
    mode_ = '?';
}

// ------------- existencia -------------
bool Files_101::exist() const{
    std::error_code ec;
    return std::filesystem::exists(path_, ec);
}

// -------------- escritura -------------
void Files_101::write(const std::string& line){
    if (!fs_.is_open()) throw std::runtime_error("Archivo no abierto para escritura.");
    fs_ << line << '\n';
    ++dimension_;
}

void Files_101::writeParsed(const std::vector<std::string>& fields){
    if (!fs_.is_open()) throw std::runtime_error("Archivo no abierto para escritura.");
    for (size_t i=0;i<fields.size();++i){
        if (i) fs_ << ',';
        fs_ << fields[i];
    }
    fs_ << '\n';
    ++dimension_;
}

// -------------- lectura ----------------
std::vector<std::string> Files_101::readAllLines() const{
    std::ifstream in(path_);
    std::vector<std::string> v; std::string s;
    while (std::getline(in, s)){
        chop_cr_(s);
        if (!s.empty()) v.push_back(s);  // si querés conservar vacías, quitá este if
    }
    return v;
}

void Files_101::ensureOpenForRead(){
    if (!fs_.is_open() || mode_ != 'r')
        throw std::runtime_error("Stream no abierto en modo lectura.");
}

std::optional<std::string> Files_101::getLine(){
    ensureOpenForRead();
    std::string s;
    if (std::getline(fs_, s)) {
        chop_cr_(s);
        if (s.empty()) return std::nullopt; // opcional: saltear vacías
        return s;
    }
    return std::nullopt;
}

// --------- helpers CSV -------------
std::vector<std::string> Files_101::splitCSV_(const std::string& line){
    std::vector<std::string> out;
    std::string cur; bool inq=false;
    for(char ch: line){
        if (ch=='"'){ inq=!inq; continue; }
        if (ch==',' && !inq){ out.push_back(cur); cur.clear(); }
        else cur.push_back(ch);
    }
    out.push_back(cur);
    return out;
}

// --------- recuento para -m r -------
std::size_t Files_101::recount(){
    std::ifstream in(path_);
    if (!in) { dimension_ = 0; return 0; }

    std::string s;
    std::size_t count = 0;
    bool header_skipped = false;

    while (std::getline(in, s)) {
        chop_cr_(s);
        if (s.empty()) continue;
        if (!header_skipped) { header_skipped = true; continue; } // saltea cabecera
        ++count;
    }
    dimension_ = count;
    return count;
}

// -------- getters formateados --------
std::string Files_101::getCSV(){
    std::ifstream in(path_, std::ios::in);
    if (!in) return "";

    std::ostringstream oss;
    std::string s;
    bool first = true;

    while (std::getline(in, s)) {
        chop_cr_(s);
        if (!first) { oss << '\n'; }
        first = false;
        oss << s;
    }
    return oss.str();
}

std::string Files_101::getJSON(){
    auto lines = readAllLines();
    if (lines.empty()) return "[]";

    auto headers = splitCSV_(lines.front());
    std::ostringstream out;
    out << "[";

    bool first_row = true;
    for (size_t i=1;i<lines.size();++i){
        auto row = splitCSV_(lines[i]);
        if (row.size() < headers.size()) row.resize(headers.size());

        bool all_empty = true;
        for (auto& v: row) if (!v.empty()) { all_empty=false; break; }
        if (all_empty) continue;

        if (!first_row) out << ",\n";
        first_row = false;

        out << "{";
        for (size_t c=0;c<headers.size();++c){
            if (c) out << ",";
            out << "\"" << headers[c] << "\": ";
            if (looks_number_(row[c])) out << row[c];
            else out << "\"" << row[c] << "\"";
        }
        out << "}";
    }
    out << "]";
    return out.str();
}

std::string Files_101::getXML(){
    auto lines = readAllLines();
    if (lines.empty()) return "<root/>";

    auto headers = splitCSV_(lines.front());
    std::ostringstream out;
    out << "<root>";
    for (size_t i=1;i<lines.size();++i){
        auto row = splitCSV_(lines[i]);
        if (row.size() < headers.size()) row.resize(headers.size());

        bool all_empty = true;
        for (auto& v: row) if (!v.empty()) { all_empty=false; break; }
        if (all_empty) continue;

        out << "<row>";
        for (size_t c=0;c<headers.size();++c){
            out << "<" << headers[c] << ">" << row[c] << "</" << headers[c] << ">";
        }
        out << "</row>";
    }
    out << "</root>";
    return out.str();
}

// --------------- info ----------------
FileInfo Files_101::getInfo() const{
    FileInfo info;
    auto pos = path_.find_last_of("/\\");
    info.name = (pos == std::string::npos) ? path_ : path_.substr(pos+1);
    info.datetime  = now_iso();
    const char* u  = std::getenv("USER");
    info.owner     = u ? u : "unknown";
    info.dimension = dimension_;
    return info;
}
