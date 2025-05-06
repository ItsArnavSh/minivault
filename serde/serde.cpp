#include"serde.h"
// Define a special delimiter
const char DELIM = '\x1E';  // ASCII Record Separator

std::string encode(const std::vector<std::string>& strs) {
    std::ostringstream oss;
    for (size_t i = 0; i < strs.size(); ++i) {
        oss << strs[i];
        if (i < strs.size() - 1) {
            oss << DELIM;
        }
    }
    return oss.str();
}

std::vector<std::string> decode(const std::string& s) {
    std::vector<std::string> result;
    std::string current;
    for (char c : s) {
        if (c == DELIM) {
            result.push_back(current);
            current.clear();
        } else {
            current += c;
        }
    }
    result.push_back(current); // last one
    return result;
}
