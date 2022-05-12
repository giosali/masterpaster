#pragma once

#include <filesystem>
#include <fstream>
#include <map>
#include <optional>
#include <string>
#include "utils.h"

namespace ini {

struct Value {
    bool boolean = false;
    std::string string;
};

class IniFile {
public:
    IniFile() = default;
    IniFile(const std::filesystem::path&);
    IniFile(const std::filesystem::path&, const char*);

    std::string& operator[](const std::string&);

    bool exists() const;
    void save() const;
    void load(const std::string&);
    void set(const std::string&, bool);
    void set(const std::string&, const char*);
    template <typename T>
    std::optional<Value> get(const std::string& key)
    {
        Value value;
        if (std::is_same<T, bool>::value) {
            value.boolean = utils::stringToBool(m_map[key]);
        } else if (std::is_same<T, std::string>::value) {
            value.string = m_map[key];
        }

        return value;
    }

private:
    void parse();
    void write(const std::map<std::string, std::string>&) const;
    void write(const std::string&) const;

    std::filesystem::path m_filePath;
    std::map<std::string, std::string> m_map;
};

} // namespace ini
