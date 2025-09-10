#include "Files_101.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <stdexcept>

/*
  chrono: para manejo de fechas/horas
  iomanip: para std::put_time
  sstream: para std::stringstream
  cstdlib: para std::getenv
  stdexcept: para std::runtime_error
*/

using namespace std;

/*
static string pathJoin(const string& a, const string& b){
    return (std::filesystem::path(a) / b).string();
}
*/

string Files_101::sanitize(const string& s){
    // minimal: quitar espacios de los extremos
    size_t i = s.find_first_not_of(" \t\r\n");
    size_t j = s.find_last_not_of(" \t\r\n");
    if(i==string::npos) return "";
    return s.substr(i, j-i+1);
}

string Files_101::nowISO8601(){
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);
    tm tmv{};
#ifdef _WIN32
    localtime_s(&tmv, &t);
#else
    localtime_r(&t, &tmv);
#endif
    stringstream ss; ss<<put_time(&tmv, "%Y-%m-%dT%H:%M:%S");
    return ss.str();
}

string Files_101::defaultOwner(){
    const char* u = std::getenv("USER");
#ifdef _WIN32
    if(!u) u = std::getenv("USERNAME");
#endif
    return u? string(u) : "unknown";
}

// %%%%%%%%%%%%%%%%%%%% //
//    Constructores     //
// %%%%%%%%%%%%%%%%%%%% //

Files_101::Files_101()
: name_("noname"), CreationDate_(nowISO8601()), Owner_(defaultOwner()) {
    std::filesystem::create_directories(kBaseDir);
    path_ = std::filesystem::path(kBaseDir) / (name_ + kExt);
}

Files_101::Files_101(const string& name, const string& datetime, const string& owner)
: name_(sanitize(name)), CreationDate_(sanitize(datetime)), Owner_(sanitize(owner)) {
    std::filesystem::create_directories(kBaseDir);
    path_ = std::filesystem::path(kBaseDir) / (name_ + kExt);
}

Files_101::Files_101(const string& name, const string& datetime)
: name_(sanitize(name)), CreationDate_(sanitize(datetime)), Owner_(defaultOwner()) {
    std::filesystem::create_directories(kBaseDir);
    path_ = std::filesystem::path(kBaseDir) / (name_ + kExt);
}

Files_101::Files_101(const std::string& name)
: name_(sanitize(name)), CreationDate_(nowISO8601()), Owner_(defaultOwner()) {
    std::filesystem::create_directories(kBaseDir);
    path_ = std::filesystem::path(kBaseDir) / (name_ + kExt);
}

// %%%%%%%%%%%%%%%%%%%% //
// ---- Destructor ---- //
// %%%%%%%%%%%%%%%%%%%% //

Files_101::~Files_101() = default;


// %%%%%%%%%%%%%%%%%%%% //
//   Apertura/Cierre   //
// %%%%%%%%%%%%%%%%%%%% //

void Files_101::open(char mode){
    ios::openmode m;
    if(mode=='r'){
        if(!exist()) throw runtime_error("Archivo no existe: " + path_.string());
        m = ios::in;
    } else if(mode=='w'){
        m = ios::out | ios::trunc;
    } else if(mode=='A'){
        m = std::ios::out | std::ios::app;
    } else {
<<<<<<< HEAD
        throw runtime_error("Modo inválido (use 'r','w' o 'a').");
=======
        throw runtime_error("Modo inválido (use 'r','w' o 'A').");
>>>>>>> 46247936515697c259e5ef01fc329b8d40adbab0
    }

    fs_.open(path_, m);
    if(!fs_) throw runtime_error("No se pudo abrir: " + path_.string());

    // setear flags correctos
    is_read_mode_  = (mode=='r');
    is_write_mode_ = (mode=='w' || mode=='A');

    if(mode=='r' || mode=='A'){
        auto lines = readAllLines();
        Dimension_ = (int)lines.size();
    } else {
        Dimension_ = 0;
    }
}

void Files_101::close(){
    if(fs_.is_open()) fs_.close();
    is_read_mode_ = is_write_mode_ = false;  // importante
}



// %%%%%%%%%%%%%%%%%%%% //
//        Métodos       //
// %%%%%%%%%%%%%%%%%%%% //

bool Files_101::exist() const { return std::filesystem::exists(path_); }
string Files_101::getNombre() const { return name_ + kExt; }
string Files_101::getCreationDate() const { return CreationDate_; }
string Files_101::getOwner() const { return Owner_; }
int Files_101::getDimension() const { return Dimension_; }
FileInfo Files_101::getInfo() const { return { getNombre(), getCreationDate(), getOwner(), getDimension() }; }

// ______ Lectura ______ //

void Files_101::ensureOpenForRead() const {
    if(!fs_.is_open() || !is_read_mode_) {
        throw runtime_error("Archivo no abierto para lectura.");
    }
}

void Files_101::ensureOpenForWrite(){
    if(!fs_.is_open() || !is_write_mode_) {
        throw runtime_error("Archivo no abierto para escritura.");
    }
}

vector<string> Files_101::readAllLines() const{
    ifstream in(path_);
    vector<string> v; string s;
    while(getline(in, s)){
        if(!s.empty()) v.push_back(s);
    }
    return v;
}

optional<string> Files_101::getLine(){
    ensureOpenForRead();
    string s;
    if(std::getline(fs_, s)) return s;
    return std::nullopt;
}

// _________ Escritura _________ //

string Files_101::joinCSV(const vector<string>& f){
    ostringstream os;
    for(size_t i=0;i<f.size();++i){
        if(i) os<<",";
        // escape mínimo: si hay coma o comillas, envolver
        bool needQuote = (f[i].find(',')!=string::npos) || (f[i].find('"')!=string::npos);
        if(needQuote){
            os<<'"';
            for(char c: f[i]) os << (c=='"'? "\"\"" : string(1,c));
            os<<'"';
        } else {
            os<<f[i];
        }
    }
    return os.str();
}

void Files_101::write(const string& raw){
    ensureOpenForWrite();
    fs_ << raw << "\n";
    ++Dimension_;
}

void Files_101::writeParsed(const vector<string>& fields){
    write(joinCSV(fields));
}

// _________ Formatos de salida _________ //

vector<string> Files_101::splitCSV(const string& line){
    // split simple que respeta comillas dobles básicas
    vector<string> out;
    string cur; bool inq=false;
    for(size_t i=0;i<line.size();++i){
        char ch=line[i];
        if(ch=='"'){
            inq = !inq;
        } else if(ch==',' && !inq){
            out.push_back(cur); cur.clear();
        } else {
            cur.push_back(ch);
        }
    }
    out.push_back(cur);
    return out;
}

string Files_101::toJSON(const vector<string>& headers,
                         const vector<vector<string>>& rows){
    ostringstream os;
    os<<"[\n";
    for(size_t r=0;r<rows.size();++r){
        os<<"  {";
        for(size_t c=0;c<headers.size() && c<rows[r].size();++c){
            if(c) os<<", ";
            os<<"\""<<headers[c]<<"\": \""<<rows[r][c]<<"\"";
        }
        os<<"}"<<(r+1<rows.size()? ",":"")<<"\n";
    }
    os<<"]\n";
    return os.str();
}

string Files_101::toXML(const vector<string>& headers,
                        const vector<vector<string>>& rows){
    ostringstream os;
    os<<"<root>\n";
    for(const auto& row: rows){
        os<<"  <row>";
        for(size_t c=0;c<headers.size() && c<row.size();++c){
            os<<"<"<<headers[c]<<">"<<row[c]<<"</"<<headers[c]<<">";
        }
        os<<"</row>\n";
    }
    os<<"</root>\n";
    return os.str();
}

string Files_101::getCSV() const{
    auto lines = readAllLines();
    ostringstream os;
    for(auto& s: lines) os<<s<<"\n";
    return os.str();
}

string Files_101::getJSON() const{
    auto lines = readAllLines();
    if(lines.empty()) return "[]\n";
    auto headers = splitCSV(lines.front());
    vector<vector<string>> rows;
    for(size_t i=1;i<lines.size();++i) rows.push_back(splitCSV(lines[i]));
    return toJSON(headers, rows);
}

string Files_101::getXML() const{
    auto lines = readAllLines();
    if(lines.empty()) return "<root/>\n";
    auto headers = splitCSV(lines.front());
    vector<vector<string>> rows;
    for(size_t i=1;i<lines.size();++i) rows.push_back(splitCSV(lines[i]));
    return toXML(headers, rows);
}
