#ifndef APS_H
#define APS_H
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <queue>
#include <memory>
void smartSortByColumn(std::vector<std::vector<std::string>>& data, size_t colIndex, bool asc);
std::vector<std::vector<std::string>> kmpSearchInColumn(
    const std::vector<std::vector<std::string>>& data,
    const std::string& pattern,
    size_t colIndex
);
std::vector<std::vector<std::string>> regexSearchInColumn(
    const std::vector<std::vector<std::string>>& data,
    const std::string& pattern,
    size_t colIndex
);
std::string huffmanEncode(const std::string& input);
std::string huffmanDecode(const std::string& encodedWithKey);
#endif
