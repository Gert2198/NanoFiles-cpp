#pragma once

#include <string>
#include <vector>

class StringUtils {
public:
    // trim from start (in place)
    static void ltrim(std::string &s);

    // trim from end (in place)
    static void rtrim(std::string &s);

    // trim from both ends (in place)
    static void trim(std::string &s);

    static std::vector<std::string> split(std::string s, const char delimiter);

    static bool ichar_equals(char a, char b);

    static bool iequals(const std::string& a, const std::string& b);

    static void toLower(std::string& str);
};