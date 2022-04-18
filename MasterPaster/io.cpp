#include "io.h"

namespace io {

namespace {

    const std::string appDataDirectoryName = "MasterPaster";

} // namespace

ConfigurationFile::ConfigurationFile()
    : m_delimiter()
    , m_filePath()
    , m_settings()
{
}

ConfigurationFile::ConfigurationFile(const std::string& filename, const char& delimiter)
    : m_delimiter(delimiter)
    , m_filePath()
    , m_settings()
{
    std::filesystem::path applicationAppDataDirectoryPath = std::filesystem::path(getAppDataDirectoryPath()) / appDataDirectoryName;
    std::filesystem::create_directory(applicationAppDataDirectoryPath);
    m_filePath = applicationAppDataDirectoryPath / filename;
    parse();
}

std::string& ConfigurationFile::operator[](const std::string& key)
{
    return m_settings[key];
}

bool ConfigurationFile::exists() const
{
    return std::filesystem::exists(m_filePath);
}

void ConfigurationFile::save() const
{
    if (!m_settings.empty()) {
        write(m_settings);
    }
}

void ConfigurationFile::overwrite(const std::unordered_map<std::string, std::string>& umap) const
{
    write(umap);
}

void ConfigurationFile::write(const std::unordered_map<std::string, std::string>& umap) const
{
    std::ofstream fs(m_filePath);
    if (fs.is_open()) {
        for (const std::pair<const std::string, std::string>& pair : umap) {
            std::string first = pair.first;
            first.push_back(m_delimiter);
            fs << first + pair.second << std::endl;
        }
    }
}

void ConfigurationFile::parse()
{
    std::unordered_map<std::string, std::string> umap;
    std::ifstream fs(m_filePath);
    std::string line;
    if (fs.is_open()) {
        while (std::getline(fs, line)) {
            size_t pos = line.find(m_delimiter);

            // Skips the line if it doesn't contain the delimiter.
            if (pos == std::string::npos) {
                continue;
            }

            umap[line.substr(0, pos)] = line.substr(pos + 1, std::string::npos);
        }
    }

    fs.close();
    m_settings = umap;
}

std::string getAppDataDirectoryPath()
{
    char* buffer = nullptr;
    std::string path;
    errno_t err = _dupenv_s(&buffer, NULL, "APPDATA");
    if (!err && buffer != NULL) {
        path = std::string(buffer);
    }

    free(buffer);
    return path;
}

std::unordered_map<std::string, std::string> createSettings()
{
    std::unordered_map<std::string, std::string> umap;
    umap["runAtStartup"] = "true";
    return umap;
}

} // namespace io