#pragma once

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <Windows.h>

namespace utils {

std::string boolToString(bool);
bool stringToBool(std::string);
std::wstring stringToWString(const std::string&);
std::string replace(std::string, char);
std::string replace(std::string, const std::string&);
std::string trim(const std::string&, char = ' ');
std::string ltrim(const std::string&, char = ' ');
std::string rtrim(const std::string&, char = ' ');
std::vector<std::string> split(const std::string&, char = ' ');

} // namespace utils