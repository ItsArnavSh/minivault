#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>

class Logger {
public:
    enum class Level { DEBUG, INFO, WARNING, ERROR };

    static Logger& getInstance();

    void setLogFile(const std::string& filename);
    void log(Level level, const std::string& message);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);

private:
    Logger();
    ~Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    std::ofstream fileStream;
    bool logToFile = false;
    std::mutex mu;

    std::string levelToString(Level level);
};

#endif // LOGGER_H
