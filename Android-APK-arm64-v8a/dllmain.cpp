#include "pch.h"
#include "getAPKpackagename.h"
#include "unzip-APK.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
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

//Usage:lcr <funcName>[<argus>...]  argu: <your-APK-file> [-d]
extern "C" __declspec(dllexport) int lcrmain(int argc, const char* argv[]) {
    std::ofstream lcrmain_log("lcrmain.log", std::ios::app);
    if (!lcrmain_log) {
        return 3;
    }
    lcrmain_log << "\n     =====The application has started.=====\n";
    if (argc < 3) { std::cout << "Too few parameters.\nargu:\n\t<your-APK-file> [-d]\nThe -d switch specifies whether to print debug information." << std::endl; return 1; }
    bool debuggable = false;
    if (argc >= 4 && strcmp(argv[3], "-d") == 0) { debuggable = true; std::cout << "debuggable = true" << std::endl; lcrmain_log << "debuggable = true\n"; }
    auto now = std::chrono::system_clock::now();
    auto timestamp_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ).count();
    lcrmain_log << "timestamp: " << timestamp_ms << "\n";
    std::string ori_apk_path = argv[2];
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
    ExtractFromApk(ori_apk_path.c_str(), path_tmp.c_str(), "AndroidManifest.xml");
    std::string apk_packagename = getPackageName(path_tmp + "AndroidManifest.xml");
    if (apk_packagename == "") {
        std::cout << "Incorrect package name." << std::endl;
        lcrmain_log << "Incorrect package name.\n";
        return 1;
    }
    std::cout << "Packagename: " << apk_packagename << std::endl;
    lcrmain_log << "Packagename: " << apk_packagename << "\n";
    std::string path_root, path_dex, path_lib, path_data;
    path_root = path_workpath + apk_packagename + "/";
    path_dex = path_root + "dex/";
    path_lib = path_root + "lib/arm64-v8a/";
    path_data = path_root + "data/";
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
    std::string file_apk = path_root + "base.apk";
    if (std::filesystem::exists(file_apk)) {
    
        if (std::filesystem::is_regular_file(file_apk)){
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
    //PREP APK ,finished

    return 0;
}