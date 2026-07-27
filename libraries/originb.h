#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <initializer_list>
#include <algorithm>
#include <chrono>
#include <variant>

using namespace std;

using namespace std;

using int_t = int;
using double_t = double;
using float_t = float;
using char_t = char;
using bool_t = bool;
using string_t = std::string;


template<typename... Args>
std::ostream& operator<<(std::ostream& os, const std::variant<Args...>& var) {
    std::visit([&os](const auto& val) {
        os << val;
    }, var);
    return os;
}






//range
template <typename T>
vector<T> range(T start, T end, T step) {
    vector<T> array; 
    for (T i = start; i < end; i=i+step) {
        array.push_back(i);
    }
    return array;
}
template <typename T>
vector<T> range(T start, T end) {
    return range(start, end, T(1));
}
template <typename T>
vector<T> range(T end) {
    return range(T(0), end, T(1));
}



//scan
class Console_Origin {
    public:
        int scan_int(const string &text) {
            int cislo;
            cout << text;
            cin >> cislo;
            return cislo;
        }

        int scan_int() {
            int cislo;
            cin >> cislo;
            return cislo;
        }

        double scan_double(const string &text) {
            double cislo;
            cout << text;
            cin >> cislo;
            return cislo;
        }

        double scan_double() {
            double cislo;
            cin >> cislo;
            return cislo;
        }

        float scan_float(const string &text) {
            float cislo;
            cout << text;
            cin >> cislo;
            return cislo;
        }

        float scan_float() {
            float cislo;
            cin >> cislo;
            return cislo;
        }

    string scan_string(const string &text) {
        string riadok;
        cout << text;
        cin >> std::ws;
        getline(std::cin, riadok);
        return riadok;
    }

    string scan_string() {
        string riadok;
        cin >> std::ws;
        getline(std::cin, riadok);
        return riadok;
    }

}; Console_Origin console;



template <typename T>
int _int(T val) {
    return static_cast<int>(val);
}
inline int _int(const string& val) {
    return stoi(val);
}
inline int _int(const char* val) {
    return stoi(val);
}

template <typename T>
double _double(T val) {
    return static_cast<double>(val);
}
inline double _double(const string& val) {
    return stod(val);
}
inline double _double(const char* val) {
    return stod(val);
}

template <typename T>
string _string(T val) {
    return to_string(val);
}
inline string _string(const string& val) {
    return val;
}
inline string _string(const char* val) {
    return string(val);
}


// myArray implementation for lists
template <typename T>
class myArray {
private:
    std::vector<T> data;

public:
    myArray() = default;
    myArray(std::initializer_list<T> init) : data(init) {}

    void append(const T& val) {
        data.push_back(val);
    }

    T& operator[](size_t index) {
        return data[index];
    }
    const T& operator[](size_t index) const {
        return data[index];
    }

    size_t size() const {
        return data.size();
    }

    auto begin() { return data.begin(); }
    auto end() { return data.end(); }
    auto begin() const { return data.begin(); }
    auto end() const { return data.end(); }
};

template <typename T>
using list = myArray<T>;

template<typename T>
std::ostream& operator<<(std::ostream& os, const myArray<T>& arr) {
    os << "[";
    for (size_t i = 0; i < arr.size(); ++i) {
        os << arr[i];
        if (i + 1 < arr.size()) {
            os << ", ";
        }
    }
    os << "]";
    return os;
}