#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "lexer/include/lexer.h"
#include "parser/parser.h"
#include "Interpreter/interpreter.h"

using namespace std;


int main(int arg, char *args[]) {
    if (arg < 2) {
        cout << "Error: provide file path" << endl;
        return 1;
    }

    ifstream ReadFile(args[1]);
    if (!ReadFile.is_open()) {
        cout << "Error: cannot open file" << endl;
        return 1;
    }

    string riadok;
    Lexer Lex;

    
    while (getline(ReadFile, riadok)) {
        Lex.add_code(riadok);
    }

    vector<string> text = Lex.token();
   

    ofstream Record("rec.txt");
    for (const string &l : text) {
        Record << l << endl;
    }
    Record.close();

    ReadFile.close();

    Paser parser; AST drevo;
    drevo = parser.create_ast(text);
    
    Interpreter interpreter;
    interpreter.print_ast(drevo);
    
    return 0;
}