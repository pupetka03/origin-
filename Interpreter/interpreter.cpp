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
    file << "#include <tuple>" << endl;
    file << "#include <variant>" << endl;
    

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
    static const vector<pair<string, string>> replacements = {
        {"_int", "int"},
        {"_double", "double"},
        {"_string", "string"},
        {"_bool", "bool"},
        {"_float", "float"},
        {"_char", "char"},
        {"_void", "void"},
        {"_list", "list"}
    };
    for (const auto& r : replacements) {
        size_t pos = 0;
        while ((pos = type.find(r.first, pos)) != string::npos) {
            type.replace(pos, r.first.length(), r.second);
            pos += r.second.length();
        }
    }
    return type;
}

string type_of_fun(const vector<vector<string>>& control_return) {
    if (control_return.empty()) {
        return "auto";
    }
    
    vector<string> alt;
    for(const auto &kod : control_return) {
        if (kod.size() > 1) {
            string tuple_type = "std::tuple<";
            for(size_t i = 0; i < kod.size(); i++) {
                tuple_type += clean_type(kod[i]);
                if (i + 1 < kod.size()) tuple_type += ", ";
            }
            tuple_type += ">";
            alt.push_back(tuple_type);
        }
        else {
            // поодинокий тип
            alt.push_back(clean_type(kod[0]));
        }
    }

    if (alt.size() == 1) {
        return alt[0]; 
    }

    string variant_type = "Variant<";
    for (size_t i = 0; i < alt.size(); ++i) {
        variant_type += alt[i];
        if (i + 1 < alt.size()) variant_type += ", ";
    }
    variant_type += ">";


    return variant_type;
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
            file << type_of_fun(f->control_return) << " " << f->name << "(";
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
            if (s == ",") {
                file << " << ' ' << ";
            } else {
                file << s;
            }

        }
        
        file << " << endl;" << endl;
    }
    

    else if (auto f = dynamic_pointer_cast<Func_create>(node)) {

        /*
        for (int i = 0; i < f->control_return.size(); i++) {
            for (int y = 0; y < f->control_return[i].size(); y++) {
                cout << f->control_return[i][y] << endl;
            }
        }
        */


        auto types = type_of_fun(f->control_return);
        //cout << types << endl;
    
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
        file << types <<" "<<  f->name << "(";
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

        vector<string> var_true = var_or;
        for (const auto& child : j->true_branch) {
            print_tree(child, depth + 1, file, var_true);
        }

        file << indent << "}" << endl;

        if (!j->false_branch.empty()) {
            if (j->false_branch.size() == 1 && dynamic_pointer_cast<Jak>(j->false_branch[0])) {
                file << indent << "else ";
                print_tree(j->false_branch[0], depth, file, var_or);
            } else {
                file << indent << "else {" << endl;
                vector<string> var_false = var_or;
                for (const auto& child : j->false_branch) {
                    print_tree(child, depth + 1, file, var_false);
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

        vector<string> var_cycle = var_or;
        for (const auto& child : c->body) {
            print_tree(child, depth + 1, file, var_cycle);
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

        vector<string> var_loop = var_or;
        if (!t->condition.empty()) {
            var_loop.push_back(t->condition[0]);
        }
        for (const auto& child : t->body) {
            print_tree(child, depth + 1, file, var_loop);
        }

        file << indent << "}" << endl;
    }

    else if (auto v = dynamic_pointer_cast<Variables>(node)) {
        file << indent;
        
        // Якщо це розпаковка кортежу (Structured Binding)
        if (!v->unpack_vars.empty()) {
            file << "auto [";
            for (size_t i = 0; i < v->unpack_vars.size(); ++i) {
                file << v->unpack_vars[i];
                if (i + 1 < v->unpack_vars.size()) {
                    file << ", ";
                }
            }
            file << "]";
        } 
        // Якщо це звичайне оголошення змінної
        else {
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
                file << "list<" << clean_type(v->type) << "> " << v->name;
            } else {
                    file << v->type << " " << v->name; 
                }
            }
        }

        // Виводимо значення (якщо воно є)
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
        // Виводимо виклик функції (якщо він є)
        else if (!v->func.empty()) {
            bool use_variant_get = false;
            string tuple_type = "";
            
            if (!v->unpack_vars.empty() && function_returns.count(v->func[0].name)) {
                const auto& control_return = function_returns[v->func[0].name];
                if (control_return.size() > 1) { // It's a variant type
                    for (const auto& alt : control_return) {
                        if (alt.size() == v->unpack_vars.size()) {
                            use_variant_get = true;
                            if (alt.size() > 1) {
                                tuple_type = "std::tuple<";
                                for (size_t i = 0; i < alt.size(); i++) {
                                    tuple_type += clean_type(alt[i]);
                                    if (i + 1 < alt.size()) tuple_type += ", ";
                                }
                                tuple_type += ">";
                            } else {
                                tuple_type = clean_type(alt[0]);
                            }
                            break;
                        }
                    }
                }
            }

            if (use_variant_get) {
                file << " = std::get<" << tuple_type << ">(" << v->func[0].name << "(";
            } else {
                file << " = " << v->func[0].name << "(";
            }

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

            if (use_variant_get) {
                file << "));" << endl;
            } else {
                file << ");" << endl;
            }
        }
        else {
            file << ";" << endl;
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
    function_returns.clear();
    for (const auto& node : tree.rekurzia) {
        if (auto f = dynamic_pointer_cast<Func_create>(node)) {
            function_returns[f->name] = f->control_return;
        }
    }

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
