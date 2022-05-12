#include "ini.h"

namespace ini {

IniFile::IniFile(const std::filesystem::path& filePath)
    : m_filePath(filePath)
{
    if (exists()) {
        parse();
    }
}

IniFile::IniFile(const std::filesystem::path& filePath, const char* text)
    : m_filePath(filePath)
{
    write(text);
    parse();
}

std::string& IniFile::operator[](const std::string& key)
{
    return m_map[key];
}

bool IniFile::exists() const
{
    return std::filesystem::exists(m_filePath);
}

void IniFile::save() const
{
    if (!m_map.empty()) {
        write(m_map);
    }
}

void IniFile::load(const std::string& text)
{
    std::filesystem::create_directories(m_filePath.parent_path());
    write(text);
    parse();
}

void IniFile::set(const std::string& key, bool value)
{
    m_map[key] = utils::boolToString(value);
}

void IniFile::set(const std::string& key, const char* value)
{
    m_map[key] = std::string(value);
}

void IniFile::parse()
{
    std::map<std::string, std::string> map;
    std::ifstream fs(m_filePath);
    if (fs.is_open()) {
        std::string section;
        std::string line;
        while (std::getline(fs, line)) {
            if (line.find('[') == 0 && line.rfind(']') == line.length() - 1) {
                section = line.substr(1, line.length() - 2);
                continue;
            }

            // Skips the line if it doesn't contain a '='.
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                continue;
            }

            std::string name = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            map[section + "." + utils::rtrim(name)] = utils::ltrim(value);
        }
    }

    fs.close();
    m_map = map;
}

void IniFile::write(const std::map<std::string, std::string>& map) const
{
    std::ofstream fs(m_filePath);
    if (fs.is_open()) {
        std::string section;
        for (const std::pair<const std::string, const std::string>& pair : map) {
            size_t dotPos = pair.first.find('.');
            std::string tempSection = pair.first.substr(0, dotPos);
            if (section != tempSection) {
                section = tempSection;
                fs << "[" << section << "]" << std::endl;
            }

            std::string first = pair.first.substr(dotPos + 1);
            fs << first << "=" << pair.second << std::endl;
        }
    }

    fs.close();
}

void IniFile::write(const std::string& text) const
{
    std::ofstream fs(m_filePath);
    if (fs.is_open()) {
        char bom[] = { (char)0xEF, (char)0xBB, (char)0xBF, '\0' };
        std::vector<std::string> lines = utils::split(text, '\n');
        for (int i = 0; i < lines.size(); i++) {
            fs << utils::trim(utils::replace(lines[i], std::string(bom)), '\r') << std::endl;
        }
    }

    fs.close();
}

} // namespace ini