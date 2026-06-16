#include "pch.h"
#include <chrono>
#include <sstream>
#include "unzip-APK.h"
#include "lcr_s_APK_androidmanifest.h"
#include "isFileContentEmpty.h"
#include "ConfigReader.h"
#include "getCanonicalPath.h"

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

static void prepareAPK(const std::string& apkPath, const std::string& libDir, const std::string& dexDir) {
    if (!isDirectoryNonEmpty(libDir)) {
        ExtractFromApk(apkPath.c_str(), libDir.c_str(), "lib/arm64-v8a/");
    }
    else {
        std::cout << "Will use cache.\n";
    }

    if (!isDirectoryNonEmpty(dexDir)) {
        for (int i = 1; ; ++i) {
            std::ostringstream name;
            if (i == 1) name << "classes.dex";
            else name << "classes" << i << ".dex";
            if (!ExtractFromApk(apkPath.c_str(), dexDir.c_str(), name.str().c_str()))
                break;
        }
    }
    else {
        std::cout << "Will use cache.\n";
    }
}
// Usage: lcr <dll> <packagename> [-d]
extern "C" __declspec(dllexport) int lcrmain(int argc, const char* argv[]) {
    std::ofstream lcrmain_log("lcrmain.log", std::ios::app);
    if (!lcrmain_log) return 3;
    lcrmain_log << "\n=====The application has started.=====\n";

    if (argc < 3) {
        std::cout << "Too few parameters.\n"
            "argu:\n\t<packagename> [<path>] [-d]\n"
            "The -d switch specifies whether to print debug information." << std::endl;
        return 1;
    }

    bool debuggable = false;
    std::string apk_packagename = argv[2];
    if (apk_packagename.find_first_of("\\/:*?\"<>|") != std::string::npos ||
        apk_packagename.front() == '.' || apk_packagename.back() == '.' ||
        apk_packagename.find("..") != std::string::npos) {
        std::cout << "Invalid package name: " << apk_packagename << std::endl;
        lcrmain_log << "Invalid package name: " << apk_packagename << std::endl;
        return 1;
    }

    for (int i = 3; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-d") {
            debuggable = true;
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
    std::string file_config = path_root + "Settings.config";
    std::filesystem::create_directories(path_root);
    std::filesystem::create_directories(path_data);
    std::filesystem::create_directories(path_lib);
    std::filesystem::create_directories(path_dex);
    if (isFileContentEmpty(file_config)) {
        std::fstream t_configFile(file_config, std::ios_base::out);
        t_configFile << "#This is the basic configuration file.\n#It's recommended to use English only.\n"
            << "file_ori_apk=\n#file_ori_apk: the path of the original APK.\n"
            << "path_apk_dec=\n#path_apk_dec: the path to the output files after decompiling.\n";
        std::cout << "The configuration file is located at " << getCanonicalPath(file_config) << std::endl;
        std::cout << "Basic setup complete, restart to continue.\n";
        lcrmain_log << "Basic setup complete, restart to continue.\n";
        return 101;
    }
    ConfigReader config_Settings;
    if (!config_Settings.load(file_config)) {
        std::cerr << "Failed to load config file." << std::endl;
        return 1;
    }
    std::string file_ori_apk, path_apk_dec, file_AndroidManifest;
    file_ori_apk = config_Settings.get("file_ori_apk");
    path_apk_dec = config_Settings.get("path_apk_dec");
    file_AndroidManifest = path_apk_dec + "AndroidManifest.xml";
    lcrmain_log << "Infor: {"
        << "path_workpath: " << path_workpath << ","
        << "path_root: " << path_root << ","
        << "path_tmp: " << path_tmp << ","
        << "path_dex: " << path_dex << ","
        << "path_lib: " << path_lib << ","
        << "path_data: " << path_data << ","
        << "apk_packagename: " << apk_packagename << ","
        << "path_apk_dec: " << path_apk_dec << ","
        << "file_ori_apk: " << file_ori_apk << ","
        << "file_AndroidManifest: " << file_AndroidManifest << ","
        << "}\n";
    if (path_apk_dec == "" ||
        file_ori_apk == "" ||
        !std::filesystem::exists(path_apk_dec) ||
        !std::filesystem::exists(file_ori_apk)||
        !std::filesystem::exists(file_AndroidManifest)
        ) {
        std::cout << "Configuration error.\n";
        lcrmain_log << "Configuration error.\n";
        return 1;
    }
    prepareAPK(file_ori_apk, path_lib, path_dex);

    //next: AndroidManifest.xml文本解析，资源预映射，dex/lib执行，无头准备，数据欺骗


    return 0;
}