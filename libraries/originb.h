#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <initializer_list>
#include <algorithm>
#include <chrono>
#include <variant>

template <typename... Args>
class Variant;

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

    template <typename... Args>
    void append(const std::variant<Args...>& var) {
        std::visit([this](const auto& val) {
            using V = std::decay_t<decltype(val)>;
            if constexpr (std::is_convertible_v<V, T>) {
                this->data.push_back(static_cast<T>(val));
            } else {
                std::cerr << "Помилка виконання: Несумісний тип при додаванні в список!" << std::endl;
                exit(1);
            }
        }, var);
    }

    template <typename... Args>
    void append(const Variant<Args...>& var) {
        std::visit([this](const auto& val) {
            using V = std::decay_t<decltype(val)>;
            if constexpr (std::is_convertible_v<V, T>) {
                this->data.push_back(static_cast<T>(val));
            } else {
                std::cerr << "Помилка виконання: Несумісний тип при додаванні в список!" << std::endl;
                exit(1);
            }
        }, var);
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

template <typename T>
std::string _string(const myArray<T>& arr) {
    std::string res = "[";
    for (size_t i = 0; i < arr.size(); ++i) {
        res += _string(arr[i]);
        if (i + 1 < arr.size()) {
            res += ", ";
        }
    }
    res += "]";
    return res;
}

template <typename... Args>
std::string _string(const std::variant<Args...>& var) {
    return std::visit([](const auto& val) {
        return _string(val);
    }, var);
}

template <typename... Args>
int _int(const std::variant<Args...>& var) {
    return std::visit([](const auto& val) {
        using V = std::decay_t<decltype(val)>;
        if constexpr (std::is_convertible_v<V, int> || std::is_same_v<V, std::string> || std::is_same_v<V, const char*>) {
            return _int(val);
        } else {
            std::cerr << "Помилка виконання: Неможливо перетворити цей тип на int!" << std::endl;
            exit(1);
            return 0;
        }
    }, var);
}

template <typename... Args>
double _double(const std::variant<Args...>& var) {
    return std::visit([](const auto& val) {
        using V = std::decay_t<decltype(val)>;
        if constexpr (std::is_convertible_v<V, double> || std::is_same_v<V, std::string> || std::is_same_v<V, const char*>) {
            return _double(val);
        } else {
            std::cerr << "Помилка виконання: Неможливо перетворити цей тип на double!" << std::endl;
            exit(1);
            return 0.0;
        }
    }, var);
}

// Template checks for subscript support
template <typename T, typename = void>
struct has_subscript : std::false_type {};

template <typename T>
struct has_subscript<T, std::void_t<decltype(std::declval<T>()[0])>> : std::true_type {};

template <typename T>
inline constexpr bool has_subscript_v = has_subscript<T>::value;

template <typename T, typename = void>
struct element_type_or_self {
    using type = T;
};

template <typename T>
struct element_type_or_self<T, std::void_t<decltype(std::declval<T>()[0])>> {
    using type = std::decay_t<decltype(std::declval<T>()[0])>;
};

template <typename... Args>
class Variant : public std::variant<Args...> {
public:
    using std::variant<Args...>::variant;
    using std::variant<Args...>::operator=;

    auto operator[](size_t index) const {
        using ReturnType = Variant<typename element_type_or_self<Args>::type...>;
        return std::visit([index](const auto& val) -> ReturnType {
            using V = std::decay_t<decltype(val)>;
            if constexpr (has_subscript_v<V>) {
                return ReturnType(val[index]);
            } else {
                std::cerr << "Помилка виконання: Спроба індексації типу, що не підтримує []!" << std::endl;
                exit(1);
                return ReturnType();
            }
        }, *this);
    }
};

template<typename... Args>
std::ostream& operator<<(std::ostream& os, const Variant<Args...>& var) {
    std::visit([&os](const auto& val) {
        os << val;
    }, var);
    return os;
}

template <typename... Args>
std::string _string(const Variant<Args...>& var) {
    return std::visit([](const auto& val) {
        return _string(val);
    }, var);
}

template <typename... Args>
int _int(const Variant<Args...>& var) {
    return std::visit([](const auto& val) {
        using V = std::decay_t<decltype(val)>;
        if constexpr (std::is_convertible_v<V, int> || std::is_same_v<V, std::string> || std::is_same_v<V, const char*>) {
            return _int(val);
        } else {
            std::cerr << "Помилка виконання: Неможливо перетворити цей тип на int!" << std::endl;
            exit(1);
            return 0;
        }
    }, var);
}

template <typename... Args>
double _double(const Variant<Args...>& var) {
    return std::visit([](const auto& val) {
        using V = std::decay_t<decltype(val)>;
        if constexpr (std::is_convertible_v<V, double> || std::is_same_v<V, std::string> || std::is_same_v<V, const char*>) {
            return _double(val);
        } else {
            std::cerr << "Помилка виконання: Неможливо перетворити цей тип на double!" << std::endl;
            exit(1);
            return 0.0;
        }
    }, var);
}