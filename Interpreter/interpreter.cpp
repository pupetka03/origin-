#include "interpreter.h"
#include <iostream>
#include <fstream>
#include <unordered_set>
/*#include <iostream>
#include <fstream>
#include <vector>
#include <string>*/


// libraries of origin++.  libraries origib
void import_data(ofstream &file) {
    file << "#include <iostream>" << endl;
    file << "#include <fstream>" << endl;
    file << "#include <vector>" << endl;
    file << "#include <string>" << endl;

    file << "#include \"libraries/originb.h\"" << endl;


    file << "using namespace std;" << endl;
}



//func of intepritator
int verific_var(vector<string> &var, string x) {
    for(const string &l : var) {
        if (l == x) {
            return 1;
        }
    }

    return 0;
}

string clean_type(string type) {
    if (type.size() > 1 && type[0] == '_') {
        string base = type.substr(1);
        static const std::unordered_set<string> base_types = {
            "int", "float", "double", "char", "bool", "void", "string"
        };
        if (base_types.count(base)) {
            return base; // повертаємо чистий "int", "double" тощо
        }
    }
    return type; // якщо це користувацький тип (наприклад, Hero), повертаємо як є
}





//print func (оголошення функцій)
void print_baza(const vector<shared_ptr<AST>>& tree, ofstream &file) {
    for (const auto& node : tree) {
        if (auto f = dynamic_pointer_cast<Func_create>(node)) {
            vector<string> clean_args;
            for (const auto& arg : f->arg_func) {
                if (arg != " " && arg != "," && !arg.empty()) {
                    clean_args.push_back(arg);
                }
            }
            if (!clean_args.empty()) {
                file << "template <typename T> ";
            }
            file << "auto " << f->name << "(";
            for (size_t k = 0; k < clean_args.size(); ++k) {
                file << "T " << clean_args[k];
                if (k + 1 < clean_args.size()) {
                    file << ", ";
                }
            }
            file << ");" << endl;
        }
    }
}

//intepritator
void Interpreter::print_tree(const shared_ptr<AST>& node, int depth, ofstream &file, vector<string> &var_or) {
    if (!node) return;

    string indent(depth * 2, ' ');

    if (auto p = dynamic_pointer_cast<Print>(node)) {
        file << indent << "cout << ";
        bool f_in = false;

        for (size_t i = 0; i < p->str.size(); ++i) {
            const auto& s = p->str[i];
            
            if (s == "(" || f_in) {
                file << s;
                if (s == ")") {
                    f_in = false;
                    if (i + 1 < p->str.size() && p->str[i+1] != "," && p->str[i+1] != " ") {
                        file << " << ";
                    }
                } else {
                    f_in = true;
                }
                continue;
            }

            if (s == "," || s == " ") {
                file << " << ";
                continue;
            }

            file << s;

            if (i + 1 < p->str.size()) {
                const auto& next = p->str[i+1];
                
                if (next == "(" || next == "." || s == "." ||
                    next == "+" || next == "-" || next == "*" || next == "/" ||
                    s == "+" || s == "-" || s == "*" || s == "/" ||
                    next == "<" || next == ">" || next == "<=" || next == ">=" || next == "==" || next == "!=" ||
                    s == "<" || s == ">" || s == "<=" || s == ">=" || s == "==" || s == "!=" ||
                    next == "&&" || next == "||" || s == "&&" || s == "||") 
                {
                    // part of the same expression
                } else {
                    file << " << ";
                }
            }
        }
        
        file << " << endl;" << endl;
    }
    

    else if (auto f = dynamic_pointer_cast<Func_create>(node)) {
        
        vector<string> clean_args;
        for (const auto& arg : f->arg_func) {
            if (arg != " " && arg != "," && !arg.empty()) {
                clean_args.push_back(arg);
            }
        }
        file << indent;
        if (!clean_args.empty()) {
            file << "template <typename T> ";
        }
        file << "auto " << f->name << "(";
        for (size_t k = 0; k < clean_args.size(); ++k) {
            file << "T " << clean_args[k];
            if (k + 1 < clean_args.size()) {
                file << ", ";
            }
        }
        file << ") {" << endl;

        vector<string> var_of_fun = var_or;
        for (const auto& arg : f->arg_func) {
            if (arg != " " && !arg.empty()) {
                var_of_fun.push_back(arg);
            }
        }
        for (const auto& child : f->body) {
            print_tree(child, depth + 1, file, var_of_fun);
        }

        file << indent << "}" << endl;
    }

    else if (auto j = dynamic_pointer_cast<Jak>(node)) {

        file << indent << "if (";
        for (size_t k = 0; k < j->condition.size(); ++k) {
            file << j->condition[k];
            if (k + 1 < j->condition.size()) {
                file << " ";
            }
        }
        file << ") {" << endl;

        for (const auto& child : j->true_branch) {
            print_tree(child, depth + 1, file, var_or);
        }

        file << indent << "}" << endl;

        if (!j->false_branch.empty()) {
            if (j->false_branch.size() == 1 && dynamic_pointer_cast<Jak>(j->false_branch[0])) {
                file << indent << "else ";
                print_tree(j->false_branch[0], depth, file, var_or);
            } else {
                file << indent << "else {" << endl;
                for (const auto& child : j->false_branch) {
                    print_tree(child, depth + 1, file, var_or);
                }
                file << indent << "}" << endl;
            }
        }
    }

    else if (auto c = dynamic_pointer_cast<Cykl>(node)) {
        file << indent << "while (";
        for (size_t k = 0; k < c->condition.size(); ++k) {
            file << c->condition[k];
            if (k + 1 < c->condition.size()) {
                file << " ";
            }
        }
        file << ") {" << endl;

        for (const auto& child : c->body) {
            print_tree(child, depth + 1, file, var_or);
        }

        file << indent << "}" << endl;
    }

    else if (auto t = dynamic_pointer_cast<Takt>(node)) {
        int i = 0; int size_r = t->condition.size();
        file << indent << "for(const auto &";

  
        while(i < size_r) {
            file << t->condition[i];
            if (i == 0) {
                file << ":";
            }
            i++;
        }
        file << ") {" << endl;

        for (const auto& child : t->body) {
            print_tree(child, depth + 1, file, var_or);
        }

        file << "}";
    }

    else if (auto v = dynamic_pointer_cast<Variables>(node)) {
            
        int inicializacia = 0;

        if (verific_var(var_or, v->name)) {
            inicializacia = 1;
        } else {
            var_or.push_back(v->name);
        }

        if (inicializacia) {
            file << v->name;
        } else {
            if (v->type != "auto" && !v->value.empty() && v->value[0] == "[") {
                file << "_list<" << clean_type(v->type) << "> " << v->name;
            } else {
                file << v->type << " " << v->name; 
            }
        }

        if (!v->value.empty()) {
            file << " = ";
            bool is_list_init = (v->value[0] == "[");
            for (const auto& val : v->value) {
                if (is_list_init) {
                    if (val == "[") file << "{";
                    else if (val == "]") file << "}";
                    else file << val;
                } else {
                    file << val;
                }
            }
            file << ";" << endl;
        }

        else if (!v->func.empty()) {
            file << " = " << v->func[0].name << "(";
            vector<string> clean_args;
            for (const auto& arg : v->func[0].arg_func) {
                if (arg != " " && arg != "," && !arg.empty()) {
                    clean_args.push_back(arg);
                }
            }
            for (size_t k = 0; k < clean_args.size(); ++k) {
                file << clean_args[k];
                if (k + 1 < clean_args.size()) {
                    file << ", ";
                }
            }
            file << ");" << endl;
        }
        else {
            file << ";" << endl;
        }

        if (!v->value.empty()) {

        }

        if (!v->func.empty()) {
            
        }
    }

    else if (auto call = dynamic_pointer_cast<Call_func>(node)) {
        file << indent << call->name << "(";
        vector<string> clean_args;
        for (const auto& arg : call->arg_func) {
            if (arg != " " && arg != "," && !arg.empty()) {
                clean_args.push_back(arg);
            }
        }
        for (size_t k = 0; k < clean_args.size(); ++k) {
            file << clean_args[k];
            if (k + 1 < clean_args.size()) {
                file << ", ";
            }
        }
        file << ");" << endl;
    }
    
    else if (auto r = dynamic_pointer_cast<Return_node>(node)) {
        file << indent << "return ";
        for (const auto& val : r->expr) {
            file << val;
        }
        file << ";" << endl;
    }

    else if (auto b = dynamic_pointer_cast<Break_node>(node)) {
        file << indent << "break;" << endl;
    }
    
    else if (auto c = dynamic_pointer_cast<Continue_node>(node)) {
        file << indent << "continue;" << endl;
    }
    
    else if (auto n = dynamic_pointer_cast<Nic_node>(node)) {
        file << indent << "; // nic" << endl;
    }

    else {
       
    }

    for (const auto& child : node->rekurzia) {
        print_tree(child, depth + 1, file, var_or);
    }
}



void Interpreter::print_ast(const AST& tree) {
    ofstream MyFile("origin.cpp");
    import_data(MyFile);
    print_baza(tree.rekurzia, MyFile);
    
    MyFile << endl;

    vector<string> dummy_vars;
    for (const auto& node : tree.rekurzia) {
        if (auto f = dynamic_pointer_cast<Func_create>(node)) {
            print_tree(node, 0, MyFile, dummy_vars);
            MyFile << endl;
        }
    }

    // 3. global
    MyFile << "int main() {" << endl;

    vector<string> variables_or;
    for (const auto& node : tree.rekurzia) {
        if (dynamic_pointer_cast<Func_create>(node)) {
            continue;
        }
        print_tree(node, 1, MyFile, variables_or); 
    }

    MyFile << "    return 0;" << endl;
    MyFile << "}" << endl;

    MyFile.close();
}
