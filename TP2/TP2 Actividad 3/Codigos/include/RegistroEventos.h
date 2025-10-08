#pragma once

#include "File.h"

#include <string>
#include <vector>

struct EventRecord {
    std::string fecha;
    std::string id;
    std::string tipoAccion;
    std::string nivel;
    std::string modulo;
    int linea = 0;
    std::string userId;
    std::string deviceId;
    std::string mensaje;
};

class RegistroEventos {
public:
    // Constructor
    explicit RegistroEventos(File& file);

// ============================================================================================ //
    
    // Métodos para registrar evento
    void logInterno(const std::string& tipo, const std::string& mensaje,
                    const std::string& modulo, int linea,
                    const std::string& nivel = "INFO");

    void logExterno(const std::string& tipo, const std::string& mensaje,
                    const std::string& userId, const std::string& deviceId,
                    const std::string& modulo, int linea,
                    const std::string& nivel = "INFO");

    std::vector<EventRecord> query(const std::string& desdeISO = "",
                                   const std::string& hastaISO = "",
                                   const std::vector<std::string>& tipos = {},
                                   const std::string& userId = "") const;

// ============================================================================================ //

    // Métodos para exportar registros
    std::string exportCSV(const std::string& desdeISO = "",
                          const std::string& hastaISO = "",
                          const std::vector<std::string>& tipos = {},
                          const std::string& userId = "") const;

    std::string exportJSON(const std::string& desdeISO = "",
                           const std::string& hastaISO = "",
                           const std::vector<std::string>& tipos = {},
                           const std::string& userId = "") const;

    std::string exportXML(const std::string& desdeISO = "",
                          const std::string& hastaISO = "",
                          const std::vector<std::string>& tipos = {},
                          const std::string& userId = "") const;

// ============================================================================================ //

private:
    File& file_;
    std::vector<std::string> header_;

    EventRecord buildRecord(const std::string& tipo, const std::string& mensaje,
                            const std::string& modulo, int linea,
                            const std::string& nivel,
                            const std::string& userId,
                            const std::string& deviceId) const;

    void append(const EventRecord& record);
    std::vector<EventRecord> readAll() const;
    std::vector<EventRecord> filter(const std::vector<EventRecord>& origen,
                                    const std::string& desdeISO,
                                    const std::string& hastaISO,
                                    const std::vector<std::string>& tipos,
                                    const std::string& userId) const;

    static std::string nowISO8601();
    static std::string generateId();
    static std::string toUpper(std::string value);
    static std::string escapeCsvField(const std::string& field);
    static std::string escapeJson(const std::string& value);
    static std::string escapeXml(const std::string& value);
};

