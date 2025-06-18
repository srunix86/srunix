#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include "code_ed.h"

#define COM1 0x3F8
#define VIDEO_MEMORY ((volatile uint16_t*)0xB8000)
#define COLOR_BLACK   0x0
#define COLOR_BLUE    0x1
#define COLOR_GREEN   0x2
#define COLOR_CYAN    0x3
#define COLOR_RED     0x4
#define COLOR_MAGENTA 0x5
#define COLOR_BROWN   0x6
#define COLOR_GRAY    0x7
#define COLOR_DARK_GRAY 0x8
#define COLOR_BRIGHT_BLUE 0x9
#define COLOR_BRIGHT_GREEN 0xA
#define COLOR_BRIGHT_CYAN 0xB
#define COLOR_BRIGHT_RED 0xC
#define COLOR_BRIGHT_MAGENTA 0xD
#define COLOR_YELLOW 0xE
#define COLOR_WHITE   0xF
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define MAX_CMD_LEN 256
#define PIT_FREQUENCY 1193182
#define PIT_CHANNEL0 0x40
#define PIT_COMMAND 0x43
#define CMOS_ADDRESS 0x70
#define CMOS_DATA 0x71
#define MAX_FILES 128
#define MAX_PROCESSES 16
#define SECTOR_SIZE 512
#define MAX_PATH_LEN 256
#define TOTAL_MEMORY_KB 640
#define MAX_BLOCKS 1024
#define MAX_INODES 128
#define BLOCK_SIZE 1024
#define INODE_DIRECT_BLOCKS 12
#define MAX_TTYS 9

#define KEY_ENTER     0x1C
#define KEY_BACKSPACE 0x0E
#define KEY_ESC       0x01
#define KEY_UP        0x48
#define KEY_DOWN      0x50
#define KEY_LEFT      0x4B
#define KEY_RIGHT     0x4D
#define KEY_LSHIFT    0x2A
#define KEY_RSHIFT    0x36
#define KEY_CTRL      0x1D
#define KEY_ALT       0x38
#define KEY_CAPSLOCK  0x3A
#define KEY_SPACE     0x39
#define KEY_F1        0x3B
#define KEY_F2        0x3C
#define KEY_F3        0x3D
#define KEY_F4        0x3E
#define KEY_F5        0x3F
#define KEY_F6        0x40
#define KEY_F7        0x41
#define KEY_F8        0x42
#define KEY_F9        0x43
#define KEY_F10       0x44

#define FILE_REGULAR 0
#define FILE_DIR     1
#define FILE_SYMLINK 2

#define FS_SUCCESS 0
#define FS_ERROR   1

volatile uint16_t* terminal_buffer = VIDEO_MEMORY;
size_t terminal_row = 0;
size_t terminal_column = 0;
uint8_t terminal_color = (COLOR_BLACK << 4) | COLOR_WHITE;

size_t strlen(const char* s);
void* memcpy(void* dest, const void* src, size_t n);
void* memset(void* s, int c, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);
char* strchr(const char* s, int c);
char* strstr(const char* haystack, const char* needle);
int strcmp(const char* s1, const char* s2);
char* strcpy(char* dest, const char* src);
char* strcat(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t n);
int strncmp(const char* s1, const char* s2, size_t n);

void* malloc(size_t size);
void free(void* ptr);
void* calloc(size_t nmemb, size_t size);
void* realloc(void* ptr, size_t size);

void* memcpy(void* dest, const void* src, size_t n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

void* memset(void* s, int c, size_t n) {
    unsigned char* p = s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

int memcmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* p1 = s1, *p2 = s2;
    while (n--) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}

char* strchr(const char* s, int c) {
    while (*s != '\0') {
        if (*s == c) {
            return (char*)s;
        }
        s++;
    }
    return NULL;
}

char* strstr(const char* haystack, const char* needle) {
    size_t needle_len = strlen(needle);
    if (needle_len == 0) {
        return (char*)haystack;
    }
    
    for (; *haystack; haystack++) {
        if (*haystack == *needle && !memcmp(haystack, needle, needle_len)) {
            return (char*)haystack;
        }
    }
    return NULL;
}

int atoi(const char* str) {
    int res = 0;
    while (*str >= '0' && *str <= '9') {
        res = res * 10 + (*str - '0');
        str++;
    }
    return res;
}

static inline int abs(int n) {
    return (n < 0) ? -n : n;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len]) len++;
    return len;
}

char* strcpy(char* dest, const char* src) {
    char* ret = dest;
    while ((*dest++ = *src++));
    return ret;
}

char* strcat(char* dest, const char* src) {
    char* ret = dest;
    while (*dest) dest++;
    while ((*dest++ = *src++));
    return ret;
}

char* strncpy(char* dest, const char* src, size_t n) {
    char* ret = dest;
    while (n-- && (*dest++ = *src++));
    while (n-- > 0) *dest++ = '\0';
    return ret;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) {
        return 0;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char* strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* new = malloc(len);
    if (new == NULL) return NULL;
    memcpy(new, s, len);
    return new;
}

typedef struct {
    char name[32];
    uint64_t total_bytes;
    uint64_t free_bytes;
} Disk;

typedef struct {
    uint32_t mode;       
    uint32_t uid;       
    uint32_t gid;       
    uint32_t size;      
    uint32_t atime;     
    uint32_t ctime;     
    uint32_t mtime;     
    uint32_t dtime;     
    uint32_t blocks;     
    uint32_t block[INODE_DIRECT_BLOCKS]; 
} Inode;

typedef struct {
    char name[32];
    uint32_t inode;
    uint32_t parent_inode;
} FileEntry;

typedef struct {
    uint32_t pid;
    uint32_t ppid;
    char name[32];
    uint32_t priority;
    uint32_t state;
    uintptr_t stack_ptr;
    uintptr_t entry_point;
} Process;

typedef struct {
    uint32_t magic;
    uint32_t block_size;
    uint32_t inodes_count;
    uint32_t free_inodes;
    uint32_t blocks_count;
    uint32_t free_blocks;
    uint32_t first_data_block;
    uint32_t inodes_per_group;
    uint32_t mtime;
    uint32_t wtime;
    uint16_t mounts_count;
    uint16_t max_mounts;
    uint16_t magic_signature;
    uint16_t state;
    uint16_t errors;
    uint16_t minor_rev_level;
    uint32_t lastcheck;
    uint32_t checkinterval;
    uint32_t creator_os;
    uint32_t rev_level;
    uint16_t def_resuid;
    uint16_t def_resgid;
} Superblock;

typedef struct {
    uint32_t inode;
    char name[32];
    uint32_t parent_inode;
    uint8_t type;
    uint32_t size;
} File;

typedef struct {
    size_t row;
    size_t column;
    uint8_t color;
    uint16_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH];
    char input_buffer[MAX_CMD_LEN];
    int input_pos;
    uint32_t current_inode;
    bool logged_in;
} TTY;

volatile uint32_t timer_ticks = 0;
#define TIMER_HZ 18.2
uint32_t boot_time = 0;

static uintptr_t next_node_addr = 0x100000;
static uintptr_t heap_start = 0x200000;
static uintptr_t heap_end = 0x400000;

Disk disks[] = {
    {"sda", 500ULL * 1024 * 1024, 250ULL * 1024 * 1024},
    {"sdb", 1024ULL * 1024 * 1024, 768ULL * 1024 * 1024}
};
int disk_count = 2;

File files[MAX_FILES];
int file_count = 5;
uint32_t current_inode = 1;

Inode inodes[MAX_INODES];
uint8_t blocks[MAX_BLOCKS][BLOCK_SIZE];
bool block_used[MAX_BLOCKS];
uint32_t free_blocks = MAX_BLOCKS;
uint32_t free_inodes = MAX_INODES;

TTY ttys[MAX_TTYS];
int current_tty = 0;

bool shift_pressed = false;
bool ctrl_pressed = false;
bool alt_pressed = false;
bool caps_lock = false;

Process processes[MAX_PROCESSES];
int process_count = 0;

void kernel_main();
void kernel_panic(const char* message);
void terminal_initialize();
void move_cursor(size_t x, size_t y);
void terminal_setcolor(uint8_t fg, uint8_t bg);
void terminal_clear();
void terminal_scroll();
void terminal_putchar(char c);
void terminal_write(const char* data, size_t size);
void terminal_writestring(const char* data);
void terminal_printf(const char* format, ...);
void shell();
void login_screen();
void switch_tty(int tty_num);
void save_tty_state();
void restore_tty_state();
void initialize_ttys();

static int strcmp_case_insensitive(const char* s1, const char* s2) {
    while (*s1 && *s2) {
        char c1 = *s1;
        char c2 = *s2;
        
        if (c1 >= 'A' && c1 <= 'Z') c1 += 32;
        if (c2 >= 'A' && c2 <= 'Z') c2 += 32;
        
        if (c1 != c2) return c1 - c2;
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

static void int_to_str(int num, char* str) {
    int i = 0;
    int is_negative = 0;

    if (num < 0) {
        is_negative = 1;
        num = -num;
    }
    else if (num == 0) {
        str[i++] = '0';
    }

    while (num != 0) {
        str[i++] = (num % 10) + '0';
        num /= 10;
    }

    if (is_negative) {
        str[i++] = '-';
    }

    str[i] = '\0';

    // Разворачиваем строку
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char tmp = str[start];
        str[start] = str[end];
        str[end] = tmp;
        start++;
        end--;
    }
}

static inline void outb(uint16_t port, uint8_t val) {
    asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline void outw(uint16_t port, uint16_t val) {
    asm volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline void outl(uint16_t port, uint32_t val) {
    asm volatile ("outl %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    asm volatile ("inw %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline uint32_t inl(uint16_t port) {
    uint32_t ret;
    asm volatile ("inl %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void io_wait() {
    outb(0x80, 0);
}

void initialize_ttys() {
    for (int i = 0; i < MAX_TTYS; i++) {
        ttys[i].row = 0;
        ttys[i].column = 0;
        ttys[i].color = (COLOR_BLACK << 4) | COLOR_WHITE;
        ttys[i].input_pos = 0;
        ttys[i].current_inode = 1;
        ttys[i].logged_in = false;
        memset(ttys[i].buffer, ' ', sizeof(ttys[i].buffer));
    }
    current_tty = 0;
}

void save_tty_state() {
    TTY* tty = &ttys[current_tty];
    tty->row = terminal_row;
    tty->column = terminal_column;
    tty->color = terminal_color;
    tty->current_inode = current_inode;
    
    for (size_t y = 0; y < SCREEN_HEIGHT; y++) {
        for (size_t x = 0; x < SCREEN_WIDTH; x++) {
            tty->buffer[y][x] = terminal_buffer[y * SCREEN_WIDTH + x];
        }
    }
}

void restore_tty_state() {
    TTY* tty = &ttys[current_tty];
    terminal_row = tty->row;
    terminal_column = tty->column;
    terminal_color = tty->color;
    current_inode = tty->current_inode;
    
    for (size_t y = 0; y < SCREEN_HEIGHT; y++) {
        for (size_t x = 0; x < SCREEN_WIDTH; x++) {
            terminal_buffer[y * SCREEN_WIDTH + x] = tty->buffer[y][x];
        }
    }
    
    move_cursor(terminal_column, terminal_row);
}

void switch_tty(int tty_num) {
    if (tty_num < 0 || tty_num >= MAX_TTYS) return;
    
    save_tty_state();
    current_tty = tty_num;
    restore_tty_state();
    
    if (!ttys[current_tty].logged_in) {
        login_screen();
    }
}

void terminal_initialize() {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = (COLOR_BLACK << 4) | COLOR_WHITE;
    terminal_buffer = VIDEO_MEMORY;
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_clear();
    initialize_ttys();
}

void move_cursor(size_t x, size_t y) {
    uint16_t pos = y * SCREEN_WIDTH + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void terminal_setcolor(uint8_t fg, uint8_t bg) {
    terminal_color = (bg << 4) | (fg & 0x0F);
    ttys[current_tty].color = terminal_color;
}

void terminal_clear() {
    for (size_t y = 0; y < SCREEN_HEIGHT; y++) {
        for (size_t x = 0; x < SCREEN_WIDTH; x++) {
            terminal_buffer[y * SCREEN_WIDTH + x] = ' ' | (terminal_color << 8);
        }
    }
    terminal_row = 0;
    terminal_column = 0;
    move_cursor(0, 0);
}

void terminal_scroll() {
    for (size_t y = 1; y < SCREEN_HEIGHT; y++) {
        for (size_t x = 0; x < SCREEN_WIDTH; x++) {
            terminal_buffer[(y-1)*SCREEN_WIDTH + x] = terminal_buffer[y*SCREEN_WIDTH + x];
        }
    }
    
    for (size_t x = 0; x < SCREEN_WIDTH; x++) {
        terminal_buffer[(SCREEN_HEIGHT-1)*SCREEN_WIDTH + x] = ' ' | (terminal_color << 8);
    }
    
    terminal_row = SCREEN_HEIGHT - 1;
    terminal_column = 0;
}

void terminal_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == SCREEN_HEIGHT) {
            terminal_scroll();
        }
    } else if (c == '\b') {
        if (terminal_column > 0) {
            terminal_column--;
            terminal_buffer[terminal_row * SCREEN_WIDTH + terminal_column] = ' ' | (terminal_color << 8);
        } else if (terminal_row > 0) {
            terminal_row--;
            terminal_column = SCREEN_WIDTH - 1;
            terminal_buffer[terminal_row * SCREEN_WIDTH + terminal_column] = ' ' | (terminal_color << 8);
        }
    } else {
        terminal_buffer[terminal_row * SCREEN_WIDTH + terminal_column] = c | (terminal_color << 8);
        if (++terminal_column == SCREEN_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == SCREEN_HEIGHT) {
                terminal_scroll();
            }
        }
    }
    move_cursor(terminal_column, terminal_row);
}

void terminal_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        terminal_putchar(data[i]);
    }
}

void terminal_writestring(const char* data) {
    while (*data) {
        terminal_putchar(*data++);
    }
}

void terminal_printf(const char* format, ...) {
    va_list args;
    va_start(args, format);
    
    char buffer[256];
    char *ptr = buffer;
    
    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 'd': {
                    int num = va_arg(args, int);
                    char num_str[16];
                    int_to_str(num, num_str);
                    char *tmp = num_str;
                    while (*tmp) {
                        *ptr++ = *tmp++;
                    }
                    break;
                }
                case 's': {
                    char *str = va_arg(args, char*);
                    while (*str) {
                        *ptr++ = *str++;
                    }
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    *ptr++ = c;
                    break;
                }
                case 'x': {
                    uint32_t num = va_arg(args, uint32_t);
                    const char* hex = "0123456789ABCDEF";
                    for (int i = 7; i >= 0; i--) {
                        *ptr++ = hex[(num >> (i * 4)) & 0xF];
                    }
                    break;
                }
                case '%': {
                    *ptr++ = '%';
                    break;
                }
            }
        } else {
            *ptr++ = *format;
        }
        format++;
    }
    *ptr = '\0';
    
    terminal_writestring(buffer);
    va_end(args);
}

uint8_t cmos_read(uint8_t reg) {
    outb(CMOS_ADDRESS, reg);
    return inb(CMOS_DATA);
}

int bcd_to_bin(int bcd) {
    return (bcd & 0x0F) + ((bcd >> 4) * 10);
}

char keyboard_getchar() {
    while (1) {
        if (inb(0x64) & 0x01) {
            uint8_t scancode = inb(0x60);
            
            if (scancode & 0x80) {
                uint8_t released_key = scancode & 0x7F;
                if (released_key == KEY_LSHIFT || released_key == KEY_RSHIFT) {
                    shift_pressed = false;
                } else if (released_key == KEY_CTRL) {
                    ctrl_pressed = false;
                } else if (released_key == KEY_ALT) {
                    alt_pressed = false;
                }
                continue;
            }
            
            if (scancode == KEY_LSHIFT || scancode == KEY_RSHIFT) {
                shift_pressed = true;
                continue;
            } else if (scancode == KEY_CTRL) {
                ctrl_pressed = true;
                continue;
            } else if (scancode == KEY_ALT) {
                alt_pressed = true;
                continue;
            } else if (scancode == KEY_CAPSLOCK) {
                caps_lock = !caps_lock;
                continue;
            }
            
            if (scancode == 0xE0) {
                scancode = inb(0x60);
                switch(scancode) {
                    case KEY_UP:    return '\x11';
                    case KEY_DOWN:  return '\x12';
                    case KEY_LEFT:  return '\x13';
                    case KEY_RIGHT: return '\x14';
                }
            }
            
            if (scancode == KEY_ENTER) return '\n';
            if (scancode == KEY_BACKSPACE) return '\b';
            if (scancode == KEY_ESC) return '\x1B';
            if (scancode == KEY_SPACE) return ' ';
            if (scancode == KEY_F1) { switch_tty(0); continue; }
            if (scancode == KEY_F2) { switch_tty(1); continue; }
            if (scancode == KEY_F3) { switch_tty(2); continue; }
            if (scancode == KEY_F4) { switch_tty(3); continue; }
            if (scancode == KEY_F5) { switch_tty(4); continue; }
            if (scancode == KEY_F6) { switch_tty(5); continue; }
            if (scancode == KEY_F7) { switch_tty(6); continue; }
            if (scancode == KEY_F8) { switch_tty(7); continue; }
            if (scancode == KEY_F9) { switch_tty(8); continue; }
            if (scancode == KEY_F10) return '\xFA';
            
            if (scancode < 0x80) {
                const char* keyboard_map_lower = "\x00\x1B" "1234567890-=" "\x08"
                    "\x00" "qwertyuiop[]" "\x0D" "\x00" "asdfghjkl;'`" "\x00" 
                    "\\zxcvbnm,./" "\x00\x00\x00" " ";
                
                const char* keyboard_map_upper = "\x00\x1B" "!@#$%^&*()_+" "\x08"
                    "\x00" "QWERTYUIOP{}" "\x0D" "\x00" "ASDFGHJKL:\"~" "\x00" 
                    "|ZXCVBNM<>?" "\x00\x00\x00" " ";
                
                bool uppercase = (shift_pressed != caps_lock);
                
                if (ctrl_pressed) {
                    if (scancode >= 0x10 && scancode <= 0x1C) {
                        return scancode - 0x10 + 1;
                    }
                    continue;
                }
                
                if (alt_pressed) {
                    continue;
                }
                
                if (uppercase && keyboard_map_upper[scancode]) {
                    return keyboard_map_upper[scancode];
                } else if (!uppercase && keyboard_map_lower[scancode]) {
                    return keyboard_map_lower[scancode];
                }
            }
        }
    }
}

void init_timer() {
    uint32_t divisor = PIT_FREQUENCY / TIMER_HZ;
    outb(PIT_COMMAND, 0x36);
    outb(PIT_CHANNEL0, divisor & 0xFF);
    outb(PIT_CHANNEL0, (divisor >> 8) & 0xFF);
    boot_time = timer_ticks;
}

void* malloc(size_t size) {
    if (heap_start + size > heap_end) {
        return NULL;
    }
    void* ptr = (void*)heap_start;
    heap_start += size;
    return ptr;
}

void free(void* ptr) {
}

void* calloc(size_t num, size_t size) {
    void* ptr = malloc(num * size);
    if (ptr) {
        memset(ptr, 0, num * size);
    }
    return ptr;
}

void* realloc(void* ptr, size_t new_size) {
    if (new_size == 0) {
        free(ptr);
        return NULL;
    }
    
    void* new_ptr = malloc(new_size);
    if (new_ptr && ptr) {
        memcpy(new_ptr, ptr, new_size);
        free(ptr);
    }
    return new_ptr;
}

void beep(uint32_t frequency) {
    uint32_t divisor = 1193180 / frequency;
    outb(0x43, 0xB6);
    outb(0x42, (uint8_t)(divisor & 0xFF));
    outb(0x42, (uint8_t)((divisor >> 8) & 0xFF));
    
    uint8_t tmp = inb(0x61);
    if (tmp != (tmp | 3)) {
        outb(0x61, tmp | 3);
    }
}

void no_sound() {
    uint8_t tmp = inb(0x61) & 0xFC;
    outb(0x61, tmp);
}

int fs_alloc_inode() {
    if (free_inodes == 0) return -1;
    
    for (int i = 0; i < MAX_INODES; i++) {
        if (inodes[i].mode == 0) {
            memset(&inodes[i], 0, sizeof(Inode));
            inodes[i].mode = 1; 
            free_inodes--;
            return i + 1; 
        }
    }
    return -1;
}

void fs_free_inode(uint32_t inode_num) {
    if (inode_num == 0 || inode_num > MAX_INODES) return;
    
    memset(&inodes[inode_num - 1], 0, sizeof(Inode));
    free_inodes++;
}

int fs_alloc_block() {
    if (free_blocks == 0) return -1;
    
    for (int i = 0; i < MAX_BLOCKS; i++) {
        if (!block_used[i]) {
            block_used[i] = true;
            free_blocks--;
            return i;
        }
    }
    return -1;
}

void fs_free_block(uint32_t block_num) {
    if (block_num >= MAX_BLOCKS) return;
    
    block_used[block_num] = false;
    free_blocks++;
}

int fs_create_file(const char* name, uint32_t parent_inode, uint8_t type) {
    if (file_count >= MAX_FILES) return FS_ERROR;
    
    int inode_num = fs_alloc_inode();
    if (inode_num == -1) return FS_ERROR;
    
    strncpy(files[file_count].name, name, 31);
    files[file_count].inode = inode_num;
    files[file_count].parent_inode = parent_inode;
    files[file_count].type = type;
    files[file_count].size = 0;
    
    Inode* inode = &inodes[inode_num - 1];
    inode->mode = type == FILE_DIR ? 0x4000 : 0x8000;
    inode->uid = 0; 
    inode->gid = 0; 
    inode->size = 0;
    inode->atime = timer_ticks;
    inode->ctime = timer_ticks;
    inode->mtime = timer_ticks;
    inode->blocks = 0;
    
    file_count++;
    
    if (file_count == 0) {
        kernel_panic("CRITICAL: Root filesystem corrupted - no files left\nAttempt to access null pointer\nKernel stack overflow detected");
    }
    
    return FS_SUCCESS;
}

int fs_delete_file(uint32_t inode_num) {
    if (inode_num == 0 || inode_num > MAX_INODES) return FS_ERROR;
    
    Inode* inode = &inodes[inode_num - 1];
    
    for (uint32_t i = 0; i < inode->blocks && i < INODE_DIRECT_BLOCKS; i++) {
        if (inode->block[i] != 0) {
            fs_free_block(inode->block[i]);
        }
    }
    
    for (int i = 0; i < file_count; i++) {
        if (files[i].inode == inode_num) {
            for (int j = i; j < file_count - 1; j++) {
                files[j] = files[j + 1];
            }
            file_count--;
            break;
        }
    }
    
    if (file_count == 0) {
        kernel_panic("CRITICAL: Root filesystem corrupted - all files deleted\nbkFS: Cannot mount root bkfs\nKernel panic - not syncing: Attempted to kill init!");
    }
    
    fs_free_inode(inode_num);
    return FS_SUCCESS;
}

int fs_write_file(uint32_t inode_num, const void* data, uint32_t size) {
    if (inode_num == 0 || inode_num > MAX_INODES) return FS_ERROR;
    
    Inode* inode = &inodes[inode_num - 1];
    uint32_t blocks_needed = (size + BLOCK_SIZE - 1) / BLOCK_SIZE;
    
    if (blocks_needed > INODE_DIRECT_BLOCKS) return FS_ERROR;
    
    for (uint32_t i = 0; i < inode->blocks; i++) {
        if (inode->block[i] != 0) {
            fs_free_block(inode->block[i]);
        }
    }
    
    for (uint32_t i = 0; i < blocks_needed; i++) {
        int block_num = fs_alloc_block();
        if (block_num == -1) {
            for (uint32_t j = 0; j < i; j++) {
                fs_free_block(inode->block[j]);
            }
            return FS_ERROR;
        }
        inode->block[i] = block_num;
    }
    
    inode->blocks = blocks_needed;
    inode->size = size;
    inode->mtime = timer_ticks;
    
    uint32_t remaining = size;
    for (uint32_t i = 0; i < blocks_needed; i++) {
        uint32_t to_copy = remaining > BLOCK_SIZE ? BLOCK_SIZE : remaining;
        memcpy(blocks[inode->block[i]], (char*)data + i * BLOCK_SIZE, to_copy);
        remaining -= to_copy;
    }
    
    return FS_SUCCESS;
}

int fs_read_file(uint32_t inode_num, void* buffer, uint32_t size) {
    if (inode_num == 0 || inode_num > MAX_INODES) return FS_ERROR;
    
    Inode* inode = &inodes[inode_num - 1];
    if (size > inode->size) size = inode->size;
    
    uint32_t remaining = size;
    for (uint32_t i = 0; i < inode->blocks && remaining > 0; i++) {
        uint32_t to_copy = remaining > BLOCK_SIZE ? BLOCK_SIZE : remaining;
        memcpy((char*)buffer + i * BLOCK_SIZE, blocks[inode->block[i]], to_copy);
        remaining -= to_copy;
    }
    
    inode->atime = timer_ticks;
    return size;
}

int fs_change_dir(uint32_t inode_num) {
    for (int i = 0; i < file_count; i++) {
        if (files[i].inode == inode_num && files[i].type == FILE_DIR) {
            current_inode = inode_num;
            return FS_SUCCESS;
        }
    }
    return FS_ERROR;
}

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

void execute_echo(char* args[], int arg_count) {
    for (int i = 1; i < arg_count; i++) {
        terminal_writestring(args[i]);
        terminal_writestring("");
    }
    terminal_writestring("\n");
}

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
    int_to_str(seconds, sec_str);
    terminal_writestring("");
    terminal_writestring(sec_str);
    terminal_writestring(" seconds\n");
}

void execute_ver() {
    terminal_writestring("Srunix R.E.\n");
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
    terminal_writestring("system Info:\n");
    
    uint32_t used_memory_kb = (next_node_addr - 0x100000) / 1024;
    uint32_t free_memory_kb = TOTAL_MEMORY_KB - used_memory_kb;
    terminal_printf("Memory: %d KB used, %d KB free\n", used_memory_kb, free_memory_kb);
    
    uint32_t seconds = timer_ticks / TIMER_HZ;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    terminal_printf("Uptime: %d hours, %d minutes, %d seconds\n", hours, minutes % 60, seconds % 60);
    
    terminal_writestring("\nDisks:\n");
    for (int i = 0; i < disk_count; i++) {
        terminal_printf("- %s: %d MB total, %d MB free bkFS\n", 
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
    strcpy(buffer, "80x25 (text mode)");
}

void execute_fetch() {
    char cpu_name[64] = {0};
    get_cpu_name(cpu_name);

    uint32_t total_mem_kb, used_mem_kb;
    get_memory_info(&total_mem_kb, &used_mem_kb);

    char resolution[32] = {0};
    get_resolution(resolution);

    // Форматируем uptime вручную
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

    terminal_writestring("              srunix\n");
    terminal_writestring("              ----------- \n");
    terminal_writestring("  ___ ___     OS: Srunix R.E. x86_64 \n");
    terminal_writestring(" / __| _ |    Host: PC x86\n");
    terminal_writestring(" |__ |   /    Kernel: SrunixKernel R.E. 8.6\n");

    terminal_writestring(" |___/_|_| _  Uptime: ");
    terminal_writestring(uptime_str);
    terminal_writestring("\n");

    terminal_writestring(" | | | | || | Packages: 0 (built-in)\n");
    terminal_writestring(" | |_| | .` | Shell: ush 3.2\n");

    terminal_writestring("  |___/|_|._| Resolution: ");
    terminal_writestring(resolution);
    terminal_writestring("\n");

    terminal_writestring(" |_ _| || /   Terminal: /dev/tty");
    char tty_num[4];
    int_to_str(current_tty + 1, tty_num);
    terminal_writestring(tty_num);
    terminal_writestring("\n");

    terminal_writestring("  | | >  <    CPU: ");
    terminal_writestring(cpu_name);
    terminal_writestring("\n");

    terminal_writestring(" |___/_/|_|   GPU: VGA (text mode)\n");

    terminal_writestring("              Memory: ");
    char mem_used_str[16], mem_total_str[16];
    int_to_str(used_mem_kb, mem_used_str);
    int_to_str(total_mem_kb, mem_total_str);
    terminal_writestring(mem_used_str);
    terminal_writestring(" KB / ");
    terminal_writestring(mem_total_str);
    terminal_writestring(" KB\n");

    terminal_writestring("              ▇▇▇▇▇▇▇\n");
    terminal_writestring("              ▇▇▇▇▇▇▇\n");
}

void execute_mkfile(char* filename) {
    if (fs_create_file(filename, current_inode, FILE_REGULAR) == FS_SUCCESS) {
        terminal_printf("File '%s' created\n", filename);
    } else {
        terminal_writestring("Failed to create file\n");
    }
}

void execute_mkdir(char* dirname) {
    if (fs_create_file(dirname, current_inode, FILE_DIR) == FS_SUCCESS) {
        terminal_printf("'%s' created\n", dirname);
    } else {
        terminal_writestring("Failed!\n");
    }
}

void execute_rm(char* name) {
    for (int i = 0; i < file_count; i++) {
        if (files[i].parent_inode == current_inode && strcmp(files[i].name, name) == 0) {
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

void execute_help(int page) {
    switch(page) {
        case 1:
            terminal_writestring("Commands (page 1/2):\n");
            terminal_writestring("help [page] - Show help\n");
            terminal_writestring("cls - Clear screen\n");
            terminal_writestring("ver - Show version\n");
            terminal_writestring("ls - List files\n");
            terminal_writestring("pwd - Print working directory\n");
            terminal_writestring("cd - Change directory\n");
            terminal_writestring("echo - Display message\n");
            terminal_writestring("cat - Display file contents\n");
            terminal_writestring("date - Show current date/time\n");
            terminal_writestring("time - Show current time\n");
            terminal_writestring("whoami - Show current user\n");
            terminal_writestring("mem - Show memory usage\n");
            terminal_writestring("Type 'help 2' for more commands\n");
            break;
        case 2:
            terminal_writestring("Commands (page 2/2):\n");
            terminal_writestring("kptest - Test Kernel Panic\n");
            terminal_writestring("lsblk - Show disk information\n");
            terminal_writestring("pause - Wait for keypress\n");
            terminal_writestring("poweroff - Shut down\n");
            terminal_writestring("reboot - Reboot\n");
            terminal_writestring("exit - Log out\n");
            terminal_writestring("sysinfo - System info\n");
            terminal_writestring("touch - Create file\n");
            terminal_writestring("mkdir - Create directory\n");
            terminal_writestring("rm - Delete file\n");
            terminal_writestring("beep - Play test sound\n");
            break;
        default:
            terminal_writestring("Invalid help page\n");
            break;
    }
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
    for (int i = 1; i < arg_count; i++) {
        if (strcmp(args[i], ">") == 0) {
            redirect_pos = i;
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
            execute_redirect_output(args[redirect_pos + 1], text);
        }
        return;
    }
    
    if (strcmp_case_insensitive(args[0], "help") == 0) {
        int page = 1;
        if (arg_count > 1) {
            page = atoi(args[1]);
        }
        execute_help(page);
    }
    else if (strcmp_case_insensitive(args[0], "cls") == 0 || 
             strcmp_case_insensitive(args[0], "clear") == 0) {
        execute_cls();
    }
    else if (strcmp_case_insensitive(args[0], "ver") == 0) {
        execute_ver();
    }
    else if (strcmp_case_insensitive(args[0], "ls") == 0) {
        execute_ls();
    }
    else if (strcmp_case_insensitive(args[0], "pwd") == 0) {
        execute_pwd();
    }
    else if (strcmp_case_insensitive(args[0], "cd") == 0) {
        if (arg_count > 1) execute_cd(args[1]);
        else execute_cd(NULL);
    }
    else if (strcmp_case_insensitive(args[0], "echo") == 0) {
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
    }
    else if (strcmp_case_insensitive(args[0], "cat") == 0) {
        if (arg_count > 1) execute_cat(args[1]);
        else terminal_writestring("Usage: cat <filename>\n");
    }
    else if (strcmp_case_insensitive(args[0], "date") == 0) {
        execute_date();
    }
    else if (strcmp_case_insensitive(args[0], "time") == 0) {
        execute_time();
    }
    else if (strcmp_case_insensitive(args[0], "whoami") == 0) {
        execute_whoami();
    }
    else if (strcmp_case_insensitive(args[0], "uptime") == 0) {
        execute_uptime();
    }
    else if (strcmp_case_insensitive(args[0], "mem") == 0 || 
             strcmp_case_insensitive(args[0], "memory") == 0) {
        execute_memory();
    }
    else if (strcmp_case_insensitive(args[0], "lsblk") == 0) {
        execute_disk();
    }
    else if (strcmp_case_insensitive(args[0], "pause") == 0) {
        execute_pause();
    }
    else if (strcmp_case_insensitive(args[0], "poweroff") == 0 || 
             strcmp_case_insensitive(args[0], "shutdown") == 0) {
        execute_poweroff();
    }
    else if (strcmp_case_insensitive(args[0], "kptest") == 0) {
        kernel_panic("CRITICAL: error!\nSystem is crashed\nKernel panic - not syncing: Attempted to kill init!");
    }
    else if (strcmp_case_insensitive(args[0], "reboot") == 0 || 
             strcmp_case_insensitive(args[0], "restart") == 0) {
        execute_reboot();
    }
    else if (strcmp_case_insensitive(args[0], "exit") == 0) {
        execute_exit();
    }
    else if (strcmp_case_insensitive(args[0], "sysinfo") == 0) {
        execute_sysinfo();
    }
    else if (strcmp_case_insensitive(args[0], "ced") == 0) {
    uint32_t inode = 0;
    if (arg_count > 1) {
    }
    code_ed_run(inode, arg_count > 1 ? args[1] : "newfile");
}
    else if (strcmp_case_insensitive(args[0], "touch") == 0) {
        if (arg_count > 1) execute_mkfile(args[1]);
        else terminal_writestring("Usage: touch <filename>\n");
    }
    else if (strcmp_case_insensitive(args[0], "fetch") == 0 ||
             strcmp_case_insensitive(args[0], "neofetch") == 0 ||
             strcmp_case_insensitive(args[0], "./fetch") == 0) {
	    execute_fetch();
    }  
    else if (strcmp_case_insensitive(args[0], "mkdir") == 0) {
        if (arg_count > 1) execute_mkdir(args[1]);
        else terminal_writestring("Usage: mkdir <dirname>\n");
    }
    else if (strcmp_case_insensitive(args[0], "rm") == 0) {
        if (arg_count > 1) execute_rm(args[1]);
        else terminal_writestring("Usage: rm <filename>\n");
    }
    else if (strcmp_case_insensitive(args[0], "beep") == 0) {
        execute_beep();
    }
    else if (args[0][0] != '\0') {
        terminal_writestring("Command not found: ");
        terminal_writestring(args[0]);
        terminal_writestring("\nType 'help' for commands\n");
    }
}

void print_prompt() {
    terminal_setcolor(COLOR_WHITE, terminal_color >> 4);
    terminal_writestring("root:");
    
    if (current_inode != 1) {
        terminal_writestring("~");
        
        char path[MAX_PATH_LEN] = {0};
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
        
        terminal_writestring(path);
    } else {
        terminal_writestring("~"); 
    }
    
    terminal_writestring("# ");
    terminal_setcolor(COLOR_WHITE, terminal_color >> 4);
    move_cursor(terminal_column, terminal_row);
}

void login_screen() {
    terminal_setcolor(COLOR_WHITE, COLOR_BLACK);
    terminal_clear();
    
    terminal_writestring("\n\n");
    terminal_writestring("Srunix R.E. tty");
    char tty_num[2];
    int_to_str(current_tty + 1, tty_num);
    terminal_writestring(tty_num);
    terminal_writestring("\n");
    terminal_writestring("OpenSource Operation System.\nPlease login: root, password 1\n\n");
    
    char username[32] = {0};
    char password[32] = {0};
    int pos = 0;
    
    terminal_writestring("srunix login: ");
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
            terminal_putchar(' ');
        }
    }
    
    if (strcmp(username, "srunix") == 0 && strcmp(password, "1") == 0) {
        terminal_writestring("\nWelcome to Srunix R.E.\n");
        ttys[current_tty].logged_in = true;
        shell();
    }
    if (strcmp(username, "chebupeli") == 0 && strcmp(password, "1") == 0) {
        ttys[current_tty].logged_in = false;
        shell();
        terminal_writestring("\nCHEBUPELISIII EBAT\n");
    }
    
    if (strcmp(username, "root") == 0 && strcmp(password, "1") == 0) {
        terminal_writestring("\nWelcome to Srunix R.E.\n");
        ttys[current_tty].logged_in = true;
        shell();
    } else {
        terminal_writestring("\nlogin incorrect\n");
        for (volatile int i = 0; i < 10000000; i++);
        login_screen();
    }
}

void shell() {
    char cmd[MAX_CMD_LEN];
    int pos = 0;
    
    terminal_writestring("\nSrunix R.E. tty");
    char tty_num[2];
    int_to_str(current_tty + 1, tty_num);
    terminal_writestring(tty_num);
    terminal_writestring("\n\n");
    
    while (1) {
        print_prompt();
        pos = 0;
        
        while (pos < MAX_CMD_LEN - 1) {
            char c = keyboard_getchar();
            
            if (c == '\n') {
                terminal_putchar('\n');
                cmd[pos] = '\0';
                execute_command(cmd);
                break;
            }
            else if (c == '\b') {
                if (pos > 0) {
                    pos--;
                    terminal_putchar('\b');
                }
            }
            else if (c == '\x11' || c == '\x12' || c == '\x13' || c == '\x14') {
            }
            else {
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

        const char* fetch_content = 
            "���������␃�>�␁��� �␁�����@�������àÉ␅���������@�8�␋�@�␞�␝�␆���␄���@�������@�������@�������h␂������h␂������␈�������␃���␄���¨␂������¨␂������¨␂������␜�������␜�������␁�������␁���␄���������������������������␔�␁�����␔�␁������␐������␁���␅��� �␁����� �␁����� �␁������%␄������%␄������␐������␁���␆��� ­␅����� Í␅����� Í␅�����x␔������x␔�������␐������␁���␆��� Á␅����� ñ␅����� ñ␅�����ð␆������Ø␏�������␐������␂���␆���ð¸␅�����ðØ␅�����ðØ␅�����p␂������p␂������␈�������Råtd␄��� ­␅����� Í␅����� Í␅�����x\n"
            "������� \"              srunix\"\n"
            "������� \"              ----------- \"\n"
            "������� \"  ___ ___     OS: Srunix R.E. x86_64 \"\n"
            "������� \" / __| _ |    Host: PC amd64\"\n"
            "������� \" |__ |   /    Kernel: SrunixKernel R.E. 8.6\"\n"
            "������� \" |___/_|_| _  Uptime: \n"
            "������� \" | | | | || | Packages:\"\n"
            "������� \" | |_| | .` | Shell: ush 3.2\"\n"
            "������� \"  |___/|_|._| Resolution: 1920x1080\"\n"
            "������� \" |_ _| || /   Terminal: /dev/gsh-tty\"\n"
            "������� \"  | | >  <    CPU:\"\n"
            "������� \" |___/_/|_|   GPU:\"\n"
            "������� \"              Memory:\"\n"
            "������� \"              ▇▇▇▇▇▇▇\"\n"
            "������� \"              ▇▇▇▇▇▇▇\"\n";
        
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
