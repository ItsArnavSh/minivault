#include "aps.h"
#include <sstream>
#include <algorithm>

bool isNumericColumn(const std::vector<std::vector<std::string>>& data, size_t colIndex) {
    for (const auto& row : data) {
        if (colIndex >= row.size()) return false;
        const std::string& s = row[colIndex];
        if (s.empty() || !std::all_of(s.begin(), s.end(), ::isdigit)) {
            return false;
        }
    }
    return true;
}

void merge(std::vector<std::vector<std::string>>& data, size_t left, size_t mid, size_t right, size_t colIndex, bool asc, bool isNumeric) {
    std::vector<std::vector<std::string>> leftVec(data.begin() + left, data.begin() + mid + 1);
    std::vector<std::vector<std::string>> rightVec(data.begin() + mid + 1, data.begin() + right + 1);

    size_t i = 0, j = 0, k = left;

    while (i < leftVec.size() && j < rightVec.size()) {
        bool condition;
        if (isNumeric) {
            int lval = std::stoi(leftVec[i][colIndex]);
            int rval = std::stoi(rightVec[j][colIndex]);
            condition = asc ? (lval <= rval) : (lval >= rval);
        } else {
            condition = asc ? (leftVec[i][colIndex] <= rightVec[j][colIndex])
                            : (leftVec[i][colIndex] >= rightVec[j][colIndex]);
        }

        data[k++] = condition ? std::move(leftVec[i++]) : std::move(rightVec[j++]);
    }

    while (i < leftVec.size()) data[k++] = std::move(leftVec[i++]);
    while (j < rightVec.size()) data[k++] = std::move(rightVec[j++]);
}

void mergeSort(std::vector<std::vector<std::string>>& data, size_t left, size_t right, size_t colIndex, bool asc, bool isNumeric) {
    if (left < right) {
        size_t mid = left + (right - left) / 2;
        mergeSort(data, left, mid, colIndex, asc, isNumeric);
        mergeSort(data, mid + 1, right, colIndex, asc, isNumeric);
        merge(data, left, mid, right, colIndex, asc, isNumeric);
    }
}

void smartSortByColumn(std::vector<std::vector<std::string>>& data, size_t colIndex, bool asc) {
    if (data.empty() || data[0].size() <= colIndex) return;
    bool isNum = isNumericColumn(data, colIndex);
    mergeSort(data, 0, data.size() - 1, colIndex, asc, isNum);
}
// KMP Helpers
std::vector<int> computeLPS(const std::string& pattern) {
    std::vector<int> lps(pattern.length(), 0);
    int len = 0;
    for (size_t i = 1; i < pattern.length(); ) {
        if (pattern[i] == pattern[len]) {
            lps[i++] = ++len;
        } else if (len != 0) {
            len = lps[len - 1];
        } else {
            lps[i++] = 0;
        }
    }
    return lps;
}

bool kmpSearch(const std::string& text, const std::string& pattern) {
    if (pattern.empty()) return true;
    if (text.empty()) return false;

    std::vector<int> lps = computeLPS(pattern);
    size_t i = 0, j = 0;

    while (i < text.length()) {
        if (pattern[j] == text[i]) {
            i++;
            j++;
        }

        if (j == pattern.length()) {
            // Match found
            return true;
        } else if (i < text.length() && pattern[j] != text[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    return false;
}

std::vector<std::vector<std::string>> kmpSearchInColumn(
    const std::vector<std::vector<std::string>>& data,
    const std::string& pattern,
    size_t colIndex
) {
    std::vector<std::vector<std::string>> matches;

    for (const auto& row : data) {
        if (colIndex < row.size()) {
            if (kmpSearch(row[colIndex], pattern)) {
                matches.push_back(row);
            }
        }
    }

    return matches;
}


struct HNode {
    char ch;
    int freq;
    HNode *left, *right;
    HNode(char c, int f) : ch(c), freq(f), left(nullptr), right(nullptr) {}
};

struct Compare {
    bool operator()(HNode* a, HNode* b) {
        return a->freq > b->freq;
    }
};

void buildCodeMap(HNode* root, std::string code, std::unordered_map<char, std::string>& codeMap) {
    if (!root) return;
    if (!root->left && !root->right) codeMap[root->ch] = code;
    buildCodeMap(root->left, code + "0", codeMap);
    buildCodeMap(root->right, code + "1", codeMap);
}

std::string serializeCodeMap(const std::unordered_map<char, std::string>& codeMap) {
    std::ostringstream oss;
    for (const auto& pair : codeMap) {
        if (pair.first == ';' || pair.first == ':' || pair.first == '\\')
            oss << '\\'; // escape special chars
        oss << pair.first << ":" << pair.second << ";";
    }
    return oss.str();
}

std::unordered_map<char, std::string> deserializeCodeMap(const std::string& serialized, size_t& index) {
    std::unordered_map<char, std::string> codeMap;
    while (index < serialized.size()) {
        char ch = serialized[index++];
        if (ch == '\\') ch = serialized[index++]; // unescape
        if (serialized[index++] != ':') break;
        std::string code;
        while (serialized[index] != ';') code += serialized[index++];
        index++; // skip ';'
        codeMap[ch] = code;
        if (serialized[index] == '|') break; // end of key part
    }
    return codeMap;
}

std::string huffmanEncode(const std::string& input) {
    std::unordered_map<char, int> freq;
    for (char ch : input) freq[ch]++;

    std::priority_queue<HNode*, std::vector<HNode*>, Compare> pq;
    for (auto& pair : freq) pq.push(new HNode(pair.first, pair.second));

    while (pq.size() > 1) {
        HNode *left = pq.top(); pq.pop();
        HNode *right = pq.top(); pq.pop();
        HNode *parent = new HNode('\0', left->freq + right->freq);
        parent->left = left;
        parent->right = right;
        pq.push(parent);
    }

    HNode* root = pq.top();
    std::unordered_map<char, std::string> codeMap;
    buildCodeMap(root, "", codeMap);

    std::string encoded;
    for (char ch : input) encoded += codeMap[ch];

    std::string serializedKey = serializeCodeMap(codeMap);
    return serializedKey + "|" + encoded;
}

std::string huffmanDecode(const std::string& encodedWithKey) {
    size_t sep = encodedWithKey.find('|');
    if (sep == std::string::npos) return "";

    size_t index = 0;
    std::unordered_map<char, std::string> codeMap = deserializeCodeMap(encodedWithKey, index);
    std::unordered_map<std::string, char> reverseMap;
    for (auto& pair : codeMap) reverseMap[pair.second] = pair.first;

    std::string encodedBits = encodedWithKey.substr(sep + 1);
    std::string decoded, current;
    for (char bit : encodedBits) {
        current += bit;
        if (reverseMap.count(current)) {
            decoded += reverseMap[current];
            current.clear();
        }
    }
    return decoded;
}


bool isMatch(const std::string& text, const std::string& pattern) {
    int n = text.size();
    int m = pattern.size();

    // DP table: dp[i][j] = true if text[0...i-1] matches pattern[0...j-1]
    std::vector<std::vector<bool>> dp(n + 1, std::vector<bool>(m + 1, false));

    // Empty pattern matches empty text
    dp[0][0] = true;

    // Handle patterns starting with special characters
    for (int j = 1; j <= m; j++) {
        // Handle '*' for zero occurrences
        if (j > 1 && pattern[j-1] == '*') {
            dp[0][j] = dp[0][j-2];
        }
        // Handle '?' for zero occurrences
        else if (pattern[j-1] == '?') {
            dp[0][j] = dp[0][j-1];
        }
        // '+' always requires at least one occurrence, so dp[0][j] remains false
    }

    // Fill the DP table
    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            char p = pattern[j-1];
            char t = text[i-1];

            if (p == '.' || p == t) {
                // Direct character match
                dp[i][j] = dp[i-1][j-1];
            }
            else if (p == '*' && j > 1) {
                // '*' represents zero or more occurrences of the preceding character
                char prev = pattern[j-2];

                // Zero occurrences case
                dp[i][j] = dp[i][j-2];

                // One or more occurrences case (if previous char matches)
                if (prev == '.' || prev == t) {
                    dp[i][j] = dp[i][j] || dp[i-1][j];
                }
            }
            else if (p == '+' && j > 1) {
                // '+' represents one or more occurrences of the preceding character
                char prev = pattern[j-2];

                // For '+', we need at least one match of the previous character
                if (prev == '.' || prev == t) {
                    // Check if text[0...i-2] matches pattern[0...j-1] (more occurrences)
                    // OR text[0...i-2] matches pattern[0...j-3] (exactly one occurrence)
                    dp[i][j] = dp[i-1][j] || (j > 2 && dp[i-1][j-2]);
                }
            }
            else if (p == '?' && j > 1) {
                // '?' represents zero or one occurrence of the preceding character
                char prev = pattern[j-2];

                // Zero occurrence case
                dp[i][j] = dp[i][j-2];

                // One occurrence case (if previous char matches)
                if (prev == '.' || prev == t) {
                    dp[i][j] = dp[i][j] || dp[i-1][j-2];
                }
            }
        }
    }

    return dp[n][m];
}

// Function to perform regex search in a specific column of data
std::vector<std::vector<std::string>> regexSearchInColumn(
    const std::vector<std::vector<std::string>>& data,
    const std::string& pattern,
    size_t colIndex
) {
    std::vector<std::vector<std::string>> result;
    for (const auto& row : data) {
        if (colIndex < row.size() && isMatch(row[colIndex], pattern)) {
            result.push_back(row);
        }
    }
    return result;
}
