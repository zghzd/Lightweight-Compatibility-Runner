# 使用方法——预处理APK
使用命令启动：`lcr Android-APK-arm64-v8a <packagename> [-d]`  
如果之前没有运行过对应`<packagename>`的APK或已删除缓存，你需要进行配置：  
- 使用上述命令启动LCR，由于配置文件不可用，程序将自动建立有默认配置项的配置文件。根据提示修改他们。
- 其中，你需要为`file_ori_apk`项配置原始APK的路径。
- 同时，你需要为`path_apk_dec`项配置反编译后的APK的路径。
- 剩余项按需填充或修改

以下是符合要求的反编译后的APK的路径所需满足的条件：
- `path_apk_dec/AndroidManifest.xml`必须存在且是标准的文本xml（而不是二进制axml）
- `path_apk_dec/res/`下的所有文件都已反编译（如果可以反编译），对于axml文件，必须已反编译且已映射arsc的原始资源。

*可以使用APKTOOL一类的程序反编译整个APK，之后将path_apk_dec指定为输出路径*
