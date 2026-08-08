#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <initializer_list>
#include <algorithm>
#include <chrono>
#include <variant>
#include <tuple>
#include <type_traits>
#include <map>

// Forward declaration of Variant
template <typename... Args>
class Variant;

using namespace std;

// Alias types to prevent Lexer renames
using int_t = int;
using double_t = double;
using float_t = float;
using char_t = char;
using bool_t = bool;
using string_t = std::string;

// ============================================================================
// 1. CORE META-PROGRAMMING HELPERS
// ============================================================================

// Check if type supports subscript operator []
template <typename T, typename = void>
struct has_subscript : std::false_type {};

template <typename T>
struct has_subscript<T, std::void_t<decltype(std::declval<T>()[0])>> : std::true_type {};

template <typename T>
inline constexpr bool has_subscript_v = has_subscript<T>::value;

// Extract element type for container, or type itself if not a container
template <typename T, typename = void>
struct element_type_or_self {
    using type = T;
};

template <typename T>
struct element_type_or_self<T, std::void_t<decltype(std::declval<T>()[0])>> {
    using type = std::decay_t<decltype(std::declval<T>()[0])>;
};

// Check if type contains an iterator type
template <typename T, typename = void>
struct iterator_type_or_dummy {
    using type = int*;
};

template <typename T>
struct iterator_type_or_dummy<T, std::void_t<typename T::const_iterator>> {
    using type = typename T::const_iterator;
};

// Template pack filtering for unique types
template <typename... Ts>
struct type_pack {};

template <typename Pack, typename T>
struct append_if_unique;

template <typename... Ts, typename T>
struct append_if_unique<type_pack<Ts...>, T> {
private:
    template <typename U, typename... Us>
    struct contains : std::false_type {};
    
    template <typename U, typename V, typename... Us>
    struct contains<U, V, Us...> : std::conditional_t<std::is_same_v<U, V>, std::true_type, contains<U, Us...>> {};

public:
    using type = std::conditional_t<contains<T, Ts...>::value,
                                    type_pack<Ts...>,
                                    type_pack<Ts..., T>>;
};

template <typename InputPack, typename OutputPack>
struct filter_duplicates;

template <typename OutputPack>
struct filter_duplicates<type_pack<>, OutputPack> {
    using type = OutputPack;
};

template <typename T, typename... Ts, typename OutputPack>
struct filter_duplicates<type_pack<T, Ts...>, OutputPack> {
    using type = typename filter_duplicates<type_pack<Ts...>, typename append_if_unique<OutputPack, T>::type>::type;
};

template <typename... Ts>
using unique_types_t = typename filter_duplicates<type_pack<Ts...>, type_pack<>>::type;

template <typename Pack>
struct pack_to_variant;

template <typename... Us>
struct pack_to_variant<type_pack<Us...>> {
    using type = std::conditional_t<sizeof...(Us) == 1,
                                    typename std::tuple_element<0, std::tuple<Us...>>::type,
                                    Variant<Us...>>;
};

// Check if type has tuple index element
template <typename T, size_t I, typename = void>
struct has_tuple_index : std::false_type {};

template <typename T, size_t I>
struct has_tuple_index<T, I, std::void_t<typename std::tuple_element<I, T>::type>> : std::true_type {};

template <typename T, size_t I>
inline constexpr bool has_tuple_index_v = has_tuple_index<T, I>::value;

// Metaprogramming tuple size search
template <typename... Ts>
struct find_tuple_size;

template <>
struct find_tuple_size<> {
    static constexpr size_t value = 1;
};

template <typename T, typename... Ts>
struct find_tuple_size<T, Ts...> {
private:
    template <typename U, typename = void>
    struct get_size : std::integral_constant<size_t, 1> {};

    template <typename U>
    struct get_size<U, std::void_t<decltype(std::tuple_size<U>::value)>> : std::integral_constant<size_t, std::tuple_size<U>::value> {};

public:
    static constexpr size_t value = get_size<T>::value > 1 ? get_size<T>::value : find_tuple_size<Ts...>::value;
};

// Metaprogramming tuple element type lookup
template <size_t I, typename... Ts>
struct find_tuple_element;

template <size_t I>
struct find_tuple_element<I> {
    using type = void;
};

template <size_t I, typename T, typename... Ts>
struct find_tuple_element<I, T, Ts...> {
private:
    template <typename U, typename = void>
    struct get_elem { using type = void; };

    template <typename U>
    struct get_elem<U, std::void_t<typename std::tuple_element<I, U>::type>> {
        using type = typename std::tuple_element<I, U>::type;
    };

public:
    using type = typename std::conditional_t<!std::is_same_v<typename get_elem<T>::type, void>,
                                             typename get_elem<T>::type,
                                             typename find_tuple_element<I, Ts...>::type>;
};

// ============================================================================
// 2. VARIANT TYPE IMPLEMENTATION
// ============================================================================

template <typename... Args>
class Variant : public std::variant<Args...> {
public:
    using std::variant<Args...>::variant;
    using std::variant<Args...>::operator=;

    auto operator[](size_t index) const {
        using ReturnType = typename pack_to_variant<unique_types_t<typename element_type_or_self<Args>::type...>>::type;
        return std::visit([index](const auto& val) -> ReturnType {
            using V = std::decay_t<decltype(val)>;
            if constexpr (has_subscript_v<V>) {
                return ReturnType(val[index]);
            } else {
                std::cerr << "Помилка виконання: Спроба індексації типу, що не підтримує []!" << std::endl;
                exit(1);
                return ReturnType{};
            }
        }, *this);
    }

    // Dynamic Range Iterator Support for takt loops over Variant
    class const_iterator {
    private:
        using IterVar = std::variant<typename iterator_type_or_dummy<Args>::type...>;
        IterVar iter;

    public:
        const_iterator(IterVar it) : iter(it) {}

        const_iterator& operator++() {
            std::visit([](auto& it) {
                if constexpr (!std::is_pointer_v<std::decay_t<decltype(it)>>) {
                    ++it;
                }
            }, iter);
            return *this;
        }

        bool operator!=(const const_iterator& other) const {
            return std::visit([&other](const auto& it1) {
                using I1 = std::decay_t<decltype(it1)>;
                return std::visit([&it1](const auto& it2) {
                    using I2 = std::decay_t<decltype(it2)>;
                    if constexpr (std::is_same_v<I1, I2>) {
                        return it1 != it2;
                    } else {
                        return true;
                    }
                }, other.iter);
            }, iter);
        }

        auto operator*() const {
            using ElemType = Variant<typename element_type_or_self<Args>::type...>;
            return std::visit([](const auto& it) -> ElemType {
                using I = std::decay_t<decltype(it)>;
                if constexpr (std::is_pointer_v<I>) {
                    std::cerr << "Помилка виконання: Спроба розіменування невалідного ітератора!" << std::endl;
                    exit(1);
                    return ElemType();
                } else {
                    return ElemType(*it);
                }
            }, iter);
        }
    };

    const_iterator begin() const {
        using IterVar = std::variant<typename iterator_type_or_dummy<Args>::type...>;
        return std::visit([](const auto& container) -> const_iterator {
            using C = std::decay_t<decltype(container)>;
            if constexpr (has_subscript_v<C>) {
                return const_iterator(IterVar(container.begin()));
            } else {
                std::cerr << "Помилка виконання: Спроба ітерації по типу, що не підтримує обхід!" << std::endl;
                exit(1);
                return const_iterator(IterVar(nullptr));
            }
        }, *this);
    }

    const_iterator end() const {
        using IterVar = std::variant<typename iterator_type_or_dummy<Args>::type...>;
        return std::visit([](const auto& container) -> const_iterator {
            using C = std::decay_t<decltype(container)>;
            if constexpr (has_subscript_v<C>) {
                return const_iterator(IterVar(container.end()));
            } else {
                return const_iterator(IterVar(nullptr));
            }
        }, *this);
    }
};

// Global Variant structured binding get<I> helper
template <size_t I, typename... Args>
auto get(const Variant<Args...>& var) {
    using ElemType = typename find_tuple_element<I, Args...>::type;
    return std::visit([](const auto& val) -> ElemType {
        using V = std::decay_t<decltype(val)>;
        if constexpr (has_tuple_index_v<V, I>) {
            return std::get<I>(val);
        } else {
            std::cerr << "Помилка виконання: Спроба розпаковки Variant з неактивним кортежем!" << std::endl;
            exit(1);
            return ElemType();
        }
    }, var);
}

// Specializations inside std namespace for Variant structured bindings
namespace std {
    template <typename... Args>
    struct tuple_size<Variant<Args...>> : integral_constant<size_t, find_tuple_size<Args...>::value> {};

    template <size_t I, typename... Args>
    struct tuple_element<I, Variant<Args...>> {
        using type = typename find_tuple_element<I, Args...>::type;
    };
}

// ============================================================================
// 3. LIST TYPE IMPLEMENTATION
// ============================================================================

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


// ============================================================================
// 4.1 Map
// ============================================================================

template <typename K, typename V>
class dict {
private:
    std::map<K, V> data;

public:
    dict() = default;

    dict(std::initializer_list<std::pair<const K, V>> init) : data(init) {}

    V& operator[](const K& key) {
        return data[key];
    }

    const V& at(const K& key) const {
        return data.at(key);
    }

    bool has_key(const K& key) const {
        return data.find(key) != data.end();
    }

    void erase(const K& key) {
        data.erase(key);
    }

    void clear() {
        data.clear();
    }

    size_t size() const {
        return data.size();
    }

    bool empty() const {
        return data.empty();
    }

    auto begin() { return data.begin(); }
    auto end() { return data.end(); }
    auto begin() const { return data.begin(); }
    auto end() const { return data.end(); }
};



// ============================================================================
// 4. TYPE CONVERSION FUNCTIONS (Casting)
// ============================================================================

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

// Advanced string casts
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
std::string _string(const Variant<Args...>& var) {
    return std::visit([](const auto& val) {
        return _string(val);
    }, var);
}

template <typename... Us>
std::string _string(const std::tuple<Us...>& t) {
    std::string res = "(";
    std::apply([&res](const auto&... args) {
        size_t n = 0;
        ((res += _string(args) + (++n < sizeof...(args) ? ", " : "")), ...);
    }, t);
    res += ")";
    return res;
}

// Variant casting functions
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

// ============================================================================
// 5. STREAM OSTREAM INSERTION OPERATORS (Printing)
// ============================================================================

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

template<typename T, typename... Args>
std::ostream& operator<<(std::ostream& os, const std::variant<T, Args...>& var) {
    os << _string(var);
    return os;
}

template<typename T, typename... Args>
std::ostream& operator<<(std::ostream& os, const Variant<T, Args...>& var) {
    os << _string(var);
    return os;
}

// ============================================================================
// 6. TYPE CHECKING FUNCTIONS
// ============================================================================

template <typename T>
bool is_int(const T&) {
    return std::is_same_v<std::decay_t<T>, int>;
}
template <typename... Args>
bool is_int(const std::variant<Args...>& var) {
    return std::holds_alternative<int>(var);
}
template <typename... Args>
bool is_int(const Variant<Args...>& var) {
    return std::holds_alternative<int>(var);
}

template <typename T>
bool is_double(const T&) {
    return std::is_same_v<std::decay_t<T>, double>;
}
template <typename... Args>
bool is_double(const std::variant<Args...>& var) {
    return std::holds_alternative<double>(var);
}
template <typename... Args>
bool is_double(const Variant<Args...>& var) {
    return std::holds_alternative<double>(var);
}

template <typename T>
bool is_string(const T&) {
    return std::is_same_v<std::decay_t<T>, std::string> || std::is_same_v<std::decay_t<T>, const char*>;
}
template <typename... Args>
bool is_string(const std::variant<Args...>& var) {
    return std::holds_alternative<std::string>(var);
}
template <typename... Args>
bool is_string(const Variant<Args...>& var) {
    return std::holds_alternative<std::string>(var);
}

template <typename T>
bool is_list(const T&) {
    return false;
}
template <typename U>
bool is_list(const myArray<U>&) {
    return true;
}
template <typename... Args>
bool is_list(const std::variant<Args...>& var) {
    return std::visit([](const auto& val) {
        return is_list(val);
    }, var);
}
template <typename... Args>
bool is_list(const Variant<Args...>& var) {
    return std::visit([](const auto& val) {
        return is_list(val);
    }, var);
}

template <typename T>
bool is_tuple(const T&) {
    return false;
}
template <typename... Us>
bool is_tuple(const std::tuple<Us...>&) {
    return true;
}
template <typename... Args>
bool is_tuple(const std::variant<Args...>& var) {
    return std::visit([](const auto& val) {
        return is_tuple(val);
    }, var);
}
template <typename... Args>
bool is_tuple(const Variant<Args...>& var) {
    return std::visit([](const auto& val) {
        return is_tuple(val);
    }, var);
}

// ============================================================================
// 7. STANDARD RUNTIME UTILITIES
// ============================================================================

// Range functions
template <typename T>
vector<T> range(T start, T end, T step) {
    vector<T> array; 
    for (T i = start; i < end; i = i + step) {
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

// Console input class and object
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
};

Console_Origin console;