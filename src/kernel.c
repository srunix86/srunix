#include "port.h"
#include "pr.h"
#include "info.h"
#include "help.h"

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
    terminal_writestring("Srunix R.E. LiveCD x86_64 v0.0.4\n");
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
    terminal_writestring("Welcome to Srunix R.E.\n");
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

void execute_poweroff() {
    terminal_writestring("");
    outw(0x604, 0x2000);
    outw(0xB004, 0x2000);
    outw(0x4004, 0x3400);
    asm volatile ("cli");
    while (1) asm volatile ("hlt");
}

void execute_reboot() {
    terminal_writestring("Reboot\n");
    uint8_t temp = inb(0x64);
    while (temp & 0x02)
        temp = inb(0x64);
    outb(0x64, 0xFE);
    asm volatile ("cli");
    asm volatile ("int $0xFF");
    while (1);
}

void execute_beep() {
    beep(1000);
    for (volatile int i = 0; i < 1000000; i++);
    no_sound();
}

void execute_sysinfo() {
    terminal_writestring("System Info:\n");
    uint32_t used_memory_kb = (next_node_addr - 0x100000) / 1024;
    uint32_t free_memory_kb = TOTAL_MEMORY_KB - used_memory_kb;
    char used_str[16], free_str[16], total_str[16];
    int_to_str(used_memory_kb, used_str);
    int_to_str(free_memory_kb, total_str);
    terminal_writestring("Memory: ");
    terminal_writestring(used_str);
    terminal_writestring(" KB used, ");
    terminal_writestring(total_str);
    terminal_writestring(" KB total\n");
    terminal_writestring("Uptime: ");
    terminal_writestring("\nDisks:\n");
    for (int i = 0; i < disk_count; i++) {
        terminal_printf("- %s: %d MB total, %d MB free\n",
                      disks[i].name,
                      (int)(disks[i].total_bytes / (1024 * 1024)),
                      (int)(disks[i].free_bytes / (1024 * 1024)));
    }
}

void get_cpu_name(char* buffer) {
    uint32_t regs[12];
    asm volatile (
        "cpuid" 
        : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3]) 
        : "a" (0x80000002)
    );
    asm volatile (
        "cpuid" 
        : "=a" (regs[4]), "=b" (regs[5]), "=c" (regs[6]), "=d" (regs[7]) 
        : "a" (0x80000003)
    );
    asm volatile (
        "cpuid" 
        : "=a" (regs[8]), "=b" (regs[9]), "=c" (regs[10]), "=d" (regs[11]) 
        : "a" (0x80000004)
    );
    memcpy(buffer, regs, 48);
    buffer[48] = '\0';
    while (*buffer == ' ') buffer++;
    char* end = buffer + strlen(buffer) - 1;
    while (end > buffer && *end == ' ') end--;
    *(end + 1) = '\0';
}

void get_memory_info(uint32_t* total_kb, uint32_t* used_kb) {
    *total_kb = TOTAL_MEMORY_KB;
    *used_kb = (next_node_addr - 0x100000) / 1024;
}

void get_resolution(char* buffer) {
    strcpy(buffer, "1920x1080");
}

void execute_fetch() {
    char cpu_name[64] = {0};
    get_cpu_name(cpu_name);
    uint32_t total_mem_kb, used_mem_kb;
    get_memory_info(&total_mem_kb, &used_mem_kb);
    char resolution[32] = {0};
    get_resolution(resolution);
    char hours_str[8], mins_str[8];
    uint8_t colors[] = {
        COLOR_RED, COLOR_GREEN, COLOR_YELLOW, COLOR_BLUE, 
        COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE, COLOR_BRIGHT_RED,
        COLOR_BRIGHT_GREEN, COLOR_BRIGHT_BLUE, COLOR_BRIGHT_MAGENTA,
        COLOR_BRIGHT_CYAN
    };
    terminal_setcolor(COLOR_BRIGHT_GREEN, COLOR_BLACK);
    terminal_writestring("              srunix@(unknown)\n");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring("              ----------- \n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring("  ___ ___     OS:");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring(" Srunix R.E. v0.0.4 amd64\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring(" / __| _ |    Host:");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring(" PC x86_64\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring(" |__ |   /    Kernel:");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring(" SrKernel R.E. 9.2\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring(" |___/_|_| _  License:");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring(" SPL v1 license\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring(" | | | | || | Packages:");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring(" 0 (build-in)\n");
    terminal_setcolor(COLOR_YELLOW, COLOR_BLACK);
    terminal_writestring(" | |_| | .` | Shell:");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_writestring(" ush 5.0-02\n");
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
}

void execute_mkfile(char* filename) {
    if (!is_valid_filename(filename)) {
        terminal_writestring("Invalid filename: contains forbidden characters\n");
        return;
    }
    if (file_exists_in_current_dir(filename)) {
        terminal_writestring("\n");
        return;
    }
    if (fs_create_file(filename, current_inode, FILE_REGULAR) == FS_SUCCESS) {
        terminal_printf("File '%s' created\n", filename);
    } else {
        terminal_writestring("Failed to create file\n");
    }
}

void execute_mkdir(char* dirname) {
    if (!is_valid_filename(dirname)) {
        terminal_writestring("Invalid directory name: contains forbidden characters\n");
        return;
    }
    if (file_exists_in_current_dir(dirname)) {
        terminal_writestring("Change name\n");
        return;
    }
    if (fs_create_file(dirname, current_inode, FILE_DIR) == FS_SUCCESS) {
        terminal_printf("'%s' created\n", dirname);
    } else {
        terminal_writestring("Failed!\n");
    }
}

void execute_rm(char* name, bool recursive) {
    for (int i = 0; i < file_count; i++) {
        if (files[i].parent_inode == current_inode && strcmp(files[i].name, name) == 0) {
            if (files[i].type == FILE_DIR && !recursive) {
                terminal_writestring("Cannot remove directory: use 'rm -rf' for directories\n");
                return;
            }
            if (files[i].type == FILE_DIR && recursive) {
                uint32_t dir_inode = files[i].inode;
                for (int j = 0; j < file_count; j++) {
                    if (files[j].parent_inode == dir_inode) {
                        execute_rm(files[j].name, true);
                        j--;
                    }
                }
            }
            if (fs_delete_file(files[i].inode) == FS_SUCCESS) {
                terminal_printf("'%s' deleted\n", name);
            } else {
                terminal_writestring("Failed to delete\n");
            }
            return;
        }
    }
    terminal_printf("File not found: %s\n", name);
}

void execute_ps() {
    terminal_writestring("PID \t PPID \t PGID \t STATE \t NAME \n");
    for (int i = 0; i < process_count; i++) {
        terminal_printf("%d\t%d\t%d\t%s\t%s\n",
                      processes[i].pid,
                      processes[i].ppid,
                      processes[i].pgid,
                      processes[i].state == 0 ? "RUN" : 
                      processes[i].state == 1 ? "STOP" : "ZOMB",
                      processes[i].name);
    }
}

void execute_jobs() {
    for (int i = 0; i < process_count; i++) {
        if (processes[i].pgid == processes[i].pid) {
            terminal_printf("[%d] %s %s\n", 
                          i, 
                          processes[i].state == 1 ? "Stopped" : "Running",
                          processes[i].name);
        }
    }
}

void execute_kill(char* pid_str, char* sig_str) {
    uint32_t pid = atoi(pid_str);
    uint32_t sig = atoi(sig_str);
    send_signal(pid, sig);
}

void klog(int level, const char* message) {
    char* levels[] = {"DEBUG", "INFO", "WARN", "ERROR"};
}

uint32_t sys_fork() {
    if (process_count >= MAX_PROCESSES) return -1;
    Process* parent = &processes[current_process];
    Process* child = &processes[process_count++];
    memcpy(child, parent, sizeof(Process));
    child->pid = process_count + 1;
    child->ppid = parent->pid;
    return child->pid;
}

void sys_exit(uint32_t status) {
    processes[current_process].state = 2;
    processes[current_process].exit_code = status;
}

void send_signal(uint32_t pid, uint32_t sig) {
    for (int i = 0; i < process_count; i++) {
        if (processes[i].pid == pid) {
            switch(sig) {
                case SIGINT:
                case SIGKILL:
                    for (int j = i; j < process_count - 1; j++) {
                        processes[j] = processes[j + 1];
                    }
                    process_count--;
                    break;
                case SIGSTOP:
                    processes[i].state = 1;
                    break;
                case SIGCONT:
                    processes[i].state = 0;
                    break;
            }
            return;
        }
    }
    terminal_printf("Process %d not found\n", pid);
}

void execute_command(char* cmd) {
    char* args[10];
    int arg_count = 0;
    char cmd_copy[MAX_CMD_LEN];
    strcpy(cmd_copy, cmd);
    for (char* p = cmd_copy; *p; p++) {
        if (*p >= 'A' && *p <= 'Z') *p += 32;
    }
    char* token = cmd_copy;
    while (*token && arg_count < 10 - 1) {
        while (*token == ' ') token++;
        if (*token == '\0') break;
        args[arg_count++] = token;
        while (*token && *token != ' ') token++;
        if (*token) *token++ = '\0';
    }
    args[arg_count] = 0;
    if (arg_count == 0) return;
    int redirect_pos = -1;
    int redirect_type = 0;
    for (int i = 1; i < arg_count; i++) {
        if (strcmp(args[i], ">") == 0) {
            redirect_pos = i;
            redirect_type = 1;
            break;
        } else if (strcmp(args[i], ">>") == 0) {
            redirect_pos = i;
            redirect_type = 2;
            break;
        }
    }
    if (redirect_pos != -1 && redirect_pos < arg_count - 1) {
        if (strcmp_case_insensitive(args[0], "echo") == 0) {
            char text[MAX_CMD_LEN] = {0};
            for (int i = 1; i < redirect_pos; i++) {
                if (i > 1) strcat(text, " ");
                strcat(text, args[i]);
            }
            if (redirect_type == 1) {
                execute_redirect_output(args[redirect_pos + 1], text);
            } else if (redirect_type == 2) {
                execute_append_output(args[redirect_pos + 1], text);
            }
            return;
        } 
        else if (strcmp_case_insensitive(args[0], "cat") == 0) {
            if (redirect_pos == 1 && arg_count == 3) {
                bool append = (redirect_type == 2);
                execute_cat_redirect(args[1], args[2], append);
            } else {
                terminal_writestring("Usage: cat <source_file> > <dest_file> or cat <source_file> >> <dest_file>\n");
            }
            return;
        }
    }
    if (strcmp_case_insensitive(args[0], "help") == 0) {
        int page = 1;
        if (arg_count > 1) {
            page = atoi(args[1]);
        }
        execute_help(page);
    } else if (strcmp_case_insensitive(args[0], "cls") == 0 || 
               strcmp_case_insensitive(args[0], "clear") == 0) {
        execute_cls();
    } else if (strcmp_case_insensitive(args[0], "ver") == 0) {
        execute_ver();
    } else if (strcmp_case_insensitive(args[0], "ls") == 0) {
        execute_ls();
    } else if (strcmp_case_insensitive(args[0], "pwd") == 0) {
        execute_pwd();
    } else if (strcmp_case_insensitive(args[0], "cd") == 0) {
        if (arg_count > 1) execute_cd(args[1]);
        else execute_cd(NULL);
    } else if (strcmp_case_insensitive(args[0], "echo") == 0) {
        char* orig_args[10];
        int orig_arg_count = 0;
        char* orig_token = cmd;
        while (*orig_token && orig_arg_count < 10 - 1) {
            while (*orig_token == ' ') orig_token++;
            if (*orig_token == '\0') break;
            orig_args[orig_arg_count++] = orig_token;
            while (*orig_token && *orig_token != ' ') orig_token++;
            if (*orig_token) *orig_token++ = '\0';
        }
        orig_args[orig_arg_count] = 0;
        execute_echo(orig_args, orig_arg_count);
    } else if (strcmp_case_insensitive(args[0], "cat") == 0) {
        if (arg_count > 1) execute_cat(args[1]);
        else terminal_writestring("Usage: cat <filename>\n");
    } else if (strcmp_case_insensitive(args[0], "date") == 0) {
        execute_date();
    } else if (strcmp_case_insensitive(args[0], "time") == 0) {
        execute_time();
    } else if (strcmp_case_insensitive(args[0], "whoami") == 0) {
        execute_whoami();
    } else if (strcmp_case_insensitive(args[0], "uptime") == 0) {
        execute_uptime();
    } else if (strcmp_case_insensitive(args[0], "mem") == 0 || 
               strcmp_case_insensitive(args[0], "memory") == 0) {
        execute_memory();
    } else if (strcmp_case_insensitive(args[0], "lsblk") == 0) {
        execute_disk();
    } else if (strcmp_case_insensitive(args[0], "pause") == 0) {
        execute_pause();
    } else if (strcmp_case_insensitive(args[0], "poweroff") == 0 || 
               strcmp_case_insensitive(args[0], "shutdown") == 0) {
        execute_poweroff();
    } else if (strcmp_case_insensitive(args[0], "kptest") == 0) {
        kernel_panic("CRITICAL: error!\nSystem is crashed\nKernel panic - not syncing: Attempted to kill init!");
    } else if (strcmp_case_insensitive(args[0], "reboot") == 0 || 
               strcmp_case_insensitive(args[0], "restart") == 0) {
        execute_reboot();
    } else if (strcmp_case_insensitive(args[0], "exit") == 0) {
        execute_exit();
    } else if (strcmp_case_insensitive(args[0], "info") == 0) {
        terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
        info_program_run();
    } else if (strcmp_case_insensitive(args[0], "touch") == 0) {
        if (arg_count > 1) execute_mkfile(args[1]);
        else terminal_writestring("Usage: touch <filename>\n");
    } else if (strcmp_case_insensitive(args[0], "fetch") == 0 ||
               strcmp_case_insensitive(args[0], "neofetch") == 0 ||
               strcmp_case_insensitive(args[0], "./fetch") == 0) {
        execute_fetch();
    } else if (strcmp_case_insensitive(args[0], "mkdir") == 0) {
        if (arg_count > 1) execute_mkdir(args[1]);
        else terminal_writestring("Usage: mkdir <dirname>\n");
    } else if (strcmp_case_insensitive(args[0], "rm") == 0) {
        bool recursive = false;
        char* filename = NULL;
        if (arg_count > 1) {
            if (strcmp(args[1], "-rf") == 0) {
                recursive = true;
                if (arg_count > 2) {
                    filename = args[2];
                }
            } else {
                filename = args[1];
            }
        }
        if (filename) {
            execute_rm(filename, recursive);
        } else {
            terminal_writestring("Usage: rm <filename> or rm -rf <dirname>\n");
        }
    } else if (strcmp_case_insensitive(args[0], "beep") == 0) {
        execute_beep();
    } else if (strcmp_case_insensitive(args[0], "ps") == 0) {
        execute_ps();
    } else if (strcmp_case_insensitive(args[0], "jobs") == 0) {
        execute_jobs();
    } else if (strcmp_case_insensitive(args[0], "kill") == 0) {
        if (arg_count >= 3) execute_kill(args[1], args[2]);
        else terminal_writestring("Usage: kill <pid> <signal>\n");
    } else if (strcmp_case_insensitive(args[0], "smouse") == 0) {
        execute_smouse();
    } else if (args[0][0] != '\0') {
        terminal_writestring("Command not found: ");
        terminal_writestring(args[0]);
        terminal_writestring("\nType 'help' for commands\n");
    }
}

void print_prompt() {
    if (smouse_mode) return;
    terminal_setcolor(COLOR_BRIGHT_GREEN, terminal_color >> 4);
    terminal_writestring("srunix");
    terminal_setcolor(COLOR_GREEN, terminal_color >> 4);
    terminal_writestring("# ");
    terminal_setcolor(COLOR_WHITE, terminal_color >> 4);
    move_cursor(terminal_column, terminal_row);
}

void login_screen() {
    terminal_setcolor(COLOR_GRAY, COLOR_BLACK);
    terminal_clear();
    terminal_writestring("\n\n");
    terminal_writestring("Srunix R.E. tty");
    char tty_num[2];
    int_to_str(current_tty + 1, tty_num);
    terminal_writestring(tty_num);
    terminal_writestring("\n");
    terminal_setcolor(COLOR_GRAY, COLOR_BLACK);
    terminal_writestring("OpenSource Operation System.\nPlease login: root, password 1\n\n");
    char username[32] = {0};
    char password[32] = {0};
    int pos = 0;
    terminal_writestring("login: ");
    while (1) {
        char c = keyboard_getchar();
        if (c == '\n') {
            terminal_putchar('\n');
            break;
        } else if (c == '\b') {
            if (pos > 0) {
                pos--;
                username[pos] = '\0';
                terminal_putchar('\b');
            }
        } else if (pos < 31) {
            username[pos++] = c;
            terminal_putchar(c);
        }
    }
    pos = 0;
    terminal_writestring("Password: ");
    while (1) {
        char c = keyboard_getchar();
        if (c == '\n') {
            terminal_putchar('\n');
            break;
        } else if (c == '\b') {
            if (pos > 0) {
                pos--;
                password[pos] = '\0';
                terminal_putchar('\b');
            }
        } else if (pos < 31) {
            password[pos++] = c;
            terminal_putchar('*');
        }
    }
    if (strcmp(username, "srunix") == 0 && strcmp(password, "1") == 0) {
        terminal_setcolor(COLOR_GREEN, COLOR_BLACK);
        terminal_writestring("\nWelcome to Srunix R.E. livecd\n");
        ttys[current_tty].logged_in = true;
        shell();
    }
    if (strcmp(username, "root") == 0 && strcmp(password, "1") == 0) {
        terminal_writestring("\nWelcome to Srunix R.E. livecd\n");
        ttys[current_tty].logged_in = true;
        shell();
    } else {
        terminal_writestring("\nlogin incorrect\n");
        for (volatile int i = 0; i < 10000000; i++);
        login_screen();
    }
}

void add_to_history(const char* cmd) {
    if (smouse_mode) return;
    if (strlen(cmd) == 0) return;
    if (history_count < HISTORY_SIZE) {
        strcpy(command_history[history_count++], cmd);
    } else {
        for (int i = 0; i < HISTORY_SIZE-1; i++) {
            strcpy(command_history[i], command_history[i+1]);
        }
        strcpy(command_history[HISTORY_SIZE-1], cmd);
    }
    history_pos = history_count;
}

void shell() {
    if (smouse_mode) return;
    char cmd[MAX_CMD_LEN];
    int pos = 0;
    terminal_setcolor(COLOR_BRIGHT_RED, COLOR_BLACK);
    terminal_writestring("\nSrunix R.E. tty");
    char tty_num[2];
    int_to_str(current_tty + 1, tty_num);
    terminal_writestring(tty_num);
    terminal_writestring("\n");
    terminal_setcolor(COLOR_GRAY, COLOR_BLACK);
    terminal_writestring("Type \"help\" to see all the commands in the OS.\n\n");
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    while (1) {
        print_prompt();
        pos = 0;
        while (pos < MAX_CMD_LEN - 1) {
            char c = keyboard_getchar();
            if (c == '\n') {
                terminal_putchar('\n');
                cmd[pos] = '\0';
                add_to_history(cmd);
                execute_command(cmd);
                break;
            } else if (c == '\b') {
                if (pos > 0) {
                    pos--;
                    terminal_putchar('\b');
                }
            } else if (c == '\x11') {
                if (history_pos > 0) {
                    history_pos--;
                    strcpy(cmd, command_history[history_pos]);
                    pos = strlen(cmd);
                    terminal_column = 0;
                    for (int i = 0; i < pos; i++) {
                        terminal_putchar(cmd[i]);
                    }
                }
            } else if (c == '\x12') {
                if (history_pos < history_count - 1) {
                    history_pos++;
                    strcpy(cmd, command_history[history_pos]);
                    pos = strlen(cmd);
                    terminal_column = 0;
                    for (int i = 0; i < pos; i++) {
                        terminal_putchar(cmd[i]);
                    }
                }
            } else if (c == '\x13' || c == '\x14') {
            } else {
                cmd[pos++] = c;
                terminal_putchar(c);
            }
        }
    }
}

void kernel_panic(const char* message) {
    terminal_setcolor(COLOR_RED, COLOR_WHITE);
    terminal_clear();
    terminal_writestring("\n\n*** KERNEL PANIC ***\n");
    terminal_writestring(message);
    terminal_writestring("\n\nStack trace:\n");
    terminal_writestring(" [<ffffffff>] ?\n");
    terminal_writestring(" [<ffffffff>] ?\n");
    terminal_writestring(" [<ffffffff>] ?\n");
    terminal_writestring("\nKernel panic - not syncing: Fatal exception\n");
}

void kernel_main() {
    terminal_initialize();
    memset(inodes, 0, sizeof(inodes));
    memset(block_used, 0, sizeof(block_used));
    free_blocks = MAX_BLOCKS;
    free_inodes = MAX_INODES;
    fs_create_file("root", 1, FILE_DIR);
    current_inode = 1;
    fs_create_file("bin", 1, FILE_DIR);
    fs_create_file("etc", 1, FILE_DIR);
    fs_create_file("home", 1, FILE_DIR);
    fs_create_file("dev", 1, FILE_DIR);
    fs_create_file("tmp", 1, FILE_DIR);
    fs_create_file("sys", 1, FILE_DIR);
    fs_create_file("fetch", 1, FILE_REGULAR);
    uint32_t dev_inode = 0;
    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].name, "dev") == 0) {
            dev_inode = files[i].inode;
            break;
        }
    }
    if (dev_inode != 0) {
        fs_create_file("sda", dev_inode, FILE_REGULAR);
        fs_create_file("sda1", dev_inode, FILE_REGULAR);
        fs_create_file("sdb1", dev_inode, FILE_REGULAR);
        fs_create_file("sdb", dev_inode, FILE_REGULAR);
        fs_create_file("vga", dev_inode, FILE_REGULAR);
        fs_create_file("vga1", dev_inode, FILE_REGULAR);
        fs_create_file("vga2", dev_inode, FILE_REGULAR);
        fs_create_file("null", dev_inode, FILE_REGULAR);
        fs_create_file("random", dev_inode, FILE_REGULAR);
    }
    uint32_t bin_inode = 0;
    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].name, "bin") == 0) {
            bin_inode = files[i].inode;
            break;
        }
    }
    if (bin_inode != 0) {
        fs_create_file("ush", bin_inode, FILE_REGULAR);
        fs_create_file("ls", bin_inode, FILE_REGULAR);
        fs_create_file("cat", bin_inode, FILE_REGULAR);
        fs_create_file("echo", bin_inode, FILE_REGULAR);
        fs_create_file("fetch", bin_inode, FILE_REGULAR);
        const char* fetch_content = "priveeet\n";
        for (int i = 0; i < file_count; i++) {
            if (files[i].parent_inode == bin_inode && strcmp(files[i].name, "fetch") == 0) {
                fs_write_file(files[i].inode, fetch_content, strlen(fetch_content));
                break;
            }
        }
    }
    init_timer();
    login_screen();
    while (1) asm volatile ("hlt");
}
