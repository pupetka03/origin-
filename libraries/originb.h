#pragma once
#include <iostream>
#include <vector>
#include <string>

using namespace std;


//range
vector<int> range(int start, int end, int step) {
    vector<int> array; 
    for (int i = start; i < end; i=i+step) {
        array.push_back(i);
    }
    return array;
}

vector<int> range(int start, int end) {
    return range(start, end, 1);
}

vector<int> range(int end) {
    return range(0, end, 1);
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

        double scan_float() {
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
