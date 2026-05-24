#pragma once
#include <string_view>
#include "../lcr-android/android_APK.h"
namespace platforms::Android {
	inline constexpr std::string_view Android_str = "Android";
	enum class Androidfiletype {
		APK
	};
	inline constexpr std::string_view Androidfiletype_APK = "APK";
}
