#include "logger.h"
#include <filesystem>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() {
    std::filesystem::create_directories("logs");  // ensure logs/ exists
    fileStream.open("app.log", std::ios::app);
    if (fileStream.is_open()) {
        logToFile = true;
        log(Level::INFO, "Logger initialized, logging to logs/app.log");
    } else {
        std::cerr << "Failed to open log file. Falling back to stderr.\n";
    }
}

Logger::~Logger() {
    if (fileStream.is_open())
        fileStream.close();
}

void Logger::setLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(mu);
    if (fileStream.is_open()) fileStream.close();

    fileStream.open(filename, std::ios::app);
    if (fileStream.is_open()) {
        logToFile = true;
        log(Level::INFO, "Log file changed to " + filename);
    } else {
        logToFile = false;
        std::cerr << "Failed to switch to new log file: " << filename << std::endl;
    }
}

void Logger::log(Level level, const std::string& message) {
    std::lock_guard<std::mutex> lock(mu);
    std::ostream& out = logToFile && fileStream.is_open() ? fileStream : std::cerr;
    out << "[" << levelToString(level) << "] " << message << std::endl;
}

void Logger::debug(const std::string& message) { log(Level::DEBUG, message); }
void Logger::info(const std::string& message) { log(Level::INFO, message); }
void Logger::warn(const std::string& message) { log(Level::WARNING, message); }
void Logger::error(const std::string& message) { log(Level::ERROR, message); }

std::string Logger::levelToString(Level level) {
    switch (level) {
        case Level::DEBUG:   return "DEBUG";
        case Level::INFO:    return "INFO";
        case Level::WARNING: return "WARNING";
        case Level::ERROR:   return "ERROR";
        default:             return "UNKNOWN";
    }
}
