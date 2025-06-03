#ifndef MENU_H
#define MENU_H

#include "logger.h"
#include <string>
#include <vector>

struct MenuItem {
    int id;
    std::string title;
    std::string description;
    bool isActive;

    MenuItem(int _id, const std::string& _title, const std::string& _desc = "", bool _active = true)
        : id(_id), title(_title), description(_desc), isActive(_active) {
    }
};

class MenuManager {
private:
    std::vector<MenuItem> mainMenuItems;
    std::vector<MenuItem> loggingMenuItems;
    int selectedIndex;
    int lastChoice;
    bool useColorOutput;

    void initializeMenus();
    void clearScreen();
    void printHeader(const std::string& title);
    void printFooter();
    void printMenuItem(const MenuItem& item, bool isSelected, bool showIndex = true);
    void printSeparator(const std::string& text = "");
    void setColor(int color);
    void resetColor();
    std::string getLogLevelString(LogLevel level);
    void showProgressBar(int progress, int total, const std::string& text = "");

public:
    MenuManager();
    ~MenuManager();

    void displayMainMenu();
    void displayLoggingMenu(Logger& logger);
    int getUserChoice();
    int getInteractiveChoice(const std::vector<MenuItem>& items, const std::string& title);
    void handleLoggingMenu(Logger& logger);
    void showTestResults(const std::string& testName, bool passed, const std::string& details = "");
    void waitForKeyPress(const std::string& message = "Нажмите любую клавишу для продолжения...");
    void showWelcomeScreen();
    void showExitScreen();
    void enableColorOutput(bool enable) { useColorOutput = enable; }
};

// Глобальные функции для совместимости
void display_main_menu();
void display_logging_menu(Logger& logger);
int get_user_choice();
void handle_logging_menu(Logger& logger);

void show_welcome_screen();
void show_exit_screen();
void show_test_results(const std::string& testName, bool passed, const std::string& details = "");

#endif