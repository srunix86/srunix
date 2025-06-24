#ifndef INFO_H
#define INFO_H

#include <stddef.h>

void terminal_writestring(const char* data);
char keyboard_getchar();

static void display_info() {
    terminal_writestring("  .--.--.    \n");
    terminal_writestring(" /  /    '.  A free and OpenSource Operation System - Srunix R.E.\n");
    terminal_writestring("|  :  /`. /  A simple and non-commercial OS similar to Unix and\n");
    terminal_writestring(";  |  |--`   Unix-like systems designed for educational OS development\n");
    terminal_writestring("|  :  ;_     and fun. A reworking of the original Srunix, which was\n");
    terminal_writestring(" \\  \\    `.  designed for typewriters, Srunix R.E. It is also made\n");
    terminal_writestring("  `----.   \\ for personal PCs. Distributed under the BSD 3 license.\n");
    terminal_writestring(" /  /`--'  / You can easily fork the OS and remake it however you want.\n");
    terminal_writestring("  __ \\  \\  | __________________________________________________________\n");
    terminal_writestring("'--'.     /                      Srunix R.E.\n");
    terminal_writestring("  `--'---'\n");
}

void info_program_run() {
    display_info();
    keyboard_getchar();
}

#endif
