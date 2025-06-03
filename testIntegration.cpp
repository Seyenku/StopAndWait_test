#include "testIntegration.h"


// успешная передача без потерь
void test_successful_single_packet_transmission(TestRunner& runner, Logger& logger) {
    logger.logDetailed("Тест: Успешная передача одного пакета без потерь");

    Channel channel(0.0f);
    Sender sender(channel, 1000);
    Receiver receiver(channel);

    sender.start();
    receiver.start();

    string test_message = "Hello, World!";
    sender.sendData(test_message);

    bool transmission_complete = IntegrationTestHelper::wait_for_transmission_complete(
        sender, receiver, 3000);

    runner.assert_test(transmission_complete,
        "Передача одного пакета завершена успешно");
    runner.assert_test(sender.isReady(),
        "Отправитель в состоянии READY после передачи");
    runner.assert_test(receiver.getState() == ReceiverState::WAITING,
        "Получатель в состоянии WAITING после передачи");

    sender.stop();
    receiver.stop();

    logger.logDetailed("Тест завершен");
}

void test_successful_multiple_packets_transmission(TestRunner& runner, Logger& logger) {
    logger.logDetailed("Тест: Успешная передача нескольких пакетов без потерь");

    Channel channel(0.0f);
    Sender sender(channel, 1000);
    Receiver receiver(channel);

    sender.start();
    receiver.start();

    vector<string> messages = {
        "Packet 1", "Packet 2", "Packet 3", "Packet 4", "Packet 5"
    };

    auto start_time = chrono::steady_clock::now();
    int packets_transmitted = 0;

    for (const auto& message : messages) {
        // Ждем когда отправитель будет готов
        while (!sender.isReady()) {
            this_thread::sleep_for(chrono::milliseconds(10));
            auto current_time = chrono::steady_clock::now();
            if (chrono::duration_cast<chrono::milliseconds>(
                current_time - start_time).count() > 10000) {
                break; // Таймаут
            }
        }

        if (sender.isReady()) {
            sender.sendData(message);
            packets_transmitted++;

            // Ждем завершения передачи пакета
            IntegrationTestHelper::wait_for_transmission_complete(sender, receiver, 2000);
        }
    }

    runner.assert_test(packets_transmitted == static_cast<int>(messages.size()),
        "Все пакеты отправлены успешно");
    runner.assert_test(sender.isReady(),
        "Отправитель готов после передачи всех пакетов");

    sender.stop();
    receiver.stop();

    logger.logDetailed("Передано пакетов: " + to_string(packets_transmitted) + "/" +
        to_string(messages.size()));
}

// Передача с потерей пакетов и аск
void test_packet_loss_with_retransmission(TestRunner& runner, Logger& logger) {
    logger.logDetailed("Тест: Потеря пакета с повторной передачей");

    Channel channel(0.7f);
    Sender sender(channel, 500);
    Receiver receiver(channel);

    sender.start();
    receiver.start();

    string test_message = "Test packet loss";
    auto start_time = chrono::steady_clock::now();

    sender.sendData(test_message);

    // Мониторим для обнаружения повторных передач
    bool timeout_occurred = false;
    int state_changes = 0;
    SenderState last_state = sender.getState();

    while (!sender.isReady()) {
        this_thread::sleep_for(chrono::milliseconds(50));

        SenderState current_state = sender.getState();
        if (current_state != last_state) {
            state_changes++;
            if (current_state == SenderState::TIMEOUT) {
                timeout_occurred = true;
            }
            last_state = current_state;
        }

        auto current_time = chrono::steady_clock::now();
        if (chrono::duration_cast<chrono::milliseconds>(
            current_time - start_time).count() > 10000) {
            break; // Таймаут теста
        }
    }

    runner.assert_test(timeout_occurred, "Произошел таймаут (ожидаемо при потере пакетов)");
    runner.assert_test(state_changes >= 2, "Произошли изменения состояния (повторные передачи)");
    runner.assert_test(sender.isReady(), "Пакет в итоге доставлен успешно");

    sender.stop();
    receiver.stop();

    logger.logDetailed("Изменений состояния: " + to_string(state_changes));
}

void test_ack_loss_with_retransmission(TestRunner& runner, Logger& logger) {
    logger.logDetailed("Тест: Потеря ACK с повторной передачей");

    // Создаем канал с умеренной вероятностью потерь
    Channel channel(0.5f);
    Sender sender(channel, 800);
    Receiver receiver(channel);

    sender.start();
    receiver.start();

    string test_message = "Test ACK loss";
    auto start_time = chrono::steady_clock::now();

    sender.sendData(test_message);

    // Мониторим для обнаружения повторных передач
    int retransmission_count = 0;
    bool transmission_completed = false;

    while (chrono::duration_cast<chrono::milliseconds>(
        chrono::steady_clock::now() - start_time).count() < 8000) {

        if (sender.getState() == SenderState::TIMEOUT) {
            retransmission_count++;
        }

        if (sender.isReady()) {
            transmission_completed = true;
            break;
        }

        this_thread::sleep_for(chrono::milliseconds(100));
    }

    runner.assert_test(transmission_completed, "Передача завершена несмотря на потери ACK");
    runner.assert_test(retransmission_count > 0, "Произошли повторные передачи");

    sender.stop();
    receiver.stop();

    logger.logDetailed("Повторных передач: " + to_string(retransmission_count));
}

void test_multiple_losses_with_retransmission(TestRunner& runner, Logger& logger) {
    logger.logDetailed("Тест: Множественные потери с повторными передачами");

    Channel channel(0.6f); // Высокая вероятность потерь
    Sender sender(channel, 600);
    Receiver receiver(channel);

    sender.start();
    receiver.start();

    vector<string> messages = { "Msg1", "Msg2", "Msg3" };
    int successful_transmissions = 0;
    int total_retransmissions = 0;

    for (const auto& message : messages) {
        auto start_time = chrono::steady_clock::now();
        sender.sendData(message);

        int message_retransmissions = 0;
        bool message_transmitted = false;

        while (chrono::duration_cast<chrono::milliseconds>(
            chrono::steady_clock::now() - start_time).count() < 15000) {

            if (sender.getState() == SenderState::TIMEOUT) {
                message_retransmissions++;
            }

            if (sender.isReady()) {
                message_transmitted = true;
                break;
            }

            this_thread::sleep_for(chrono::milliseconds(50));
        }

        if (message_transmitted) {
            successful_transmissions++;
            total_retransmissions += message_retransmissions;
        }

        // пауза между пакетами
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    runner.assert_test(successful_transmissions == static_cast<int>(messages.size()),
        "Все сообщения переданы несмотря на потери");
    runner.assert_test(total_retransmissions > 0,
        "Зафиксированы повторные передачи");

    sender.stop();
    receiver.stop();

    logger.logDetailed("Успешных передач: " + to_string(successful_transmissions) +
        ", Повторных передач: " + to_string(total_retransmissions));
}

void test_alternating_packet_ack_loss(TestRunner& runner, Logger& logger) {
    logger.logDetailed("Тест: Чередование потери пакетов и ACK");

    // Моделируем контролируемые потери
    Channel channel(0.0f); // Начинаем без потерь
    Sender sender(channel, 700);
    Receiver receiver(channel);

    sender.start();
    receiver.start();

    // Тест 1: Сначала потеря пакета, потом нормальная передача
    channel.setLossProbability(0.9f);
    sender.sendData("First message");

    // Ждем первой попытки и таймаута
    this_thread::sleep_for(chrono::milliseconds(1000));

    // Уменьшаем потери для успешной передачи
    channel.setLossProbability(0.1f);

    bool first_transmission_complete = IntegrationTestHelper::wait_for_transmission_complete(
        sender, receiver, 5000);

    runner.assert_test(first_transmission_complete,
        "Первое сообщение передано после первоначальной потери");

    // Тест 2: Нормальная отправка, потом потеря ACK
    channel.setLossProbability(0.5f);
    sender.sendData("Second message");

    bool second_transmission_complete = IntegrationTestHelper::wait_for_transmission_complete(
        sender, receiver, 8000);

    runner.assert_test(second_transmission_complete,
        "Второе сообщение передано несмотря на возможную потерю ACK");

    sender.stop();
    receiver.stop();
}

// Тесты с различными таймаутами
void test_very_short_timeout(TestRunner& runner, Logger& logger) {
    logger.logDetailed("Тест: Очень короткий таймаут");

    Channel channel(0.3f); // Низкие потери
    Sender sender(channel, 50); // Очень короткий таймаут
    Receiver receiver(channel);

    sender.start();
    receiver.start();

    string test_message = "Short timeout test";
    auto start_time = chrono::steady_clock::now();

    sender.sendData(test_message);

    int timeout_count = 0;
    bool transmission_complete = false;

    while (chrono::duration_cast<chrono::milliseconds>(
        chrono::steady_clock::now() - start_time).count() < 5000) {

        if (sender.getState() == SenderState::TIMEOUT) {
            timeout_count++;
        }

        if (sender.isReady()) {
            transmission_complete = true;
            break;
        }

        this_thread::sleep_for(chrono::milliseconds(25));
    }

    runner.assert_test(transmission_complete, "Передача завершена с коротким таймаутом");
    runner.assert_test(timeout_count > 3, "Множественные таймауты из-за короткого интервала");

    auto total_time = chrono::duration_cast<chrono::milliseconds>(
        chrono::steady_clock::now() - start_time).count();

    sender.stop();
    receiver.stop();

    logger.logDetailed("Таймаутов: " + to_string(timeout_count) +
        ", Время: " + to_string(total_time) + "мс");
}

void test_very_long_timeout(TestRunner& runner, Logger& logger) {
    logger.logDetailed("Тест: Очень длинный таймаут");

    Channel channel(0.8f); // Высокие потери
    Sender sender(channel, 5000); // Очень длинный таймаут
    Receiver receiver(channel);

    sender.start();
    receiver.start();

    string test_message = "Long timeout test";
    auto start_time = chrono::steady_clock::now();

    sender.sendData(test_message);

    // Ждем первого таймаута
    bool first_timeout_occurred = false;
    while (chrono::duration_cast<chrono::milliseconds>(
        chrono::steady_clock::now() - start_time).count() < 6000) {

        if (sender.getState() == SenderState::TIMEOUT) {
            first_timeout_occurred = true;
            break;
        }

        this_thread::sleep_for(chrono::milliseconds(100));
    }

    auto first_timeout_time = chrono::duration_cast<chrono::milliseconds>(
        chrono::steady_clock::now() - start_time).count();

    // Уменьшаем потери для завершения передачи
    channel.setLossProbability(0.1f);

    bool transmission_complete = IntegrationTestHelper::wait_for_transmission_complete(
        sender, receiver, 10000);

    runner.assert_test(first_timeout_occurred, "Произошел таймаут с длинным интервалом");
    runner.assert_test(first_timeout_time >= 4500, "Таймаут произошел через ожидаемое время");
    runner.assert_test(transmission_complete, "Передача в итоге завершена");

    sender.stop();
    receiver.stop();

    logger.logDetailed("Время до первого таймаута: " + to_string(first_timeout_time) + "мс");
}

void test_optimal_timeout_determination(TestRunner& runner, Logger& logger) {
    logger.logDetailed("Тест: Определение оптимального таймаута");

    vector<int> timeout_values = { 200, 500, 1000, 2000, 3000 };
    vector<pair<int, double>> performance_results;

    for (int timeout_ms : timeout_values) {
        Channel channel(0.3f); // Фиксированная вероятность потерь
        Sender sender(channel, timeout_ms);
        Receiver receiver(channel);

        sender.start();
        receiver.start();

        auto start_time = chrono::steady_clock::now();
        sender.sendData("Timeout optimization test");

        bool completed = IntegrationTestHelper::wait_for_transmission_complete(
            sender, receiver, 15000);

        auto end_time = chrono::steady_clock::now();
        double transmission_time = chrono::duration_cast<chrono::milliseconds>(
            end_time - start_time).count();

        if (completed) {
            performance_results.push_back({ timeout_ms, transmission_time });
        }

        sender.stop();
        receiver.stop();

        this_thread::sleep_for(chrono::milliseconds(100)); // Пауза между тестами
    }

    runner.assert_test(performance_results.size() >= 3,
        "Получены результаты для нескольких значений таймаута");

    // поиск оптимального таймаута (мин время передачи)
    if (!performance_results.empty()) {
        auto optimal = min_element(performance_results.begin(), performance_results.end(),
            [](const pair<int, double>& a, const pair<int, double>& b) {
                return a.second < b.second;
            });

        runner.assert_test(optimal != performance_results.end(),
            "Найден оптимальный таймаут");

        logger.logDetailed("Оптимальный таймаут: " + to_string(optimal->first) +
            "мс, Время передачи: " + to_string(optimal->second) + "мс");

        // Логируем все результаты
        for (const auto& result : performance_results) {
            logger.logDebug("Таймаут: " + to_string(result.first) +
                "мс -> Время: " + to_string(result.second) + "мс");
        }
    }
}

// Доп тесты
void test_sender_receiver_synchronization(TestRunner& runner, Logger& logger) {
    logger.logDetailed("Тест: Синхронизация отправителя и получателя");

    Channel channel(0.2f);
    Sender sender(channel, 1000);
    Receiver receiver(channel);

    // Тест синхр последовательных номеров
    runner.assert_test(sender.getCurrentSeqNumber() == 0,
        "Начальный номер отправителя: 0");
    runner.assert_test(receiver.getExpectedSeqNumber() == 0,
        "Ожидаемый номер получателя: 0");

    sender.start();
    receiver.start();

    // Отправка нескольких пакетов
    for (int i = 0; i < 3; i++) {
        int initial_sender_seq = sender.getCurrentSeqNumber();
        int initial_receiver_seq = receiver.getExpectedSeqNumber();

        sender.sendData("Sync test " + to_string(i));

        IntegrationTestHelper::wait_for_transmission_complete(sender, receiver, 5000);

        int final_sender_seq = sender.getCurrentSeqNumber(); // После успешной передачи номера должны измениться
        int final_receiver_seq = receiver.getExpectedSeqNumber();

        runner.assert_test(final_sender_seq != initial_sender_seq ||
            final_receiver_seq != initial_receiver_seq,
            "Номера последовательности обновились после передачи " +
            to_string(i + 1));
    }

    sender.stop();
    receiver.stop();
}

void test_protocol_under_high_loss_conditions(TestRunner& runner, Logger& logger) {
    logger.logDetailed("Тест: Протокол в условиях высоких потерь");

    Channel channel(0.9f);
    Sender sender(channel, 400);
    Receiver receiver(channel);

    sender.start();
    receiver.start();

    string test_message = "High loss test";
    auto start_time = chrono::steady_clock::now();

    sender.sendData(test_message);

    int retransmission_count = 0;
    bool transmission_complete = false;

    // Даем больше времени для высоких потерь
    while (chrono::duration_cast<chrono::milliseconds>(
        chrono::steady_clock::now() - start_time).count() < 30000) {

        if (sender.getState() == SenderState::TIMEOUT) {
            retransmission_count++;
        }

        if (sender.isReady()) {
            transmission_complete = true;
            break;
        }

        this_thread::sleep_for(chrono::milliseconds(50));
    }

    auto total_time = chrono::duration_cast<chrono::milliseconds>(
        chrono::steady_clock::now() - start_time).count();

    runner.assert_test(transmission_complete,
        "Протокол работает даже при 90% потерь");
    runner.assert_test(retransmission_count > 5,
        "Много повторных передач при высоких потерях");

    sender.stop();
    receiver.stop();

    logger.logDetailed("Повторных передач: " + to_string(retransmission_count) +
        ", Время: " + to_string(total_time) + "мс");
}

void test_concurrent_operations(TestRunner& runner, Logger& logger) {
    logger.logDetailed("Тест: Параллельные операции");

    Channel channel(0.3f);
    Sender sender(channel, 800);
    Receiver receiver(channel);

    sender.start();
    receiver.start();

    // Тестируем отправку сообщения, пока предыдущее еще передается
    sender.sendData("First message");

    this_thread::sleep_for(chrono::milliseconds(100));

    bool sender_busy = !sender.isReady();
    runner.assert_test(sender_busy, "Отправитель занят во время передачи");

    // Ждем конца первой передачи
    IntegrationTestHelper::wait_for_transmission_complete(sender, receiver, 5000);

    // отправляем второе сообщение
    bool can_send_second = sender.isReady();
    runner.assert_test(can_send_second, "Можно отправить второе сообщение после первого");

    if (can_send_second) {
        sender.sendData("Second message");
        bool second_complete = IntegrationTestHelper::wait_for_transmission_complete(
            sender, receiver, 5000);
        runner.assert_test(second_complete, "Второе сообщение передано успешно");
    }

    sender.stop();
    receiver.stop();
}

// Остальные функции
bool IntegrationTestHelper::wait_for_transmission_complete(Sender& sender, Receiver& receiver,
    int timeout_ms) {
    auto start_time = chrono::steady_clock::now();

    while (chrono::duration_cast<chrono::milliseconds>(
        chrono::steady_clock::now() - start_time).count() < timeout_ms) {

        if (sender.isReady() && receiver.getState() == ReceiverState::WAITING) {
            return true;
        }

        this_thread::sleep_for(chrono::milliseconds(50));
    }

    return false;
}

TransmissionStats IntegrationTestHelper::measure_transmission_performance(
    Channel& channel, Sender& sender, Receiver& receiver,
    const vector<string>& messages, int timeout_ms) {

    TransmissionStats stats;
    auto start_time = chrono::steady_clock::now();

    for (const auto& message : messages) {
        if (sender.isReady()) {
            sender.sendData(message);
            stats.packets_sent++;

            if (wait_for_transmission_complete(sender, receiver, timeout_ms / messages.size())) {
                stats.packets_received++;
            }
        }
    }

    auto end_time = chrono::steady_clock::now();
    stats.total_time = chrono::duration_cast<chrono::milliseconds>(end_time - start_time);

    return stats;
}

void IntegrationTestHelper::simulate_controlled_losses(Channel& channel,
    const vector<bool>& packet_loss_pattern,
    const vector<bool>& ack_loss_pattern) {
    float avg_packet_loss = 0.0f;
    for (bool loss : packet_loss_pattern) {
        if (loss) avg_packet_loss += 1.0f;
    }
    avg_packet_loss /= packet_loss_pattern.size();

    channel.setLossProbability(avg_packet_loss);
}

double IntegrationTestHelper::calculate_optimal_timeout(float loss_probability,
    int base_transmission_time_ms) {
    double expected_retransmissions = loss_probability / (1.0 - loss_probability);
    return base_transmission_time_ms * (1.5 + expected_retransmissions * 0.5);
}