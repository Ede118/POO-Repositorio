#ifndef APP_H
#define APP_H

#include "CLI.h"
#include <vector>
#include <string>

class App {
public:
    int run(int argc, char** argv);  // orquesta todo
    requestLineFromSerial(const std::string& dev, char reqByte);
private:
    static char fmtByte(InputFormat f);          // c|j|x para el µC

    static std::vector<std::string> parseLine(InputFormat fmt, const std::string& line);
    #ifdef __linux__
        // Abre y configura la TTY. Devuelve fd listo para usar.
        static int  openSerialRaw8N1(const std::string& dev, int baud);
        // Escribe un byte. Devuelve true si se envió.
        static bool writeByte(int fd, char b);
        // Lee una línea con timeout (ms). Devuelve "" en timeout.
        static std::string readLine(int fd, int timeout_ms);
    #endif
};

#endif // APP_H
