#ifndef INFO_H
#define INFO_H

#include <stddef.h>
#include "../lib/pring.h"

void terminal_writestring(const char* data);
char keyboard_getchar();

static void display_info() {
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring("  *##*                 ****                *##*  \n");
    terminal_writestring("   *####*****      *##########*      *****#####   \n");
    terminal_writestring("    *###  ******   ***** #*####*#  +****  ###*    \n");
    terminal_writestring("     *##     #*****        @*####*        ##*     \n");
    terminal_writestring("      *#*       ##****       #*####*     *#*      \n");
    terminal_writestring("       ***  ***#*  *#***  ***  *####**  *#*       \n");
    terminal_writestring("          *#####*    *  *****    **** *##*        \n");
    terminal_writestring("        *#####*       ****#          ###*         ");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring("OS: Srunix86\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring("      *#####*       ****%  ***      *#*# ***      ");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring("Release: 0.0.5-1\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring("    #*####*  ***  **#*#    @*##*  ****  *###*#    ");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring("License: BSD-3-Clause license\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring("   *#####*   *##*  **        *###* #*    *####*   ");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring("Langs: ASM, Linker and C\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring("  *####*       *#* *#*        *#* #*     *####*   ");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring("POSIX support\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring("   *####*    *# *###*        **  *##*   *#####*   ");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring("FS: bkFS 0.3.1 \n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring("    %*###*  ****  *##*@    #*#**  ***  *####*#    ");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring("Loader: GRUB2\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring("      *** %*#*      ***  #****       *#####*      ");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring("Arch: x86_64\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring("         *###          #****       *#####*        ");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring("Uploaded: /boot/krn.img\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring("        *##* ****    *****  *    *#####*          \n");
    terminal_writestring("       *#*  **####*  ***  ***#*  *#***  ***       \n");
    terminal_writestring("      *#*     *####*#       ****##       *#*      \n");
    terminal_writestring("     *##        *####*@        *****#     ##*     \n");
    terminal_writestring("    *###  ****+  #*####** *****   ******  ###*    \n");
    terminal_writestring("   #####*****      *##########*      *****####*   \n");
    terminal_writestring("   *##***%           *######*            ****#*   \n");
    terminal_writestring("  ***                  ****                   **  \n");
}

void info_program_run() {
    display_info();
    keyboard_getchar();
}

#endif
