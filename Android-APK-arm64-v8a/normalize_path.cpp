#include "pch.h"
#include "normalize_path.h"

void normalize_path(std::string& path) {
    for (char& c : path) {
        if (c == '\\') {
            c = '/';
        }
    }

    size_t write = 0;
    bool last_was_slash = false;
    for (size_t read = 0; read < path.size(); ++read) {
        char c = path[read];
        if (c == '/') {
            if (!last_was_slash) {
                path[write++] = '/';
                last_was_slash = true;
            }
        }
        else {
            path[write++] = c;
            last_was_slash = false;
        }
    }
    path.resize(write);

    if (path.size() > 1 && path.back() == '/') {
        path.pop_back();
    }
}