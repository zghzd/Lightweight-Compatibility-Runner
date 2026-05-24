#include "pch.h"
#include "android_APK.h"
#include <iostream>
int procAndroid_APK(std::vector<std::string> files) {
	std::string file_Android_APK = files[0];
	if (files.size() != 1) {
		std::cout << "Warning: Only one file will be processed:" << file_Android_APK << std::endl;
	}
	//...
}