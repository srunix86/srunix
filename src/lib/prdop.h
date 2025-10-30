void vga_set_font(const uint8_t* new_font) {
    vga_save_current_font(old_vga_font);

    outb(0x3C4, 0x04); 
    outb(0x3C4, 0x02); 
    outb(0x3CE, 0x04); 
    outb(0x3CE, 0x02); 
    outb(0x3CE, 0x05); 
    outb(0x3CE, 0x00); 
    outb(0x3CE, 0x06); 
    outb(0x3CE, 0x00); 

    volatile uint8_t* font_mem = (volatile uint8_t*)0xA0000;
    for (int i = 0; i < VGA_FONT_SIZE; i++) {
        font_mem[i] = new_font[i];
    }

    outb(0x3C4, 0x03); 
    outb(0x3C4, 0x00);
    outb(0x3CE, 0x04); 
    outb(0x3CE, 0x00);
    outb(0x3CE, 0x05);
    outb(0x3CE, 0x10);
    outb(0x3CE, 0x06);
    outb(0x3CE, 0x0E);

    outb(0x3D4, 0x09); 
    outb(0x3D5, 0x00); 
}

void execute_append_output(char* filename, char* text) {
    if (filename == NULL || text == NULL) {
        terminal_writestring("Usage: echo text >> filename\n");
        return;
    }
    for (int i = 0; i < file_count; i++) {
        if (files[i].parent_inode == current_inode && 
            strcmp(files[i].name, filename) == 0 && 
            files[i].type == FILE_REGULAR) {
            char current_content[MAX_FILE_SIZE];
            uint32_t bytes_read = fs_read_file(files[i].inode, current_content, MAX_FILE_SIZE - 1);
            current_content[bytes_read] = '\0';
            char new_content[MAX_FILE_SIZE];
            strcpy(new_content, current_content);
            if (bytes_read > 0 && current_content[bytes_read - 1] != '\n') {
                strcat(new_content, "\n");
            }
            strcat(new_content, text);
            if (fs_write_file(files[i].inode, new_content, strlen(new_content)) == FS_SUCCESS) {
            } else {
                terminal_writestring("Failed to append to file\n");
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

void execute_cat_redirect(char* source_file, char* dest_file, bool append) {
    if (source_file == NULL || dest_file == NULL) {
        terminal_writestring("Usage: cat <source_file> > <dest_file> or cat <source_file> >> <dest_file>\n");
        return;
    }
    char source_content[MAX_FILE_SIZE];
    uint32_t bytes_read = 0;
    bool source_found = false;
    for (int i = 0; i < file_count; i++) {
        if (files[i].parent_inode == current_inode && 
            strcmp(files[i].name, source_file) == 0 && 
            files[i].type == FILE_REGULAR) {
            bytes_read = fs_read_file(files[i].inode, source_content, MAX_FILE_SIZE - 1);
            source_found = true;
            break;
        }
    }
    if (!source_found || bytes_read == 0) {
        terminal_printf("Source file not found or empty: %s\n", source_file);
        return;
    }
    source_content[bytes_read] = '\0';
    bool dest_found = false;
    uint32_t dest_inode = 0;
    for (int i = 0; i < file_count; i++) {
        if (files[i].parent_inode == current_inode && 
            strcmp(files[i].name, dest_file) == 0 && 
            files[i].type == FILE_REGULAR) {
            dest_found = true;
            dest_inode = files[i].inode;
            break;
        }
    }
    if (append) {
        if (dest_found) {
            char dest_content[MAX_FILE_SIZE];
            uint32_t dest_bytes = fs_read_file(dest_inode, dest_content, MAX_FILE_SIZE - 1);
            dest_content[dest_bytes] = '\0';
            char new_content[MAX_FILE_SIZE];
            strcpy(new_content, dest_content);
            if (dest_bytes > 0 && dest_content[dest_bytes - 1] != '\n') {
                strcat(new_content, "\n");
            }
            strcat(new_content, source_content);
            if (fs_write_file(dest_inode, new_content, strlen(new_content)) == FS_SUCCESS) {
                terminal_printf("Content from %s appended to %s\n", source_file, dest_file);
            } else {
                terminal_writestring("Failed to append to file\n");
            }
        } else {
            if (fs_create_file(dest_file, current_inode, FILE_REGULAR) == FS_SUCCESS) {
                for (int i = 0; i < file_count; i++) {
                    if (files[i].parent_inode == current_inode && 
                        strcmp(files[i].name, dest_file) == 0 && 
                        files[i].type == FILE_REGULAR) {
                        if (fs_write_file(files[i].inode, source_content, strlen(source_content)) == FS_SUCCESS) {
                            terminal_printf("File %s created with content from %s\n", dest_file, source_file);
                        } else {
                            terminal_writestring("File created but failed to write content\n");
                        }
                        break;
                    }
                }
            } else {
                terminal_writestring("Failed to create file\n");
            }
        }
    } else {
        if (dest_found) {
            if (fs_write_file(dest_inode, source_content, strlen(source_content)) == FS_SUCCESS) {
                terminal_printf("Content from %s written to %s\n", source_file, dest_file);
            } else {
                terminal_writestring("Failed to write to file\n");
            }
        } else {
            if (fs_create_file(dest_file, current_inode, FILE_REGULAR) == FS_SUCCESS) {
                for (int i = 0; i < file_count; i++) {
                    if (files[i].parent_inode == current_inode && 
                        strcmp(files[i].name, dest_file) == 0 && 
                        files[i].type == FILE_REGULAR) {
                        if (fs_write_file(files[i].inode, source_content, strlen(source_content)) == FS_SUCCESS) {
                            terminal_printf("File %s created with content from %s\n", dest_file, source_file);
                        } else {
                            terminal_writestring("File created but failed to write content\n");
                        }
                        break;
                    }
                }
            } else {
                terminal_writestring("Failed to create file\n");
            }
        }
    }
}

void execute_date() {
    uint8_t second = cmos_read(0x00);
    uint8_t minute = cmos_read(0x02);
    uint8_t hour = cmos_read(0x04);
    uint8_t day = cmos_read(0x07);
    uint8_t month = cmos_read(0x08);
    uint8_t year = cmos_read(0x09);
    second = bcd_to_bin(second);
    minute = bcd_to_bin(minute);
    hour = bcd_to_bin(hour);
    day = bcd_to_bin(day);
    month = bcd_to_bin(month);
    year = bcd_to_bin(year);
    char hour_str[3], min_str[3], sec_str[3];
    int_to_str(hour, hour_str);
    int_to_str(minute, min_str);
    int_to_str(second, sec_str);
    char day_str[3], month_str[3], year_str[5];
    int_to_str(day, day_str);
    int_to_str(month, month_str);
    int_to_str(year + 2000, year_str);
    terminal_writestring("");
    terminal_writestring(day_str);
    terminal_writestring("/");
    terminal_writestring(month_str);
    terminal_writestring("/");
    terminal_writestring(year_str);
    terminal_writestring(" ");
    terminal_writestring(hour_str);
    terminal_writestring(":");
    terminal_writestring(min_str);
    terminal_writestring(":");
    terminal_writestring(sec_str);
    terminal_writestring("\n");
}

void execute_time() {
    uint8_t hour = cmos_read(0x04);
    uint8_t minute = cmos_read(0x02);
    uint8_t second = cmos_read(0x00);
    hour = bcd_to_bin(hour);
    minute = bcd_to_bin(minute);
    second = bcd_to_bin(second);
    char hour_str[3], min_str[3], sec_str[3];
    int_to_str(hour, hour_str);
    int_to_str(minute, min_str);
    int_to_str(second, sec_str);
    terminal_writestring("");
    terminal_writestring(hour_str);
    terminal_writestring(":");
    terminal_writestring(min_str);
    terminal_writestring(":");
    terminal_writestring(sec_str);
    terminal_writestring("\n");
}

void execute_whoami() {
    terminal_writestring("root\n");
}

void execute_uptime() {
    uint32_t seconds = timer_ticks / TIMER_HZ;
    char sec_str[16];
    terminal_writestring("");
    terminal_writestring(sec_str);
    terminal_writestring(" seconds\n");
    terminal_printf("Uptime: seconds\n");
}

void execute_ver() {
    terminal_setcolor(COLOR_GREEN, COLOR_BLACK);
    terminal_writestring("Srunix86 LiveCD x86_64 v0.0.5-1\n");
}

void execute_pause() {
    terminal_writestring("System is paused\n");
    keyboard_getchar();
    terminal_writestring("\n");
}

void execute_cls() {
    terminal_clear();
}

void execute_exit() {
    terminal_writestring("Welcome to Srunix86\n\n");
    ttys[current_tty].logged_in = false;
    login_screen();
}

void execute_memory() {
    uint32_t used_memory_kb = (next_node_addr - 0x100000) / 1024;
    uint32_t free_memory_kb = TOTAL_MEMORY_KB - used_memory_kb;
    char used_str[16], free_str[16], total_str[16];
    int_to_str(used_memory_kb, used_str);
    int_to_str(free_memory_kb, free_str);
    int_to_str(TOTAL_MEMORY_KB, total_str);
    terminal_writestring("");
    terminal_writestring("total: ");
    terminal_writestring(total_str);
    terminal_writestring(" KB\n");
    terminal_writestring("used:  ");
    terminal_writestring(used_str);
    terminal_writestring(" KB\n");
    terminal_writestring("free:  ");
    terminal_writestring(free_str);
    terminal_writestring(" KB\n");
}

void execute_disk() {
    terminal_writestring("");
    terminal_writestring(" \n");
    for (int i = 0; i < disk_count; i++) {
        char total_str[16], used_str[16], free_str[16];
        int_to_str(disks[i].total_bytes / (1024 * 1024), total_str);
        int_to_str((disks[i].total_bytes - disks[i].free_bytes) / (1024 * 1024), used_str);
        int_to_str(disks[i].free_bytes / (1024 * 1024), free_str);
        terminal_writestring(disks[i].name);
        terminal_writestring(":");
        terminal_writestring("  Total: ");
        terminal_writestring(total_str);
        terminal_writestring(" MB\n");
        terminal_writestring("  Used:  ");
        terminal_writestring(used_str);
        terminal_writestring(" MB\n");
        terminal_writestring("  Free:  ");
        terminal_writestring(free_str);
        terminal_writestring(" MB\n");
        if (i < disk_count - 1) {
            terminal_writestring("\n");
        }
    }
}
