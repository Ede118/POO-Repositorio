#ifndef CLI_H
#define CLI_H

#include "Types.h"
#include <string>

class CLI {
public:
    // parsea argv y completa los campos
    void parse(int argc, char** argv);

    // getters con nombres del diagrama
    Mode mode() const { return mode_; }
    InputFormat inFormat() const { return inFormat_; }
    OutputFormat outFormat() const { return outFormat_; }
    std::string fileName() const { return fileName_; }
    int readCount() const { return readCount_; }
    bool append() const { return append_; }
    bool useSerial() const { return useSerial_; }
    std::string serialDev() const { return serialDev_; }
    std::string baseDir() const { return baseDir_; }
    int baud() const { return baud_; }

private:
    Mode mode_ = Mode::Write;
    InputFormat inFormat_ = InputFormat::CSV;
    OutputFormat outFormat_ = OutputFormat::CSV;
    bool useSerial_ = false;
    std::string serialDev_;
    int baud_ = 9600;
    std::string baseDir_ = ".";         // opcional, por si querés prefijo
    std::string fileName_ = "sensores.csv";
    int readCount_ = 1;
    bool append_ = false;
};

#endif // CLI_H
