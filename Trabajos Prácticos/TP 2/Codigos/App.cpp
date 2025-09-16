#include "App.h"
#include "CLI.h"
#include "Files_101.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <chrono>
#include <memory>
#include <fstream>
#include <cctype>

using namespace std;

static const vector<string> kHeader = {"Id","V","Level","Q"};

static char fmtChar(InputFmt f){
    switch(f){
        case InputFmt::CSV:  return 'c';
        case InputFmt::JSON: return 'j';
        case InputFmt::XML:  return 'x';
    }
    return '?';
}

// parseo mínimo de una línea a campos según formato ENTRADA
static vector<string> parseToFields(InputFmt fmt, const string& line){
    vector<string> fields;
    if (fmt == InputFmt::CSV){
        string cur; bool inq=false;
        for(char ch: line){
            if (ch=='"'){ inq = !inq; continue; }
            if (ch==',' && !inq){ fields.push_back(cur); cur.clear(); }
            else cur.push_back(ch);
        }
        fields.push_back(cur);
        return fields;
    }
    if (fmt == InputFmt::JSON){
        string v; bool inStr=false; bool afterColon=false;
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
        for(auto& s: fields){
            while(!s.empty() && isspace(static_cast<unsigned char>(s.front()))) s.erase(s.begin());
            while(!s.empty() && isspace(static_cast<unsigned char>(s.back())))  s.pop_back();
        }
        return fields;
    }
    if (fmt == InputFmt::XML){
        for(size_t i=0;i<line.size();){
            auto open = line.find('>', i);
            if(open==string::npos) break;
            auto close = line.find('<', open+1);
            if(close==string::npos) break;
            string val = line.substr(open+1, close-open-1);
            auto nz = val.find_first_not_of(" \t\r\n");
            if(nz!=string::npos){
                auto rz = val.find_last_not_of(" \t\r\n");
                fields.push_back(val.substr(nz, rz-nz+1));
            }
            i = close+1;
        }
        return fields;
    }
    return fields;
}

int App::run(int argc, char** argv){
    try{
        CLI cli(argc, argv);
        cli.parse();

        // ========== ESCRITURA ==========
        if (cli.mode() == Mode::Write) {
            // 1) decidir si hay que escribir cabecera
            bool need_header = true;
            if (cli.append()) {
                Files_101 probe(cli.filename());
                if (probe.exist()) {
                    probe.open('r');
                    need_header = (probe.getDimension() == 0);
                    probe.close();
                }
            }

            // 2) abrir archivo para escribir
            Files_101 doc(cli.filename());
            doc.open(cli.append() ? 'A' : 'w');
            if (need_header) {
                ostringstream oss;
                for (size_t i = 0; i < kHeader.size(); ++i) {
                    if (i) oss << ',';
                    oss << kHeader[i];
                }
                doc.write(oss.str());
            }

            // 3) abrir serial si corresponde
            unique_ptr<fstream> serial;
            if (cli.useSerial()) {
                if (cli.serialDev().empty()) throw AppError("No se especificó dispositivo serie.");
                // seteo rápido del puerto (opcional)
                string cmd = "stty -F " + cli.serialDev() + " 9600 raw -echo -ixon -ixoff";
                (void)std::system(cmd.c_str());
                serial = make_unique<fstream>(cli.serialDev(), ios::in | ios::out | ios::binary);
                if (!*serial) throw AppError("No se pudo abrir el dispositivo " + cli.serialDev());
                serial->rdbuf()->pubsetbuf(nullptr,0);
                this_thread::sleep_for(chrono::milliseconds(350)); // por si resetea
                serial->clear();
            }

            // 4) loop de N lecturas
            const int N = cli.readCount() > 0 ? cli.readCount() : 5;
            string line;

            for (int i = 0; i < N; ++i) {
                if (cli.useSerial()) {
                    if (serial->fail() || serial->bad()) serial->clear();
                    const char req = fmtChar(cli.inputFmt());
                    serial->put(req);
                    serial->flush();
                    this_thread::sleep_for(chrono::milliseconds(120));
                    if (!std::getline(*serial, line)) continue;
                } else {
                    if (!std::getline(std::cin, line)) break;
                }

                if (!line.empty() && line.back()=='\r') line.pop_back();
                if (line.empty()) continue;

                auto fields = parseToFields(cli.inputFmt(), line);
                if (fields.empty()) continue;
                if (fields.size() < kHeader.size()) fields.resize(kHeader.size());
                bool vacia=true; for(auto& s: fields) if(!s.empty()){ vacia=false; break; }
                if (vacia) continue;

                doc.writeParsed(fields);
            }

            doc.close();

            // 5) feedback rápido
            Files_101 rd(cli.filename());
            rd.open('r');
            std::cout << "\n" << rd.getCSV();
            rd.close();
            return 0;
        }

        // ========== LECTURA ==========
        if (cli.mode() == Mode::Read) {
            Files_101 rd(cli.filename());
            rd.open('r');
            auto info = rd.getInfo();
            std::cout << "name: "      << info.name      << "\n"
                      << "datetime: "  << info.datetime  << "\n"
                      << "owner: "     << info.owner     << "\n"
                      << "dimension: " << info.dimension << "\n\n";

            switch (cli.outputFmt()) {
                case OutputFmt::CSV:  std::cout << rd.getCSV();  break;
                case OutputFmt::JSON: std::cout << rd.getJSON(); break;
                case OutputFmt::XML:  std::cout << rd.getXML();  break;
            }
            rd.close();
            return 0;
        }

        throw AppError("Modo inválido");

    } catch (const AppError& e) {
        std::cerr << "[ERROR] " << e.what() << "\n";
        return 2;
    } catch (const std::exception& e) {
        std::cerr << "[FATAL] " << e.what() << "\n";
        return 1;
    }
}
