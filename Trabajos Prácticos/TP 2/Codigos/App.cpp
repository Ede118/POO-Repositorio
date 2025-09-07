#include "App.h"
#include "CLI.h"
#include "Files_101.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <cctype>

// Parsing mínimo de una línea a campos segun formato de ENTRADA
static std::vector<std::string> parseToFields(InputFmt fmt, const std::string& line){
    std::vector<std::string> fields;

    if(fmt==InputFmt::CSV){
        // split básico por comas sin comillas
        std::string cur; bool inq=false;
        for(char ch: line){
            if(ch=='"') inq = !inq;
            else if(ch==',' && !inq){ fields.push_back(cur); cur.clear(); }
            else cur.push_back(ch);
        }
        fields.push_back(cur);
        return fields;
    }
    if(fmt==InputFmt::JSON){
        // súper simple: {"a":1,"b":"2"} -> 1,2
        std::string v; bool inStr=false; bool afterColon=false;
        for(size_t i=0;i<line.size();++i){
            char ch=line[i];
            if(ch=='"'){ inStr = !inStr; continue; }
            if(inStr){ v.push_back(ch); continue; }
            if(ch==':'){ afterColon=true; continue; }
            if(ch==',' || ch=='}'){
                if(!v.empty()){ fields.push_back(v); v.clear(); }
                afterColon=false;
                continue;
            }
            if(afterColon && !isspace(static_cast<unsigned char>(ch)) && ch!='{' ){
                v.push_back(ch);
            }
        }
        if(!v.empty()) fields.push_back(v);
        // trim espacios
        for(auto& s: fields){
            while(!s.empty() && isspace(static_cast<unsigned char>(s.front()))) s.erase(s.begin());
            while(!s.empty() && isspace(static_cast<unsigned char>(s.back()))) s.pop_back();
        }
        return fields;
    }
    // XML: <row><a>1</a><b>2</b></row> -> 1,2
    if(fmt==InputFmt::XML){
        for(size_t i=0;i<line.size();){
            auto open = line.find('>', i);
            if(open==std::string::npos) break;
            auto close = line.find('<', open+1);
            if(close==std::string::npos) break;
            std::string val = line.substr(open+1, close-open-1);
            if(!val.empty() && val.find_first_not_of(" \t\r\n")!=std::string::npos)
                fields.push_back(val);
            i = close+1;
        }
        return fields;
    }
    return fields;
}

static void printInfo(const FileInfo& info){
    std::cout << "name: "      << info.name      << "\n"
              << "datetime: "  << info.datetime  << "\n"
              << "owner: "     << info.owner     << "\n"
              << "dimension: " << info.dimension << "\n";
}

App::App() = default;
App::~App() = default;

int App::run(int argc, char** argv){
    try{
        CLI cli(argc, argv);
        cli.parse();

        if (cli.mode() == Mode::Write) {
            // 1) Decidir si hay que escribir cabecera
            bool need_header = true;
            {
                Files_101 probe(cli.filename());
                if (cli.append() && probe.exist()) {
                    probe.open('r');
                    need_header = (probe.getDimension() == 0);
                    probe.close();
                }
            }

            // 2) Abrir en 'a' si -A, si no en 'w'
            Files_101 doc(cli.filename());
            doc.open(cli.append() ? 'A' : 'w');

            if (need_header) {
                // Cabecera simple por defecto; cambiala si querés algo más semántico
                doc.write("Id,%V,Level,Q");
            }

            // 3) Ingesta de N líneas (desde stdin o pipe)
            int N = cli.readCount() > 0 ? cli.readCount() : 5;
            std::string line;
            for (int i = 0; i < N; ++i) {
                if (!std::getline(std::cin, line)) break;
                auto fields = parseToFields(cli.inputFmt(), line);
                if (fields.empty()) doc.write(line);
                else                doc.writeParsed(fields);
            }
            doc.close();

            // 4) Mostrar metadatos y una vista rápida en CSV
            doc.open('r');
            printInfo(doc.getInfo());
            std::cout << "\n" << doc.getCSV();
            doc.close();
            return 0;

        } else { // Mode::Read
            Files_101 doc(cli.filename());
            doc.open('r');
            printInfo(doc.getInfo());
            std::cout << "\n";
            switch (cli.outputFmt()) {
                case OutputFmt::CSV:  std::cout << doc.getCSV();  break;
                case OutputFmt::JSON: std::cout << doc.getJSON(); break;
                case OutputFmt::XML:  std::cout << doc.getXML();  break;
            }
            doc.close();
            return 0;
        }

    } catch (const AppError& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 2;
    } catch (const std::exception& e) {
        std::cerr << "[FATAL] " << e.what() << "\n";
        return 1;
    }
}

/*
int App::run(int argc, char** argv){
    try{
        CLI cli(argc, argv);
        cli.parse();

        Files_101 doc(cli.filename());

        if(cli.mode()==Mode::Write){
            if (cli.mode()==Mode::Write){
                bool need_header = true;

                // si vamos a agregar y el archivo ya tiene contenido, no escribas cabecera
                Files_101 probe(cli.filename());
                if (cli.append() && probe.exist()) {
                    probe.open('r');
                    need_header = (probe.getDimension()==0);
                    probe.close();
                }

                // abrir en 'a' si -A, si no 'w'
                Files_101 doc(cli.filename());
                doc.open(cli.append() ? 'a' : 'w');

                if (need_header) {
                    doc.write("c0,c1,c2,c3");
                }

                int N = cli.readCount() > 0 ? cli.readCount() : 5;
                std::string line;
                for (int i=0; i<N; ++i){
                    if(!std::getline(std::cin, line)) break;
                    auto fields = parseToFields(cli.inputFmt(), line);
                    if(fields.empty()) doc.write(line);
                    else               doc.writeParsed(fields);
                }
                doc.close();
            } 
        } else {
            // READ
            doc.open('r');
            printInfo(doc.getInfo());
            std::cout << "\n";
            switch(cli.outputFmt()){
                case OutputFmt::CSV:  std::cout << doc.getCSV();  break;
                case OutputFmt::JSON: std::cout << doc.getJSON(); break;
                case OutputFmt::XML:  std::cout << doc.getXML();  break;
            }
            doc.close();
        }
        return 0;

    } catch(const AppError& e){
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 2;
    } catch(const std::exception& e){
        std::cerr << "[FATAL] " << e.what() << "\n";
        return 1;
    }
}
*/