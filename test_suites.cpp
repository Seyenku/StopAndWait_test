#include "test_suites.h"
#include "testsUnit.h"
#include "testIntegration.h"
#include "test_runner.h"

void run_all_tests(Logger& logger) {
    logger.logBasic("=== ÏÎËÍÛÉ ÍÀÁÎĞ ÒÅÑÒÎÂ STOP-AND-WAIT ÏĞÎÒÎÊÎËÀ ===");

    TestRunner runner(&logger);

    // Unit òåñòû
    logger.logBasic("\n--- UNIT ÒÅÑÒÛ ---");
    test_packet_loss_boundary_values(runner, logger);
    test_packet_loss_distribution(runner, logger);
    test_packet_loss_invalid_input(runner, logger);
    test_packet_initialization(runner, logger);
    test_packet_data_operations(runner, logger);
    test_packet_buffer_overflow(runner, logger);
    test_sender_initial_state(runner, logger);
    test_sender_state_transitions(runner, logger);
    test_sender_sequence_number_management(runner, logger);
    test_receiver_initial_state(runner, logger);
    test_receiver_state_transitions(runner, logger);
    test_receiver_duplicate_detection(runner, logger);
    test_acknowledgement_initialization(runner, logger);
    test_channel_basic_operations(runner, logger);

    // Èíòåãğàöèîííûå òåñòû
    logger.logBasic("\n--- ÈÍÒÅÃĞÀÖÈÎÍÍÛÅ ÒÅÑÒÛ ---");
    test_successful_single_packet_transmission(runner, logger);
    test_successful_multiple_packets_transmission(runner, logger);
    test_packet_loss_with_retransmission(runner, logger);
    test_ack_loss_with_retransmission(runner, logger);
    test_multiple_losses_with_retransmission(runner, logger);
    test_alternating_packet_ack_loss(runner, logger);
    test_very_short_timeout(runner, logger);
    test_very_long_timeout(runner, logger);
    test_optimal_timeout_determination(runner, logger);
    test_sender_receiver_synchronization(runner, logger);
    test_protocol_under_high_loss_conditions(runner, logger);
    test_concurrent_operations(runner, logger);

    // Âûâîä ğåçóëüòàòîâ
    runner.print_summary();
}

// ôóíêöèè äëÿ unit òåñòîâ
void run_unit_tests(Logger& logger) {
    logger.logBasic("=== ÂÑÅ UNIT ÒÅÑÒÛ ===");
    TestRunner runner(&logger);

    test_packet_loss_boundary_values(runner, logger);
    test_packet_loss_distribution(runner, logger);
    test_packet_loss_invalid_input(runner, logger);
    test_packet_initialization(runner, logger);
    test_packet_data_operations(runner, logger);
    test_packet_buffer_overflow(runner, logger);
    test_sender_initial_state(runner, logger);
    test_sender_state_transitions(runner, logger);
    test_sender_sequence_number_management(runner, logger);
    test_receiver_initial_state(runner, logger);
    test_receiver_state_transitions(runner, logger);
    test_receiver_duplicate_detection(runner, logger);
    test_acknowledgement_initialization(runner, logger);
    test_channel_basic_operations(runner, logger);

    runner.print_summary();
}

void run_packet_tests(Logger& logger) {
    logger.logBasic("=== ÒÅÑÒÛ ÏÀÊÅÒÎÂ ===");
    TestRunner runner(&logger);

    test_packet_loss_boundary_values(runner, logger);
    test_packet_loss_distribution(runner, logger);
    test_packet_loss_invalid_input(runner, logger);
    test_packet_initialization(runner, logger);
    test_packet_data_operations(runner, logger);
    test_packet_buffer_overflow(runner, logger);

    runner.print_summary();
}

void run_sender_tests(Logger& logger) {
    logger.logBasic("=== ÒÅÑÒÛ ÎÒÏĞÀÂÈÒÅËß ===");
    TestRunner runner(&logger);

    test_sender_initial_state(runner, logger);
    test_sender_state_transitions(runner, logger);
    test_sender_sequence_number_management(runner, logger);

    runner.print_summary();
}

void run_receiver_tests(Logger& logger) {
    logger.logBasic("=== ÒÅÑÒÛ ÏÎËÓ×ÀÒÅËß ===");
    TestRunner runner(&logger);

    test_receiver_initial_state(runner, logger);
    test_receiver_state_transitions(runner, logger);
    test_receiver_duplicate_detection(runner, logger);

    runner.print_summary();
}

void run_channel_tests(Logger& logger) {
    logger.logBasic("=== ÒÅÑÒÛ ÊÀÍÀËÀ ===");
    TestRunner runner(&logger);

    test_channel_basic_operations(runner, logger);
    test_acknowledgement_initialization(runner, logger);

    runner.print_summary();
}

// ôóíêöèè äëÿ èíòåãğàöèîííûõ òåñòîâ
void run_integration_tests(Logger& logger) {
    logger.logBasic("=== ÂÑÅ ÈÍÒÅÃĞÀÖÈÎÍÍÛÅ ÒÅÑÒÛ ===");
    TestRunner runner(&logger);

    test_successful_single_packet_transmission(runner, logger);
    test_successful_multiple_packets_transmission(runner, logger);
    test_packet_loss_with_retransmission(runner, logger);
    test_ack_loss_with_retransmission(runner, logger);
    test_multiple_losses_with_retransmission(runner, logger);
    test_alternating_packet_ack_loss(runner, logger);
    test_very_short_timeout(runner, logger);
    test_very_long_timeout(runner, logger);
    test_optimal_timeout_determination(runner, logger);
    test_sender_receiver_synchronization(runner, logger);
    test_protocol_under_high_loss_conditions(runner, logger);
    test_concurrent_operations(runner, logger);

    runner.print_summary();
}

void run_transmission_tests(Logger& logger) {
    logger.logBasic("=== ÒÅÑÒÛ ÏÅĞÅÄÀ×È ÄÀÍÍÛÕ ===");
    TestRunner runner(&logger);

    test_successful_single_packet_transmission(runner, logger);
    test_successful_multiple_packets_transmission(runner, logger);
    test_sender_receiver_synchronization(runner, logger);

    runner.print_summary();
}

void run_loss_recovery_tests(Logger& logger) {
    logger.logBasic("=== ÒÅÑÒÛ ÂÎÑÑÒÀÍÎÂËÅÍÈß ÏÎÑËÅ ÏÎÒÅĞÜ ===");
    TestRunner runner(&logger);

    test_packet_loss_with_retransmission(runner, logger);
    test_ack_loss_with_retransmission(runner, logger);
    test_multiple_losses_with_retransmission(runner, logger);
    test_alternating_packet_ack_loss(runner, logger);
    test_protocol_under_high_loss_conditions(runner, logger);

    runner.print_summary();
}

void run_timeout_tests(Logger& logger) {
    logger.logBasic("=== ÒÅÑÒÛ ÒÀÉÌÀÓÒÎÂ ===");
    TestRunner runner(&logger);

    test_very_short_timeout(runner, logger);
    test_very_long_timeout(runner, logger);
    test_optimal_timeout_determination(runner, logger);

    runner.print_summary();
}

void run_performance_tests(Logger& logger) {
    logger.logBasic("=== ÒÅÑÒÛ ÏĞÎÈÇÂÎÄÈÒÅËÜÍÎÑÒÈ ===");
    TestRunner runner(&logger);

    test_protocol_under_high_loss_conditions(runner, logger);
    test_concurrent_operations(runner, logger);
    test_optimal_timeout_determination(runner, logger);

    runner.print_summary();
}