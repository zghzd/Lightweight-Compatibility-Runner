#pragma once
#pragma once
#include <string>
#include <vector>

struct lcr_s_APK_feature {
    std::string name;
    std::string version;
    bool required;
};

struct lcr_s_APK_metadata {
    std::string name;
    std::string resource;
    std::string value;
};

struct lcr_s_APK_intentfilter {
    std::vector<std::pair<std::string, std::string>> actions;
    std::vector<std::pair<std::string, std::string>> categories;
    std::vector<std::tuple<std::string, std::string, std::string>> data;
};

struct lcr_s_APK_activity {
    std::string name;
    bool exported;
    std::string theme;
    std::string label;
    std::string icon;
    std::string process;
    std::string screenOrientation;
    std::string configChanges;
    bool resizeableActivity;
    bool hardwareAccelerated;
    std::string launchMode;
    std::vector<lcr_s_APK_intentfilter> intentFilters;
    std::vector<lcr_s_APK_metadata> metaData;
};

struct lcr_s_APK_activityalias {
    std::string name;
    std::string targetActivity;
    bool exported;
    std::vector<lcr_s_APK_intentfilter> intentFilters;
    std::vector<lcr_s_APK_metadata> metaData;
};

struct lcr_s_APK_provider {
    std::string name;
    std::string authorities;
    bool exported;
    bool grantUriPermissions;
    std::vector<lcr_s_APK_metadata> metaData;
};

struct lcr_s_APK_receiver {
    std::string name;
    bool exported;
    std::vector<lcr_s_APK_intentfilter> intentFilters;
    std::vector<lcr_s_APK_metadata> metaData;
};

struct lcr_s_APK_service {
    std::string name;
    bool exported;
    bool enabled;
    std::string process;
    std::vector<lcr_s_APK_intentfilter> intentFilters;
    std::vector<lcr_s_APK_metadata> metaData;
};

struct lcr_s_APK_application {
    std::string appComponentFactory;
    std::string icon;
    std::string label;
    std::string name;
    bool requestLegacyExternalStorage;
    bool allowBackup;
    std::string theme;
    bool extractNativeLibs;
    bool usesCleartextTraffic;
    std::vector<lcr_s_APK_activity> activities;
    std::vector<lcr_s_APK_activityalias> activityAliases;
    std::vector<lcr_s_APK_provider> providers;
    std::vector<lcr_s_APK_receiver> receivers;
    std::vector<lcr_s_APK_service> services;
    std::vector<lcr_s_APK_metadata> metaData;
};

struct lcr_s_APK_androidmanifest {
    std::string packagename;
    std::string compileSdkVersion;
    std::string compileSdkVersionCodename;
    std::string platformBuildVersionCode;
    std::string platformBuildVersionName;
    std::vector<std::string> permissions;
    std::vector<lcr_s_APK_feature> features;
    std::vector<lcr_s_APK_metadata> metaData;
    lcr_s_APK_application application;
};