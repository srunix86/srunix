#ifndef HELP_H
#define HELP_H

#include <stddef.h>
#include "port.h"

void terminal_writestring(const char* data);
char keyboard_getchar();

void execute_help(int page) {
    switch(page) {
        case 1:
	    terminal_setcolor(COLOR_BRIGHT_GREEN, COLOR_BLACK);
            terminal_writestring("Commands (page 1/2):\n");
	    terminal_setcolor(COLOR_GRAY, COLOR_BLACK);
            terminal_writestring("help [page] - Show help\n");
            terminal_writestring("cls - Clear screen\n");
            terminal_writestring("ver - Show version\n");
            terminal_writestring("ls - List files\n");
            terminal_writestring("pwd - Print working directory\n");
            terminal_writestring("cd - Change directory\n");
            terminal_writestring("echo - Display message\n");
            terminal_writestring("cat - Display file contents\n");
            terminal_writestring("date - Show current date/time\n");
            terminal_writestring("time - Show current time\n");
            terminal_writestring("whoami - Show current user\n");
            terminal_writestring("mem - Show memory usage\n");
	    terminal_setcolor(COLOR_BRIGHT_GREEN, COLOR_BLACK);
            terminal_writestring("Type 'help 2' for more commands\n");
	    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
            break;
        case 2:
	    terminal_setcolor(COLOR_BRIGHT_GREEN, COLOR_BLACK);
            terminal_writestring("Commands (page 2/2):\n");
	    terminal_setcolor(COLOR_GRAY, COLOR_BLACK);
            terminal_writestring("kptest - Test Kernel Panic\n");
            terminal_writestring("lsblk - Show disk information\n");
            terminal_writestring("pause - Wait for keypress\n");
            terminal_writestring("poweroff - Shut down\n");
            terminal_writestring("reboot - Reboot\n");
            terminal_writestring("exit - Log out\n");
            terminal_writestring("touch - Create file\n");
            terminal_writestring("mkdir - Create directory\n");
            terminal_writestring("rm - Delete file (use -rf for directories)\n");
            terminal_writestring("beep - Play test sound\n");
            terminal_writestring("smouse - Test a mouse support\n");
	    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
            break;
        default:
            terminal_writestring("Invalid help page\n");
            break;
    }
}

#endif
