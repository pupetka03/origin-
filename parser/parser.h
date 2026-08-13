#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <type_traits>
#include <cctype>

#include "../ast/ast.h"
using namespace std;


class Paser {
public:

    vector<shared_ptr<AST>> parse_block(int *i, const vector<string>& text) {
        //загальне дерево
        vector<shared_ptr<AST>> nodes;
        
        while(*i < text.size()) {
            if (text[*i] == "}") {
                (*i)++; 
                break;
            }

            if(text[*i] == "baza") {
                Func_create Func = create_fun(i, text);
                nodes.push_back(make_shared<Func_create>(Func));
            } 

            else if (text[*i] == "break"){
                nodes.push_back(make_shared<Break_node>());
                (*i)++;
            }
            else if (text[*i] == "continue"){
                nodes.push_back(make_shared<Continue_node>());
                (*i)++;
            }
            
            else if (text[*i] == "print"){
                Print printn = print_origin(i, text);
                nodes.push_back(make_shared<Print>(printn));
            }
            
            else if (text[*i] == "jak") {
                Jak jak = create_jak(i, text);
                nodes.push_back(make_shared<Jak>(jak));
            }

            else if (text[*i] == "cykl"){
                Cykl cykl = create_cykl(i, text);
                nodes.push_back(make_shared<Cykl>(cykl));
            }

            else if (text[*i] == "takt"){
                Takt takt = create_takt(i, text);
                nodes.push_back(make_shared<Takt>(takt));
            }
            
            else if (text[*i] == "return") {
                Return_node ret;
                (*i)++;
                while (*i < text.size() && text[*i] != "\n") {
                    //cout << "f: " << text[*i] <<  endl;
                    if (*i < text.size() && text[*i + 1] == ",") {
                        string make = "std::make_tuple("; //make_tuple

                        while (*i < text.size() && text[*i] != "\n") { 
                            if (text[*i] != ",") {
                                make += text[*i];
                            } else {
                                make += ", ";
                            } (*i)++;
    
                        }
                        make += ")";
                        ret.expr.push_back(make);
                        break;
                    }
                    ret.expr.push_back(text[*i]);
                    (*i)++;
                }
                if (*i < text.size() && text[*i] == "\n") {
                    (*i)++;
                }
                nodes.push_back(make_shared<Return_node>(ret));
            }

            else if (text[*i] == "import") {
                auto import_ast = create_import(text[(*i)+1]);
                auto import = make_shared<Import>();
                import->import_name = text[(*i) + 1];
                import->import_body = import_ast.rekurzia;
                nodes.push_back(import);
                (*i)+= 2;
                
            }

            else if (text[*i] == "typ") {
                auto typ = make_shared<Typ>();
                typ->name = text[(*i)+1];
                if (*i + 2 < text.size() && text[*i+2] == "(") {
                    if (text[*i+3] != ")") {
                        typ->podoba = text[*i+3];
                        (*i)+= 6;
                    } else {
                        typ->podoba = "false";
                        (*i)+= 5;
                    }
                } else {
                    typ->podoba = "false";
                        (*i) += 3;
                }
                typ->body = parse_block(i, text);
                nodes.push_back(typ);
            }
            
            
            else {
                // Check if it is a subscript assignment: e.g. xx[index] = ...
                bool is_subscript_assign = false;
                int subscript_end_idx = -1;
                if (*i + 1 < text.size() && text[*i + 1] == "[") {
                    int depth = 0;
                    for (int k = *i + 1; k < text.size(); ++k) {
                        if (text[k] == "[") {
                            depth++;
                        } else if (text[k] == "]") {
                            depth--;
                            if (depth == 0) {
                                if (k + 1 < text.size() && text[k + 1] == "=") {
                                    is_subscript_assign = true;
                                    subscript_end_idx = k;
                                }
                                break;
                            }
                        }
                    }
                }

                bool is_untyped = (*i + 1 < text.size() && text[*i + 1] == "=" || *i + 1 < text.size() && text[*i + 1] == "," );
                bool is_typed = false;
                int equals_idx = -1;
                if (*i + 2 < text.size() && text[*i + 1] == "-" && text[*i + 2] == ">") {
                    is_typed = true;
                    for (int k = *i + 3; k < text.size(); ++k) {
                        if (text[k] == "\n") break;
                        if (text[k] == "=") {
                            equals_idx = k;
                            break;
                        }
                    }
                }

                if (is_subscript_assign || is_untyped || is_typed) {
                    Variables variables;
                    
                    if (is_subscript_assign) {
                        string name_expr = "";
                        for (int k = *i; k <= subscript_end_idx; ++k) {
                            name_expr += text[k];
                        }
                        variables.name = name_expr;
                        variables.type = "auto";
                        *i = subscript_end_idx + 2;
                    }
                    else {
                        variables.name = text[*i];
                        if (is_typed) {
                            string type_str = "";
                            int end_type_idx = equals_idx;
                            if (end_type_idx == -1) {
                                end_type_idx = *i + 3;
                                while (end_type_idx < text.size() && text[end_type_idx] != "\n") {
                                    end_type_idx++;
                                }
                            }
                            for (int k = *i + 3; k < end_type_idx; ++k) {
                                type_str += text[k];
                            }
                            variables.type = type_str;
                            *i = end_type_idx;
                            if (equals_idx != -1) {
                                (*i)++;
                            }
                        } else {
                            variables.type = "auto";
                            if (text[*i + 1] == ",") {
                                while (*i < text.size() && text[*i] != "=") {
                                    if (text[*i] != "," && text[*i] != " ") {
                                        variables.unpack_vars.push_back(text[*i]);
                                    } 
                                    (*i)++;
                                }    
                                (*i)++; 
                            }
                            else {
                                (*i) = (*i) + 2;
                            }
                    }
                } 

                    if (*i + 1 < text.size() && text[*i + 1] == "(") {
                        Call_func call;
                        call.name = text[*i]; 
                        (*i) = (*i) + 2;
                        
                        int depth = 1;
                        string current_arg = "";
                        while (*i < text.size() && depth > 0) {
                            if (text[*i] == "(") {
                                depth++;
                                current_arg += text[*i];
                            } else if (text[*i] == ")") {
                                depth--;
                                if (depth > 0) {
                                    current_arg += text[*i];
                                }
                            } else if (text[*i] == "," && depth == 1) {
                                if (!current_arg.empty()) {
                                    call.arg_func.push_back(current_arg);
                                    current_arg = "";
                                }
                            } else if (text[*i] != " ") {
                                if (!current_arg.empty() && 
                                    isalnum(static_cast<unsigned char>(current_arg.back())) && 
                                    isalnum(static_cast<unsigned char>(text[*i][0]))) {
                                    current_arg += " ";
                                }
                                current_arg += text[*i];
                            }
                            (*i)++;
                        }
                        if (!current_arg.empty()) {
                            call.arg_func.push_back(current_arg);
                        }
                        variables.func.push_back(call);
                    } else {
                        while (*i < text.size() && text[*i] != "\n") {
                            variables.value.push_back(text[*i]);
                            (*i)++;
                        }
                        if (*i < text.size() && text[*i] == "\n") {
                            (*i)++;
                        }
                    }
                    nodes.push_back(make_shared<Variables>(variables));
                }
                else if (*i + 1 < text.size() && text[*i + 1] == "(") {
                    Call_func call;
                    call.name = text[*i]; 
                    (*i) = (*i) + 2;
                    
                    int depth = 1;
                    string current_arg = "";
                    while (*i < text.size() && depth > 0) {
                        if (text[*i] == "(") {
                            depth++;
                            current_arg += text[*i];
                        } else if (text[*i] == ")") {
                            depth--;
                            if (depth > 0) {
                                current_arg += text[*i];
                            }
                        } else if (text[*i] == "," && depth == 1) {
                            if (!current_arg.empty()) {
                                call.arg_func.push_back(current_arg);
                                current_arg = "";
                            }
                        } else if (text[*i] != " ") {
                            if (!current_arg.empty() && 
                                isalnum(static_cast<unsigned char>(current_arg.back())) && 
                                isalnum(static_cast<unsigned char>(text[*i][0]))) {
                                current_arg += " ";
                            }
                            current_arg += text[*i];
                        }
                        (*i)++;
                    }
                    if (!current_arg.empty()) {
                        call.arg_func.push_back(current_arg);
                    }
                    nodes.push_back(make_shared<Call_func>(call));
                }
                else {
                    (*i)++;
                }
            }
        }
        return nodes;
    }

    AST create_import(const string &file) {
        AST import; Lexer Lex;
        //cout << file << endl;
        ifstream ReadFile(file + ".ori");
        if (!ReadFile.is_open()) {
            cout << "Error: cannot open file " << file << endl;
            return import;
        }
        string riadok;
        while (getline(ReadFile, riadok)) {
            Lex.add_code(riadok);
        }

        ReadFile.close();
        vector<string> text = Lex.token();

        return create_ast(text);



        return import;

    }

    Func_create create_fun(int *i, const vector<string>& text) {
        Func_create Func;
        vector<vector<string>> without_auto;

        (*i)++; Func.name = text[*i]; (*i)++;

        
        
        if (text[*i] == "(") {
            (*i)++;
            while(text[*i] != ")") {
                if (text[*i] != "," && text[*i] != " ") {
                    Func.arg_func.push_back(text[*i]);
                }
                (*i)++;
            } (*i)++;

            if (Func.arg_func.empty()) {
                Func.arg_func.push_back(" ");
            } 

            
            if (*i < text.size() && text[*i] == "-" && text[*i+1] == ">") {
                (*i) = *i + 2;
                
                while (*i < text.size() && text[*i] != "{") {
                    //кортеж
                    if (text[*i] == "[") {
                        (*i)++; // Skip outer '['
                        vector<string> args;
                        string current_type = "";
                        int nest_depth = 0;
                        while (*i < text.size()) {
                            if (text[*i] == "[") {
                                nest_depth++;
                                current_type += "[";
                            }
                            else if (text[*i] == "]") {
                                if (nest_depth == 0) {
                                    if (!current_type.empty()) {
                                        args.push_back(current_type);
                                    }
                                    (*i)++; // Skip outer ']'
                                    break;
                                } else {
                                    nest_depth--;
                                    current_type += "]";
                                    if (current_type.size() >= 2 && current_type.substr(current_type.size() - 2) == "[]") {
                                        string base_type = current_type.substr(0, current_type.size() - 2);
                                        current_type = "_list<" + base_type + ">";
                                    }
                                }
                            }
                            else if (text[*i] == "," && nest_depth == 0) {
                                if (!current_type.empty()) {
                                    args.push_back(current_type);
                                    current_type = "";
                                }
                            }
                            else if (text[*i] != " ") {
                                current_type += text[*i];
                            }
                            (*i)++;
                        }
                        without_auto.push_back(args);
                    }

                    else if (text[*i] != "||" && text[*i] != " ") {
                        if (*i + 2 < text.size() && text[*i + 1] == "[" && text[*i + 2] == "]") {
                            without_auto.push_back({"_list<" + text[*i] + ">"});
                            (*i) = (*i) + 3;
                        } else {
                            without_auto.push_back({text[*i]});
                            (*i)++;
                        }
                    }

                    else if (text[*i] == "||") {
                        (*i)++;
                    }

                    else {
                        (*i)++;
                    }
                }
            }


            if (*i < text.size() && text[*i] == "{") {
                (*i)++; 
                Func.body = parse_block(i, text);
            }

            Func.control_return = without_auto;
        }

        return Func;
    }

    Print print_origin(int *i, const vector<string>& text) {
        Print printn; (*i)++;
        if (text[*i] == "(") { (*i)++;
            int depth = 1;
            while (*i < text.size() && depth > 0) {
                if (text[*i] == "(") {
                    depth++;
                } else if (text[*i] == ")") {
                    depth--;
                }
                if (depth > 0) {
                    printn.str.push_back(text[*i]);
                    (*i)++;
                }
            }
            if (*i < text.size() && text[*i] == ")") {
                (*i)++;
            }
        }
        return printn;
    }

    Jak create_jak(int *i, const vector<string>& text) {
        Jak jak; (*i)++;
        while (*i < text.size() && text[*i] != "{") {
            jak.condition.push_back(text[*i]); (*i)++;
        }
        if (*i >= text.size()) {
            cerr << "Помилка синтаксису: Очікувався символ '{' після умови 'jak'" << endl;
            exit(1);
        }
        (*i)++;
    
        jak.true_branch = parse_block(i, text);
 
        while (*i < text.size() && (text[*i] == "\n" || text[*i] == " ")) {
            (*i)++;
        }
 
        if (*i < text.size() && text[*i] == "inakjak") {
            jak.false_branch.push_back(make_shared<Jak>(create_jak(i, text)));
        }
 
        else if  (*i < text.size() && text[*i] == "inak") { (*i)++;
 
            while (*i < text.size() && (text[*i] == "\n" || text[*i] == " ")) {
                (*i)++;
            }
        
            if (*i < text.size() && text[*i] == "{") {
                (*i)++;
                jak.false_branch = parse_block(i, text);
            }
        }
        
        return jak;
}
 
    Cykl create_cykl(int *i, const vector<string>& text) {
        Cykl cykl;
        (*i)++;
        while(*i < text.size() && text[*i] != "{") {
            cykl.condition.push_back(text[*i]); (*i)++;
        }
        if (*i >= text.size()) {
            cerr << "Помилка синтаксису: Очікувався символ '{' після умови 'cykl'" << endl;
            exit(1);
        }
        (*i)++;
        
        cykl.body = parse_block(i, text);
 
        return cykl;
    }
 
    Takt create_takt(int *i, const vector<string>& text) {
        Takt takt; (*i)++;
        while (*i < text.size() && text[*i] != "{"){
            takt.condition.push_back(text[*i]); (*i)++;
        }
        if (*i >= text.size()) {
            cerr << "Помилка синтаксису: Очікувався символ '{' після умови 'takt'" << endl;
            exit(1);
        }
        (*i)++;
        
        takt.body = parse_block(i, text);
 
        return takt;
    }


    AST create_ast(const vector<string>& text) {
        AST main_brain;
        int i = 0;

        main_brain.rekurzia = parse_block(&i, text); 
        
        return main_brain;
    }

};