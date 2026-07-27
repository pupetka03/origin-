#pragma once
#include <memory>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "../ast/ast.h"

class Interpreter {
public:
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> function_returns;
    void print_tree(const shared_ptr<AST>& node, int depth, std::ofstream &file, vector<string> &var_or);
    void print_ast(const AST& tree);
};
