#include "io.h"

namespace io {

std::filesystem::path getAppDataDirectoryPath()
{
    char* buffer = nullptr;
    std::string path;
    errno_t err = _dupenv_s(&buffer, NULL, "APPDATA");
    if (!err && buffer != NULL) {
        path = std::string(buffer);
    }

    free(buffer);
    return std::filesystem::path(path);
}

std::filesystem::path getSettingsIniPath()
{
    return getAppDataDirectoryPath() / appDataDirectoryName / settingsIniFilename;
}

std::string getIniResource(uint32_t resource)
{
    HINSTANCE hInstance = GetModuleHandle(NULL);
    HRSRC hResource = FindResource(hInstance, MAKEINTRESOURCE(resource), L"INI");
    if (hResource == NULL) {
        return "";
    }

    HGLOBAL hResourceData = LoadResource(hInstance, hResource);
    if (hResourceData == NULL) {
        return "";
    }

    return std::string(static_cast<const char*>(LockResource(hResourceData)));
}

} // namespace io