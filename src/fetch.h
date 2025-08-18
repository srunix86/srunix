#include "pr.h"
#include "port.h"
void execute_fetch() {
    char cpu_name[64] = {0};
    get_cpu_name(cpu_name);
    uint32_t total_mem_kb, used_mem_kb;
    get_memory_info(&total_mem_kb, &used_mem_kb);
    char resolution[32] = {0};
    get_resolution(resolution);
    char uptime_str[32] = {0};
    uint32_t seconds = timer_ticks / TIMER_HZ;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    char hours_str[8], mins_str[8];
    int_to_str(hours, hours_str);
    int_to_str(minutes % 60, mins_str);
    strcpy(uptime_str, hours_str);
    strcat(uptime_str, " hours, ");
    strcat(uptime_str, mins_str);
    strcat(uptime_str, " mins");
    
    uint8_t colors[] = {
        COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE, 
        COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE, COLOR_BRIGHT_RED,
        COLOR_BRIGHT_GREEN, COLOR_BRIGHT_BLUE, COLOR_BRIGHT_MAGENTA,
        COLOR_BRIGHT_CYAN
    };
    terminal_setcolor(COLOR_GREEN, COLOR_BLACK);
    terminal_writestring("              srunix\n");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring("              ----------- \n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring("  ___ ___     OS:");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring(" Srunix R.E.\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring(" / __| _ |    Host:");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring(" PC x86_64\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring(" |__ |   /    Kernel:");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring(" SruKernel R.E. 8.7\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring(" |___/_|_| _  License:");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring(" BSD-3-Clause license\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring(" | | | | || | Packages:");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring(" 0 (build-in)\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring(" | |_| | .` | Shell:");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring(" ush 3.2-12\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring("  |___/|_|._| Resolution: ");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring(resolution);
    terminal_writestring("\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring(" |_ _| || /   Terminal: ");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring("/dev/tty");
    char tty_num[4];
    int_to_str(current_tty + 1, tty_num);
    terminal_writestring(tty_num);
    terminal_writestring("\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring("  | | >  <    CPU: ");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring(cpu_name);
    terminal_writestring("\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring(" |___/_/|_|   GPU: ");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring("VGA (text mode)\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring("              Memory: ");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    char mem_used_str[16], mem_total_str[16];
    int_to_str(used_mem_kb, mem_used_str);
    int_to_str(total_mem_kb, mem_total_str);
    terminal_writestring(mem_used_str);
    terminal_writestring(" KB / ");
    terminal_writestring(mem_total_str);
    terminal_writestring(" KB\n");
    
    terminal_writestring("              ");
    for (int i = 0; i < 7; i++) {
        terminal_setcolor(colors[i % sizeof(colors)], colors[i % sizeof(colors)]);
        terminal_putchar('\xDB');
    }
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring("\n              ");
    for (int i = 0; i < 7; i++) {
        terminal_setcolor(colors[(i + 3) % sizeof(colors)], colors[(i + 3) % sizeof(colors)]);
        terminal_putchar('\xDB');
    }
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring("\n");
}
