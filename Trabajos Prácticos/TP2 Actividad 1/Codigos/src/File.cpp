#include "File.h"
#include "Types.h"
#include <ctime>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <pwd.h>
#include <filesystem>
#include <system_error>
#include <algorithm>



// ========================= Paseo ========================= //

bool tryParseHeaderTag(const std::string& line, std::vector<std::string>& headers) {
    // Convención opcional: si el µC manda "#h: a,b,c" esto define encabezados.
    if (line.rfind("#h:", 0) == 0) {
        std::string rest = line.substr(3);
        std::stringstream ss(rest);
        std::string item;
        headers.clear();
        while (std::getline(ss, item, ',')) {
            // trim básico
            item.erase(item.begin(), std::find_if(item.begin(), item.end(), [](unsigned char ch){return !std::isspace(ch);} ));
            item.erase(std::find_if(item.rbegin(), item.rend(), [](unsigned char ch){return !std::isspace(ch);} ).base(), item.end());
            headers.push_back(item);
        }
        return true;
    }
    return false;
}

static std::vector<std::string> splitCSVsimple(const std::string& line) {
    // versión simplificada para entradas del µC (sin comillas escapadas)
    std::vector<std::string> out;
    std::stringstream ss(line); std::string token;
    while (std::getline(ss, token, ',')) out.push_back(token);
    return out;
}

std::vector<std::string> parseLine(IOFormat inFmt, const std::string& line,
                                   std::vector<std::string>* headersOpt) {
    if (inFmt == IOFormat::CSV) {
        return splitCSVsimple(line);
    } else if (inFmt == IOFormat::JSON) {
        // Parseo mínimo: {"a":1,"b":2} => ["1","2"] respetando el orden de headersOpt si existe
        std::vector<std::string> values;
        if (headersOpt && !headersOpt->empty()) {
            for (auto& k : *headersOpt) {
                auto pos = line.find('\"' + k + '\"');
                if (pos == std::string::npos) { values.push_back(""); continue; }
                pos = line.find(':', pos);
                if (pos == std::string::npos) { values.push_back(""); continue; }
                auto end = line.find_first_of(",}", pos+1);
                std::string v = line.substr(pos+1, end-pos-1);
                // limpio comillas y espacios
                v.erase(remove_if(v.begin(), v.end(), [](unsigned char c){return c=='\"' || std::isspace(c);}), v.end());
                values.push_back(v);
            }
            return values;
        } else {
            // sin headers: sacamos valores en orden de aparición
            std::vector<std::string> tmp;
            size_t pos = 0;
            while ((pos = line.find(':', pos)) != std::string::npos) {
                auto end = line.find_first_of(",}", pos+1);
                std::string v = line.substr(pos+1, end-pos-1);
                v.erase(remove_if(v.begin(), v.end(), [](unsigned char c){return c=='\"' || std::isspace(c);}), v.end());
                tmp.push_back(v);
                pos = end;
            }
            return tmp;
        }
    } else {
        // XML: <row><a>1</a><b>2</b></row>
        std::vector<std::string> values;
        if (headersOpt && !headersOpt->empty()) {
            for (auto& k : *headersOpt) {
                std::string open = "<" + k + ">";
                std::string close = "</" + k + ">";
                auto p1 = line.find(open);
                auto p2 = line.find(close);
                if (p1 == std::string::npos || p2 == std::string::npos || p2 < p1) { values.push_back(""); continue; }
                values.push_back(line.substr(p1+open.size(), p2-(p1+open.size())));
            }
            return values;
        } else {
            // sin headers: extraemos todo lo que esté entre <>
            size_t pos = 0;
            while (true) {
                auto p1 = line.find('<', pos);
                if (p1 == std::string::npos) break;
                if (line[p1+1] == '/') { pos = p1+1; continue; }
                auto p2 = line.find('>', p1+1);
                auto p3 = line.find("</", p2+1);
                if (p2==std::string::npos || p3==std::string::npos) break;
                values.push_back(line.substr(p2+1, p3-(p2+1)));
                pos = p3+2;
            }
            return values;
        }
    }
}




// ========================= File ========================= //

static std::string formatTime(std::time_t t) {
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream os;
    os << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return os.str();
}

static std::string nowStr() {
    return formatTime(std::time(nullptr));
}

static std::string ownerFromUid(uid_t uid) {
    if (passwd* pw = getpwuid(uid)) return pw->pw_name ? pw->pw_name : "unknown";
    return "unknown";
}

static std::string fileOwner(const std::string& path) {
    struct stat st{};
    if (::stat(path.c_str(), &st) == 0) return ownerFromUid(st.st_uid);
    return ownerFromUid(getuid());
}

static std::string fileTimestamp(const std::string& path) {
    struct stat st{};
    if (::stat(path.c_str(), &st) == 0) return formatTime(st.st_mtime);
    return nowStr();
}

File::File(const std::string& path) {
    m_info.name = path;
}

bool File::open(char mode) {
    if (m_info.name.empty()) throw AppError("Ruta de archivo vacía.");

    std::ios::openmode m{};
    if (mode == 'r') {
        m = std::ios::in;
        m_read = true;
        m_write = false;
    } else if (mode == 'w') {
        m = std::ios::out | std::ios::trunc;
        m_read = false;
        m_write = true;
    } else if (mode == 'a') {
        m = std::ios::out | std::ios::app;
        m_read = false;
        m_write = true;
    } else {
        throw AppError("Modo de apertura inválido (r|w|a).");
    }

    const bool existedBefore = std::filesystem::exists(m_info.name);

    m_fs.open(m_info.name, m);
    if (!m_fs) throw AppError("No se pudo abrir: " + m_info.name);

    if (mode == 'r') {
        m_info.owner = fileOwner(m_info.name);
        m_info.creationDateTime = fileTimestamp(m_info.name);
        m_info.dimension = countRowsOnDisk();
    } else if (mode == 'a') {
        if (existedBefore) {
            m_info.owner = fileOwner(m_info.name);
            m_info.creationDateTime = fileTimestamp(m_info.name);
            m_info.dimension = countRowsOnDisk();
        } else {
            m_info.owner = ownerFromUid(getuid());
            m_info.creationDateTime = nowStr();
            m_info.dimension = 0;
        }
    } else { // 'w'
        m_info.owner = ownerFromUid(getuid());
        m_info.creationDateTime = nowStr();
        m_info.dimension = 0;
    }

    m_rows = m_info.dimension;
    return true;
}

void File::close() {
    if (m_fs.is_open()) m_fs.close();
    m_read = false;
    m_write = false;
}

std::string File::getInfoTable() const {
    std::ostringstream os;
    os << "Nombre       : " << m_info.name << "\n"
       << "Creación     : " << m_info.creationDateTime << "\n"
       << "Owner        : " << m_info.owner << "\n"
       << "Dimensión    : " << m_info.dimension << " lineas\n";
    return os.str();
}

bool File::exist() const {
    if (m_info.name.empty()) return false;
    std::error_code ec;
    return std::filesystem::exists(m_info.name, ec);
}

std::string File::getNombre() const { return m_info.name; }
std::string File::getFecha() const { return m_info.creationDateTime; }
std::string File::getPropietario() const { return m_info.owner; }
size_t File::getDimension() const { return m_info.dimension; }
std::string File::getInfo() const { return getInfoTable(); }

void File::writeRaw(const std::string& line) {
    if (!m_write) throw AppError("Archivo no abierto para escritura.");
    m_fs << line << "\n";
    if (!m_fs) throw AppError("Fallo escribiendo en archivo.");
    ++m_info.dimension;
}

void File::writeParsed(const std::vector<std::string>& fields) {
    std::ostringstream os;
    for (size_t i=0;i<fields.size();++i) {
        if (i) os << ',';
        // escapado mínimo
        bool needQuote = fields[i].find_first_of(",\"\n") != std::string::npos;
        if (needQuote) {
            os << '"';
            for (char c: fields[i]) {
                if (c=='"') os << "\"\"";
                else os << c;
            }
            os << '"';
        } else os << fields[i];
    }
    writeRaw(os.str());
}

bool File::getLine(std::string& out) {
    if (!m_read) throw AppError("Archivo no abierto para lectura.");
    return static_cast<bool>(std::getline(m_fs, out));
}

std::string File::readAll() {
    if (!m_read) throw AppError("Archivo no abierto para lectura.");
    std::ostringstream os;
    m_fs.clear(); m_fs.seekg(0);
    os << m_fs.rdbuf();
    return os.str();
}

std::vector<std::vector<std::string>> File::readAllRows() {
    if (!m_read) throw AppError("Archivo no abierto para lectura.");
    m_fs.clear(); m_fs.seekg(0);
    std::vector<std::vector<std::string>> rows;
    std::string line;
    while (std::getline(m_fs, line)) {
        if (line.empty()) continue;
        if (line[0] == '#') continue; // <— ignora cabeceras/comentarios
        rows.push_back(splitCSV(line));
    }
    m_info.dimension = rows.size();
    return rows;
}

std::string File::toCSV() { return readAll(); }

std::string File::toJSON(const std::vector<std::string>& headers) {
    auto rows = readAllRows();
    std::ostringstream os;
    os << "[\n";
    for (size_t r=0;r<rows.size();++r) {
        os << "  {";
        for (size_t c=0;c<rows[r].size() && c<headers.size();++c) {
            if (c) os << ", ";
            os << '"' << headers[c] << "\": \"" << rows[r][c] << '"';
        }
        os << "}";
        if (r+1<rows.size()) os << ",";
        os << "\n";
    }
    os << "]\n";
    return os.str();
}

std::string File::toXML(const std::vector<std::string>& headers) {
    auto rows = readAllRows();
    std::ostringstream os;
    os << "<rows>\n";
    for (const auto& row : rows) {
        os << "  <row>";
        for (size_t c=0;c<row.size() && c<headers.size();++c) {
            os << "<" << headers[c] << ">" << row[c] << "</" << headers[c] << ">";
        }
        os << "</row>\n";
    }
    os << "</rows>\n";
    return os.str();
}

std::vector<std::string> File::splitCSV(const std::string& line) {
    std::vector<std::string> out;
    std::string cur; bool inq=false;
    for (size_t i=0;i<line.size();++i) {
        char ch = line[i];
        if (inq) {
            if (ch=='"' && i+1<line.size() && line[i+1]=='"') { cur.push_back('"'); ++i; }
            else if (ch=='"') inq=false;
            else cur.push_back(ch);
        } else {
            if (ch==',') { out.push_back(cur); cur.clear(); }
            else if (ch=='"') inq=true;
            else cur.push_back(ch);
        }
    }
    out.push_back(cur);
    return out;
}

void File::writeHeaderTag(const std::vector<std::string>& headers) {
    if (!m_write) throw AppError("Archivo no abierto para escritura.");
    m_fs << "#h: ";
    for (size_t i=0;i<headers.size();++i) {
        if (i) m_fs << ',';
        m_fs << headers[i];
    }
    m_fs << "\n";
    // NO incrementamos dimension; es metadata
}

size_t File::countRowsOnDisk() const {
    std::ifstream is(m_info.name);
    if (!is) return 0;
    size_t count = 0;
    std::string line;
    while (std::getline(is, line)) {
        if (line.empty()) continue;
        if (!line.empty() && line[0] == '#') continue; // ignora cabeceras/comentarios
        ++count;
    }
    return count;
}
