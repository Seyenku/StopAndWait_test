#ifndef TEST_SUITES_H
#define TEST_SUITES_H

#include "logger.h"

void run_all_tests(Logger& logger);

// функции unit тестов
void run_unit_tests(Logger& logger);
void run_packet_tests(Logger& logger);
void run_sender_tests(Logger& logger);
void run_receiver_tests(Logger& logger);
void run_channel_tests(Logger& logger);

// функции интеграционных тестов
void run_integration_tests(Logger& logger);
void run_transmission_tests(Logger& logger);
void run_loss_recovery_tests(Logger& logger);
void run_timeout_tests(Logger& logger);
void run_performance_tests(Logger& logger);

#endif
