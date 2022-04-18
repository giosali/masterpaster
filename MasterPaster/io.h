#pragma once

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

namespace io {

class ConfigurationFile {
public:
    ConfigurationFile();
    ConfigurationFile(const std::string&, const char&);

    std::string& operator[](const std::string&);

    bool exists() const;
    void save() const;
    void overwrite(const std::unordered_map<std::string, std::string>&) const;

private:
    void write(const std::unordered_map<std::string, std::string>&) const;
    void parse();

    char m_delimiter;
    std::filesystem::path m_filePath;
    std::unordered_map<std::string, std::string> m_settings;
};

std::string getAppDataDirectoryPath();

std::unordered_map<std::string, std::string> createSettings();

} // namespace io
