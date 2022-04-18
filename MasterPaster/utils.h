#pragma once

#include <algorithm>
#include <sstream>
#include <string>
#include <Windows.h>

namespace utils {

bool stringToBool(std::string);
std::string boolToString(bool);
std::wstring stringToWString(const std::string&);

} // namespace utils