#pragma once
#include <string>
#include <filesystem>
std::string getCanonicalPath(const std::string& path) {
    try {
        std::filesystem::path p(path);
        std::filesystem::path canonPath = std::filesystem::canonical(p);
        return canonPath.string();
    }
    catch (const std::filesystem::filesystem_error& e) {
        return "";
    }
}