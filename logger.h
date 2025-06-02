#pragma once
#include "common.h"

class Logger {
private:
    LogLevel current_level = LogLevel::BASIC;

public:
    void setLevel(LogLevel level);
    LogLevel getLevel() const;
    void log(LogLevel level, const string& message);
    void logBasic(const string& message);
    void logDetailed(const string& message);
    void logDebug(const string& message);
};