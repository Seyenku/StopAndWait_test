#include "testsUnit.h"
#include "logger.h"

// тесты мин и макс потерь
void test_packet_loss_boundary_values(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- Тестирование граничных значений isPacketLost() ---");

    // Тест 1
    Channel channel_no_loss(0.0f);
    bool any_lost = false;
    logger.logDetailed("Тестирование lossProbability = 0.0 (100 попыток)");
    for (int i = 0; i < 100; i++) {
        if (channel_no_loss.isPacketLost()) {
            any_lost = true;
            logger.logDebug("Неожиданная потеря на итерации " + to_string(i));
            break;
        }
    }
    runner.assert_test(!any_lost, "Граничное значение: lossProbability = 0.0");

    // Тест 2
    Channel channel_all_loss(1.0f);
    bool all_lost = true;
    logger.logDetailed("Тестирование lossProbability = 1.0 (100 попыток)");
    for (int i = 0; i < 100; i++) {
        if (!channel_all_loss.isPacketLost()) {
            all_lost = false;
            logger.logDebug("Неожиданное сохранение на итерации " + to_string(i));
            break;
        }
    }
    runner.assert_test(all_lost, "Граничное значение: lossProbability = 1.0");
}

void test_packet_loss_distribution(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- Тестирование распределения потерь ---");

    Channel channel(0.5f);
    int lost_count = 0;
    int total_tests = 1000;

    logger.logDetailed("Проведение " + to_string(total_tests) + " тестов потери пакетов");

    for (int i = 0; i < total_tests; i++) {
        if (channel.isPacketLost()) {
            lost_count++;
        }
    }

    double actual_loss_rate = (double)lost_count / total_tests;
    double expected_loss_rate = 0.5;
    double tolerance = 0.05;

    logger.logDetailed("Потеряно пакетов: " + to_string(lost_count) + "/" + to_string(total_tests));
    logger.logDetailed("Фактический коэффициент потерь: " + to_string(actual_loss_rate));
    logger.logDetailed("Ожидаемый коэффициент потерь: " + to_string(expected_loss_rate));
    logger.logDetailed("Допуск: ±" + to_string(tolerance));

    bool within_tolerance = abs(actual_loss_rate - expected_loss_rate) <= tolerance;
    runner.assert_test(within_tolerance,
        "Распределение потерь (ожидаемо: 0.5, фактически: " +
        to_string(actual_loss_rate) + ")");
}

void test_packet_loss_invalid_input(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- Тестирование некорректных входных данных isPacketLost() ---");

    // Тест с отриц вероятностью
    logger.logDetailed("Тестирование lossProbability < 0");
    bool exception_caught_negative = false;
    try {
        Channel channel(-0.1f);
        channel.isPacketLost();
    }
    catch (const invalid_argument& e) {
        exception_caught_negative = true;
        logger.logDebug("Перехвачено исключение для отрицательной вероятности: " + string(e.what()));
    }
    catch (...) {
        logger.logDebug("Перехвачено неожиданное исключение для отрицательной вероятности");
    }

    runner.assert_test(exception_caught_negative,
        "Обработка некорректных данных: lossProbability < 0");

    // Тест с вероятностью потерь больше 1
    logger.logDetailed("Тестирование lossProbability > 1");
    bool exception_caught_excessive = false;
    try {
        Channel channel(1.1f);
        channel.isPacketLost();
    }
    catch (const invalid_argument& e) {
        exception_caught_excessive = true;
        logger.logDebug("Перехвачено исключение для чрезмерной вероятности: " + string(e.what()));
    }
    catch (...) {
        logger.logDebug("Перехвачено неожиданное исключение для чрезмерной вероятности");
    }

    runner.assert_test(exception_caught_excessive,
        "Обработка некорректных данных: lossProbability > 1");


    logger.logDetailed("Тестирование граничных корректных значений");
    bool boundary_test_passed = true;
    try {
        Channel channel_zero(0.0f);
        Channel channel_one(1.0f);

        channel_zero.isPacketLost();
        channel_one.isPacketLost();

        logger.logDebug("Граничные корректные значения обработаны успешно");
    }
    catch (...) {
        boundary_test_passed = false;
        logger.logDebug("Неожиданное исключение для корректных граничных значений");
    }

    runner.assert_test(boundary_test_passed,
        "Корректная обработка граничных значений: 0.0 и 1.0");
}

// Тесты для Packet
void test_packet_initialization(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- Тестирование инициализации Packet ---");

    // Инициализация
    Packet default_packet;
    logger.logDetailed("Создан пакет по умолчанию");
    logger.logDebug("sequence_number: " + to_string(default_packet.sequence_number));
    logger.logDebug("data[0]: " + to_string((int)default_packet.data[0]));

    runner.assert_test(default_packet.sequence_number == 0,
        "Packet: инициализация sequence_number по умолчанию");
    runner.assert_test(default_packet.data[0] == '\0',
        "Packet: инициализация data по умолчанию");
}

void test_packet_data_operations(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- Тестирование операций с данными Packet ---");

    // Запись и чтение
    string test_message = "Тестовое сообщение";
    Packet packet(1, test_message);

    logger.logDetailed("Создан пакет с сообщением: '" + test_message + "'");
    logger.logDebug("sequence_number: " + to_string(packet.sequence_number));
    logger.logDebug("data: '" + string(packet.data) + "'");

    runner.assert_test(packet.sequence_number == 1,
        "Packet: корректная запись sequence_number");
    runner.assert_test(string(packet.data) == test_message,
        "Packet: корректная запись и чтение данных");
}

void test_packet_buffer_overflow(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- Тестирование переполнения буфера Packet ---");

    // Превышение размера буфера
    string long_message(300, 'A'); // 300 символов 'A'
    Packet packet(2, long_message);

    logger.logDetailed("Создан пакет с длинным сообщением (300 символов)");
    logger.logDebug("Длина исходного сообщения: " + to_string(long_message.length()));
    logger.logDebug("Длина сохраненных данных: " + to_string(strlen(packet.data)));
    logger.logDebug("Последний символ data[255]: " + to_string((int)packet.data[255]));

    runner.assert_test(strlen(packet.data) < 256,
        "Packet: данные усечены при превышении размера буфера");
    runner.assert_test(packet.data[255] == '\0',
        "Packet: корректное завершение строки при усечении");
}

// Тесты для Sender
void test_sender_initial_state(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- Тестирование начального состояния Sender ---");

    Channel channel(0.0f);
    Sender sender(channel);

    logger.logDetailed("Создан отправитель с каналом без потерь");
    logger.logDebug("Начальное состояние: " + to_string(static_cast<int>(sender.getState())));
    logger.logDebug("Начальный seq_number: " + to_string(sender.getCurrentSeqNumber()));

    runner.assert_test(sender.getState() == SenderState::READY,
        "Sender: начальное состояние READY");
    runner.assert_test(sender.getCurrentSeqNumber() == 0,
        "Sender: начальный номер последовательности = 0");
}

void test_sender_state_transitions(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- Тестирование переходов состояний Sender ---");

    // READY -> WAITING при отправке данных
    {
        logger.logDetailed("Тест 1: Переход READY -> WAITING при отправке данных");
        Channel channel(0.0f);
        Sender sender(channel);

        logger.logDebug("Начальное состояние: " + to_string(static_cast<int>(sender.getState())));
        runner.assert_test(sender.getState() == SenderState::READY,
            "Sender: начальное состояние READY");

        sender.sendData("Тестовое сообщение");
        logger.logDebug("Состояние после отправки: " + to_string(static_cast<int>(sender.getState())));

        runner.assert_test(sender.getState() == SenderState::WAITING,
            "Sender: переход READY -> WAITING при отправке данных");
    }

    // WAITING -> READY при получении ACK
    {
        logger.logDetailed("Тест 2: Переход WAITING -> READY при получении корректного ACK");
        Channel channel(0.0f);
        Sender sender(channel);

        // Приводим в состояние WAITING
        sender.sendData("Тест ACK");
        runner.assert_test(sender.getState() == SenderState::WAITING,
            "Sender: предварительное состояние WAITING");

        if (channel.hasPacket()) {
            Packet packet = channel.receivePacket();
            logger.logDebug("Получен пакет с seq_number: " + to_string(packet.sequence_number));

            Acknowledgement ack(packet.sequence_number);
            channel.sendAck(ack);
            logger.logDebug("Отправлен корректный ACK с номером: " + to_string(ack.ack_number));
        }

        int seq_before = sender.getCurrentSeqNumber();
        sender.receiveAck();
        int seq_after = sender.getCurrentSeqNumber();

        logger.logDebug("Состояние после получения ACK: " + to_string(static_cast<int>(sender.getState())));
        logger.logDebug("Sequence number до ACK: " + to_string(seq_before));
        logger.logDebug("Sequence number после ACK: " + to_string(seq_after));

        runner.assert_test(sender.getState() == SenderState::READY,
            "Sender: переход WAITING -> READY при корректном ACK");
        runner.assert_test(seq_after == 1 - seq_before,
            "Sender: номер последовательности изменился при корректном ACK");
    }

    // WAITING -> TIMEOUT -> WAITING (при таймауте)
    {
        logger.logDetailed("Тест 3: Переход WAITING -> TIMEOUT -> WAITING при таймауте");
        Channel channel(0.0f);
        Sender sender(channel, 100); // Короткий таймаут

        sender.sendData("Тест таймаут");
        runner.assert_test(sender.getState() == SenderState::WAITING,
            "Sender: предварительное состояние WAITING для теста таймаута");

        int seq_before_timeout = sender.getCurrentSeqNumber();

        // Ручной вызов handleTimeout
        logger.logDebug("Имитация таймаута...");
        sender.handleTimeout();

        logger.logDebug("Состояние после таймаута: " + to_string(static_cast<int>(sender.getState())));
        int seq_after_timeout = sender.getCurrentSeqNumber();

        runner.assert_test(sender.getState() == SenderState::WAITING,
            "Sender: состояние WAITING после обработки таймаута");
        runner.assert_test(seq_before_timeout == seq_after_timeout,
            "Sender: номер последовательности не изменился при таймауте");

        runner.assert_test(channel.hasPacket(),
            "Sender: пакет повторно отправлен после таймаута");
    }

    // WAITING -> WAITING при получении неправильного ACK
    {
        logger.logDetailed("Тест 4: Состояние WAITING сохраняется при неправильном ACK");
        Channel channel(0.0f);
        Sender sender(channel);

        sender.sendData("Тест неправильный ACK");
        int expected_seq = sender.getCurrentSeqNumber();
        runner.assert_test(sender.getState() == SenderState::WAITING,
            "Sender: предварительное состояние WAITING для теста неправильного ACK");

        int wrong_ack_number = 1 - expected_seq;
        Acknowledgement wrong_ack(wrong_ack_number);
        channel.sendAck(wrong_ack);
        logger.logDebug("Отправлен неправильный ACK с номером: " + to_string(wrong_ack_number) +
            " (ожидался: " + to_string(expected_seq) + ")");

        int seq_before = sender.getCurrentSeqNumber();
        sender.receiveAck();
        int seq_after = sender.getCurrentSeqNumber();

        logger.logDebug("Состояние после неправильного ACK: " + to_string(static_cast<int>(sender.getState())));

        runner.assert_test(sender.getState() == SenderState::WAITING,
            "Sender: состояние остается WAITING при неправильном ACK");
        runner.assert_test(seq_before == seq_after,
            "Sender: номер последовательности не изменился при неправильном ACK");
    }

    // отправка в WAITING должна игнорироваться
    {
        logger.logDetailed("Тест 5: Попытка отправки данных в состоянии WAITING");
        Channel channel(0.0f);
        Sender sender(channel);

        sender.sendData("Первое сообщение");
        runner.assert_test(sender.getState() == SenderState::WAITING,
            "Sender: состояние WAITING установлено");

        if (channel.hasPacket()) {
            channel.receivePacket();
        }

        sender.sendData("Второе сообщение (должно игнорироваться)");

        logger.logDebug("Состояние после попытки повторной отправки: " +
            to_string(static_cast<int>(sender.getState())));

        runner.assert_test(sender.getState() == SenderState::WAITING,
            "Sender: состояние остается WAITING при попытке повторной отправки");
        runner.assert_test(!channel.hasPacket(),
            "Sender: новый пакет не отправлен в состоянии WAITING");
    }

    // обработка ACK в READY должна игнорироваться
    {
        logger.logDetailed("Тест 6: Обработка ACK в состоянии READY (должна игнорироваться)");
        Channel channel(0.0f);
        Sender sender(channel);

        runner.assert_test(sender.getState() == SenderState::READY,
            "Sender: начальное состояние READY для теста ACK");

        Acknowledgement spurious_ack(0);
        channel.sendAck(spurious_ack);
        logger.logDebug("ACK отправлен в состоянии READY");

        int seq_before = sender.getCurrentSeqNumber();
        sender.receiveAck();
        int seq_after = sender.getCurrentSeqNumber();

        logger.logDebug("Состояние после обработки ACK в READY: " +
            to_string(static_cast<int>(sender.getState())));

        runner.assert_test(sender.getState() == SenderState::READY,
            "Sender: состояние остается READY при обработке ACK в READY");
        runner.assert_test(seq_before == seq_after,
            "Sender: номер последовательности не изменился при ACK в READY");
    }

    // Полный цикл READY -> WAITING -> READY
    {
        logger.logDetailed("Тест 7: Полный цикл взаимодействия отправителя");
        Channel channel(0.0f);
        Sender sender(channel);

        runner.assert_test(sender.getState() == SenderState::READY &&
            sender.getCurrentSeqNumber() == 0,
            "Sender: начальное состояние для полного цикла");

        sender.sendData("Первый пакет");
        runner.assert_test(sender.getState() == SenderState::WAITING,
            "Sender: READY -> WAITING при первой отправке");

        if (channel.hasPacket()) {
            Packet packet = channel.receivePacket();
            Acknowledgement ack(packet.sequence_number);
            channel.sendAck(ack);
        }
        sender.receiveAck();

        runner.assert_test(sender.getState() == SenderState::READY &&
            sender.getCurrentSeqNumber() == 1,
            "Sender: WAITING -> READY после первого ACK, seq=1");

        sender.sendData("Второй пакет");
        runner.assert_test(sender.getState() == SenderState::WAITING,
            "Sender: READY -> WAITING при второй отправке");

        if (channel.hasPacket()) {
            Packet packet = channel.receivePacket();
            Acknowledgement ack(packet.sequence_number);
            channel.sendAck(ack);
        }
        sender.receiveAck();

        runner.assert_test(sender.getState() == SenderState::READY &&
            sender.getCurrentSeqNumber() == 0,
            "Sender: WAITING -> READY после второго ACK, seq=0 (цикл завершен)");
    }

    logger.logDetailed("Все тесты переходов состояний отправителя завершены");
}

void test_sender_sequence_number_management(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- Тестирование управления номером последовательности Sender ---");

    Channel channel(0.0f);
    Sender sender(channel);
    Receiver receiver(channel);

    int initial_seq = sender.getCurrentSeqNumber();
    logger.logDetailed("Начальный номер последовательности: " + to_string(initial_seq));

    sender.sendData("Тест");
    logger.logDebug("Пакет отправлен");

    if (channel.hasPacket()) {
        Packet packet = channel.receivePacket();
        logger.logDebug("Получен пакет с seq_number: " + to_string(packet.sequence_number));
        Acknowledgement ack(packet.sequence_number);
        channel.sendAck(ack);
        logger.logDebug("Отправлен ACK с номером: " + to_string(ack.ack_number));
    }

    sender.receiveAck();

    int new_seq = sender.getCurrentSeqNumber();
    logger.logDetailed("Новый номер последовательности: " + to_string(new_seq));
    logger.logDebug("Ожидаемое изменение: " + to_string(initial_seq) + " -> " + to_string(1 - initial_seq));

    runner.assert_test(new_seq == 1 - initial_seq,
        "Sender: корректное изменение номера последовательности");
}

// тесты для Receiver
void test_receiver_initial_state(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- Тестирование начального состояния Receiver ---");

    Channel channel(0.0f);
    Receiver receiver(channel);

    logger.logDetailed("Создан получатель с каналом без потерь");
    logger.logDebug("Начальное состояние: " + to_string(static_cast<int>(receiver.getState())));
    logger.logDebug("Ожидаемый seq_number: " + to_string(receiver.getExpectedSeqNumber()));

    runner.assert_test(receiver.getState() == ReceiverState::WAITING,
        "Receiver: начальное состояние WAITING");
    runner.assert_test(receiver.getExpectedSeqNumber() == 0,
        "Receiver: ожидаемый номер последовательности = 0");
}

void test_receiver_state_transitions(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- Тестирование переходов состояний Receiver ---");

    // WAITING -> ACK_SENT
    {
        logger.logDetailed("Тест 1: Переход WAITING -> ACK_SENT при получении корректного пакета");
        Channel channel(0.0f);
        Receiver receiver(channel);

        logger.logDebug("Начальное состояние: " + to_string(static_cast<int>(receiver.getState())));
        runner.assert_test(receiver.getState() == ReceiverState::WAITING,
            "Receiver: начальное состояние WAITING");

        Packet correct_packet(0, "Корректный пакет");
        channel.sendPacket(correct_packet);
        logger.logDebug("Отправлен пакет с seq_number: " + to_string(correct_packet.sequence_number));

        ReceiverState state_before = receiver.getState();
        int expected_seq_before = receiver.getExpectedSeqNumber();
        receiver.receivePacket();
        ReceiverState state_after = receiver.getState();
        int expected_seq_after = receiver.getExpectedSeqNumber();

        logger.logDebug("Состояние до получения: " + to_string(static_cast<int>(state_before)));
        logger.logDebug("Состояние после получения: " + to_string(static_cast<int>(state_after)));
        logger.logDebug("Ожидаемый seq до: " + to_string(expected_seq_before));
        logger.logDebug("Ожидаемый seq после: " + to_string(expected_seq_after));

        runner.assert_test(state_after == ReceiverState::ACK_SENT,
            "Receiver: переход WAITING -> ACK_SENT при корректном пакете");
        runner.assert_test(expected_seq_after == 1 - expected_seq_before,
            "Receiver: ожидаемый номер последовательности изменился при корректном пакете");
        runner.assert_test(channel.hasAck(),
            "Receiver: ACK отправлен после получения корректного пакета");

        if (channel.hasAck()) {
            Acknowledgement ack = channel.receiveAck();
            logger.logDebug("Получен ACK с номером: " + to_string(ack.ack_number));
            runner.assert_test(ack.ack_number == correct_packet.sequence_number,
                "Receiver: корректный номер в отправленном ACK");
        }
    }

    // WAITING -> ACK_SENT состояние меняется, ожидаемый номер не меняется
    {
        logger.logDetailed("Тест 2: Переход WAITING -> ACK_SENT при получении дубликата");
        Channel channel(0.0f);
        Receiver receiver(channel);

        // Сначала отправляем корректный пакет
        Packet first_packet(0, "Первый пакет");
        channel.sendPacket(first_packet);
        receiver.receivePacket();

        // делаем обратно WAITING
        receiver.setState(ReceiverState::WAITING);
        int expected_seq_after_first = receiver.getExpectedSeqNumber();

        logger.logDebug("Ожидаемый seq после первого пакета: " + to_string(expected_seq_after_first));

        Packet duplicate_packet(0, "Дубликат пакета");
        channel.sendPacket(duplicate_packet);
        logger.logDebug("Отправлен дубликат с seq_number: " + to_string(duplicate_packet.sequence_number));

        ReceiverState state_before_duplicate = receiver.getState();
        int expected_seq_before_duplicate = receiver.getExpectedSeqNumber();
        receiver.receivePacket();
        ReceiverState state_after_duplicate = receiver.getState();
        int expected_seq_after_duplicate = receiver.getExpectedSeqNumber();

        logger.logDebug("Состояние до дубликата: " + to_string(static_cast<int>(state_before_duplicate)));
        logger.logDebug("Состояние после дубликата: " + to_string(static_cast<int>(state_after_duplicate)));
        logger.logDebug("Ожидаемый seq до дубликата: " + to_string(expected_seq_before_duplicate));
        logger.logDebug("Ожидаемый seq после дубликата: " + to_string(expected_seq_after_duplicate));

        runner.assert_test(state_after_duplicate == ReceiverState::ACK_SENT,
            "Receiver: переход WAITING -> ACK_SENT при получении дубликата");
        runner.assert_test(expected_seq_before_duplicate == expected_seq_after_duplicate,
            "Receiver: ожидаемый номер НЕ изменился при получении дубликата");
        runner.assert_test(channel.hasAck(),
            "Receiver: ACK отправлен даже для дубликата");

        if (channel.hasAck()) {
            Acknowledgement ack = channel.receiveAck();
            logger.logDebug("Получен ACK для дубликата с номером: " + to_string(ack.ack_number));
            runner.assert_test(ack.ack_number == duplicate_packet.sequence_number,
                "Receiver: корректный номер в ACK для дубликата");
        }
    }

    //  WAITING -> ACK_SENT при получении пакета с неправильным seq
    {
        logger.logDetailed("Тест 3: Переход WAITING -> ACK_SENT при получении пакета с неправильным seq_number");
        Channel channel(0.0f);
        Receiver receiver(channel);

        int expected_seq = receiver.getExpectedSeqNumber();
        int wrong_seq = 1 - expected_seq;

        Packet wrong_packet(wrong_seq, "Пакет с неправильным номером");
        channel.sendPacket(wrong_packet);
        logger.logDebug("Отправлен пакет с неправильным seq_number: " + to_string(wrong_seq) +
            " (ожидался: " + to_string(expected_seq) + ")");

        ReceiverState state_before = receiver.getState();
        int expected_seq_before = receiver.getExpectedSeqNumber();
        receiver.receivePacket();
        ReceiverState state_after = receiver.getState();
        int expected_seq_after = receiver.getExpectedSeqNumber();

        logger.logDebug("Состояние до получения неправильного пакета: " + to_string(static_cast<int>(state_before)));
        logger.logDebug("Состояние после получения неправильного пакета: " + to_string(static_cast<int>(state_after)));

        runner.assert_test(state_after == ReceiverState::ACK_SENT,
            "Receiver: переход WAITING -> ACK_SENT при неправильном пакете");
        runner.assert_test(expected_seq_before == expected_seq_after,
            "Receiver: ожидаемый номер НЕ изменился при неправильном пакете");
        runner.assert_test(channel.hasAck(),
            "Receiver: ACK отправлен даже для неправильного пакета");

        if (channel.hasAck()) {
            Acknowledgement ack = channel.receiveAck();
            logger.logDebug("Получен ACK для неправильного пакета с номером: " + to_string(ack.ack_number));
            runner.assert_test(ack.ack_number == wrong_packet.sequence_number,
                "Receiver: ACK содержит номер полученного пакета (даже неправильного)");
        }
    }

    // Поведение в ACK_SENT
    {
        logger.logDetailed("Тест 4: Обработка пакетов в состоянии ACK_SENT");
        Channel channel(0.0f);
        Receiver receiver(channel);

        // ставим ACK_SENT
        Packet first_packet(0, "Первый пакет");
        channel.sendPacket(first_packet);
        receiver.receivePacket();

        runner.assert_test(receiver.getState() == ReceiverState::ACK_SENT,
            "Receiver: предварительное состояние ACK_SENT установлено");

        // Отправляем 2 пакет в состоянии ACK_SENT
        Packet second_packet(1, "Второй пакет");
        channel.sendPacket(second_packet);
        logger.logDebug("Отправлен второй пакет в состоянии ACK_SENT");

        int expected_seq_before = receiver.getExpectedSeqNumber();
        receiver.receivePacket();
        int expected_seq_after = receiver.getExpectedSeqNumber();
        ReceiverState final_state = receiver.getState();

        logger.logDebug("Финальное состояние: " + to_string(static_cast<int>(final_state)));
        logger.logDebug("Ожидаемый seq до второго пакета: " + to_string(expected_seq_before));
        logger.logDebug("Ожидаемый seq после второго пакета: " + to_string(expected_seq_after));

        runner.assert_test(final_state == ReceiverState::ACK_SENT,
            "Receiver: состояние остается ACK_SENT после обработки в ACK_SENT");
        runner.assert_test(expected_seq_after == 1 - expected_seq_before,
            "Receiver: ожидаемый номер изменился при корректном втором пакете");
    }

    // Поведение в RECEIVED
    {
        logger.logDetailed("Тест 5: Обработка пакетов в состоянии RECEIVED");
        Channel channel(0.0f);
        Receiver receiver(channel);

        receiver.setState(ReceiverState::RECEIVED);
        logger.logDebug("Установлено состояние RECEIVED вручную");

        runner.assert_test(receiver.getState() == ReceiverState::RECEIVED,
            "Receiver: состояние RECEIVED установлено");

        // Отправляем пакет
        Packet test_packet(0, "Тест в RECEIVED");
        channel.sendPacket(test_packet);

        int expected_seq_before = receiver.getExpectedSeqNumber();
        receiver.receivePacket();
        int expected_seq_after = receiver.getExpectedSeqNumber();
        ReceiverState state_after = receiver.getState();

        logger.logDebug("Состояние после обработки в RECEIVED: " + to_string(static_cast<int>(state_after)));

        runner.assert_test(state_after == ReceiverState::ACK_SENT,
            "Receiver: переход RECEIVED -> ACK_SENT при обработке пакета");
        runner.assert_test(expected_seq_after == 1 - expected_seq_before,
            "Receiver: ожидаемый номер изменился при корректном пакете в RECEIVED");
    }

    // Полный цикл
    {
        logger.logDetailed("Тест 6: Полный цикл взаимодействия получателя");
        Channel channel(0.0f);
        Receiver receiver(channel);

        runner.assert_test(receiver.getState() == ReceiverState::WAITING &&
            receiver.getExpectedSeqNumber() == 0,
            "Receiver: начальное состояние для полного цикла");

        // Первый пакет
        Packet packet_0(0, "Пакет 0");
        channel.sendPacket(packet_0);
        receiver.receivePacket();

        runner.assert_test(receiver.getState() == ReceiverState::ACK_SENT &&
            receiver.getExpectedSeqNumber() == 1,
            "Receiver: после первого пакета - ACK_SENT, ожидаем 1");

        // Возвращаем WAITING
        receiver.setState(ReceiverState::WAITING);

        // Второй пакет
        Packet packet_1(1, "Пакет 1");
        channel.sendPacket(packet_1);
        receiver.receivePacket();

        runner.assert_test(receiver.getState() == ReceiverState::ACK_SENT &&
            receiver.getExpectedSeqNumber() == 0,
            "Receiver: после второго пакета - ACK_SENT, ожидаем 0 (цикл завершен)");
    }

    {
        logger.logDetailed("Тест 7: Поведение при отсутствии пакетов в канале");
        Channel channel(0.0f);
        Receiver receiver(channel);

        ReceiverState state_before = receiver.getState();
        int expected_seq_before = receiver.getExpectedSeqNumber();

        // Вызываем receivePacket() при пустом канале
        receiver.receivePacket();

        ReceiverState state_after = receiver.getState();
        int expected_seq_after = receiver.getExpectedSeqNumber();

        logger.logDebug("Состояние до вызова receivePacket() на пустом канале: " +
            to_string(static_cast<int>(state_before)));
        logger.logDebug("Состояние после вызова receivePacket() на пустом канале: " +
            to_string(static_cast<int>(state_after)));

        runner.assert_test(state_before == state_after,
            "Receiver: состояние не изменилось при отсутствии пакетов");
        runner.assert_test(expected_seq_before == expected_seq_after,
            "Receiver: ожидаемый номер не изменился при отсутствии пакетов");
        runner.assert_test(!channel.hasAck(),
            "Receiver: ACK не отправлен при отсутствии пакетов");
    }

    {
        logger.logDetailed("Тест 8: Последовательная обработка множественных пакетов");
        Channel channel(0.0f);
        Receiver receiver(channel);

        // Отправляем несколько пакетов подряд
        for (int i = 0; i < 4; i++) {
            int expected_seq = receiver.getExpectedSeqNumber();
            Packet packet(expected_seq, "Пакет " + to_string(i));
            channel.sendPacket(packet);

            logger.logDebug("Отправлен пакет " + to_string(i) + " с seq_number: " + to_string(expected_seq));

            receiver.receivePacket();

            runner.assert_test(receiver.getState() == ReceiverState::ACK_SENT,
                "Receiver: состояние ACK_SENT после пакета " + to_string(i));
            runner.assert_test(channel.hasAck(),
                "Receiver: ACK отправлен для пакета " + to_string(i));

            if (channel.hasAck()) {
                channel.receiveAck(); // Очищаем ACK из канала
            }

            // Сбрасываем состояние
            receiver.setState(ReceiverState::WAITING);
        }

        runner.assert_test(receiver.getExpectedSeqNumber() == 0,
            "Receiver: номер последовательности вернулся к 0 после четырех пакетов");
    }

    logger.logDetailed("Все тесты переходов состояний получателя завершены");
}

void test_receiver_duplicate_detection(TestRunner& runner, Logger& logger) {
    logger.logBasic("\n--- Тестирование обнаружения дубликатов Receiver ---");

    Channel channel(0.0f);
    Receiver receiver(channel);

    Packet packet1(0, "Первый пакет");
    channel.sendPacket(packet1);
    logger.logDetailed("Отправлен первый пакет с seq_number: 0");

    receiver.receivePacket();
    int expected_after_first = receiver.getExpectedSeqNumber();
    logger.logDebug("Ожидаемый seq_number после первого пакета: " + to_string(expected_after_first));

    // Отправляем дубликат
    Packet duplicate(0, "Дубликат");
    channel.sendPacket(duplicate);
    logger.logDetailed("Отправлен дубликат с seq_number: 0");

    // Получаем дубликат
    receiver.receivePacket();
    int expected_after_duplicate = receiver.getExpectedSeqNumber();
    logger.logDebug("Ожидаемый seq_number после дубликата: " + to_string(expected_after_duplicate));

    runner.assert_test(expected_after_first == expected_after_duplicate,
        "Receiver: ожидаемый номер не изменился после дубликата");
}

// Тесты Ask
void test_acknowledgement_initialization(TestRunner& runner, Logger& logger) {
    cout << "\n--- Тестирование инициализации Acknowledgement ---" << endl;

    // инициализация по умолчанию
    Acknowledgement default_ack;
    runner.assert_test(default_ack.ack_number == 0,
        "Acknowledgement: инициализация по умолчанию");

    // с параметром
    Acknowledgement param_ack(5);
    runner.assert_test(param_ack.ack_number == 5,
        "Acknowledgement: инициализация с параметром");
}

// Тесты Channel
void test_channel_basic_operations(TestRunner& runner, Logger& logger) {
    cout << "\n--- Тестирование базовых операций Channel ---" << endl;

    Channel channel(0.0f);

    // Тест отправки и получения
    Packet test_packet(1, "Тест");
    channel.sendPacket(test_packet);

    runner.assert_test(channel.hasPacket(),
        "Channel: пакет доступен после отправки");

    Packet received = channel.receivePacket();
    runner.assert_test(received.sequence_number == test_packet.sequence_number,
        "Channel: корректная передача номера последовательности");
    runner.assert_test(string(received.data) == string(test_packet.data),
        "Channel: корректная передача данных");

    // отправка и получение ACK
    Acknowledgement test_ack(2);
    channel.sendAck(test_ack);

    runner.assert_test(channel.hasAck(),
        "Channel: ACK доступен после отправки");

    Acknowledgement received_ack = channel.receiveAck();
    runner.assert_test(received_ack.ack_number == test_ack.ack_number,
        "Channel: корректная передача ACK");
}
