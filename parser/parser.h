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
            
            
            else {
                bool is_untyped = (*i + 1 < text.size() && text[*i + 1] == "=" || *i + 1 < text.size() && text[*i + 1] == "," );
                bool is_typed   = (*i + 4 < text.size() && text[*i + 1] == "-" && text[*i + 2] == ">" && text[*i + 4] == "=");

                if (is_untyped || is_typed) {
                    Variables variables;
                    variables.name = text[*i];

                    if (is_typed) {
                        variables.type = text[*i + 3];
                        (*i) = (*i) + 5;
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
                        (*i)++;
                        vector<string> args;
                        while (*i < text.size() && text[*i] != "]") {
                            if (text[*i] != "," && text[*i] != " ") {
                                args.push_back(text[*i]);
                            } (*i)++;
                        } (*i)++;
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
        while (text[*i] != "{") {
            jak.condition.push_back(text[*i]); (*i)++;
        }(*i)++;
    
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
        while(text[*i] != "{") {
            cykl.condition.push_back(text[*i]); (*i)++;
        } (*i)++;
        
        cykl.body = parse_block(i, text);

        return cykl;
    }

    Takt create_takt(int *i, const vector<string>& text) {
        Takt takt; (*i)++;
        while (text[*i] != "{"){
            takt.condition.push_back(text[*i]); (*i)++;
        }(*i)++;
        
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