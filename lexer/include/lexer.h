#pragma once
#include <string>
#include <vector>

using namespace std;



class Lexer {
    public:
        vector<string> code;
        vector<string> token();

        void add_code(string code);
};