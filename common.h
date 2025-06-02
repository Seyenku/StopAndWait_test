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

// ������������ ���������
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
    NONE = 0,    // ��� �����
    BASIC = 1,   // ������ ���������� ������
    DETAILED = 2, // ��������� ����������
    DEBUG = 3    // ���������� ����������
};