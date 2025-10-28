#include "../lib/pring.h"
#include <stddef.h>
void execute_cd(char* dirname) {
    if (dirname == NULL || strlen(dirname) == 0) {
        current_inode = 1;
        return;
    }
    if (strcmp(dirname, "..") == 0) {
        for (int i = 0; i < file_count; i++) {
            if (files[i].inode == current_inode) {
                current_inode = files[i].parent_inode;
                return;
            }
        }
        terminal_writestring("Already at root directory\n");
        return;
    }
    for (int i = 0; i < file_count; i++) {
        if (files[i].parent_inode == current_inode &&
            strcmp(files[i].name, dirname) == 0 &&
            files[i].type == FILE_DIR) {
            current_inode = files[i].inode;
            return;
        }
    }
    terminal_writestring("Directory not found: ");
    terminal_writestring(dirname);
    terminal_writestring("\n");
}
