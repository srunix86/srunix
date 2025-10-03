#include "../lib/pring.h"
#include <stddef.h>
void execute_echo(char* args[], int arg_count) {
    for (int i = 1; i < arg_count; i++) {
        terminal_writestring(args[i]);
        terminal_writestring("");
    }
    terminal_writestring("\n");
}

