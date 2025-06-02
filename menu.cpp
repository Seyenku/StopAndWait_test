#include "menu.h"
#include <iostream>
#include <cstdlib>

using namespace std;

void display_main_menu() {
    system("cls");
    cout << "\n========================================" << endl;
    cout << "    ������������ STOP-AND-WAIT" << endl;
    cout << "========================================" << endl;
    cout << "1. ��������� ��� �����" << endl;
    cout << "----------------------------------------" << endl;
    cout << "UNIT �����:" << endl;
    cout << "2. ��� UNIT �����" << endl;
    cout << "3. ����� ������� � ������" << endl;
    cout << "4. ����� �����������" << endl;
    cout << "5. ����� ����������" << endl;
    cout << "6. ����� ������ � ACK" << endl;
    cout << "----------------------------------------" << endl;
    cout << "�������������� �����:" << endl;
    cout << "7. ��� �������������� �����" << endl;
    cout << "8. ����� �������� ������" << endl;
    cout << "9. ����� �������������� ����� ������" << endl;
    cout << "10. ����� ���������" << endl;
    cout << "11. ����� ������������������" << endl;
    cout << "----------------------------------------" << endl;
    cout << "12. ��������� �����������" << endl;
    cout << "0. �����" << endl;
    cout << "========================================" << endl;
    cout << "�������� �����: ";
}

void display_logging_menu(Logger& logger) {
    system("cls");
    cout << "\n========================================" << endl;
    cout << "      ��������� �����������" << endl;
    cout << "========================================" << endl;
    cout << "������� �������: ";

    switch (logger.getLevel()) {
    case LogLevel::NONE: cout << "NONE (��� �����)"; break;
    case LogLevel::BASIC: cout << "BASIC (������ ����������)"; break;
    case LogLevel::DETAILED: cout << "DETAILED (��������)"; break;
    case LogLevel::DEBUG: cout << "DEBUG (�������)"; break;
    }
    cout << endl << endl;

    cout << "1. NONE - ��� �����" << endl;
    cout << "2. BASIC - ������ ���������� ������" << endl;
    cout << "3. DETAILED - ��������� ����������" << endl;
    cout << "4. DEBUG - ���������� ����������" << endl;
    cout << "0. ����� � ������� ����" << endl;
    cout << "========================================" << endl;
    cout << "�������� �������: ";
}

int get_user_choice() {
    int choice;
    while (!(cin >> choice)) {
        cout << "������ �����. ����������, ������� �����: ";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    cin.ignore(); // ������� ������
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
            cout << "���������� ������� �����������: NONE" << endl;
            break;
        case 2:
            logger.setLevel(LogLevel::BASIC);
            cout << "���������� ������� �����������: BASIC" << endl;
            break;
        case 3:
            logger.setLevel(LogLevel::DETAILED);
            cout << "���������� ������� �����������: DETAILED" << endl;
            break;
        case 4:
            logger.setLevel(LogLevel::DEBUG);
            cout << "���������� ������� �����������: DEBUG" << endl;
            break;
        case 0:
            break;
        default:
            cout << "�������� �����. ���������� �����." << endl;
        }

        if (choice >= 1 && choice <= 4) {
            cout << "������� Enter ��� �����������...";
            cin.get();
        }
    } while (choice != 0);
}