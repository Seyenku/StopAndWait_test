#include "logger.h"

void Logger::setLevel(LogLevel level) {
    current_level = level;
}

LogLevel Logger::getLevel() const {
    return current_level;
}

void Logger::log(LogLevel level, const string& message) {
    if (level <= current_level) {
        cout << message << endl;
    }
}

void Logger::logBasic(const string& message) {
    log(LogLevel::BASIC, message);
}

void Logger::logDetailed(const string& message) {
    log(LogLevel::DETAILED, "  [DETAIL] " + message);
}

void Logger::logDebug(const string& message) {
    log(LogLevel::DEBUG, "  [DEBUG] " + message);
}