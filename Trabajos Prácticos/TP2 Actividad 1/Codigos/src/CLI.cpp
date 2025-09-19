#include "CLI.h"
#include "Types.h"
#include <cstring>
#include <iostream>

void CLI::parse(int argc, char** argv) {
    // Opciones:
    // -m r|w
    // -i c|j|x  (solo write)
    // -o c|j|x  (solo read)
    // -n N      (cantidad de lecturas/pedidos en write; en read se ignora)
    // -s /dev/ttyACM0 (opcional)
    // -d baseDir
    // -f fileName (sin extensión)
    // -a (append CSV)
    for (int i = 1; i < argc; ++i) {
        if (!std::strcmp(argv[i], "-m") && i+1 < argc) {
            char c = argv[++i][0];
            if (c=='r' || c=='R') m_mode = Mode::Read;
            else if (c=='w'||c=='W') m_mode = Mode::Write;
            else throw AppError("Valor para -m inválido (use r|w).");
        } else if (!std::strcmp(argv[i], "-i") && i+1 < argc) {
            m_inFmt = charToFormat(argv[++i][0]);
        } else if (!std::strcmp(argv[i], "-o") && i+1 < argc) {
            m_outFmt = charToFormat(argv[++i][0]);
        } else if (!std::strcmp(argv[i], "-n") && i+1 < argc) {
            m_readCount = std::stoi(argv[++i]);
            if (m_readCount < 0) throw AppError("El parámetro -n debe ser no negativo.");
        } else if (!std::strcmp(argv[i], "-s") && i+1 < argc) {
            m_useSerial = true;
            m_serialDev = argv[++i];
        } else if (!std::strcmp(argv[i], "-d") && i+1 < argc) {
            m_baseDir = argv[++i];
        } else if (!std::strcmp(argv[i], "-f") && i+1 < argc) {
            m_fileName = argv[++i];
        } else if (!std::strcmp(argv[i], "-a")) {
            m_append = true;
        } else if (!std::strcmp(argv[i], "-h") || !std::strcmp(argv[i], "--help")) {
            std::cout <<
            "Uso:\n"
            "  -m r|w           Modo lectura/escritura\n"
            "  -i c|j|x         Formato de entrada (write)\n"
            "  -o c|j|x         Formato de salida (read)\n"
            "  -n N             Cantidad de pedidos/lecturas (write)\n"
            "  -s DEV           Dispositivo serie (opcional)\n"
            "  -d DIR           Directorio base (default '.')\n"
            "  -f NAME          Nombre base de archivo (sin extension)\n"
            "  -a               Append al CSV existente\n";
            std::exit(0);
        } else {
            throw AppError(std::string("Opción no reconocida: ") + argv[i]);
        }
    }
    if (m_mode == Mode::Write && m_readCount == 0) {
        // cero => leer hasta EOF; >0 => lecturas fijas
        // lo dejamos permitido, solo avisamos si es 0 y no hay EOF nunca...
    }
}
