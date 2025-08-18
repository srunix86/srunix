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
char* strtok(char* str, const char* delim);

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

char* strpbrk(const char* s, const char* accept) {
    while (*s != '\0') {
        const char* a = accept;
        while (*a != '\0') {
            if (*s == *a) {
                return (char*)s;
            }
            a++;
        }
        s++;
    }
    return NULL;
}

char* strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* new = malloc(len);
    if (new == NULL) return NULL;
    memcpy(new, s, len);
    return new;
}

char* strtok(char* str, const char* delim) {
    static char* saved_str = NULL;
    if (str != NULL) {
        saved_str = str;
    }
    if (saved_str == NULL || *saved_str == '\0') {
        return NULL;
    }
    char* token_start = saved_str;
    char* token_end = strpbrk(saved_str, delim);
    if (token_end != NULL) {
        *token_end = '\0';
        saved_str = token_end + 1;
    } else {
        saved_str = NULL;
    }
    return token_start;
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
    uint32_t pgid;
    char name[32];
    uint32_t priority;
    uint32_t state;
    uintptr_t stack_ptr;
    uintptr_t entry_point;
    uint32_t exit_code;
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

typedef struct {
    char buffer[1024];
    size_t read_pos;
    size_t write_pos;
    bool is_full;
    bool is_empty;
} Pipe;

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
int current_process = 0;

Pipe pipes[MAX_PIPES];
int pipe_count = 0;

char command_history[HISTORY_SIZE][MAX_CMD_LEN];
int history_count = 0;
int history_pos = 0;

bool mouse_enabled = false;
int mouse_x = 40;
int mouse_y = 12;
bool mouse_left_pressed = false;
bool mouse_right_pressed = false;
int selection_start_x = -1;
int selection_start_y = -1;
int selection_end_x = -1;
int selection_end_y = -1;

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
void execute_ps();
void execute_jobs();
void execute_kill(char* pid_str, char* sig_str);
void klog(int level, const char* message);
uint32_t sys_fork();
void sys_exit(uint32_t status);
void send_signal(uint32_t pid, uint32_t sig);
void mouse_wait(uint8_t type);
void mouse_write(uint8_t data);
uint8_t mouse_read();
void mouse_handler();
void init_mouse();
void draw_mouse();
void clear_mouse();
void update_selection();
void draw_selection();
void clear_selection();
void execute_smouse();

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
    } else if (num == 0) {
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

bool is_valid_filename(const char* name) {
    const char* invalid_chars = "&;|*?'\"`[]()$<>{}^#\\/%!";
    if (strlen(name) == 0) return false;
    
    for (const char* c = name; *c; c++) {
        if (strchr(invalid_chars, *c)) {
            return false;
        }
    }
    return true;
}

bool file_exists_in_current_dir(const char* name) {
    for (int i = 0; i < file_count; i++) {
        if (files[i].parent_inode == current_inode && strcmp(files[i].name, name) == 0) {
            return true;
        }
    }
    return false;
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

bool smouse_mode = false;
uint32_t last_smouse_clear = 0;

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
    if (smouse_mode) return;

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
    if (smouse_mode) return;

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
    if (smouse_mode) return;

    for (size_t i = 0; i < size; i++) {
        terminal_putchar(data[i]);
    }
}

void terminal_writestring(const char* data) {
    if (smouse_mode) return;

    while (*data) {
        terminal_putchar(*data++);
    }
}

void terminal_printf(const char* format, ...) {
    if (smouse_mode) return;

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

void mouse_wait(uint8_t type) {
    uint32_t timeout = 100000;
    if (type == 0) {
        while (timeout--) {
            if ((inb(0x64) & 1) == 1) return;
        }
    } else {
        while (timeout--) {
            if ((inb(0x64) & 2) == 0) return;
        }
    }
}

void mouse_write(uint8_t data) {
    mouse_wait(1);
    outb(0x64, 0xD4);
    mouse_wait(1);
    outb(0x60, data);
}

uint8_t mouse_read() {
    mouse_wait(0);
    return inb(0x60);
}

void mouse_handler() {
    static uint8_t mouse_cycle = 0;
    static char mouse_byte[3];

    switch(mouse_cycle) {
        case 0:
            mouse_byte[0] = mouse_read();
            if (!(mouse_byte[0] & 0x08)) return;
            mouse_cycle++;
            break;
        case 1:
            mouse_byte[1] = mouse_read();
            mouse_cycle++;
            break;
        case 2:
            mouse_byte[2] = mouse_read();

            mouse_left_pressed = (mouse_byte[0] & 0x01);
            mouse_right_pressed = (mouse_byte[0] & 0x02);

            int dx = (int)mouse_byte[1];
            int dy = (int)mouse_byte[2];

            if (mouse_byte[0] & 0x10) dx |= 0xFFFFFF00;
            if (mouse_byte[0] & 0x20) dy |= 0xFFFFFF00;

            dy = -dy;

            mouse_x += dx / 2;
            mouse_y += dy / 2;

            if (mouse_x < 0) mouse_x = 0;
            if (mouse_x >= SCREEN_WIDTH) mouse_x = SCREEN_WIDTH - 1;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_y >= SCREEN_HEIGHT) mouse_y = SCREEN_HEIGHT - 1;

            if (mouse_left_pressed) {
                if (selection_start_x == -1) {
                    selection_start_x = mouse_x;
                    selection_start_y = mouse_y;
                }
                selection_end_x = mouse_x;
                selection_end_y = mouse_y;
            } else {
                if (selection_start_x != -1) {
                    clear_selection();
                    selection_start_x = -1;
                    selection_start_y = -1;
                    selection_end_x = -1;
                    selection_end_y = -1;
                }
            }

            mouse_cycle = 0;
            break;
    }
}

void init_mouse() {
    mouse_wait(1);
    outb(0x64, 0xA8);

    mouse_wait(1);
    outb(0x64, 0x20);
    mouse_wait(0);
    uint8_t status = inb(0x60) | 2;
    mouse_wait(1);
    outb(0x64, 0x60);
    mouse_wait(1);
    outb(0x60, status);

    mouse_write(0xF6);
    mouse_read();

    mouse_write(0xF4);
    mouse_read();

    mouse_enabled = true;
}

void draw_mouse() {
    if (!mouse_enabled) return;

    uint16_t pos = mouse_y * SCREEN_WIDTH + mouse_x;
    uint16_t attr = terminal_buffer[pos];
    terminal_buffer[pos] = 0xDB | (COLOR_WHITE << 8);
}

void clear_mouse() {
    if (!mouse_enabled) return;

    uint16_t pos = mouse_y * SCREEN_WIDTH + mouse_x;
    uint16_t attr = terminal_buffer[pos];
    terminal_buffer[pos] = attr;
}

void update_selection() {
    if (selection_start_x == -1 || selection_end_x == -1) return;

    clear_selection();
    draw_selection();
}

void draw_selection() {
    if (selection_start_x == -1 || selection_end_x == -1) return;

    int start_x = selection_start_x < selection_end_x ? selection_start_x : selection_end_x;
    int end_x = selection_start_x < selection_end_x ? selection_end_x : selection_start_x;
    int start_y = selection_start_y < selection_end_y ? selection_start_y : selection_end_y;
    int end_y = selection_start_y < selection_end_y ? selection_end_y : selection_start_y;

    for (int y = start_y; y <= end_y; y++) {
        for (int x = (y == start_y ? start_x : 0); x <= (y == end_y ? end_x : SCREEN_WIDTH - 1); x++) {
            uint16_t pos = y * SCREEN_WIDTH + x;
            uint16_t attr = terminal_buffer[pos];
            terminal_buffer[pos] = (attr & 0xFF) | ((COLOR_BLACK << 4) | COLOR_WHITE) << 8;
        }
    }
}

void clear_selection() {
    if (selection_start_x == -1 || selection_end_x == -1) return;

    int start_x = selection_start_x < selection_end_x ? selection_start_x : selection_end_x;
    int end_x = selection_start_x < selection_end_x ? selection_end_x : selection_start_x;
    int start_y = selection_start_y < selection_end_y ? selection_start_y : selection_end_y;
    int end_y = selection_start_y < selection_end_y ? selection_end_y : selection_start_y;

    for (int y = start_y; y <= end_y; y++) {
        for (int x = (y == start_y ? start_x : 0); x <= (y == end_y ? end_x : SCREEN_WIDTH - 1); x++) {
            uint16_t pos = y * SCREEN_WIDTH + x;
            uint16_t attr = terminal_buffer[pos];
            terminal_buffer[pos] = (attr & 0xFF) | (terminal_color << 8);
        }
    }
}

void execute_smouse() {
    if (mouse_enabled) {
        smouse_mode = true;
        terminal_setcolor(COLOR_WHITE, COLOR_BLUE);
        terminal_clear();
        last_smouse_clear = timer_ticks;

        while (smouse_mode) {
            if (timer_ticks - last_smouse_clear >= TIMER_HZ) {
                terminal_clear();
                last_smouse_clear = timer_ticks;
            }

            if (mouse_enabled && (inb(0x64) & 1)) {
                uint8_t data = inb(0x60);
                mouse_handler();
                clear_mouse();
                draw_mouse();
                update_selection();
            }

        }
    } else {
        terminal_writestring("Mouse is not enabled\n");
    }
}

char keyboard_getchar() {
    if (smouse_mode) {
        while (1) {
            if (mouse_enabled && (inb(0x64) & 1)) {
                uint8_t data = inb(0x60);
                mouse_handler();
                clear_mouse();
                draw_mouse();
                update_selection();
            }
            asm volatile ("pause");
        }
    }

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

        if (mouse_enabled && (inb(0x64) & 1)) {
            uint8_t data = inb(0x60);
            mouse_handler();
            clear_mouse();
            draw_mouse();
            update_selection();
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
    if (!is_valid_filename(name)) {
        terminal_writestring("Invalid filename: contains forbidden characters\n");
        return FS_ERROR;
    }

    if (file_exists_in_current_dir(name)) {
        terminal_writestring("Change name\n");
        return FS_ERROR;
    }

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
	    terminal_setcolor(COLOR_RED, COLOR_BLACK);
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
	    terminal_setcolor(COLOR_RED, COLOR_BLACK);
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
