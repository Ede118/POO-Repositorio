#ifndef FILE_H
#define FILE_H

#include <string>
#include <vector>

class File {
public:
    explicit File(std::string path);

    // Aperturas y chequeos básicos
    bool exists() const;
    bool hasHeader() const;                  // true si primera línea parece cabecera
    void ensureHeader(const std::vector<std::string>& header);

    // Escritura y lectura en CSV (persistencia SIEMPRE CSV)
    void appendCSVLine(const std::vector<std::string>& cols);   // agrega fila
    std::vector<std::string> readAllLines() const;              // sin la cabecera si querés, después vemos
    std::vector<std::string> readAllLinesWithHeader() const;    // incluye cabecera (posición 0)

    // Helpers CSV <-> vector
    static std::string joinCSV(const std::vector<std::string>& cols);
    static std::vector<std::string> splitCSV(const std::string& line);

    // Conversión: toma todas las filas y emite en formato pedido
    static std::string toCSV(const std::vector<std::string>& lines);
    static std::string toJSON(const std::vector<std::string>& lines); // lines[0] = header
    static std::string toXML (const std::vector<std::string>& lines); // lines[0] = header

private:
    std::string path_;
};

#endif // FILE_H
