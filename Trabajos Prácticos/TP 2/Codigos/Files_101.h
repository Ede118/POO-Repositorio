
#ifndef FILES_101_H
#define FILES_101_H

#include <string>
#include <vector>
#include <fstream>
#include <optional>

// Info para modo lectura
struct FileInfo {
    std::string name;       // nombre del archivo (sin ruta)
    std::string datetime;   // timestamp ISO
    std::string owner;      // $USER
    std::size_t dimension;  // filas de datos (excluye cabecera)
};

class Files_101 {
public:
    explicit Files_101(const std::string& base);  // "x", "x.csv" o "carpeta/x(.csv)"
    ~Files_101() = default;

    // Abrir/cerrar: 'w' (trunc), 'A' (append), 'r' (read)
    void open(char mode);
    void close();
    bool exist() const;

    // Escritura
    void write(const std::string& line);
    void writeParsed(const std::vector<std::string>& fields);

    // Lectura “de una”
    std::vector<std::string> readAllLines() const;

    // Lectura incremental con fs_ en 'r'
    std::optional<std::string> getLine();

    // Salidas para -m r (leen desde disco, NO usan fs_)
    std::string getCSV();
    std::string getJSON();
    std::string getXML();

    // Metadata
    FileInfo getInfo() const;
    std::size_t getDimension() const { return dimension_; }
    std::string getPath() const { return path_; }

private:
    void ensureOpenForRead();
    std::size_t recount();  // cuenta filas reales (excluye cabecera) al abrir 'r'

    // Helpers CSV y strings
    static std::vector<std::string> splitCSV_(const std::string& line);
    static inline void chop_cr_(std::string& s){
        if (!s.empty() && s.back()=='\r') s.pop_back();
    }
    static inline bool looks_number_(const std::string& v){
        if (v.empty()) return false;
        for(char c: v){
            if (!(std::isdigit((unsigned char)c) || c=='.' || c=='-' )) return false;
        }
        return true;
    }
    static std::string now_iso();
    static std::string normalize_path_arg(const std::string& base); // agrega .csv si falta

private:
    std::string  path_;
    std::fstream fs_;
    char         mode_ {'?'};
    std::size_t  dimension_ {0};
};

#endif