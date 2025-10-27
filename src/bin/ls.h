#include <stddef.h>
#include "../lib/pring.h"
void execute_ls() {
    terminal_writestring("");
    for (int i = 0; i < file_count; i++) {
        if (files[i].parent_inode == current_inode) {
            terminal_writestring(files[i].name);
            terminal_writestring(files[i].type == FILE_DIR ? "/ " : "  ");
        }
    }
    terminal_writestring("\n");
}

