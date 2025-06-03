#include "testsUnit.h"
#include "logger.h"

// ����� ��� � ���� ������
void test_packet_loss_boundary_values(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- ������������ ��������� �������� isPacketLost() ---");

    // ���� 1
    Channel channel_no_loss(0.0f);
    bool any_lost = false;
    logger.logDetailed("������������ lossProbability = 0.0 (100 �������)");
    for (int i = 0; i < 100; i++) {
        if (channel_no_loss.isPacketLost()) {
            any_lost = true;
            logger.logDebug("����������� ������ �� �������� " + to_string(i));
            break;
        }
    }
    runner.assert_test(!any_lost, "��������� ��������: lossProbability = 0.0");

    // ���� 2
    Channel channel_all_loss(1.0f);
    bool all_lost = true;
    logger.logDetailed("������������ lossProbability = 1.0 (100 �������)");
    for (int i = 0; i < 100; i++) {
        if (!channel_all_loss.isPacketLost()) {
            all_lost = false;
            logger.logDebug("����������� ���������� �� �������� " + to_string(i));
            break;
        }
    }
    runner.assert_test(all_lost, "��������� ��������: lossProbability = 1.0");
}

void test_packet_loss_distribution(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- ������������ ������������� ������ ---");

    Channel channel(0.5f);
    int lost_count = 0;
    int total_tests = 1000;

    logger.logDetailed("���������� " + to_string(total_tests) + " ������ ������ �������");

    for (int i = 0; i < total_tests; i++) {
        if (channel.isPacketLost()) {
            lost_count++;
        }
    }

    double actual_loss_rate = (double)lost_count / total_tests;
    double expected_loss_rate = 0.5;
    double tolerance = 0.05;

    logger.logDetailed("�������� �������: " + to_string(lost_count) + "/" + to_string(total_tests));
    logger.logDetailed("����������� ����������� ������: " + to_string(actual_loss_rate));
    logger.logDetailed("��������� ����������� ������: " + to_string(expected_loss_rate));
    logger.logDetailed("������: �" + to_string(tolerance));

    bool within_tolerance = abs(actual_loss_rate - expected_loss_rate) <= tolerance;
    runner.assert_test(within_tolerance,
        "������������� ������ (��������: 0.5, ����������: " +
        to_string(actual_loss_rate) + ")");
}

void test_packet_loss_invalid_input(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- ������������ ������������ ������� ������ isPacketLost() ---");

    // ���� � ����� ������������
    logger.logDetailed("������������ lossProbability < 0");
    bool exception_caught_negative = false;
    try {
        Channel channel(-0.1f);
        channel.isPacketLost();
    }
    catch (const invalid_argument& e) {
        exception_caught_negative = true;
        logger.logDebug("����������� ���������� ��� ������������� �����������: " + string(e.what()));
    }
    catch (...) {
        logger.logDebug("����������� ����������� ���������� ��� ������������� �����������");
    }

    runner.assert_test(exception_caught_negative,
        "��������� ������������ ������: lossProbability < 0");

    // ���� � ������������ ������ ������ 1
    logger.logDetailed("������������ lossProbability > 1");
    bool exception_caught_excessive = false;
    try {
        Channel channel(1.1f);
        channel.isPacketLost();
    }
    catch (const invalid_argument& e) {
        exception_caught_excessive = true;
        logger.logDebug("����������� ���������� ��� ���������� �����������: " + string(e.what()));
    }
    catch (...) {
        logger.logDebug("����������� ����������� ���������� ��� ���������� �����������");
    }

    runner.assert_test(exception_caught_excessive,
        "��������� ������������ ������: lossProbability > 1");


    logger.logDetailed("������������ ��������� ���������� ��������");
    bool boundary_test_passed = true;
    try {
        Channel channel_zero(0.0f);
        Channel channel_one(1.0f);

        channel_zero.isPacketLost();
        channel_one.isPacketLost();

        logger.logDebug("��������� ���������� �������� ���������� �������");
    }
    catch (...) {
        boundary_test_passed = false;
        logger.logDebug("����������� ���������� ��� ���������� ��������� ��������");
    }

    runner.assert_test(boundary_test_passed,
        "���������� ��������� ��������� ��������: 0.0 � 1.0");
}

// ����� ��� Packet
void test_packet_initialization(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- ������������ ������������� Packet ---");

    // �������������
    Packet default_packet;
    logger.logDetailed("������ ����� �� ���������");
    logger.logDebug("sequence_number: " + to_string(default_packet.sequence_number));
    logger.logDebug("data[0]: " + to_string((int)default_packet.data[0]));

    runner.assert_test(default_packet.sequence_number == 0,
        "Packet: ������������� sequence_number �� ���������");
    runner.assert_test(default_packet.data[0] == '\0',
        "Packet: ������������� data �� ���������");
}

void test_packet_data_operations(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- ������������ �������� � ������� Packet ---");

    // ������ � ������
    string test_message = "�������� ���������";
    Packet packet(1, test_message);

    logger.logDetailed("������ ����� � ����������: '" + test_message + "'");
    logger.logDebug("sequence_number: " + to_string(packet.sequence_number));
    logger.logDebug("data: '" + string(packet.data) + "'");

    runner.assert_test(packet.sequence_number == 1,
        "Packet: ���������� ������ sequence_number");
    runner.assert_test(string(packet.data) == test_message,
        "Packet: ���������� ������ � ������ ������");
}

void test_packet_buffer_overflow(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- ������������ ������������ ������ Packet ---");

    // ���������� ������� ������
    string long_message(300, 'A'); // 300 �������� 'A'
    Packet packet(2, long_message);

    logger.logDetailed("������ ����� � ������� ���������� (300 ��������)");
    logger.logDebug("����� ��������� ���������: " + to_string(long_message.length()));
    logger.logDebug("����� ����������� ������: " + to_string(strlen(packet.data)));
    logger.logDebug("��������� ������ data[255]: " + to_string((int)packet.data[255]));

    runner.assert_test(strlen(packet.data) < 256,
        "Packet: ������ ������� ��� ���������� ������� ������");
    runner.assert_test(packet.data[255] == '\0',
        "Packet: ���������� ���������� ������ ��� ��������");
}

// ����� ��� Sender
void test_sender_initial_state(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- ������������ ���������� ��������� Sender ---");

    Channel channel(0.0f);
    Sender sender(channel);

    logger.logDetailed("������ ����������� � ������� ��� ������");
    logger.logDebug("��������� ���������: " + to_string(static_cast<int>(sender.getState())));
    logger.logDebug("��������� seq_number: " + to_string(sender.getCurrentSeqNumber()));

    runner.assert_test(sender.getState() == SenderState::READY,
        "Sender: ��������� ��������� READY");
    runner.assert_test(sender.getCurrentSeqNumber() == 0,
        "Sender: ��������� ����� ������������������ = 0");
}

void test_sender_state_transitions(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- ������������ ��������� ��������� Sender ---");

    // READY -> WAITING ��� �������� ������
    {
        logger.logDetailed("���� 1: ������� READY -> WAITING ��� �������� ������");
        Channel channel(0.0f);
        Sender sender(channel);

        logger.logDebug("��������� ���������: " + to_string(static_cast<int>(sender.getState())));
        runner.assert_test(sender.getState() == SenderState::READY,
            "Sender: ��������� ��������� READY");

        sender.sendData("�������� ���������");
        logger.logDebug("��������� ����� ��������: " + to_string(static_cast<int>(sender.getState())));

        runner.assert_test(sender.getState() == SenderState::WAITING,
            "Sender: ������� READY -> WAITING ��� �������� ������");
    }

    // WAITING -> READY ��� ��������� ACK
    {
        logger.logDetailed("���� 2: ������� WAITING -> READY ��� ��������� ����������� ACK");
        Channel channel(0.0f);
        Sender sender(channel);

        // �������� � ��������� WAITING
        sender.sendData("���� ACK");
        runner.assert_test(sender.getState() == SenderState::WAITING,
            "Sender: ��������������� ��������� WAITING");

        if (channel.hasPacket()) {
            Packet packet = channel.receivePacket();
            logger.logDebug("������� ����� � seq_number: " + to_string(packet.sequence_number));

            Acknowledgement ack(packet.sequence_number);
            channel.sendAck(ack);
            logger.logDebug("��������� ���������� ACK � �������: " + to_string(ack.ack_number));
        }

        int seq_before = sender.getCurrentSeqNumber();
        sender.receiveAck();
        int seq_after = sender.getCurrentSeqNumber();

        logger.logDebug("��������� ����� ��������� ACK: " + to_string(static_cast<int>(sender.getState())));
        logger.logDebug("Sequence number �� ACK: " + to_string(seq_before));
        logger.logDebug("Sequence number ����� ACK: " + to_string(seq_after));

        runner.assert_test(sender.getState() == SenderState::READY,
            "Sender: ������� WAITING -> READY ��� ���������� ACK");
        runner.assert_test(seq_after == 1 - seq_before,
            "Sender: ����� ������������������ ��������� ��� ���������� ACK");
    }

    // WAITING -> TIMEOUT -> WAITING (��� ��������)
    {
        logger.logDetailed("���� 3: ������� WAITING -> TIMEOUT -> WAITING ��� ��������");
        Channel channel(0.0f);
        Sender sender(channel, 100); // �������� �������

        sender.sendData("���� �������");
        runner.assert_test(sender.getState() == SenderState::WAITING,
            "Sender: ��������������� ��������� WAITING ��� ����� ��������");

        int seq_before_timeout = sender.getCurrentSeqNumber();

        // ������ ����� handleTimeout
        logger.logDebug("�������� ��������...");
        sender.handleTimeout();

        logger.logDebug("��������� ����� ��������: " + to_string(static_cast<int>(sender.getState())));
        int seq_after_timeout = sender.getCurrentSeqNumber();

        runner.assert_test(sender.getState() == SenderState::WAITING,
            "Sender: ��������� WAITING ����� ��������� ��������");
        runner.assert_test(seq_before_timeout == seq_after_timeout,
            "Sender: ����� ������������������ �� ��������� ��� ��������");

        runner.assert_test(channel.hasPacket(),
            "Sender: ����� �������� ��������� ����� ��������");
    }

    // WAITING -> WAITING ��� ��������� ������������� ACK
    {
        logger.logDetailed("���� 4: ��������� WAITING ����������� ��� ������������ ACK");
        Channel channel(0.0f);
        Sender sender(channel);

        sender.sendData("���� ������������ ACK");
        int expected_seq = sender.getCurrentSeqNumber();
        runner.assert_test(sender.getState() == SenderState::WAITING,
            "Sender: ��������������� ��������� WAITING ��� ����� ������������� ACK");

        int wrong_ack_number = 1 - expected_seq;
        Acknowledgement wrong_ack(wrong_ack_number);
        channel.sendAck(wrong_ack);
        logger.logDebug("��������� ������������ ACK � �������: " + to_string(wrong_ack_number) +
            " (��������: " + to_string(expected_seq) + ")");

        int seq_before = sender.getCurrentSeqNumber();
        sender.receiveAck();
        int seq_after = sender.getCurrentSeqNumber();

        logger.logDebug("��������� ����� ������������� ACK: " + to_string(static_cast<int>(sender.getState())));

        runner.assert_test(sender.getState() == SenderState::WAITING,
            "Sender: ��������� �������� WAITING ��� ������������ ACK");
        runner.assert_test(seq_before == seq_after,
            "Sender: ����� ������������������ �� ��������� ��� ������������ ACK");
    }

    // �������� � WAITING ������ ��������������
    {
        logger.logDetailed("���� 5: ������� �������� ������ � ��������� WAITING");
        Channel channel(0.0f);
        Sender sender(channel);

        sender.sendData("������ ���������");
        runner.assert_test(sender.getState() == SenderState::WAITING,
            "Sender: ��������� WAITING �����������");

        if (channel.hasPacket()) {
            channel.receivePacket();
        }

        sender.sendData("������ ��������� (������ ��������������)");

        logger.logDebug("��������� ����� ������� ��������� ��������: " +
            to_string(static_cast<int>(sender.getState())));

        runner.assert_test(sender.getState() == SenderState::WAITING,
            "Sender: ��������� �������� WAITING ��� ������� ��������� ��������");
        runner.assert_test(!channel.hasPacket(),
            "Sender: ����� ����� �� ��������� � ��������� WAITING");
    }

    // ��������� ACK � READY ������ ��������������
    {
        logger.logDetailed("���� 6: ��������� ACK � ��������� READY (������ ��������������)");
        Channel channel(0.0f);
        Sender sender(channel);

        runner.assert_test(sender.getState() == SenderState::READY,
            "Sender: ��������� ��������� READY ��� ����� ACK");

        Acknowledgement spurious_ack(0);
        channel.sendAck(spurious_ack);
        logger.logDebug("ACK ��������� � ��������� READY");

        int seq_before = sender.getCurrentSeqNumber();
        sender.receiveAck();
        int seq_after = sender.getCurrentSeqNumber();

        logger.logDebug("��������� ����� ��������� ACK � READY: " +
            to_string(static_cast<int>(sender.getState())));

        runner.assert_test(sender.getState() == SenderState::READY,
            "Sender: ��������� �������� READY ��� ��������� ACK � READY");
        runner.assert_test(seq_before == seq_after,
            "Sender: ����� ������������������ �� ��������� ��� ACK � READY");
    }

    // ������ ���� READY -> WAITING -> READY
    {
        logger.logDetailed("���� 7: ������ ���� �������������� �����������");
        Channel channel(0.0f);
        Sender sender(channel);

        runner.assert_test(sender.getState() == SenderState::READY &&
            sender.getCurrentSeqNumber() == 0,
            "Sender: ��������� ��������� ��� ������� �����");

        sender.sendData("������ �����");
        runner.assert_test(sender.getState() == SenderState::WAITING,
            "Sender: READY -> WAITING ��� ������ ��������");

        if (channel.hasPacket()) {
            Packet packet = channel.receivePacket();
            Acknowledgement ack(packet.sequence_number);
            channel.sendAck(ack);
        }
        sender.receiveAck();

        runner.assert_test(sender.getState() == SenderState::READY &&
            sender.getCurrentSeqNumber() == 1,
            "Sender: WAITING -> READY ����� ������� ACK, seq=1");

        sender.sendData("������ �����");
        runner.assert_test(sender.getState() == SenderState::WAITING,
            "Sender: READY -> WAITING ��� ������ ��������");

        if (channel.hasPacket()) {
            Packet packet = channel.receivePacket();
            Acknowledgement ack(packet.sequence_number);
            channel.sendAck(ack);
        }
        sender.receiveAck();

        runner.assert_test(sender.getState() == SenderState::READY &&
            sender.getCurrentSeqNumber() == 0,
            "Sender: WAITING -> READY ����� ������� ACK, seq=0 (���� ��������)");
    }

    logger.logDetailed("��� ����� ��������� ��������� ����������� ���������");
}

void test_sender_sequence_number_management(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- ������������ ���������� ������� ������������������ Sender ---");

    Channel channel(0.0f);
    Sender sender(channel);
    Receiver receiver(channel);

    int initial_seq = sender.getCurrentSeqNumber();
    logger.logDetailed("��������� ����� ������������������: " + to_string(initial_seq));

    sender.sendData("����");
    logger.logDebug("����� ���������");

    if (channel.hasPacket()) {
        Packet packet = channel.receivePacket();
        logger.logDebug("������� ����� � seq_number: " + to_string(packet.sequence_number));
        Acknowledgement ack(packet.sequence_number);
        channel.sendAck(ack);
        logger.logDebug("��������� ACK � �������: " + to_string(ack.ack_number));
    }

    sender.receiveAck();

    int new_seq = sender.getCurrentSeqNumber();
    logger.logDetailed("����� ����� ������������������: " + to_string(new_seq));
    logger.logDebug("��������� ���������: " + to_string(initial_seq) + " -> " + to_string(1 - initial_seq));

    runner.assert_test(new_seq == 1 - initial_seq,
        "Sender: ���������� ��������� ������ ������������������");
}

// ����� ��� Receiver
void test_receiver_initial_state(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- ������������ ���������� ��������� Receiver ---");

    Channel channel(0.0f);
    Receiver receiver(channel);

    logger.logDetailed("������ ���������� � ������� ��� ������");
    logger.logDebug("��������� ���������: " + to_string(static_cast<int>(receiver.getState())));
    logger.logDebug("��������� seq_number: " + to_string(receiver.getExpectedSeqNumber()));

    runner.assert_test(receiver.getState() == ReceiverState::WAITING,
        "Receiver: ��������� ��������� WAITING");
    runner.assert_test(receiver.getExpectedSeqNumber() == 0,
        "Receiver: ��������� ����� ������������������ = 0");
}

void test_receiver_state_transitions(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- ������������ ��������� ��������� Receiver ---");

    // WAITING -> ACK_SENT
    {
        logger.logDetailed("���� 1: ������� WAITING -> ACK_SENT ��� ��������� ����������� ������");
        Channel channel(0.0f);
        Receiver receiver(channel);

        logger.logDebug("��������� ���������: " + to_string(static_cast<int>(receiver.getState())));
        runner.assert_test(receiver.getState() == ReceiverState::WAITING,
            "Receiver: ��������� ��������� WAITING");

        Packet correct_packet(0, "���������� �����");
        channel.sendPacket(correct_packet);
        logger.logDebug("��������� ����� � seq_number: " + to_string(correct_packet.sequence_number));

        ReceiverState state_before = receiver.getState();
        int expected_seq_before = receiver.getExpectedSeqNumber();
        receiver.receivePacket();
        ReceiverState state_after = receiver.getState();
        int expected_seq_after = receiver.getExpectedSeqNumber();

        logger.logDebug("��������� �� ���������: " + to_string(static_cast<int>(state_before)));
        logger.logDebug("��������� ����� ���������: " + to_string(static_cast<int>(state_after)));
        logger.logDebug("��������� seq ��: " + to_string(expected_seq_before));
        logger.logDebug("��������� seq �����: " + to_string(expected_seq_after));

        runner.assert_test(state_after == ReceiverState::ACK_SENT,
            "Receiver: ������� WAITING -> ACK_SENT ��� ���������� ������");
        runner.assert_test(expected_seq_after == 1 - expected_seq_before,
            "Receiver: ��������� ����� ������������������ ��������� ��� ���������� ������");
        runner.assert_test(channel.hasAck(),
            "Receiver: ACK ��������� ����� ��������� ����������� ������");

        if (channel.hasAck()) {
            Acknowledgement ack = channel.receiveAck();
            logger.logDebug("������� ACK � �������: " + to_string(ack.ack_number));
            runner.assert_test(ack.ack_number == correct_packet.sequence_number,
                "Receiver: ���������� ����� � ������������ ACK");
        }
    }

    // WAITING -> ACK_SENT ��������� ��������, ��������� ����� �� ��������
    {
        logger.logDetailed("���� 2: ������� WAITING -> ACK_SENT ��� ��������� ���������");
        Channel channel(0.0f);
        Receiver receiver(channel);

        // ������� ���������� ���������� �����
        Packet first_packet(0, "������ �����");
        channel.sendPacket(first_packet);
        receiver.receivePacket();

        // ������ ������� WAITING
        receiver.setState(ReceiverState::WAITING);
        int expected_seq_after_first = receiver.getExpectedSeqNumber();

        logger.logDebug("��������� seq ����� ������� ������: " + to_string(expected_seq_after_first));

        Packet duplicate_packet(0, "�������� ������");
        channel.sendPacket(duplicate_packet);
        logger.logDebug("��������� �������� � seq_number: " + to_string(duplicate_packet.sequence_number));

        ReceiverState state_before_duplicate = receiver.getState();
        int expected_seq_before_duplicate = receiver.getExpectedSeqNumber();
        receiver.receivePacket();
        ReceiverState state_after_duplicate = receiver.getState();
        int expected_seq_after_duplicate = receiver.getExpectedSeqNumber();

        logger.logDebug("��������� �� ���������: " + to_string(static_cast<int>(state_before_duplicate)));
        logger.logDebug("��������� ����� ���������: " + to_string(static_cast<int>(state_after_duplicate)));
        logger.logDebug("��������� seq �� ���������: " + to_string(expected_seq_before_duplicate));
        logger.logDebug("��������� seq ����� ���������: " + to_string(expected_seq_after_duplicate));

        runner.assert_test(state_after_duplicate == ReceiverState::ACK_SENT,
            "Receiver: ������� WAITING -> ACK_SENT ��� ��������� ���������");
        runner.assert_test(expected_seq_before_duplicate == expected_seq_after_duplicate,
            "Receiver: ��������� ����� �� ��������� ��� ��������� ���������");
        runner.assert_test(channel.hasAck(),
            "Receiver: ACK ��������� ���� ��� ���������");

        if (channel.hasAck()) {
            Acknowledgement ack = channel.receiveAck();
            logger.logDebug("������� ACK ��� ��������� � �������: " + to_string(ack.ack_number));
            runner.assert_test(ack.ack_number == duplicate_packet.sequence_number,
                "Receiver: ���������� ����� � ACK ��� ���������");
        }
    }

    //  WAITING -> ACK_SENT ��� ��������� ������ � ������������ seq
    {
        logger.logDetailed("���� 3: ������� WAITING -> ACK_SENT ��� ��������� ������ � ������������ seq_number");
        Channel channel(0.0f);
        Receiver receiver(channel);

        int expected_seq = receiver.getExpectedSeqNumber();
        int wrong_seq = 1 - expected_seq;

        Packet wrong_packet(wrong_seq, "����� � ������������ �������");
        channel.sendPacket(wrong_packet);
        logger.logDebug("��������� ����� � ������������ seq_number: " + to_string(wrong_seq) +
            " (��������: " + to_string(expected_seq) + ")");

        ReceiverState state_before = receiver.getState();
        int expected_seq_before = receiver.getExpectedSeqNumber();
        receiver.receivePacket();
        ReceiverState state_after = receiver.getState();
        int expected_seq_after = receiver.getExpectedSeqNumber();

        logger.logDebug("��������� �� ��������� ������������� ������: " + to_string(static_cast<int>(state_before)));
        logger.logDebug("��������� ����� ��������� ������������� ������: " + to_string(static_cast<int>(state_after)));

        runner.assert_test(state_after == ReceiverState::ACK_SENT,
            "Receiver: ������� WAITING -> ACK_SENT ��� ������������ ������");
        runner.assert_test(expected_seq_before == expected_seq_after,
            "Receiver: ��������� ����� �� ��������� ��� ������������ ������");
        runner.assert_test(channel.hasAck(),
            "Receiver: ACK ��������� ���� ��� ������������� ������");

        if (channel.hasAck()) {
            Acknowledgement ack = channel.receiveAck();
            logger.logDebug("������� ACK ��� ������������� ������ � �������: " + to_string(ack.ack_number));
            runner.assert_test(ack.ack_number == wrong_packet.sequence_number,
                "Receiver: ACK �������� ����� ����������� ������ (���� �������������)");
        }
    }

    // ��������� � ACK_SENT
    {
        logger.logDetailed("���� 4: ��������� ������� � ��������� ACK_SENT");
        Channel channel(0.0f);
        Receiver receiver(channel);

        // ������ ACK_SENT
        Packet first_packet(0, "������ �����");
        channel.sendPacket(first_packet);
        receiver.receivePacket();

        runner.assert_test(receiver.getState() == ReceiverState::ACK_SENT,
            "Receiver: ��������������� ��������� ACK_SENT �����������");

        // ���������� 2 ����� � ��������� ACK_SENT
        Packet second_packet(1, "������ �����");
        channel.sendPacket(second_packet);
        logger.logDebug("��������� ������ ����� � ��������� ACK_SENT");

        int expected_seq_before = receiver.getExpectedSeqNumber();
        receiver.receivePacket();
        int expected_seq_after = receiver.getExpectedSeqNumber();
        ReceiverState final_state = receiver.getState();

        logger.logDebug("��������� ���������: " + to_string(static_cast<int>(final_state)));
        logger.logDebug("��������� seq �� ������� ������: " + to_string(expected_seq_before));
        logger.logDebug("��������� seq ����� ������� ������: " + to_string(expected_seq_after));

        runner.assert_test(final_state == ReceiverState::ACK_SENT,
            "Receiver: ��������� �������� ACK_SENT ����� ��������� � ACK_SENT");
        runner.assert_test(expected_seq_after == 1 - expected_seq_before,
            "Receiver: ��������� ����� ��������� ��� ���������� ������ ������");
    }

    // ��������� � RECEIVED
    {
        logger.logDetailed("���� 5: ��������� ������� � ��������� RECEIVED");
        Channel channel(0.0f);
        Receiver receiver(channel);

        receiver.setState(ReceiverState::RECEIVED);
        logger.logDebug("����������� ��������� RECEIVED �������");

        runner.assert_test(receiver.getState() == ReceiverState::RECEIVED,
            "Receiver: ��������� RECEIVED �����������");

        // ���������� �����
        Packet test_packet(0, "���� � RECEIVED");
        channel.sendPacket(test_packet);

        int expected_seq_before = receiver.getExpectedSeqNumber();
        receiver.receivePacket();
        int expected_seq_after = receiver.getExpectedSeqNumber();
        ReceiverState state_after = receiver.getState();

        logger.logDebug("��������� ����� ��������� � RECEIVED: " + to_string(static_cast<int>(state_after)));

        runner.assert_test(state_after == ReceiverState::ACK_SENT,
            "Receiver: ������� RECEIVED -> ACK_SENT ��� ��������� ������");
        runner.assert_test(expected_seq_after == 1 - expected_seq_before,
            "Receiver: ��������� ����� ��������� ��� ���������� ������ � RECEIVED");
    }

    // ������ ����
    {
        logger.logDetailed("���� 6: ������ ���� �������������� ����������");
        Channel channel(0.0f);
        Receiver receiver(channel);

        runner.assert_test(receiver.getState() == ReceiverState::WAITING &&
            receiver.getExpectedSeqNumber() == 0,
            "Receiver: ��������� ��������� ��� ������� �����");

        // ������ �����
        Packet packet_0(0, "����� 0");
        channel.sendPacket(packet_0);
        receiver.receivePacket();

        runner.assert_test(receiver.getState() == ReceiverState::ACK_SENT &&
            receiver.getExpectedSeqNumber() == 1,
            "Receiver: ����� ������� ������ - ACK_SENT, ������� 1");

        // ���������� WAITING
        receiver.setState(ReceiverState::WAITING);

        // ������ �����
        Packet packet_1(1, "����� 1");
        channel.sendPacket(packet_1);
        receiver.receivePacket();

        runner.assert_test(receiver.getState() == ReceiverState::ACK_SENT &&
            receiver.getExpectedSeqNumber() == 0,
            "Receiver: ����� ������� ������ - ACK_SENT, ������� 0 (���� ��������)");
    }

    {
        logger.logDetailed("���� 7: ��������� ��� ���������� ������� � ������");
        Channel channel(0.0f);
        Receiver receiver(channel);

        ReceiverState state_before = receiver.getState();
        int expected_seq_before = receiver.getExpectedSeqNumber();

        // �������� receivePacket() ��� ������ ������
        receiver.receivePacket();

        ReceiverState state_after = receiver.getState();
        int expected_seq_after = receiver.getExpectedSeqNumber();

        logger.logDebug("��������� �� ������ receivePacket() �� ������ ������: " +
            to_string(static_cast<int>(state_before)));
        logger.logDebug("��������� ����� ������ receivePacket() �� ������ ������: " +
            to_string(static_cast<int>(state_after)));

        runner.assert_test(state_before == state_after,
            "Receiver: ��������� �� ���������� ��� ���������� �������");
        runner.assert_test(expected_seq_before == expected_seq_after,
            "Receiver: ��������� ����� �� ��������� ��� ���������� �������");
        runner.assert_test(!channel.hasAck(),
            "Receiver: ACK �� ��������� ��� ���������� �������");
    }

    {
        logger.logDetailed("���� 8: ���������������� ��������� ������������� �������");
        Channel channel(0.0f);
        Receiver receiver(channel);

        // ���������� ��������� ������� ������
        for (int i = 0; i < 4; i++) {
            int expected_seq = receiver.getExpectedSeqNumber();
            Packet packet(expected_seq, "����� " + to_string(i));
            channel.sendPacket(packet);

            logger.logDebug("��������� ����� " + to_string(i) + " � seq_number: " + to_string(expected_seq));

            receiver.receivePacket();

            runner.assert_test(receiver.getState() == ReceiverState::ACK_SENT,
                "Receiver: ��������� ACK_SENT ����� ������ " + to_string(i));
            runner.assert_test(channel.hasAck(),
                "Receiver: ACK ��������� ��� ������ " + to_string(i));

            if (channel.hasAck()) {
                channel.receiveAck(); // ������� ACK �� ������
            }

            // ���������� ���������
            receiver.setState(ReceiverState::WAITING);
        }

        runner.assert_test(receiver.getExpectedSeqNumber() == 0,
            "Receiver: ����� ������������������ �������� � 0 ����� ������� �������");
    }

    logger.logDetailed("��� ����� ��������� ��������� ���������� ���������");
}

void test_receiver_duplicate_detection(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- ������������ ����������� ���������� Receiver ---");

    Channel channel(0.0f);
    Receiver receiver(channel);

    Packet packet1(0, "������ �����");
    channel.sendPacket(packet1);
    logger.logDetailed("��������� ������ ����� � seq_number: 0");

    receiver.receivePacket();
    int expected_after_first = receiver.getExpectedSeqNumber();
    logger.logDebug("��������� seq_number ����� ������� ������: " + to_string(expected_after_first));

    // ���������� ��������
    Packet duplicate(0, "��������");
    channel.sendPacket(duplicate);
    logger.logDetailed("��������� �������� � seq_number: 0");

    // �������� ��������
    receiver.receivePacket();
    int expected_after_duplicate = receiver.getExpectedSeqNumber();
    logger.logDebug("��������� seq_number ����� ���������: " + to_string(expected_after_duplicate));

    runner.assert_test(expected_after_first == expected_after_duplicate,
        "Receiver: ��������� ����� �� ��������� ����� ���������");
}

// ����� Ask
void test_acknowledgement_initialization(TestRunner& runner, Logger& logger) {
    cout << "\n--- ������������ ������������� Acknowledgement ---" << endl;

    // ������������� �� ���������
    Acknowledgement default_ack;
    runner.assert_test(default_ack.ack_number == 0,
        "Acknowledgement: ������������� �� ���������");

    // � ����������
    Acknowledgement param_ack(5);
    runner.assert_test(param_ack.ack_number == 5,
        "Acknowledgement: ������������� � ����������");
}

// ����� Channel
void test_channel_basic_operations(TestRunner& runner, Logger& logger) {
    cout << "\n--- ������������ ������� �������� Channel ---" << endl;

    Channel channel(0.0f);

    // ���� �������� � ���������
    Packet test_packet(1, "����");
    channel.sendPacket(test_packet);

    runner.assert_test(channel.hasPacket(),
        "Channel: ����� �������� ����� ��������");

    Packet received = channel.receivePacket();
    runner.assert_test(received.sequence_number == test_packet.sequence_number,
        "Channel: ���������� �������� ������ ������������������");
    runner.assert_test(string(received.data) == string(test_packet.data),
        "Channel: ���������� �������� ������");

    // �������� � ��������� ACK
    Acknowledgement test_ack(2);
    channel.sendAck(test_ack);

    runner.assert_test(channel.hasAck(),
        "Channel: ACK �������� ����� ��������");

    Acknowledgement received_ack = channel.receiveAck();
    runner.assert_test(received_ack.ack_number == test_ack.ack_number,
        "Channel: ���������� �������� ACK");
}
