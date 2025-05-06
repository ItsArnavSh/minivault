#ifndef SERDE_H
#define SERDE_H
#include <string>
#include <vector>
#include <sstream>
std::string encode(const std::vector<std::string>& strs);
std::vector<std::string> decode(const std::string& s);
#endif
