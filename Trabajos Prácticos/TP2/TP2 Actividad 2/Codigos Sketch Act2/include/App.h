#pragma once

#include "File.h"
#include "NodoID.h"
#include "Types.h"
#include "Verificador.h"

#include <istream>
#include <string>
#include <vector>

class App {
public:
    App(File& file, Verificador& verificador);

    void processWrite(std::istream& input, bool appendMode);

    const std::vector<std::string>& accepted() const noexcept;
    const std::vector<std::string>& rejected() const noexcept;

private:
    File& file_;
    Verificador& verificador_;
    std::vector<std::string> accepted_;
    std::vector<std::string> rejected_;

    static std::vector<std::string> splitCSV(const std::string& line);
    static std::string trim(const std::string& s);
};
