
#ifndef CLI_H
#define CLI_H

#include <string>
#include <stdexcept>



/*
  CLI se encarga de parsear los argumentos de línea de comandos
  y proveer métodos para acceder a los parámetros.

  En caso de error en los argumentos, lanza AppError

  Uso esperado:
    programa -m <mode> -f <filename> [ -i <input_format> ] [ -o <output_format> ] [ -n <number> ] [ -d <base_dir> ]
  donde:
    - mode: 'r' (read) o 'w' (write)
    - filename: nombre del archivo (sin extensión)
    - input_format: 'csv', 'json' o 'xml' (solo si -m w), default 'csv'
    - output_format: 'csv', 'json' o 'xml' (solo si -m r), default 'csv'
    - number: cantidad de registros a generar (solo si -m w), default 0 (ilimitado)
    - base_dir: directorio base para los archivos (default "./data")
*/

class AppError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// Se definen enums para los modos y formatos
enum class Mode { Read, Write };
enum class InputFmt { CSV, JSON, XML };
enum class OutputFmt { CSV, JSON, XML };

class CLI {
public:
    // Constructor
    CLI(int argc, char** argv);
    void parse(); // lanza AppError si hay incoherencias

    Mode mode() const;
    std::string filename() const;    // sin extensión
    InputFmt inputFmt() const;       // solo si -m w
    OutputFmt outputFmt() const;     // solo si -m r
    int readCount() const;           // -n (para escritura)
    std::string baseDir() const;     // si te dejan override (sino ignorar)

private:
    // Variables miembro

    int argc_;
    char** argv_;
    Mode mode_{Mode::Read};
    InputFmt in_{InputFmt::CSV};
    OutputFmt out_{OutputFmt::CSV};
    std::string file_;
    std::string dir_;
    int n_ = 0;
};

#endif // CLI_H
