#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <unordered_map>
#include <Windows.h>
#include "ini.h"

namespace io {

namespace {

    static const std::string appDataDirectoryName = "MasterPaster";
    static const std::string settingsIniFilename = "Settings.ini";

} // namespace

namespace settings {

    const static std::string runAtStartup = "Settings.runAtStartup";

} // namespace settings

std::filesystem::path getAppDataDirectoryPath();
std::filesystem::path getSettingsIniPath();
std::string getIniResource(uint32_t);

} // namespace io
