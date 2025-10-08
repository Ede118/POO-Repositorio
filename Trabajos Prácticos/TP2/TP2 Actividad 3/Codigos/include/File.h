#pragma once

#include <filesystem>
#include <string>
#include <vector>

class File {
public:

// ============================================================================================ //

    // Constructor
    explicit File(std::filesystem::path path);

// ============================================================================================ //

    // Métodos para manipular el archivo
    bool isEmpty() const;
    void writeHeader(const std::vector<std::string>& columns);
    void appendRow(const std::vector<std::string>& columns);
    std::vector<std::vector<std::string>> readAllRows() const;

// ============================================================================================ //
    
private:

    // Path del archivo y 
    std::filesystem::path path_;

    // Métodos auxiliares
    void ensureParentDir() const;
    static std::string escapeCsv(const std::string& field);
    static std::vector<std::string> parseCsvLine(const std::string& line);
};

