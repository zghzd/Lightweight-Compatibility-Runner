#include "lcr.h"

int main(int argc, char* argv[]) {
	//Usage:lcr <Platform> <FILE_TYPE> <file_path> [<file_path> <file_path> ...]
	if (argc < 4) {
		std::cout << "Usage:lcr <Platform> <FILE_TYPE> <file_path> [<file_path> <file_path> ...]" << std::endl;
		return -1;
	}
	std::vector<std::string> files(argv + 3, argv + argc);
	std::string_view platform = argv[1];
	std::string_view fileType = argv[2];
	static int returnCode;
	//下方判断平台和文件类型，直接调用对应功能
	if (platform == platforms::Android::Android_str) {
		if (fileType == platforms::Android::Androidfiletype_APK) {
			returnCode = procAndroid_APK(files);
		}
		else { std::cout << "Unsupported type" << std::endl; return -1; }
	}
	else { std::cout << "Unsupported platform" << std::endl; return -1; }

	return returnCode;
}