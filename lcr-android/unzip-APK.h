#pragma once
bool ExtractFromApk(const char* apkPath, const char* destDir, const char* pattern);

//e.g. ExtractFromApk("app.apk", "./output", "AndroidManifest.xml")
//or ExtractFromApk("app.apk", "./output_libs", "lib/"),file will be extract at ./output_libs/lib, not ./output_libs/ !!!