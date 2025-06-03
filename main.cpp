#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <clocale>
#include "logger.h"
#include "menu.h"
#include "test_suites.h"
#include <conio.h>

using namespace std;

int main() {
    setlocale(LC_ALL, "Russian");

    show_welcome_screen();

    Logger logger;
    logger.setLevel(LogLevel::BASIC); // Уровень по умолчанию

    int choice;
    do {
        display_main_menu();
        choice = get_user_choice();

        switch (choice) {
        case 1:
            show_test_results("Запуск всех тестов", true, "Выполняется полный набор тестов...");
            run_all_tests(logger);
            break;
        case 2:
            show_test_results("UNIT тесты", true, "Выполняются модульные тесты...");
            run_unit_tests(logger);
            break;
        case 3:
            show_test_results("Тесты пакетов", true, "Проверка работы с пакетами...");
            run_packet_tests(logger);
            break;
        case 4:
            show_test_results("Тесты отправителя", true, "Проверка логики отправки...");
            run_sender_tests(logger);
            break;
        case 5:
            show_test_results("Тесты получателя", true, "Проверка логики приема...");
            run_receiver_tests(logger);
            break;
        case 6:
            show_test_results("Тесты канала", true, "Проверка канала связи...");
            run_channel_tests(logger);
            break;
        case 7:
            show_test_results("Интеграционные тесты", true, "Выполняются комплексные тесты...");
            run_integration_tests(logger);
            break;
        case 8:
            show_test_results("Тесты передачи", true, "Проверка передачи данных...");
            run_transmission_tests(logger);
            break;
        case 9:
            show_test_results("Тесты восстановления", true, "Проверка восстановления после потерь...");
            run_loss_recovery_tests(logger);
            break;
        case 10:
            show_test_results("Тесты таймаутов", true, "Проверка временных ограничений...");
            run_timeout_tests(logger);
            break;
        case 11:
            show_test_results("Тесты производительности", true, "Измерение производительности...");
            run_performance_tests(logger);
            break;
        case 12:
            handle_logging_menu(logger);
            break;
        case 0:
            show_exit_screen();
            break;
        default:
            break;
        }

        // Пауза после тестов (не для настроек)
        if (choice >= 1 && choice <= 11) {
            cout << "\n Тесты завершены. Нажмите любую клавишу для возврата в меню...";
#ifdef _WIN32
            _getch();
#endif
        }

    } while (choice != 0);

    return 0;
}