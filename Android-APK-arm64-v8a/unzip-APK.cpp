#include "pch.h"

#include <iostream>
#include <string>
#include <filesystem>

bool ExtractFromApk(const char* apkPath, const char* destDir, const char* pattern) {
    if (!apkPath || !destDir || !pattern) {
        std::cerr << "Invalid parameters." << std::endl;
        return false;
    }

    mz_zip_archive zip = {};
    if (!mz_zip_reader_init_file(&zip, apkPath, 0)) {
        std::cerr << "Failed to open archive: " << apkPath << std::endl;
        return false;
    }

    mz_uint numFiles = mz_zip_reader_get_num_files(&zip);
    bool isDirPattern = (pattern[strlen(pattern) - 1] == '/');
    bool anyMatched = false;
    std::string patternStr(pattern);

    std::error_code ec;
    std::filesystem::create_directories(destDir, ec);

    for (mz_uint i = 0; i < numFiles; ++i) {
        mz_zip_archive_file_stat stat;
        if (!mz_zip_reader_file_stat(&zip, i, &stat)) {
            std::cerr << "Warning: Failed to get stat for entry " << i << std::endl;
            continue;
        }

        const char* entryName = stat.m_filename;
        bool match = false;

        if (isDirPattern) {
            if (strncmp(entryName, patternStr.c_str(), patternStr.size()) == 0) {
                if (!mz_zip_reader_is_file_a_directory(&zip, i)) {
                    match = true;
                }
            }
        }
        else {
            if (strcmp(entryName, patternStr.c_str()) == 0 && !mz_zip_reader_is_file_a_directory(&zip, i)) {
                match = true;
            }
        }

        if (match) {
            anyMatched = true;
            std::string outName = entryName;
            if (isDirPattern) {
                outName = std::filesystem::path(entryName).filename().string();
            }
            std::string outPath = std::string(destDir) + "/" + outName;

            std::filesystem::path outFilePath(outPath);
            std::filesystem::create_directories(outFilePath.parent_path(), ec);
            if (ec) {
                std::cerr << "Failed to create directory: " << outFilePath.parent_path() << std::endl;
                mz_zip_reader_end(&zip);
                return false;
            }

            if (!mz_zip_reader_extract_to_file(&zip, i, outPath.c_str(), 0)) {
                std::cerr << "Failed to extract: " << entryName << std::endl;
                mz_zip_reader_end(&zip);
                return false;
            }

            std::cout << "Extracted: " << entryName << " -> " << outPath << std::endl;
        }
    }

    mz_zip_reader_end(&zip);

    if (!anyMatched) {
        std::cerr << "No matching entry found for pattern: " << pattern << std::endl;
        return false;
    }
    return true;
}