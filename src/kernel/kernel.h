#define MAX_FILE_SIZE 4096


void execute_redirect_output(char* filename, char* text);
void execute_append_output(char* filename, char* text);
void execute_cat_redirect(char* source_file, char* dest_file, bool append);

void execute_redirect_output(char* filename, char* text) {
    if (filename == NULL || text == NULL) {
        terminal_writestring("Usage: echo text > filename\n");
        return;
    }
    for (int i = 0; i < file_count; i++) {
        if (files[i].parent_inode == current_inode &&
            strcmp(files[i].name, filename) == 0 &&
            files[i].type == FILE_REGULAR) {
            if (fs_write_file(files[i].inode, text, strlen(text)) == FS_SUCCESS) {
            } else {
                terminal_writestring("Failed to write to file\n");
            }
            return;
        }
    }
    if (fs_create_file(filename, current_inode, FILE_REGULAR) == FS_SUCCESS) {
        if (fs_write_file(files[file_count-1].inode, text, strlen(text)) == FS_SUCCESS) {
            terminal_writestring("File created and text written\n");
        } else {
            terminal_writestring("File created but failed to write text\n");
        }
    } else {
        terminal_writestring("Failed to create file\n");
    }
}

static uint8_t old_vga_font[VGA_FONT_SIZE];
