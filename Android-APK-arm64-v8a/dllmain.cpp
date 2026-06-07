#include "pch.h"
#include <chrono>
#include <sstream>
#include "unzip-APK.h"

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

static bool isDirectoryNonEmpty(const std::string& path) {
    if (!std::filesystem::exists(path) || !std::filesystem::is_directory(path))
        return false;
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_regular_file())
            return true;
    }
    return false;
}

// Usage: lcr <dll> <packagename> [<APK-file>] [-d]
extern "C" __declspec(dllexport) int lcrmain(int argc, const char* argv[]) {
    std::ofstream lcrmain_log("lcrmain.log", std::ios::app);
    if (!lcrmain_log) return 3;
    lcrmain_log << "\n=====The application has started.=====\n";

    if (argc < 3) {
        std::cout << "Too few parameters.\n"
            "argu:\n\t<packagename> [<your-APK-file>] [-d]\n"
            "The -d switch specifies whether to print debug information." << std::endl;
        return 1;
    }

    bool debuggable = false;
    std::string apk_packagename = argv[2];
    std::string ori_apk_path;

    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-d") {
            debuggable = true;
        }
        else if (ori_apk_path.empty()) {
            ori_apk_path = arg;
        }
        else {
            std::cout << "Unexpected argument: " << arg << std::endl;
            return 1;
        }
    }

    if (debuggable) {
        std::cout << "debuggable = true" << std::endl;
        lcrmain_log << "debuggable = true\n";
    }

    auto now = std::chrono::system_clock::now();
    auto timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    lcrmain_log << "timestamp: " << timestamp_ms << "\n";

    std::string path_workpath = "./Android/APK/arm64-v8a/";
    std::string path_tmp = path_workpath + "tmp/" + std::to_string(timestamp_ms) + "/";
    std::error_code ec_mktmppath;
    if (std::filesystem::create_directories(path_tmp, ec_mktmppath)) {
    }
    else if (!ec_mktmppath) {
    }
    else {
        lcrmain_log << "Error creating tmp directory:" << ec_mktmppath.message() << '\n';
        return 1;
    }

    std::string path_root = path_workpath + apk_packagename + "/";
    std::string path_dex = path_root + "dex/";
    std::string path_lib = path_root + "lib/arm64-v8a/";
    std::string path_data = path_root + "data/";
    std::string file_apk = path_root + "base.apk";

    bool cache_valid = false;
    if (std::filesystem::exists(file_apk) && std::filesystem::is_regular_file(file_apk)) {
        bool dex_nonempty = isDirectoryNonEmpty(path_dex);
        bool lib_nonempty = isDirectoryNonEmpty(path_lib);
        cache_valid = dex_nonempty || lib_nonempty;
    }

    if (cache_valid) {
        std::cout << "Cache valid for " << apk_packagename << ", skipping preparation." << std::endl;
        lcrmain_log << "Cache valid, skipping PREP APK.\n";
        goto prep_done;
    }

    if (ori_apk_path.empty()) {
        std::cout << "No valid cache and no APK file provided. Please provide APK file.\n";
        lcrmain_log << "No cache and no APK file.\n";
        return 1;
    }

    try {
        std::filesystem::create_directories(path_dex);
        std::filesystem::create_directories(path_lib);
        std::filesystem::create_directories(path_data);
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Failed to create directory: " << e.what() << '\n';
        lcrmain_log << "Failed to create directory: " << e.what() << '\n';
        return 1;
    }

    if (std::filesystem::exists(file_apk)) {
        if (std::filesystem::is_regular_file(file_apk)) {
            lcrmain_log << "Already exists: " << file_apk << ", Will use cached content.\n";
        }
        else {
            lcrmain_log << "Error: " << file_apk << ", Exists but is not a valid file.\n";
            std::cout << "Error: " << file_apk << ", Exists but is not a valid file.\n";
            return 1;
        }
    }
    else {
        std::filesystem::copy(ori_apk_path, file_apk, std::filesystem::copy_options::overwrite_existing);
        ExtractFromApk(file_apk.c_str(), path_lib.c_str(), "lib/arm64-v8a/");
        for (int i = 1; ; ++i) {
            std::ostringstream name;
            if (i == 1)
                name << "classes.dex";
            else
                name << "classes" << i << ".dex";
            if (!ExtractFromApk(file_apk.c_str(), path_dex.c_str(), name.str().c_str()))
                break;
        }
    }

prep_done:
    // PREP APK ,finished
    lcrmain_log << "PREP APK ,done.Infor: {"
        << "path_workpath: " << path_workpath << ","
        << "path_root: " << path_root << ","
        << "path_tmp: " << path_tmp << ","
        << "file_apk: " << file_apk << ","
        << "path_dex: " << path_dex << ","
        << "path_lib: " << path_lib << ","
        << "path_data: " << path_data << ","
        << "}\n";
    
    return 0;
}