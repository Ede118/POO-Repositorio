#ifndef CLI_H
#define CLI_H

#include "Types.h"
#include <string>

class CLI {
    Mode        m_mode = Mode::Read;
    IOFormat    m_inFmt  = IOFormat::CSV;   // para -m w
    IOFormat    m_outFmt = IOFormat::CSV;   // para -m r
    bool        m_useSerial = false;
    std::string m_serialDev;
    std::string m_baseDir  = ".";
    std::string m_fileName = "datos";
    bool        m_append   = false;
    int         m_readCount = 0;

public:
    void parse(int argc, char** argv);

    Mode       mode()       const { return m_mode; }
    IOFormat   inputFmt()   const { return m_inFmt; }
    IOFormat   outputFmt()  const { return m_outFmt; }
    bool       useSerial()  const { return m_useSerial; }
    std::string serialDev() const { return m_serialDev; }
    std::string baseDir()   const { return m_baseDir; }
    std::string fileName()  const { return m_fileName; }
    bool       append()     const { return m_append; }
    int        readCount()  const { return m_readCount; }
};

#endif
