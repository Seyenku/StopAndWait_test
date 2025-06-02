#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <random>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <cassert>
#include <cmath>

using namespace std;

// Перечисления состояний
enum class SenderState {
    READY,
    WAITING,
    TIMEOUT
};

enum class ReceiverState {
    WAITING,
    RECEIVED,
    ACK_SENT
};

enum class LogLevel {
    NONE = 0,    // Без логов
    BASIC = 1,   // Только результаты тестов
    DETAILED = 2, // Подробная информация
    DEBUG = 3    // Отладочная информация
};