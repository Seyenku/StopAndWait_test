#ifndef TESTS_INTEGRATION_H
#define TESTS_INTEGRATION_H

#include "common.h"
#include "protocol.h"
#include "logger.h"
#include "test_runner.h"

// ��������������/��������� �����

// 1. ��������� �������� ��� ������
void test_successful_single_packet_transmission(TestRunner& runner, Logger& logger);
void test_successful_multiple_packets_transmission(TestRunner& runner, Logger& logger);

// 2. �������� � ������� ������� � ACK
void test_packet_loss_with_retransmission(TestRunner& runner, Logger& logger);
void test_ack_loss_with_retransmission(TestRunner& runner, Logger& logger);
void test_multiple_losses_with_retransmission(TestRunner& runner, Logger& logger);
void test_alternating_packet_ack_loss(TestRunner& runner, Logger& logger);

// 3. ����� � ���������� ����������
void test_very_short_timeout(TestRunner& runner, Logger& logger);
void test_very_long_timeout(TestRunner& runner, Logger& logger);
void test_optimal_timeout_determination(TestRunner& runner, Logger& logger);

// 4. ������ ��������
void test_sender_receiver_synchronization(TestRunner& runner, Logger& logger);
void test_protocol_under_high_loss_conditions(TestRunner& runner, Logger& logger);
void test_concurrent_operations(TestRunner& runner, Logger& logger);

// ������ �������
struct TransmissionStats {
    int packets_sent;
    int packets_received;
    int acks_sent;
    int acks_received;
    int retransmissions;
    chrono::milliseconds total_time;

    TransmissionStats() : packets_sent(0), packets_received(0),
        acks_sent(0), acks_received(0),
        retransmissions(0), total_time(0) {
    }
};

class IntegrationTestHelper {
public:
    static bool wait_for_transmission_complete(Sender& sender, Receiver& receiver,
        int timeout_ms = 5000);
    static TransmissionStats measure_transmission_performance(Channel& channel,
        Sender& sender,
        Receiver& receiver,
        const vector<string>& messages,
        int timeout_ms = 10000);
    static void simulate_controlled_losses(Channel& channel,
        const vector<bool>& packet_loss_pattern,
        const vector<bool>& ack_loss_pattern);
    static double calculate_optimal_timeout(float loss_probability,
        int base_transmission_time_ms);
};

#endif