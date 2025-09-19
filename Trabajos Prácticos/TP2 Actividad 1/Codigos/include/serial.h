#ifndef SERIAL_H
#define SERIAL_H

#include <string>
#include <termios.h>

class SerialPort {
    int      fd_ = -1;
    termios  old_{};
public:
    // baud: B9600, B115200, etc.
    SerialPort(const std::string& dev, int baud);
    ~SerialPort();

    void writeByte(char c);
    void writeData(const void* data, size_t n);

    // Lee hasta '\n' o '\r' con timeout en ms. Devuelve true si obtuvo una línea.
    bool readLine(std::string& out, int timeout_ms);

    // >>> NUEVO: limpia el buffer de entrada (evita “o>”, restos, etc.)
    void flushIn();

    // no copiable
    SerialPort(const SerialPort&) = delete;
    SerialPort& operator=(const SerialPort&) = delete;
};


#endif