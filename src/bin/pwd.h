#include "../lib/pring.h"
#include <stddef.h>
void execute_pwd() {
    char path[MAX_PATH_LEN] = "/";
    uint32_t inode = current_inode;
    while (inode != 1) {
        for (int i = 0; i < file_count; i++) {
            if (files[i].inode == inode) {
                char temp[MAX_PATH_LEN];
                strcpy(temp, "/");
                strcat(temp, files[i].name);
                strcat(temp, path);
                strcpy(path, temp);
                inode = files[i].parent_inode;
                break;
            }
        }
    }
    terminal_writestring("");
    terminal_writestring(path);
    terminal_writestring("\n");
}
