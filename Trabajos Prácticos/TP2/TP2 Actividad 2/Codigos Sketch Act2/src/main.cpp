#include "App.h"
#include "CLI.h"
#include "File.h"
#include "Types.h"
#include "Verificador.h"

#include <fstream>
#include <iostream>

namespace {
void loadRules(const CLI& cli, Verificador& verificador) {
    if (cli.allowRules().empty()) {
        std::cerr << "Aviso: sin reglas --allow; se usan reglas de ejemplo.\n";
        verificador.allow("u001", "dev-alpha", "wa");
        verificador.allow("u002", "dev-beta", "a");
        return;
    }

    for (const auto& rule : cli.allowRules()) {
        verificador.allow(rule.id, rule.device, rule.perms);
    }
}
}

int main(int argc, char** argv) {
    try {
        CLI cli;
        cli.parse(argc, argv);

        if (cli.mode() != Mode::Write) {
            std::cerr << "Modo no implementado aún.\n";
            return 1;
        }

        File file(cli.outputPath());
        Verificador verificador;
        loadRules(cli, verificador);

        App app(file, verificador);

        std::ifstream fileInput;
        std::istream* source = &std::cin;
        if (cli.inputPath()) {
            fileInput.open(*cli.inputPath());
            if (!fileInput) {
                throw AppError("No se pudo abrir la entrada: " + cli.inputPath()->string());
            }
            source = &fileInput;
        }

        app.processWrite(*source, cli.append());

        std::cout << "CSV escrito en: " << file.path() << '\n';

        if (!app.accepted().empty()) {
            std::cout << "Filas registradas: " << app.accepted().size() << '\n';
        }
        if (!app.rejected().empty()) {
            std::cout << "Filas rechazadas por permisos: " << app.rejected().size() << '\n';
            for (const auto& line : app.rejected()) {
                std::cout << "  - " << line << '\n';
            }
        }
    } catch (const AppError& e) {
        std::cerr << "AppError: " << e.what() << '\n';
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error inesperado: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
