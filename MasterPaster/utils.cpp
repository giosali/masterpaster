#include "utils.h"

namespace utils {

bool stringToBool(std::string s)
{
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    bool b;
    std::istringstream(s) >> std::boolalpha >> b;
    return b;
}

std::string boolToString(bool b)
{
    return b ? "true" : "false";
}

std::wstring stringToWString(const std::string& s)
{
    const int size = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, NULL, 0);
    std::wstring ws(size, 0);
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &ws.at(0), size);
    return ws;
}

} // namespace utils