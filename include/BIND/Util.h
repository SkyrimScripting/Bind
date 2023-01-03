#pragma once

namespace Util {
    void LowerCase(std::string& text) {
        std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    }
    inline void LeftTrim(std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    }
    inline void RightTrim(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    }
    inline void Trim(std::string& s) {
        RightTrim(s);
        LeftTrim(s);
    }
    std::vector<std::string> Split(const std::string& text, char delim) {
        std::string line;
        std::vector<std::string> vec;
        std::stringstream ss(text);
        while (std::getline(ss, line, delim)) vec.push_back(line);
        return vec;
    }
}
