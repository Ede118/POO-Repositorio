#include "RegistroEventos.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cctype>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <iterator>

// ============================================================================================ //

RegistroEventos::RegistroEventos(File& file)
    : file_(file),
      header_({"Fecha", "ID", "TipoAccion", "Nivel", "Modulo", "Linea", "UserId", "DeviceId", "Mensaje"}) {
    if (file_.isEmpty()) {
        file_.writeHeader(header_);
    }
}

// ============================================================================================ //

void RegistroEventos::logInterno(const std::string& tipo, const std::string& mensaje,
                                 const std::string& modulo, int linea,
                                 const std::string& nivel) {
    append(buildRecord(tipo, mensaje, modulo, linea, nivel, "", ""));
}

// ============================================================================================ //

void RegistroEventos::logExterno(const std::string& tipo, const std::string& mensaje,
                                 const std::string& userId, const std::string& deviceId,
                                 const std::string& modulo, int linea,
                                 const std::string& nivel) {
    if (userId.empty() || deviceId.empty()) {
        throw std::invalid_argument("Los eventos externos requieren userId y deviceId");
    }
    append(buildRecord(tipo, mensaje, modulo, linea, nivel, userId, deviceId));
}

// ============================================================================================ //

std::vector<EventRecord> RegistroEventos::query(const std::string& desdeISO,
                                                const std::string& hastaISO,
                                                const std::vector<std::string>& tipos,
                                                const std::string& userId) const {
    return filter(readAll(), desdeISO, hastaISO, tipos, userId);
}

// ============================================================================================ //

std::string RegistroEventos::exportCSV(const std::string& desdeISO,
                                       const std::string& hastaISO,
                                       const std::vector<std::string>& tipos,
                                       const std::string& userId) const {
    std::ostringstream out;
    for (size_t i = 0; i < header_.size(); ++i) {
        if (i != 0) {
            out << ',';
        }
        out << header_[i];
    }
    const auto registros = query(desdeISO, hastaISO, tipos, userId);
    for (const auto& registro : registros) {
        out << '\n'
            << escapeCsvField(registro.fecha) << ','
            << escapeCsvField(registro.id) << ','
            << escapeCsvField(registro.tipoAccion) << ','
            << escapeCsvField(registro.nivel) << ','
            << escapeCsvField(registro.modulo) << ','
            << escapeCsvField(std::to_string(registro.linea)) << ','
            << escapeCsvField(registro.userId) << ','
            << escapeCsvField(registro.deviceId) << ','
            << escapeCsvField(registro.mensaje);
    }
    return out.str();
}

// ============================================================================================ //

std::string RegistroEventos::exportJSON(const std::string& desdeISO,
                                        const std::string& hastaISO,
                                        const std::vector<std::string>& tipos,
                                        const std::string& userId) const {
    const auto registros = query(desdeISO, hastaISO, tipos, userId);
    std::ostringstream out;
    out << "[";
    for (size_t i = 0; i < registros.size(); ++i) {
        const auto& r = registros[i];
        if (i != 0) {
            out << ",";
        }
        out << "\n  {"
            << "\n    \"fecha\": \"" << escapeJson(r.fecha) << "\","
            << "\n    \"id\": \"" << escapeJson(r.id) << "\","
            << "\n    \"tipoAccion\": \"" << escapeJson(r.tipoAccion) << "\","
            << "\n    \"nivel\": \"" << escapeJson(r.nivel) << "\","
            << "\n    \"modulo\": \"" << escapeJson(r.modulo) << "\","
            << "\n    \"linea\": " << r.linea << ","
            << "\n    \"userId\": \"" << escapeJson(r.userId) << "\","
            << "\n    \"deviceId\": \"" << escapeJson(r.deviceId) << "\","
            << "\n    \"mensaje\": \"" << escapeJson(r.mensaje) << "\""
            << "\n  }";
    }
    if (!registros.empty()) {
        out << '\n';
    }
    out << "]";
    return out.str();
}

// ============================================================================================ //

std::string RegistroEventos::exportXML(const std::string& desdeISO,
                                       const std::string& hastaISO,
                                       const std::vector<std::string>& tipos,
                                       const std::string& userId) const {
    const auto registros = query(desdeISO, hastaISO, tipos, userId);
    std::ostringstream out;
    out << "<events>";
    for (const auto& r : registros) {
        out << "\n  <event>"
            << "\n    <fecha>" << escapeXml(r.fecha) << "</fecha>"
            << "\n    <id>" << escapeXml(r.id) << "</id>"
            << "\n    <tipoAccion>" << escapeXml(r.tipoAccion) << "</tipoAccion>"
            << "\n    <nivel>" << escapeXml(r.nivel) << "</nivel>"
            << "\n    <modulo>" << escapeXml(r.modulo) << "</modulo>"
            << "\n    <linea>" << r.linea << "</linea>"
            << "\n    <userId>" << escapeXml(r.userId) << "</userId>"
            << "\n    <deviceId>" << escapeXml(r.deviceId) << "</deviceId>"
            << "\n    <mensaje>" << escapeXml(r.mensaje) << "</mensaje>"
            << "\n  </event>";
    }
    if (!registros.empty()) {
        out << '\n';
    }
    out << "</events>";
    return out.str();
}

// ============================================================================================ //

EventRecord RegistroEventos::buildRecord(const std::string& tipo, const std::string& mensaje,
                                         const std::string& modulo, int linea,
                                         const std::string& nivel,
                                         const std::string& userId,
                                         const std::string& deviceId) const {
    EventRecord record;
    record.fecha = nowISO8601();
    record.id = generateId();
    record.tipoAccion = tipo;
    record.nivel = nivel;
    record.modulo = modulo;
    record.linea = linea;
    record.userId = userId;
    record.deviceId = deviceId;
    record.mensaje = mensaje;
    return record;
}

// ============================================================================================ //

void RegistroEventos::append(const EventRecord& record) {
    std::vector<std::string> row = {
        record.fecha,
        record.id,
        record.tipoAccion,
        record.nivel,
        record.modulo,
        std::to_string(record.linea),
        record.userId,
        record.deviceId,
        record.mensaje};
    file_.appendRow(row);
}

// ============================================================================================ //

std::vector<EventRecord> RegistroEventos::readAll() const {
    std::vector<EventRecord> registros;
    const auto rows = file_.readAllRows();
    registros.reserve(rows.size());
    for (const auto& row : rows) {
        if (row.size() < header_.size()) {
            continue;
        }
        EventRecord record;
        record.fecha = row[0];
        record.id = row[1];
        record.tipoAccion = row[2];
        record.nivel = row[3];
        record.modulo = row[4];
        try {
            record.linea = std::stoi(row[5]);
        } catch (...) {
            record.linea = 0;
        }
        record.userId = row[6];
        record.deviceId = row[7];
        record.mensaje = row[8];
        registros.push_back(std::move(record));
    }
    return registros;
}

// ============================================================================================ //

std::vector<EventRecord> RegistroEventos::filter(const std::vector<EventRecord>& origen,
                                                 const std::string& desdeISO,
                                                 const std::string& hastaISO,
                                                 const std::vector<std::string>& tipos,
                                                 const std::string& userId) const {
    std::vector<EventRecord> filtrados;
    std::vector<std::string> tiposUpper;
    tiposUpper.reserve(tipos.size());
    std::transform(tipos.begin(), tipos.end(), std::back_inserter(tiposUpper),
                   [](const std::string& s) { return toUpper(s); });
    const bool filtraDesde = !desdeISO.empty();
    const bool filtraHasta = !hastaISO.empty();
    const bool filtraTipos = !tiposUpper.empty();
    const bool filtraUser = !userId.empty();
    const std::string userUpper = toUpper(userId);

    for (const auto& record : origen) {
        if (filtraDesde && record.fecha < desdeISO) {
            continue;
        }
        if (filtraHasta && record.fecha > hastaISO) {
            continue;
        }
        if (filtraTipos) {
            const auto tipoUpper = toUpper(record.tipoAccion);
            if (std::find(tiposUpper.begin(), tiposUpper.end(), tipoUpper) == tiposUpper.end()) {
                continue;
            }
        }
        if (filtraUser) {
            if (toUpper(record.userId) != userUpper) {
                continue;
            }
        }
        filtrados.push_back(record);
    }
    return filtrados;
}

// ============================================================================================ //

std::string RegistroEventos::nowISO8601() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}

// ============================================================================================ //

std::string RegistroEventos::generateId() {
    static std::atomic<uint64_t> counter{0};
    const auto now = std::chrono::system_clock::now();
    const auto micros = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    const auto seq = counter.fetch_add(1, std::memory_order_relaxed);
    std::ostringstream oss;
    oss << micros << '-' << seq;
    return oss.str();
}

// ============================================================================================ //

std::string RegistroEventos::toUpper(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return value;
}

// ============================================================================================ //

std::string RegistroEventos::escapeCsvField(const std::string& field) {
    bool needsQuotes = false;
    std::string out;
    out.reserve(field.size());
    for (char c : field) {
        if (c == '"' || c == ',' || c == '\n' || c == '\r') {
            needsQuotes = true;
        }
        if (c == '"') {
            out.push_back('"');
        }
        out.push_back(c);
    }
    if (!needsQuotes) {
        return out.empty() ? field : out;
    }
    std::ostringstream oss;
    oss << '"' << (out.empty() ? field : out) << '"';
    return oss.str();
}

// ============================================================================================ //

std::string RegistroEventos::escapeJson(const std::string& value) {
    std::ostringstream oss;
    for (char c : value) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '"': oss << "\\\""; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0')
                        << static_cast<int>(static_cast<unsigned char>(c)) << std::dec;
                } else {
                    oss << c;
                }
        }
    }
    return oss.str();
}

// ============================================================================================ //

std::string RegistroEventos::escapeXml(const std::string& value) {
    std::ostringstream oss;
    for (char c : value) {
        switch (c) {
            case '&': oss << "&amp;"; break;
            case '<': oss << "&lt;"; break;
            case '>': oss << "&gt;"; break;
            case '"': oss << "&quot;"; break;
            case '\'': oss << "&apos;"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
}
