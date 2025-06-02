#pragma once
#include "common.h"
#include "logger.h"

class TestRunner {
private:
    int tests_passed = 0;
    int tests_failed = 0;
    Logger* logger;

public:
    TestRunner(Logger* log);
    void assert_test(bool condition, const string& test_name);
    void print_summary();
    int getPassedCount() const;
    int getFailedCount() const;
    void reset();
};