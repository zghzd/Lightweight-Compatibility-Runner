#include "lcr.h"
typedef int (*LCrMainFunc)(int argc, const char* argv[]);
int main(int argc, char* argv[]) {
	//Usage:lcr <funcName> [<argus>...]
	if (argc < 2) {
		std::cout << "Usage:lcr <funcName> [<argus>...]" << std::endl;
		return -1;
	}
    std::string lcrfuncname = argv[1];
    lcrfuncname += ".dll";

    HMODULE hDll = LoadLibraryA(lcrfuncname.c_str());
    if (!hDll)
    {
        std::cerr << "Failed to load " << lcrfuncname << ", error: " << GetLastError() << std::endl;
        return 1;
    }

    LCrMainFunc pfnLcrMain = (LCrMainFunc)GetProcAddress(hDll, "lcrmain");
    if (!pfnLcrMain)
    {
        std::cerr << "Failed to find lcrmain in DLL" << std::endl;
        FreeLibrary(hDll);
        return 1;
    }

    int result = pfnLcrMain(argc, const_cast<const char**>(argv));
    std::cout << "lcrmain returned: " << result << std::endl;

    FreeLibrary(hDll);
    return 0;
}