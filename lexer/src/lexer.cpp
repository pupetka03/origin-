#include "../include/lexer.h"
#include <iostream>
#include <cctype>
#include <unordered_set>




void Lexer::add_code(string text) {
    code.push_back(text);
}


vector<string> Lexer::token() {
    vector<string> code;


    for (const string &t : this->code) {
        string text = ""; 
        for (size_t i = 0; i < t.size(); i++) {
            if (t[i] == '#') {
                if (!text.empty()) {
                    code.push_back(text);
                    text = "";
                }
                break;
            }

            if (t[i] == '"') {
                if (!text.empty()) {
                    code.push_back(text);
                    text = "";
                }

                string literal = "\""; 
                i++; 

                while (i < t.size() && t[i] != '"') {
                    literal += t[i];
                    i++;
                }

                if (i < t.size() && t[i] == '"') {
                    literal += '"';
                }

                code.push_back(literal); 
                continue;
            }
            
            if (isspace(static_cast<unsigned char>(t[i]))) {
                if (!text.empty()) {
                    code.push_back(text);
                    text = "";
                }
                continue;
            }

            if (t[i] == '(' || t[i] == ')' || t[i] == '{' || t[i] == '}' || 
                t[i] == ',' || t[i] == '+' || t[i] == '-' || t[i] == '*' || t[i] == '/' ||
                t[i] == '[' || t[i] == ']' || t[i] == '=' || t[i] == '<' || t[i] == '>' || t[i] == '!') 
            {
                if (!text.empty()) {
                    code.push_back(text);
                    text = "";
                }
                
                if (t[i] == '=' && i + 1 < t.size() && t[i + 1] == '=') {
                    code.push_back("==");
                    i++; 
                } else if (t[i] == '<' && i + 1 < t.size() && t[i + 1] == '=') {
                    code.push_back("<=");
                    i++;
                } else if (t[i] == '>' && i + 1 < t.size() && t[i + 1] == '=') {
                    code.push_back(">=");
                    i++;
                } else if (t[i] == '!' && i + 1 < t.size() && t[i + 1] == '=') {
                    code.push_back("!=");
                    i++;
                
                    


                } else {
                    string specialChar(1, t[i]);
                    code.push_back(specialChar);
                }
                
                continue;
            }

            text += t[i];
        }
        
        if (!text.empty()) {
            code.push_back(text);
        }
        code.push_back("\n");
    }

    static const std::unordered_set<string> reserved_keywords = {
        "int", "float", "double", "char", "bool", "void", "auto",
        "class", "struct", "union", "enum", "template", "namespace",
        "using", "const", "static", "extern", "inline", "virtual",
        "friend", "typedef", "typename", "switch", "case", "default",
        "main", "string", "list", "dict"
    };

    for (string &token : code) {
        if (token.size() > 0 && token[0] == '_' && token[0] != '"') {
            std::cerr << "Помилка: Ідентифікатори не можуть починатися з підкреслення '_': " << token << std::endl;
            exit(1);
        }
        if (token == "ta") {
            token = "&&";
        }
        if (token == "abo") {
            token = "||";
        }
        if (token == "ne") {
            token = "!";
        }
        if (reserved_keywords.count(token)) {
            token = "_" + token;
        }
        
    }

    return code;
}