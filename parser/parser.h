#pragma once
#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <type_traits>

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
                    ret.expr.push_back(text[*i]);
                    (*i)++;
                }
                if (*i < text.size() && text[*i] == "\n") {
                    (*i)++;
                }
                nodes.push_back(make_shared<Return_node>(ret));
            }
            
            
            else {
                if (*i + 1 < text.size() && text[*i + 1] == "=") {
                    Variables variables;
                    variables.name = text[*i]; 
                    (*i) = (*i) + 2;
                    if (*i + 1 < text.size() && text[*i + 1] == "(") {
                        Call_func call;
                        call.name = text[*i]; 
                        (*i) = (*i) + 2;
                        while(*i < text.size() && text[*i] != ")") {
                            if (text[*i] != "," && text[*i] != " ") {
                                call.arg_func.push_back(text[*i]); 
                            }
                            (*i)++;
                        }
                        (*i)++; 
                        variables.func.push_back(call);
                        variables.type = "auto";
                    } else {
                        while (*i < text.size() && text[*i] != "\n") {
                            variables.value.push_back(text[*i]);
                            (*i)++;
                        }
                        if (*i < text.size() && text[*i] == "\n") {
                            (*i)++;
                        }
                        variables.type = "auto";
                    }
                    nodes.push_back(make_shared<Variables>(variables));
                }
                else if (*i + 1 < text.size() && text[*i + 1] == "(") {
                    Call_func call;
                    call.name = text[*i]; 
                    (*i) = (*i) + 2;
                    while(*i < text.size() && text[*i] != ")") {
                        if (text[*i] != "," && text[*i] != " ") {
                            call.arg_func.push_back(text[*i]); 
                        }
                        (*i)++;
                    }
                    (*i)++; 
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

            if (*i < text.size() && text[*i] == "{") {
                (*i)++; 
                Func.body = parse_block(i, text);
            }
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

        if (*i < text.size() && text[*i] == "inakjak") {
            jak.false_branch.push_back(make_shared<Jak>(create_jak(i, text)));
        }

        else if  (*i < text.size() && text[*i] == "inak") { (*i)++;
            if (text[*i] == "{") {
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