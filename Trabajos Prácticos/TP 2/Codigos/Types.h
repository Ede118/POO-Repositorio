#ifndef TYPES_H
#define TYPES_H

#include <stdexcept>
#include <string>

enum class Mode { Read, Write };
enum class InputFormat { CSV, JSON, XML };
enum class OutputFormat { CSV, JSON, XML };

// Excepción simple acorde a tu diagrama
struct AppError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

#endif // TYPES_H
