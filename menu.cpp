#include "menu.h"
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <chrono>
#include <thread>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#endif

using namespace std;

// Цветовые коды для консоли
enum ConsoleColor {
    COLOR_RESET = 0,
    COLOR_BLACK = 30,
    COLOR_RED = 31,
    COLOR_GREEN = 32,
    COLOR_YELLOW = 33,
    COLOR_BLUE = 34,
    COLOR_MAGENTA = 35,
    COLOR_CYAN = 36,
    COLOR_WHITE = 37,
    COLOR_BRIGHT_BLACK = 90,
    COLOR_BRIGHT_RED = 91,
    COLOR_BRIGHT_GREEN = 92,
    COLOR_BRIGHT_YELLOW = 93,
    COLOR_BRIGHT_BLUE = 94,
    COLOR_BRIGHT_MAGENTA = 95,
    COLOR_BRIGHT_CYAN = 96,
    COLOR_BRIGHT_WHITE = 97
};

MenuManager::MenuManager() : selectedIndex(0), useColorOutput(true) {
    initializeMenus();
}

MenuManager::~MenuManager() {
    resetColor();
}

void MenuManager::initializeMenus() {
    // Инициализация главного меню
    mainMenuItems = {
        MenuItem(1, "Запустить все тесты", "Выполнить полный набор тестов"),
        MenuItem(-1, "", "UNIT ТЕСТЫ", false), // Разделитель
        MenuItem(2, "Все UNIT тесты", "Модульные тесты компонентов"),
        MenuItem(3, "Тесты пакетов", "Проверка создания и обработки пакетов"),
        MenuItem(4, "Тесты отправителя", "Логика отправки данных"),
        MenuItem(5, "Тесты получателя", "Логика приема данных"),
        MenuItem(6, "Тесты канала", "Работа канала связи и ACK"),
        MenuItem(-1, "", "ИНТЕГРАЦИОННЫЕ ТЕСТЫ", false), // Разделитель
        MenuItem(7, "Интеграционные тесты", "Комплексные тесты взаимодействия"),
        MenuItem(8, "Тесты передачи", "Полный цикл передачи данных"),
        MenuItem(9, "Восстановление потерь", "Обработка потерянных пакетов"),
        MenuItem(10, "Тесты таймаутов", "Проверка временных ограничений"),
        MenuItem(11, "Тесты производительности", "Измерение скорости и эффективности"),
        MenuItem(-1, "", "НАСТРОЙКИ", false), // Разделитель
        MenuItem(12, "Логирование", "Управление уровнем детализации логов"),
        MenuItem(0, "Выход", "Завершить программу")
    };

    // Инициализация меню логирования
    loggingMenuItems = {
        MenuItem(1, "NONE", "Без вывода логов"),
        MenuItem(2, "BASIC", "Только результаты тестов"),
        MenuItem(3, "DETAILED", "Подробная информация"),
        MenuItem(4, "DEBUG", "Отладочная информация"),
        MenuItem(0, "Назад", "Вернуться в главное меню")
    };
}

void MenuManager::clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void MenuManager::setColor(int color) {
    if (!useColorOutput) return;

#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
#else
    cout << "\033[" << color << "m";
#endif
}

void MenuManager::resetColor() {
    if (!useColorOutput) return;

#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, 7); // Белый цвет
#else
    cout << "\033[0m";
#endif
}

void MenuManager::printHeader(const string& title) {
    cout << "\n+-----------------------------------------------------------------------+\n";
    cout << "|";

    int totalWidth = 71;
    int padding = (totalWidth - title.length()) / 2;
    cout << setw(padding) << "" << title << setw(totalWidth - padding - title.length()) << "";

    cout << "|\n";
    cout << "+-----------------------------------------------------------------------+\n";
}

void MenuManager::printSeparator(const string& text) {
    if (text.empty()) {
        cout << "------------------------------------------------------------------------\n";
    }
    else {
        cout << "\n--- " << text << " ";
        int dashCount = 65 - text.length();
        if (dashCount > 0) {
            for (int i = 0; i < dashCount; i++) cout << "-";
        }
        cout << "\n";
    }
}

void MenuManager::printMenuItem(const MenuItem& item, bool isSelected, bool showIndex) {
    if (!item.isActive) {
        printSeparator(item.description);
        return;
    }

    if (isSelected) {
        setColor(COLOR_BRIGHT_GREEN);
        cout << " > ";
    }
    else {
        cout << "   ";
    }

    if (isSelected) {
        setColor(COLOR_BRIGHT_WHITE);
    }
    else {
        setColor(COLOR_WHITE);
    }
    cout << item.title;

    if (!item.description.empty() && item.isActive) {
        cout << " - " << item.description;
    }

    resetColor();
    cout << "\n";
}

void MenuManager::printFooter() {
    resetColor();
    cout << "\n------------------------------------------------------------------------\n";
    cout << "Используйте стрелки ↑↓ для навигации, Enter для выбора, Esc для выхода\n";
}

int MenuManager::getInteractiveChoice(const std::vector<MenuItem>& items, const std::string& title) {
    int index = 0;
    // Найти первый активный элемент
    while (index < items.size() && !items[index].isActive) {
        index++;
    }

    while (true) {
        clearScreen();
        printHeader(title);

        for (size_t i = 0; i < items.size(); i++) {
            if (!items[i].isActive) {
                printSeparator(items[i].description);
            }
            else {
                printMenuItem(items[i], i == index, false);
            }
        }

        printFooter();

#ifdef _WIN32
        int key = _getch();

        if (key == 224) { // стрелочная клавиша в Windows
            int arrow = _getch();
            if (arrow == 72) { // вверх
                do {
                    index = (index - 1 + items.size()) % items.size();
                } while (!items[index].isActive);
            }
            else if (arrow == 80) { // вниз
                do {
                    index = (index + 1) % items.size();
                } while (!items[index].isActive);
            }
        }
        else if (key == 13) { // Enter
            return items[index].id;
        }
        else if (key == 27) { // Escape
            return 0;
        }
#endif
    }
}

void MenuManager::displayMainMenu() {
    // Используем интерактивное меню вместо обычного
    int choice = getInteractiveChoice(mainMenuItems, "ТЕСТИРОВАНИЕ STOP-AND-WAIT ПРОТОКОЛА");
    lastChoice = choice;
}

void MenuManager::displayLoggingMenu(Logger& logger) {
    clearScreen();
    printHeader("НАСТРОЙКИ ЛОГИРОВАНИЯ");

    // Показать текущий уровень
    cout << "\nТекущий уровень логирования: ";
    cout << getLogLevelString(logger.getLevel()) << "\n";
    resetColor();

    int choice = getInteractiveChoice(loggingMenuItems, "НАСТРОЙКИ ЛОГИРОВАНИЯ");

    switch (choice) {
    case 1:
        logger.setLevel(LogLevel::NONE);
        showTestResults("Настройка логирования", true, "Установлен уровень: NONE");
        waitForKeyPress();
        break;
    case 2:
        logger.setLevel(LogLevel::BASIC);
        showTestResults("Настройка логирования", true, "Установлен уровень: BASIC");
        waitForKeyPress();
        break;
    case 3:
        logger.setLevel(LogLevel::DETAILED);
        showTestResults("Настройка логирования", true, "Установлен уровень: DETAILED");
        waitForKeyPress();
        break;
    case 4:
        logger.setLevel(LogLevel::DEBUG);
        showTestResults("Настройка логирование", true, "Установлен уровень: DEBUG");
        waitForKeyPress();
        break;
    case 0:
        break;
    default:
        break;
    }
}

string MenuManager::getLogLevelString(LogLevel level) {
    switch (level) {
    case LogLevel::NONE: return "NONE (без логов)";
    case LogLevel::BASIC: return "BASIC (результаты тестов)";
    case LogLevel::DETAILED: return "DETAILED (подробная информация)";
    case LogLevel::DEBUG: return "DEBUG (отладочная информация)";
    default: return "UNKNOWN";
    }
}

int MenuManager::getUserChoice() {
    return lastChoice;
}

void MenuManager::handleLoggingMenu(Logger& logger) {
    displayLoggingMenu(logger);
}

void MenuManager::showTestResults(const string& testName, bool passed, const string& details) {
    clearScreen();

    if (passed) {
        setColor(COLOR_BRIGHT_GREEN);
        cout << "\n✅ [УСПЕШНО] " << testName << "\n";
    }
    else {
        setColor(COLOR_BRIGHT_RED);
        cout << "\n❌ [ОШИБКА] " << testName << "\n";
    }

    if (!details.empty()) {
        setColor(COLOR_WHITE);
        cout << "\nДетали: " << details << "\n";
    }

    resetColor();
}

void MenuManager::waitForKeyPress(const string& message) {
    cout << "\n" << message;

#ifdef _WIN32
    _getch();
#else
    getch();
#endif
}

void MenuManager::showWelcomeScreen() {
    // Убрана заставка - просто очищаем экран
    clearScreen();
}

void MenuManager::showExitScreen() {
    // Убрана конечная заставка - просто очищаем экран
    clearScreen();
    cout << "До свидания!\n";
    resetColor();
}

void MenuManager::showProgressBar(int progress, int total, const string& text) {
    const int barWidth = 50;
    float percentage = (float)progress / total;
    int filled = (int)(barWidth * percentage);

    cout << "\r";
    cout << text << " [";

    for (int i = 0; i < filled; i++) cout << "█";

    for (int i = filled; i < barWidth; i++) cout << "░";

    cout << "] " << (int)(percentage * 100) << "%";
    resetColor();
    cout.flush();
}

// Глобальные функции для совместимости с существующим кодом
static MenuManager globalMenu;

void display_main_menu() {
    globalMenu.displayMainMenu();
}

void display_logging_menu(Logger& logger) {
    globalMenu.displayLoggingMenu(logger);
}

int get_user_choice() {
    return globalMenu.getUserChoice();
}

void handle_logging_menu(Logger& logger) {
    globalMenu.handleLoggingMenu(logger);
}

void show_welcome_screen() {
    globalMenu.showWelcomeScreen();
}

void show_exit_screen() {
    globalMenu.showExitScreen();
}

void show_test_results(const string& testName, bool passed, const string& details) {
    globalMenu.showTestResults(testName, passed, details);
}