#pragma once
#include "protocol.h"
#include "logger.h"
#include "test_runner.h"

// ����� ��� ������� isPacketLost()
void test_packet_loss_boundary_values(TestRunner& runner, Logger& logger);
void test_packet_loss_distribution(TestRunner& runner, Logger& logger);
void test_packet_loss_invalid_input(TestRunner& runner, Logger& logger);

// ����� ��� ��������� Packet
void test_packet_initialization(TestRunner& runner, Logger& logger);
void test_packet_data_operations(TestRunner& runner, Logger& logger);
void test_packet_buffer_overflow(TestRunner& runner, Logger& logger);

// ����� ��� ����������� (Sender)
void test_sender_initial_state(TestRunner& runner, Logger& logger);
void test_sender_state_transitions(TestRunner& runner, Logger& logger);
void test_sender_sequence_number_management(TestRunner& runner, Logger& logger);

// ����� ��� ���������� (Receiver)
void test_receiver_initial_state(TestRunner& runner, Logger& logger);
void test_receiver_state_transitions(TestRunner& runner, Logger& logger);
void test_receiver_duplicate_detection(TestRunner& runner, Logger& logger);

// ����� ��������� Acknowledgement
void test_acknowledgement_initialization(TestRunner& runner, Logger& logger);

// ����� ������ (Channel)
void test_channel_basic_operations(TestRunner& runner, Logger& logger);

// ������ ������
void run_all_tests(Logger& logger);
void run_packet_tests(Logger& logger);
void run_sender_tests(Logger& logger);
void run_receiver_tests(Logger& logger);
void run_channel_tests(Logger& logger);