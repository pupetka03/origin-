#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdio>

#include "lexer/include/lexer.h"
#include "parser/parser.h"
#include "Interpreter/interpreter.h"

using namespace std;

void print_help() {
    cout << "Origin++ Compiler (Driver) v1.0" << endl;
    cout << "Використання: origin <вхідний_файл.ori> [опції]" << endl;
    cout << "Опції:" << endl;
    cout << "  -o <вихідний_файл>  Вказати ім'я скомпільованого бінарника" << endl;
    cout << "  -c                  Тільки транспілювати в C++ (генерує origin.cpp)" << endl;
    cout << "  -r, --run           Зкомпілювати та одразу запустити програму" << endl;
    cout << "  -h, --help          Показати цю довідку" << endl;
    cout << "  -v, --version       Показати версію компілятора" << endl;
}

bool check_compiler(const string& cmd) {
#ifdef _WIN32
    string check = cmd + " --version > nul 2>&1";
#else
    string check = cmd + " --version > /dev/null 2>&1";
#endif
    return std::system(check.c_str()) == 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    string input_file = "";
    string output_binary = "origin_bin";
    bool only_transpile = false;
    bool run_after = false;

    vector<string> run_args;

    // Parse CLI arguments
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--") {
            for (int j = i + 1; j < argc; ++j) {
                run_args.push_back(argv[j]);
            }
            break;
        }
        if (arg == "-h" || arg == "--help") {
            print_help();
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            cout << "Origin++ Compiler v1.0" << endl;
            return 0;
        } else if (arg == "-c") {
            only_transpile = true;
        } else if (arg == "-r" || arg == "--run") {
            run_after = true;
        } else if (arg == "-o") {
            if (i + 1 < argc) {
                output_binary = argv[++i];
            } else {
                cerr << "Помилка: прапорець -o вимагає вказати ім'я вихідного файлу!" << endl;
                return 1;
            }
        } else {
            if (input_file.empty()) {
                input_file = arg;
            } else {
                cerr << "Помилка: невідомий параметр або кілька вхідних файлів: " << arg << endl;
                return 1;
            }
        }
    }

    if (input_file.empty()) {
        cerr << "Помилка: не вказано вхідний файл .ori!" << endl;
        return 1;
    }

    ifstream ReadFile(input_file);
    if (!ReadFile.is_open()) {
        cerr << "Помилка: не вдалося відкрити файл " << input_file << endl;
        return 1;
    }

    string riadok;
    Lexer Lex;
    while (getline(ReadFile, riadok)) {
        Lex.add_code(riadok);
    }
    ReadFile.close();

    vector<string> text = Lex.token();

    // Write tokens to rec.txt (debug metadata)
    ofstream Record("rec.txt");
    for (const string &l : text) {
        Record << l << endl;
    }
    Record.close();

    // Parse AST
    Paser parser; 
    AST drevo = parser.create_ast(text);
    
    // Transpile to C++ (writes to origin.cpp)
    Interpreter interpreter;
    interpreter.print_ast(drevo);

    if (only_transpile) {
        cout << "Транспіляція завершена успішно. Згенеровано: origin.cpp" << endl;
        return 0;
    }

    // Determine target compiler
    string compiler = "";
    if (check_compiler("clang++")) {
        compiler = "clang++";
    } else if (check_compiler("g++")) {
        compiler = "g++";
    } else {
        cerr << "Помилка: не знайдено C++ компілятор (clang++ або g++). Будь ласка, додайте компілятор у PATH." << endl;
        return 1;
    }

    // Compile C++ code
    string compile_cmd = compiler + " -std=c++17 origin.cpp -o " + output_binary;
#ifdef _WIN32
    if (output_binary.find('.') == string::npos) {
        compile_cmd += ".exe";
    }
#endif

    cout << "==> Компіляція за допомогою " << compiler << "..." << endl;
    int compile_res = std::system(compile_cmd.c_str());
    if (compile_res != 0) {
        cerr << "Помилка: Не вдалося скомпілювати згенерований C++ код!" << endl;
        return 1;
    }

    // Clean up temporary C++ file
    std::remove("origin.cpp");

    if (run_after) {
        cout << "==> Запуск програми " << output_binary << "..." << endl;
        cout << "----------------------------------------" << endl;
        string run_cmd = "./" + output_binary;
#ifdef _WIN32
        run_cmd = output_binary;
#endif
        for (const auto& a : run_args) {
            run_cmd += " " + a;
        }
        std::system(run_cmd.c_str());
        cout << "----------------------------------------" << endl;
    }

    return 0;
}