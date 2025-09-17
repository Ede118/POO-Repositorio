#ifndef SERIAL_H
#define SERIAL_H

#include <string>
#include <termios.h>
#include <unistd.h>
#include <vector>

class SerialPort {
    int         fd_ = -1;
    termios     old_{};
public:
    SerialPort(const std::string& dev, int baud);
    ~SerialPort();

    void writeByte(char c);
    void writeData(const void* data, size_t n);

    // Lee hasta '\n' o '\r' con timeout en ms. Devuelve true si obtuvo una línea.
    bool readLine(std::string& out, int timeout_ms);

    // no copiable
    SerialPort(const SerialPort&) = delete;
    SerialPort& operator=(const SerialPort&) = delete;
};

#endif