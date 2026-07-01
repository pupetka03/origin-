#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <memory>


using namespace std;


class AST {
    public:
        string type;
        string name;
        vector<shared_ptr<AST>> rekurzia;
        virtual ~AST() = default;
};





class Func_create : public AST {
    public:
        vector<string> arg_func;
        vector<shared_ptr<AST>> body;
        vector<string> retur_n; //return
};

class Call_func :public AST {
    public:
    string name_func;
    vector<string> arg_func;
    
};

class Variables : public AST {
    public:
        vector<Call_func> func;
        shared_ptr<Variables> var;
        vector<string> value;
};

class Takt : public AST {
    public:
        vector<string> condition;
        vector<shared_ptr<AST>> body;
};

class Cykl : public AST {
    public:
        vector<string> condition;
        vector<shared_ptr<AST>> body;

};

class Print : public AST {
    public:
    vector<string> str;
    

};

class Jak: public AST {
    public:
        vector<string> condition;
        vector<shared_ptr<AST>> true_branch; 
        vector<shared_ptr<AST>> false_branch; 
    
    
};

class Return_node : public AST {
    public:
        vector<string> expr;
};

class Break_node : public AST {};
class Continue_node : public AST {};
class Nic_node : public AST {};