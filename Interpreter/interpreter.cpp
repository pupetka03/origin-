#include "interpreter.h"
#include <iostream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>

static std::vector<string> declared_classes;
static std::unordered_map<string, string> variable_types;
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
    file << "#include \"libraries/riadok.h\"" << endl;



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

string clean_expression_string(string expr) {
    if (expr == "nic") return "nullptr";
    if (expr.rfind("nas.", 0) == 0) {
        return "this->" + clean_expression_string(expr.substr(4));
    }
    if (expr == "nas") {
        return "this";
    }

    // Replace "is(A, B)" with "(std::dynamic_pointer_cast<B>(A) != nullptr)"
    size_t is_pos = 0;
    while ((is_pos = expr.find("is", is_pos)) != string::npos) {
        bool before_ok = (is_pos == 0 || (!isalnum(static_cast<unsigned char>(expr[is_pos - 1])) && expr[is_pos - 1] != '_'));
        size_t next_char = is_pos + 2;
        while (next_char < expr.length() && isspace(static_cast<unsigned char>(expr[next_char]))) {
            next_char++;
        }
        bool after_ok = (next_char < expr.length() && expr[next_char] == '(');
        if (before_ok && after_ok) {
            int depth = 1;
            size_t start_arg = next_char + 1;
            size_t end_pos = start_arg;
            while (end_pos < expr.length() && depth > 0) {
                if (expr[end_pos] == '(') depth++;
                else if (expr[end_pos] == ')') depth--;
                end_pos++;
            }
            if (depth == 0) {
                string inner = expr.substr(start_arg, end_pos - 1 - start_arg);
                size_t comma = inner.find(',');
                if (comma != string::npos) {
                    string arg1 = inner.substr(0, comma);
                    string arg2 = inner.substr(comma + 1);
                    auto trim = [](string s) {
                        size_t first = s.find_first_not_of(" \t\r\n");
                        if (first == string::npos) return string("");
                        size_t last = s.find_last_not_of(" \t\r\n");
                        return s.substr(first, (last - first + 1));
                    };
                    arg1 = trim(arg1);
                    arg2 = trim(arg2);
                    
                    string replacement = "(std::dynamic_pointer_cast<" + arg2 + ">(" + arg1 + ") != nullptr)";
                    expr.replace(is_pos, end_pos - is_pos, replacement);
                    is_pos += replacement.length();
                } else {
                    is_pos += 2;
                }
            } else {
                is_pos += 2;
            }
        } else {
            is_pos += 2;
        }
    }

    // Replace "cast(A, B)" with "std::dynamic_pointer_cast<B>(A)"
    size_t cast_pos = 0;
    while ((cast_pos = expr.find("cast", cast_pos)) != string::npos) {
        bool before_ok = (cast_pos == 0 || (!isalnum(static_cast<unsigned char>(expr[cast_pos - 1])) && expr[cast_pos - 1] != '_'));
        size_t next_char = cast_pos + 4;
        while (next_char < expr.length() && isspace(static_cast<unsigned char>(expr[next_char]))) {
            next_char++;
        }
        bool after_ok = (next_char < expr.length() && expr[next_char] == '(');
        if (before_ok && after_ok) {
            int depth = 1;
            size_t start_arg = next_char + 1;
            size_t end_pos = start_arg;
            while (end_pos < expr.length() && depth > 0) {
                if (expr[end_pos] == '(') depth++;
                else if (expr[end_pos] == ')') depth--;
                end_pos++;
            }
            if (depth == 0) {
                string inner = expr.substr(start_arg, end_pos - 1 - start_arg);
                size_t comma = inner.find(',');
                if (comma != string::npos) {
                    string arg1 = inner.substr(0, comma);
                    string arg2 = inner.substr(comma + 1);
                    auto trim = [](string s) {
                        size_t first = s.find_first_not_of(" \t\r\n");
                        if (first == string::npos) return string("");
                        size_t last = s.find_last_not_of(" \t\r\n");
                        return s.substr(first, (last - first + 1));
                    };
                    arg1 = trim(arg1);
                    arg2 = trim(arg2);
                    
                    string replacement = "std::dynamic_pointer_cast<" + arg2 + ">(" + arg1 + ")";
                    expr.replace(cast_pos, end_pos - cast_pos, replacement);
                    cast_pos += replacement.length();
                } else {
                    cast_pos += 4;
                }
            } else {
                cast_pos += 4;
            }
        } else {
            cast_pos += 4;
        }
    }
    
    // Replace "nas" with "this" checking for word boundaries
    size_t nas_pos = 0;
    while ((nas_pos = expr.find("nas", nas_pos)) != string::npos) {
        bool before_ok = (nas_pos == 0 || (!isalnum(static_cast<unsigned char>(expr[nas_pos - 1])) && expr[nas_pos - 1] != '_'));
        bool after_ok = (nas_pos + 3 == expr.length() || (!isalnum(static_cast<unsigned char>(expr[nas_pos + 3])) && expr[nas_pos + 3] != '_'));
        if (before_ok && after_ok) {
            expr.replace(nas_pos, 3, "this");
            nas_pos += 4; // skip "this"
        } else {
            nas_pos += 3; // skip "nas"
        }
    }

    // Replace "nic" with "nullptr" checking for word boundaries
    size_t nic_pos = 0;
    while ((nic_pos = expr.find("nic", nic_pos)) != string::npos) {
        bool before_ok = (nic_pos == 0 || (!isalnum(static_cast<unsigned char>(expr[nic_pos - 1])) && expr[nic_pos - 1] != '_'));
        bool after_ok = (nic_pos + 3 == expr.length() || (!isalnum(static_cast<unsigned char>(expr[nic_pos + 3])) && expr[nic_pos + 3] != '_'));
        if (before_ok && after_ok) {
            expr.replace(nic_pos, 3, "nullptr");
            nic_pos += 7; // skip "nullptr"
        } else {
            nic_pos += 3; // skip "nic"
        }
    }
    
    size_t dot_pos = 0;
    while ((dot_pos = expr.find('.', dot_pos)) != string::npos) {
        int start_pos = (int)dot_pos - 1;
        while (start_pos >= 0 && isspace(static_cast<unsigned char>(expr[start_pos]))) {
            start_pos--;
        }
        while (start_pos >= 0) {
            char c = expr[start_pos];
            if (c == ']') {
                int depth = 1;
                start_pos--;
                while (start_pos >= 0 && depth > 0) {
                    if (expr[start_pos] == ']') depth++;
                    else if (expr[start_pos] == '[') depth--;
                    start_pos--;
                }
            } else if (c == ')') {
                int depth = 1;
                start_pos--;
                while (start_pos >= 0 && depth > 0) {
                    if (expr[start_pos] == ')') depth++;
                    else if (expr[start_pos] == '(') depth--;
                    start_pos--;
                }
            } else if (isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '(' || c == ')' || c == '[' || c == ']') {
                start_pos--;
            } else if (isspace(static_cast<unsigned char>(c))) {
                start_pos--;
            } else {
                break;
            }
        }
        start_pos++;
        
        string prefix = expr.substr(start_pos, dot_pos - start_pos);
        string base_name = prefix;
        size_t bracket_pos = prefix.find('[');
        if (bracket_pos != string::npos) {
            base_name = prefix.substr(0, bracket_pos);
        }
        
        size_t first_alpha = base_name.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_");
        if (first_alpha != string::npos) {
            base_name = base_name.substr(first_alpha);
        } else {
            base_name = "";
        }
        while (!base_name.empty() && !isalnum(static_cast<unsigned char>(base_name.back())) && base_name.back() != '_') {
            base_name.pop_back();
        }
        
        string clean_pref = prefix;
        size_t pref_first = clean_pref.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_");
        if (pref_first != string::npos) {
            clean_pref = clean_pref.substr(pref_first);
        }
        while (!clean_pref.empty() && isspace(static_cast<unsigned char>(clean_pref.back()))) {
            clean_pref.pop_back();
        }
        
        bool is_class = false;
        if (variable_types.count(base_name)) {
            string var_type = variable_types[base_name];
            bool has_brackets = (prefix.find('[') != string::npos);
            if (has_brackets && var_type.rfind("list<", 0) == 0 && var_type.back() == '>') {
                var_type = var_type.substr(5, var_type.length() - 6);
            }
            if (std::find(declared_classes.begin(), declared_classes.end(), var_type) != declared_classes.end()) {
                is_class = true;
            }
        }
        if (clean_pref.rfind("nas", 0) == 0 || is_class) {
            expr.replace(dot_pos, 1, "->");
            dot_pos += 2; // skip "->"
        } else {
            dot_pos += 1; // skip "."
        }
    }
    return expr;
}

string clean_token(string token) {
    return clean_expression_string(token);
}

string clean_tokens_list(const vector<string>& tokens) {
    string joined = "";
    for (size_t i = 0; i < tokens.size(); ++i) {
        joined += tokens[i];
        if (i + 1 < tokens.size() && tokens[i] != "[" && tokens[i+1] != "]" && tokens[i] != "(" && tokens[i+1] != ")") {
            joined += " ";
        }
    }
    return clean_expression_string(joined);
}

string clean_type(string type) {
    if (std::find(declared_classes.begin(), declared_classes.end(), type) != declared_classes.end()) {
        return "std::shared_ptr<" + type + ">";
    }
    // If the type contains a colon, it's a dictionary shorthand: K:V -> dict<K, V>
    size_t colon_pos = type.find(':');
    if (colon_pos != string::npos) {
        string key_part = type.substr(0, colon_pos);
        string val_part = type.substr(colon_pos + 1);
        
        // Clean both parts first to handle any potential underscores
        key_part = clean_type(key_part);
        val_part = clean_type(val_part);
        
        return "dict<" + key_part + ", " + val_part + ">";
    }

    static const vector<pair<string, string>> replacements = {
        {"_int", "int"},
        {"_double", "double"},
        {"_string", "string"},
        {"_bool", "bool"},
        {"_float", "float"},
        {"_char", "char"},
        {"_void", "void"},
        {"_list", "list"},
        {"_dict", "dict"}
    };
    for (const auto& r : replacements) {
        size_t pos = 0;
        while ((pos = type.find(r.first, pos)) != string::npos) {
            type.replace(pos, r.first.length(), r.second);
            pos += r.second.length();
        }
    }
    
    // Convert [ and ] to < and > for templated types like dict[K, V]
    size_t bracket_open = type.find('[');
    if (bracket_open != string::npos) {
        type[bracket_open] = '<';
        size_t bracket_close = type.rfind(']');
        if (bracket_close != string::npos) {
            type[bracket_close] = '>';
        }
    }
    
    return type;
}

bool has_return_node(const shared_ptr<AST>& node) {
    if (!node) return false;
    if (dynamic_pointer_cast<Return_node>(node)) return true;
    
    if (auto j = dynamic_pointer_cast<Jak>(node)) {
        for (const auto& child : j->true_branch) {
            if (has_return_node(child)) return true;
        }
        for (const auto& child : j->false_branch) {
            if (has_return_node(child)) return true;
        }
    }
    if (auto c = dynamic_pointer_cast<Cykl>(node)) {
        for (const auto& child : c->body) {
            if (has_return_node(child)) return true;
        }
    }
    if (auto t = dynamic_pointer_cast<Takt>(node)) {
        for (const auto& child : t->body) {
            if (has_return_node(child)) return true;
        }
    }
    return false;
}

string type_of_fun(const vector<vector<string>>& control_return, const vector<shared_ptr<AST>>& body) {
    if (control_return.empty()) {
        for (const auto& node : body) {
            if (has_return_node(node)) {
                return "auto";
            }
        }
        return "void";
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
            file << type_of_fun(f->control_return, f->body) << " " << f->name << "(";
            for (size_t k = 0; k < clean_args.size(); ++k) {
                file << "T " << clean_args[k];
                if (k + 1 < clean_args.size()) {
                    file << ", ";
                }
            }
            file << ");" << endl;
        }
        else if (auto imp = dynamic_pointer_cast<Import>(node)) {
            file << "namespace " << imp->import_name << " {" << endl;
            print_baza(imp->import_body, file);
            file << "}" << endl;
        }
    }

    
}

void Interpreter::print_tree(const shared_ptr<AST>& node, int depth, ofstream &file, vector<string> &var_or) {
    if (!node) return;

    string indent(depth * 2, ' ');

    if (auto p = dynamic_pointer_cast<Print>(node)) {
        file << indent << "cout << ";
        vector<string> current_group;
        for (size_t i = 0; i < p->str.size(); ++i) {
            const auto& s = p->str[i];
            if (s == ",") {
                file << clean_tokens_list(current_group) << " << ' ' << ";
                current_group.clear();
            } else {
                current_group.push_back(s);
            }
        }
        if (!current_group.empty()) {
            file << clean_tokens_list(current_group);
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


        auto types = type_of_fun(f->control_return, f->body);
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
        file << indent << "if (" << clean_tokens_list(j->condition) << ") {" << endl;

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
        file << indent << "while (" << clean_tokens_list(c->condition) << ") {" << endl;

        vector<string> var_cycle = var_or;
        for (const auto& child : c->body) {
            print_tree(child, depth + 1, file, var_cycle);
        }

        file << indent << "}" << endl;
    }

    else if (auto t = dynamic_pointer_cast<Takt>(node)) {
        file << indent << "for(const auto &" << t->condition[0] << " : ";
        vector<string> rest_cond(t->condition.begin() + 1, t->condition.end());
        file << clean_tokens_list(rest_cond) << ") {" << endl;

        vector<string> var_loop = var_or;
        string loop_var = "";
        string old_type = "";
        bool had_old_type = false;
        if (!t->condition.empty()) {
            loop_var = t->condition[0];
            var_loop.push_back(loop_var);
            
            string collection_name = "";
            for (const auto& token : rest_cond) {
                size_t first_alpha = token.find_first_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_");
                if (first_alpha != string::npos) {
                    string id = token.substr(first_alpha);
                    while (!id.empty() && !isalnum(static_cast<unsigned char>(id.back())) && id.back() != '_') {
                        id.pop_back();
                    }
                    if (!id.empty()) {
                        collection_name = id;
                        break;
                    }
                }
            }
            if (!collection_name.empty() && variable_types.count(collection_name)) {
                string col_type = variable_types[collection_name];
                string elem_type = col_type;
                if (col_type.rfind("list<", 0) == 0 && col_type.back() == '>') {
                    elem_type = col_type.substr(5, col_type.length() - 6);
                }
                if (variable_types.count(loop_var)) {
                    old_type = variable_types[loop_var];
                    had_old_type = true;
                }
                variable_types[loop_var] = elem_type;
            }
        }

        for (const auto& child : t->body) {
            print_tree(child, depth + 1, file, var_loop);
        }

        if (!loop_var.empty()) {
            if (had_old_type) {
                variable_types[loop_var] = old_type;
            } else {
                variable_types.erase(loop_var);
            }
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
        else {
            int inicializacia = 0;
            string base_name = v->name;
            size_t bracket_pos = v->name.find('[');
            if (bracket_pos != string::npos) {
                base_name = v->name.substr(0, bracket_pos);
            }

            if (verific_var(var_or, base_name) || base_name.rfind("nas.", 0) == 0 || base_name.find('.') != string::npos) {
                inicializacia = 1;
            } else {
                var_or.push_back(v->name);
                if (v->type != "auto") {
                    if (!v->value.empty() && v->value[0] == "[" && v->type.find(':') == string::npos && v->type.find("dict") == string::npos) {
                        variable_types[v->name] = "list<" + v->type + ">";
                    } else {
                        variable_types[v->name] = v->type;
                    }
                } else if (!v->func.empty()) {
                    string func_name = v->func[0].name;
                    if (std::find(declared_classes.begin(), declared_classes.end(), func_name) != declared_classes.end()) {
                        variable_types[v->name] = func_name;
                    } else if (func_name == "cast") {
                        vector<string> clean_args;
                        for (const auto& arg : v->func[0].arg_func) {
                            if (arg != " " && arg != "," && !arg.empty()) {
                                clean_args.push_back(arg);
                            }
                        }
                        if (clean_args.size() > 1) {
                            variable_types[v->name] = clean_args[1];
                        }
                    }
                }
            }

            if (inicializacia) {
                file << clean_token(v->name);
            } else {
                if (v->type != "auto" && !v->value.empty() && v->value[0] == "[" && v->type.find(':') == string::npos && v->type.find("dict") == string::npos) {
                    file << "list<" << clean_type(v->type) << "> " << v->name;
                } else {
                    file << clean_type(v->type) << " " << v->name; 
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
                    else file << clean_token(val);
                } else {
                    file << clean_token(val);
                }
            }
            file << ";" << endl;
        }
        // Виводимо виклик функції (якщо він є)
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

            vector<string> clean_args;
            for (const auto& arg : v->func[0].arg_func) {
                if (arg != " " && arg != "," && !arg.empty()) {
                    clean_args.push_back(arg);
                }
            }

            if (v->func[0].name == "cast") {
                file << " = std::dynamic_pointer_cast<" << clean_args[1] << ">(" << clean_token(clean_args[0]) << ");" << endl;
            } else {
                if (use_variant_get) {
                    file << " = std::get<" << tuple_type << ">(" << v->func[0].name << "(";
                } else {
                    if (std::find(declared_classes.begin(), declared_classes.end(), v->func[0].name) != declared_classes.end()) {
                        file << " = std::make_shared<" << v->func[0].name << ">(";
                    } else {
                        file << " = " << clean_expression_string(v->func[0].name) << "(";
                    }
                }

                for (size_t k = 0; k < clean_args.size(); ++k) {
                    file << clean_token(clean_args[k]);
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
        }
        else {
            file << ";" << endl;
        }
    }

    else if (auto call = dynamic_pointer_cast<Call_func>(node)) {
        vector<string> clean_args;
        for (const auto& arg : call->arg_func) {
            if (arg != " " && arg != "," && !arg.empty()) {
                clean_args.push_back(arg);
            }
        }

        if (call->name == "cast") {
            file << indent << "std::dynamic_pointer_cast<" << clean_args[1] << ">(" << clean_token(clean_args[0]) << ");" << endl;
        } else {
            file << indent;
            if (std::find(declared_classes.begin(), declared_classes.end(), call->name) != declared_classes.end()) {
                file << "std::make_shared<" << call->name << ">(";
            } else {
                file << clean_expression_string(call->name) << "(";
            }
            for (size_t k = 0; k < clean_args.size(); ++k) {
                file << clean_token(clean_args[k]);
                if (k + 1 < clean_args.size()) {
                    file << ", ";
                }
            }
            file << ");" << endl;
        }
    }
    
    else if (auto r = dynamic_pointer_cast<Return_node>(node)) {
        file << indent << "return ";
        for (const auto& val : r->expr) {
            file << clean_token(val);
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
    else if (auto f = dynamic_pointer_cast<Import>(node)) { 
        for (const auto& child : f->import_body) {
            print_tree(child, depth + 1, file, var_or);
        }
    } else if (auto f = dynamic_pointer_cast<Typ>(node)) {
        file << indent << "class " << f->name;
        if (f->podoba != "false") {
            file << " : public " << f->podoba;
        }
        file << " {" << endl;
        file << indent << "public:" << endl;
        
        // Collect class variables (fields) so methods don't auto-declare them
        vector<string> class_fields;
        for (const auto& member : f->body) {
            if (auto v = dynamic_pointer_cast<Variables>(member)) {
                class_fields.push_back(v->name);
            }
        }
        
        // Print member variables (fields)
        for (const auto& member : f->body) {
            if (auto v = dynamic_pointer_cast<Variables>(member)) {
                file << indent << "  " << clean_type(v->type) << " " << v->name;
                if (!v->value.empty()) {
                    file << " = ";
                    bool is_list_init = (v->value[0] == "[");
                    for (const auto& val : v->value) {
                        if (is_list_init) {
                            if (val == "[") file << "{";
                            else if (val == "]") file << "}";
                            else file << clean_token(val);
                        } else {
                            file << clean_token(val);
                        }
                    }
                }
                file << ";" << endl;
            }
        }
        
        // Print methods and constructors
        for (const auto& member : f->body) {
            if (auto meth = dynamic_pointer_cast<Func_create>(member)) {
                vector<string> clean_args;
                for (const auto& arg : meth->arg_func) {
                    if (arg != " " && arg != "," && !arg.empty()) {
                        clean_args.push_back(arg);
                    }
                }
                
                file << indent << "  ";
                if (!clean_args.empty()) {
                    file << "template <";
                    for (size_t k = 0; k < clean_args.size(); ++k) {
                        file << "typename T" << k;
                        if (k + 1 < clean_args.size()) file << ", ";
                    }
                    file << "> ";
                }
                
                // If it is a constructor (method name matches class name)
                if (meth->name == f->name) {
                    file << meth->name << "(";
                } else {
                    if (clean_args.empty()) {
                        file << "virtual ";
                    }
                    file << type_of_fun(meth->control_return, meth->body) << " " << meth->name << "(";
                }
                
                for (size_t k = 0; k < clean_args.size(); ++k) {
                    file << "T" << k << " " << clean_args[k];
                    if (k + 1 < clean_args.size()) {
                        file << ", ";
                    }
                }
                file << ") {" << endl;
                
                // Setup variable scope inside the method
                vector<string> var_of_meth = class_fields;
                for (const auto& arg : clean_args) {
                    var_of_meth.push_back(arg);
                }
                var_of_meth.push_back("nas");
                
                // Print method body
                for (const auto& child : meth->body) {
                    print_tree(child, depth + 2, file, var_of_meth);
                }
                file << indent << "  }" << endl;
            }
        }
        
        file << indent << "  virtual ~" << f->name << "() = default;" << endl;
        file << indent << "};" << endl;
    }  

    else {
       
    }

    for (const auto& child : node->rekurzia) {
        print_tree(child, depth + 1, file, var_or);
    }
}



void Interpreter::print_ast(const AST& tree) {
    declared_classes.clear();
    variable_types.clear();
    for (const auto& node : tree.rekurzia) {
        if (auto t = dynamic_pointer_cast<Typ>(node)) {
            declared_classes.push_back(t->name);
        }
    }

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

    for (const auto& node : tree.rekurzia) {
        if (auto f = dynamic_pointer_cast<Typ>(node)) {
            print_tree(node, 0, MyFile, dummy_vars);
            MyFile << endl;
        }
    }

    for (const auto& node : tree.rekurzia) {
        if (auto f = dynamic_pointer_cast<Import>(node)) {
            MyFile << "namespace " << f->import_name << "{" << endl;
            print_tree(f, 0, MyFile, dummy_vars);
            MyFile << endl;
            MyFile << "}" << endl;
        }
    }


    // 3. global
    MyFile << "int main(int argc, char* argv[]) {" << endl;
    MyFile << "  myArray<string> arg;" << endl;
    MyFile << "  for (int i = 0; i < argc; ++i) {" << endl;
    MyFile << "    arg.append(argv[i]);" << endl;
    MyFile << "  }" << endl;

    vector<string> variables_or;
    variables_or.push_back("arg");
    for (const auto& node : tree.rekurzia) {
        if (dynamic_pointer_cast<Func_create>(node) || dynamic_pointer_cast<Import>(node) || dynamic_pointer_cast<Typ>(node)) {
            continue;
        }
        print_tree(node, 1, MyFile, variables_or); 
    }

    MyFile << "    return 0;" << endl;
    MyFile << "}" << endl;

    MyFile.close();
}
