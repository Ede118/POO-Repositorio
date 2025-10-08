#include "RegistroEventos.h"

#include <exception>
#include <filesystem>
#include <iostream>

int main() {
    try {
        File archivoEventos(std::filesystem::path("eventos.csv"));
        RegistroEventos registro(archivoEventos);

        registro.logInterno("ARRANQUE", "Inicializacion del sistema principal", "Core", 10, "INFO");
        registro.logInterno("ESTADO", "Ciclo interno completado", "Core", 42, "DEBUG");

        registro.logExterno("LOGIN", "Usuario inicio sesion", "user-123", "device-777", "AuthModule", 21, "INFO");
        registro.logExterno("CONSULTA", "Consulta de datos sensibles", "user-123", "device-777", "APIGateway", 87, "INFO");

        const auto filtrados = registro.query("2020-01-01T00:00:00", "", {"LOGIN", "CONSULTA"}, "user-123");
        std::cout << "Eventos filtrados para user-123: " << filtrados.size() << std::endl;

        std::cout << "\nCSV:\n" << registro.exportCSV() << std::endl;
        std::cout << "\nJSON:\n" << registro.exportJSON() << std::endl;
        std::cout << "\nXML:\n" << registro.exportXML() << std::endl;
    } catch (const std::exception& ex) {
        std::cerr << "Error al registrar eventos: " << ex.what() << std::endl;
        return 1;
    }
    return 0;
}

