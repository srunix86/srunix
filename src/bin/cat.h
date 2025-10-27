#include "../lib/pring.h"
#include <stddef.h>

void execute_cat(char* filename) {
    if (filename == NULL) {
        terminal_writestring("Usage: cat <filename>\n");
        return;
    }
    for (int i = 0; i < file_count; i++) {
        if (files[i].parent_inode == current_inode &&
            strcmp(files[i].name, filename) == 0 &&
            files[i].type == FILE_REGULAR) {
            char buffer[BLOCK_SIZE];
            int bytes_read = fs_read_file(files[i].inode, buffer, sizeof(buffer));
            if (bytes_read > 0) {
                terminal_write(buffer, bytes_read);
            }
            terminal_writestring("\n");
            return;
        }
    }
    terminal_printf("File not found: %s\n", filename);
}
