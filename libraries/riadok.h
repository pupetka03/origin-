#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include "originb.h"

class File {
private:
    std::string name;
    std::string mode;
    std::fstream subor;

public:
    File(std::string name, std::string mode = "r") : name(name), mode(mode) {
        otkryty();
    }

    File(File&&) = default;
    File& operator=(File&&) = default;

    File(const File&) = delete;
    File& operator=(const File&) = delete;

    bool otkryty() {
        if (!subor.is_open()) {
            std::ios_base::openmode open_flags = std::ios::in; // default is "r"
            if (mode == "w") {
                open_flags = std::ios::out | std::ios::trunc;
            } else if (mode == "a") {
                open_flags = std::ios::out | std::ios::app;
            } else if (mode == "r+") {
                open_flags = std::ios::in | std::ios::out;
            }
            
            subor.open(name, open_flags);
        }
        
        if (!subor.is_open()) {
            std::cerr << "Помилка роботи з файлом: " << name << " не вдалося відкрити!" << std::endl;
            return false;
        }
        return true;
    }

    void zakryty() {
        if (subor.is_open()) {
            subor.close();
        }
    }

    // Reads the entire file as a single string
    std::string citat() {
        if (!subor.is_open()) return "";
        subor.clear();
        subor.seekg(0, std::ios::beg);
        
        std::string content((std::istreambuf_iterator<char>(subor)), std::istreambuf_iterator<char>());
        return content;
    }

    // Reads the next single line
    std::string citat_riadok() {
        std::string line;
        if (subor.is_open() && std::getline(subor, line)) {
            return line;
        }
        return "";
    }

    // Reads all lines into a list of strings
    myArray<std::string> citat_riadky() {
        myArray<std::string> lines;
        std::string line;
        if (subor.is_open()) {
            subor.clear();
            subor.seekg(0, std::ios::beg);
            while (std::getline(subor, line)) {
                lines.append(line);
            }
        }
        return lines;
    }

    // Safe write of variables, constants and literals
    template <typename T>
    void zapis(const T& x) {
        if (subor.is_open()) {
            subor << x;
        }
    }

    // Writes newline
    void novy_riadok() {
        if (subor.is_open()) {
            subor << std::endl;
        }
    }

    ~File() {
        zakryty();
    }
};

// f_open helper
inline File f_open(std::string name, std::string mode = "r") {
    return File(name, mode);
}