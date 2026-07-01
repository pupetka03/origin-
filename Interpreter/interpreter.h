#pragma once
#include <memory>
#include <iostream>
#include <fstream>
#include "../ast/ast.h"

class Interpreter {
public:
    void print_tree(const shared_ptr<AST>& node, int depth, std::ofstream &file, vector<string> &var_or);
    void print_ast(const AST& tree);
};
