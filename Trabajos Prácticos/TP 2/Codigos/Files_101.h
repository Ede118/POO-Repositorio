
#ifndef FILES_101_H
#define FILES_101_H

#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include <fstream>

/*
  No se llama el name space std
  Para usar 'string', 'vector', 'optional', 'ifstream', etc
  se debe anteponer 'std::'
  Ejemplo: std::string, std::vector, std::optional, std::ifstream
*/

struct FileInfo {
  std::string name;
  std::string datetime;
  std::string owner;
  int dimension;
};

/**
  * class Files_101
  * 
  */

class Files_101 {
  public:
    // Constructors
    Files_101();
    Files_101(const std::string& name, const std::string& datetime, const std::string& owner); // completo
    Files_101(const std::string& name, const std::string& datetime);
    Files_101(const std::string& name); // mínimo

    // Destructor
    virtual ~Files_101();
    
    //------- Métodos ------- //
    
    // Apertura/cierre
    void open(char mode); // 'r' lectura, 'w' escritura (trunca)
    void close();

    // Consultas/atributos
    bool exist() const;
    std::string getNombre() const;
    std::string getCreationDate() const;
    std::string getOwner() const;
    int getDimension() const;
    FileInfo getInfo() const;

    // Lectura
    std::optional<std::string> getLine(); // lectura secuencial
    std::string getCSV() const;
    std::string getJSON() const;
    std::string getXML() const;

    // Escritura
    void write(const std::string& rawLine);                       // escribe tal cual
    void writeParsed(const std::vector<std::string>& fields);     // escribe vector -> CSV

    // Fijos por código (consigna)
    static constexpr const char* kBaseDir = "./data";
    static constexpr const char* kExt = ".csv";

  private:
    // ---- Propiedades ---- //
    // (nombre, creationdate, owner, dimension, path, fstream)
    std::string name_;
    std::string CreationDate_;
    std::string Owner_;
    int Dimension_ = 0;

    /* 
    Se usa filesystem para manejar paths
    Paths es una clase que maneja rutas de archivos
    */
    std::filesystem::path path_;
    mutable std::fstream fs_;
    
    // ---- Métodos privados ---- //
    static std::string nowISO8601();
    static std::string defaultOwner();
    static std::string sanitize(const std::string& s);

    void ensureOpenForRead() const;
    void ensureOpenForWrite();
    std::vector<std::string> readAllLines() const;

    static std::string joinCSV(const std::vector<std::string>& fields);
    static std::vector<std::string> splitCSV(const std::string& line);

    static std::string toJSON(const std::vector<std::string>& headers,
                              const std::vector<std::vector<std::string>>& rows);
    static std::string toXML(const std::vector<std::string>& headers,
                             const std::vector<std::vector<std::string>>& rows);
};

#endif // FILES_101_H
