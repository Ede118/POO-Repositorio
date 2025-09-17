#include "App.h"
#include "Types.h"
#include <iostream>

int main(int argc, char** argv) {
    try {
        App app;
        return app.run(argc, argv);
    } catch (const AppError& e) {
        std::cerr << "[AppError] " << e.what() << "\n";
        return 2;
    } catch (const std::exception& e) {
        std::cerr << "[Error] " << e.what() << "\n";
        return 1;
    }
}
