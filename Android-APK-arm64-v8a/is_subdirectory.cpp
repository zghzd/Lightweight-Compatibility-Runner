#include "pch.h"
#include "is_subdirectory.h"
#include <filesystem>
#include <system_error>

int is_subdirectory(const std::string& root, const std::string& path) {
    try {
        namespace fs = std::filesystem;

        fs::path canonical_root = fs::weakly_canonical(fs::absolute(root));
        fs::path canonical_path = fs::weakly_canonical(fs::absolute(path));
        auto root_it = canonical_root.begin();
        auto path_it = canonical_path.begin();
        auto root_end = canonical_root.end();
        auto path_end = canonical_path.end();

        while (root_it != root_end && path_it != path_end) {
            if (*root_it != *path_it) {
                return -1;
            }
            ++root_it;
            ++path_it;
        }

        if (root_it == root_end) {
            return 0;
        }

        return -1;
    }
    catch (const std::exception&) {
        return -1;
    }
}