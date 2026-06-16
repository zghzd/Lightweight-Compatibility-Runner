#pragma once
#include <fstream>
#include <string>
#include <algorithm>

inline bool isFileContentEmpty(const std::string& path) {
    std::ifstream file(path);
    if (!file) return true;
    std::string content((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
    return std::all_of(content.begin(), content.end(), [](char c) {
        return std::isspace(static_cast<unsigned char>(c));
        });
}