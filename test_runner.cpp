#include "test_runner.h"

TestRunner::TestRunner(Logger* log) : logger(log) {}

void TestRunner::assert_test(bool condition, const string& test_name) {
    if (condition) {
        logger->logBasic("[PASS] " + test_name);
        tests_passed++;
    }
    else {
        logger->logBasic("[FAIL] " + test_name);
        tests_failed++;
    }
}

void TestRunner::print_summary() {
    logger->logBasic("\n=== ĞÅÇÓËÜÒÀÒÛ ÒÅÑÒÈĞÎÂÀÍÈß ===");
    logger->logBasic("Ïğîéäåíî: " + to_string(tests_passed));
    logger->logBasic("Ïğîâàëåíî: " + to_string(tests_failed));
    logger->logBasic("Âñåãî: " + to_string(tests_passed + tests_failed));
}

int TestRunner::getPassedCount() const {
    return tests_passed;
}

int TestRunner::getFailedCount() const {
    return tests_failed;
}

void TestRunner::reset() {
    tests_passed = 0;
    tests_failed = 0;
}