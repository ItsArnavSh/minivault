#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <csignal>
#include "engine/core.h"
#include "utils/log.h"
using namespace std;

// Fixed split function that preserves semicolons where needed
std::vector<std::string> splitStatements(const std::string& content) {
    std::vector<std::string> statements;
    std::stringstream ss(content);
    std::string line;
    std::string currentStmt;

    while (std::getline(ss, line)) {
        // Skip empty lines
        if (line.find_first_not_of(" \t\r\n") == std::string::npos) {
            continue;
        }

        currentStmt += line;

        // If line ends with semicolon, we have a full statement
        if (line.find(';') != std::string::npos) {
            size_t start = currentStmt.find_first_not_of(" \t\r\n");
            size_t end = currentStmt.find_last_not_of(" \t\r\n;");

            if (start != std::string::npos && end != std::string::npos) {
                std::string cleaned = currentStmt.substr(start, end - start + 1);
                if (!cleaned.empty()) {
                    statements.push_back(cleaned);
                }
            }
            currentStmt.clear();
        } else {
            // Add a space if we're continuing to the next line
            currentStmt += " ";
        }
    }

    // Handle any remaining statement without semicolon
    if (!currentStmt.empty()) {
        size_t start = currentStmt.find_first_not_of(" \t\r\n");
        size_t end = currentStmt.find_last_not_of(" \t\r\n");

        if (start != std::string::npos && end != std::string::npos) {
            std::string cleaned = currentStmt.substr(start, end - start + 1);
            if (!cleaned.empty()) {
                statements.push_back(cleaned);
            }
        }
    }

    return statements;
}

void processSqlFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file) {
        std::cerr << "Could not open file: " << filepath << "\n";
        return;
    }

    try {
        // Read file content
        std::string content;
        std::string line;
        while (std::getline(file, line)) {
            content += line + "\n";
        }

        auto statements = splitStatements(content);



        // Free up memory before executing statements
        content.clear();
        content.shrink_to_fit();

        // Process each statement - adding semicolon for consistency with manual input
        for (size_t i = 0; i < statements.size(); ++i) {
            const auto& stmt = statements[i];

            try {
                // Add semicolon to make it consistent with manual input
                std::string command = stmt;
                if (command.find("select") == 0) {
                    command += ";";  // Add semicolon specifically for select statements
                }

                std::cout  << processCommand(command) << std::endl;

                // Free memory after each statement
                if (i > 0) {
                    statements[i-1].clear();
                    statements[i-1].shrink_to_fit();
                }
            } catch (const std::bad_alloc& e) {
                std::cerr << "Memory allocation failed on statement: " << stmt << std::endl;
                std::cerr << "Error: " << e.what() << std::endl;
            } catch (const std::exception& e) {
                std::cerr << "Error executing statement: " << e.what() << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error in file processing: " << e.what() << std::endl;
    }
}

bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void startTerminalSession() {
    LOG_INFO("Session Starting");
    std::string line;
    std::cout << ">>> "; // Prompt
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            std::cout << ">>> ";
            continue;
        }
        if (endsWith(line,".txt")) {
            processSqlFile(line);
        } else {
            std::cout << processCommand(line) << std::endl;
        }
        std::cout << ">>> ";
    }
    std::cout << "\nExiting on EOF (Ctrl+D). Bye!\n";
}

void handle_sigint(int) {
    std::cout << "\nUse Ctrl+D to exit\n>>> ";
    std::cout.flush();
}

int main() {
    std::signal(SIGINT, handle_sigint);
    startTerminalSession();
}
