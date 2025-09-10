#include "CLI.h"
#include <cstring>

static InputFmt parseIn(char c){
    if(c=='c') return InputFmt::CSV;
    if(c=='j') return InputFmt::JSON;
    if(c=='x') return InputFmt::XML;
    throw AppError("Formato de entrada inválido (use c|j|x).");
}

static OutputFmt parseOut(char c){
    if(c=='c') return OutputFmt::CSV;
    if(c=='j') return OutputFmt::JSON;
    if(c=='x') return OutputFmt::XML;
    throw AppError("Formato de salida inválido (use c|j|x).");
}

CLI::CLI(int argc, char** argv): argc_(argc), argv_(argv) {}

void CLI::parse(){
    for(int i=1;i<argc_;++i){
        if(std::strcmp(argv_[i],"-m")==0 && i+1<argc_){
            char c = argv_[++i][0];
            if(c=='r') mode_=Mode::Read;
            else if(c=='w') mode_=Mode::Write;
            else throw AppError("Modo inválido (use -m r|w).");
        } else if(std::strcmp(argv_[i],"-f")==0 && i+1<argc_){
            file_ = argv_[++i];
        } else if(std::strcmp(argv_[i],"-i")==0 && i+1<argc_){
            in_ = parseIn(argv_[++i][0]);
        } else if(std::strcmp(argv_[i],"-o")==0 && i+1<argc_){
            out_ = parseOut(argv_[++i][0]);
        } else if(std::strcmp(argv_[i],"-n")==0 && i+1<argc_){
            n_ = std::stoi(argv_[++i]);
        } else if(std::strcmp(argv_[i],"-d")==0 && i+1<argc_){
            dir_ = argv_[++i];
        } else if(std::strcmp(argv_[i],"-A")==0){
            append_ = true;
        } else if(std::strcmp(argv_[i],"-s")==0 && i+1<argc_){
            serialDev_ = argv_[++i];
        } else {
            throw AppError(std::string("Opción desconocida: ") + argv_[i]);
        }
    }
    if(file_.empty()) throw AppError("Falta nombre de archivo (-f).");
}

Mode CLI::mode() const { return mode_; }
std::string CLI::filename() const { return file_; }
InputFmt CLI::inputFmt() const { return in_; }
OutputFmt CLI::outputFmt() const { return out_; }
int CLI::readCount() const { return n_; }
std::string CLI::baseDir() const { return dir_; }
