#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <clocale>
#include "logger.h"
#include "menu.h"
#include "test_suites.h"

using namespace std;

int main() {
    setlocale(LC_ALL, "Russian");

    Logger logger;
    logger.setLevel(LogLevel::BASIC); // Уровень по умолчанию

    int choice;
    do {
        display_main_menu();
        choice = get_user_choice();

        switch (choice) {
        case 1:
            run_all_tests(logger);
            break;
        case 2:
            run_unit_tests(logger);
            break;
        case 3:
            run_packet_tests(logger);
            break;
        case 4:
            run_sender_tests(logger);
            break;
        case 5:
            run_receiver_tests(logger);
            break;
        case 6:
            run_channel_tests(logger);
            break;
        case 7:
            run_integration_tests(logger);
            break;
        case 8:
            run_transmission_tests(logger);
            break;
        case 9:
            run_loss_recovery_tests(logger);
            break;
        case 10:
            run_timeout_tests(logger);
            break;
        case 11:
            run_performance_tests(logger);
            break;
        case 12:
            handle_logging_menu(logger);
            break;
        case 0:
            cout << "Завершение программы..." << endl;
            break;
        default:
            cout << "Неверный выбор. Попробуйте снова." << endl;
        }

        if (choice >= 1 && choice <= 10) {
            cout << "\nНажмите Enter для продолжения...";
            cin.get();
        }

    } while (choice != 0);

    return 0;
}