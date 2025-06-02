#include "menu.h"
#include <iostream>
#include <cstdlib>

using namespace std;

void display_main_menu() {
    system("cls");
    cout << "\n========================================" << endl;
    cout << "    ТЕСТИРОВАНИЕ STOP-AND-WAIT" << endl;
    cout << "========================================" << endl;
    cout << "1. Запустить все тесты" << endl;
    cout << "----------------------------------------" << endl;
    cout << "UNIT ТЕСТЫ:" << endl;
    cout << "2. Все UNIT тесты" << endl;
    cout << "3. Тесты пакетов и потерь" << endl;
    cout << "4. Тесты отправителя" << endl;
    cout << "5. Тесты получателя" << endl;
    cout << "6. Тесты канала и ACK" << endl;
    cout << "----------------------------------------" << endl;
    cout << "ИНТЕГРАЦИОННЫЕ ТЕСТЫ:" << endl;
    cout << "7. Все интеграционные тесты" << endl;
    cout << "8. Тесты передачи данных" << endl;
    cout << "9. Тесты восстановления после потерь" << endl;
    cout << "10. Тесты таймаутов" << endl;
    cout << "11. Тесты производительности" << endl;
    cout << "----------------------------------------" << endl;
    cout << "12. Настройки логирования" << endl;
    cout << "0. Выход" << endl;
    cout << "========================================" << endl;
    cout << "Выберите опцию: ";
}

void display_logging_menu(Logger& logger) {
    system("cls");
    cout << "\n========================================" << endl;
    cout << "      НАСТРОЙКИ ЛОГИРОВАНИЯ" << endl;
    cout << "========================================" << endl;
    cout << "Текущий уровень: ";

    switch (logger.getLevel()) {
    case LogLevel::NONE: cout << "NONE (без логов)"; break;
    case LogLevel::BASIC: cout << "BASIC (только результаты)"; break;
    case LogLevel::DETAILED: cout << "DETAILED (подробно)"; break;
    case LogLevel::DEBUG: cout << "DEBUG (отладка)"; break;
    }
    cout << endl << endl;

    cout << "1. NONE - без логов" << endl;
    cout << "2. BASIC - только результаты тестов" << endl;
    cout << "3. DETAILED - подробная информация" << endl;
    cout << "4. DEBUG - отладочная информация" << endl;
    cout << "0. Назад в главное меню" << endl;
    cout << "========================================" << endl;
    cout << "Выберите уровень: ";
}

int get_user_choice() {
    int choice;
    while (!(cin >> choice)) {
        cout << "Ошибка ввода. Пожалуйста, введите число: ";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    cin.ignore(); // Очистка буфера
    system("cls");
    return choice;
}

void handle_logging_menu(Logger& logger) {
    int choice;
    do {
        display_logging_menu(logger);
        choice = get_user_choice();

        switch (choice) {
        case 1:
            logger.setLevel(LogLevel::NONE);
            cout << "Установлен уровень логирования: NONE" << endl;
            break;
        case 2:
            logger.setLevel(LogLevel::BASIC);
            cout << "Установлен уровень логирования: BASIC" << endl;
            break;
        case 3:
            logger.setLevel(LogLevel::DETAILED);
            cout << "Установлен уровень логирования: DETAILED" << endl;
            break;
        case 4:
            logger.setLevel(LogLevel::DEBUG);
            cout << "Установлен уровень логирования: DEBUG" << endl;
            break;
        case 0:
            break;
        default:
            cout << "Неверный выбор. Попробуйте снова." << endl;
        }

        if (choice >= 1 && choice <= 4) {
            cout << "Нажмите Enter для продолжения...";
            cin.get();
        }
    } while (choice != 0);
}