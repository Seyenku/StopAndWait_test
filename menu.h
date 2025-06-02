#ifndef MENU_H
#define MENU_H

#include "logger.h"

void display_main_menu();
void display_logging_menu(Logger& logger);
int get_user_choice();
void handle_logging_menu(Logger& logger);

#endif