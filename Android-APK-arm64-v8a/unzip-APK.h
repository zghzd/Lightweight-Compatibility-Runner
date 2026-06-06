#pragma once

//e.g. ExtractFromApk("app.apk", "./output", "AndroidManifest.xml")
//or ExtractFromApk("app.apk", "./output_libs", "lib/"),file will be extract at ./output_libs/lib, not ./output_libs/ !!!

/**
@param apkPath   APK/ZIP 文件路径
@param destDir   解压输出根目录（会自动创建）
@param pattern   匹配模式：
- 以 '/' 结尾：解压该目录下所有文件
- 否则：解压单个文件
 @return  成功返回 true，失败返回 false
 */
bool ExtractFromApk(const char* apkPath, const char* destDir, const char* pattern);