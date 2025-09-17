#ifndef FILE_H
#define FILE_H

#include "Types.h"
#include <string>
#include <vector>
#include <fstream>

struct FileInfo {
    std::string name;
    std::string creationDateTime;
    std::string owner;
    size_t      dimension = 0; // líneas de datos
};

class File {
    bool      m_read  = false;
    bool      m_write = false;
    FileInfo  m_info;
    std::fstream m_fs;

public:
    File() = default;
    explicit File(const std::string& path);

    bool open(char mode); // 'r' o 'w' o 'a'
    void close();

    const FileInfo& info() const { return m_info; }
    std::string     getInfoTable() const;

    // Escritura
    void writeRaw(const std::string& line);                 // escribe tal cual (CSV ya formateado)
    void writeParsed(const std::vector<std::string>& fields); // une por coma

    // Lectura
    bool   getLine(std::string& out);
    std::string readAll(); // contenido CSV completo (sin cabecera especial)
    std::vector<std::vector<std::string>> readAllRows();

    // Presentación (a partir del CSV)
    std::string toCSV();                                   // igual al archivo
    std::string toJSON(const std::vector<std::string>& headers);
    std::string toXML (const std::vector<std::string>& headers);

    // Utilidad
    static std::vector<std::string> splitCSV(const std::string& line);
    void writeHeaderTag(const std::vector<std::string>& headers);
};

#endif