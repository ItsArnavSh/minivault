#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include "printer.h"

std::string printer(const std::vector<std::vector<std::string>>& data, const std::vector<std::string>& colNames) {
    std::ostringstream out;

    if (colNames.empty()) {
        return "[ERROR] No column names provided.\n";
    }

    size_t columnCount = colNames.size();
    std::vector<size_t> columnWidths(columnCount, 0);

    // Step 1: Compute max width per column (considering column names too)
    for (size_t i = 0; i < columnCount; ++i) {
        columnWidths[i] = colNames[i].size();
    }

    for (const auto& row : data) {
        for (size_t i = 0; i < row.size() && i < columnCount; ++i) {
            columnWidths[i] = std::max(columnWidths[i], row[i].size());
        }
    }

    // Helper: print a horizontal separator
    auto printSeparator = [&]() {
        for (size_t width : columnWidths) {
            out << '+' << std::string(width + 2, '-');
        }
        out << "+\n";
    };

    out << "[OUTPUT] TABLE START\n";
    printSeparator();

    // Step 2: Print header row
    for (size_t i = 0; i < columnCount; ++i) {
        out << "| " << std::left << std::setw(columnWidths[i]) << colNames[i] << ' ';
    }
    out << "|\n";
    printSeparator();

    // Step 3: Print data rows
    for (size_t r = 0; r < data.size(); ++r) {
        const auto& row = data[r];
        for (size_t i = 0; i < columnCount; ++i) {
            std::string val = (i < row.size()) ? row[i] : "";
            if (i >= row.size()) {
                std::cerr << "[WARNING] Row " << r << " missing column " << i << ", substituting with ''\n";
            }
            out << "| " << std::left << std::setw(columnWidths[i]) << val << ' ';
        }
        out << "|\n";
        printSeparator();
    }

    out << "[OUTPUT] TABLE END\n";
    return out.str();
}
