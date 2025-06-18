#ifndef CODE_ED_H
#define CODE_ED_H

void terminal_writestring(const char* data);
void terminal_putchar(char c);
char keyboard_getchar();
void move_cursor(size_t x, size_t y);
void* malloc(size_t size);
void free(void* ptr);
int fs_write_file(uint32_t inode_num, const void* data, uint32_t size);
int fs_read_file(uint32_t inode_num, void* buffer, uint32_t size);

#define MAX_LINE_LEN 256
#define MAX_LINES 1000
#define TAB_WIDTH 4
#define STATUS_BAR_ROW 24
#define EDITOR_ROWS 23
#define CTRL(x) ((x) & 0x1f)

typedef struct Line {
    char text[MAX_LINE_LEN];
    int length;
    struct Line* next;
    struct Line* prev;
} Line;

typedef struct {
    Line* first_line;
    Line* current_line;
    int cursor_x;
    int cursor_y;
    int row_offset;
    int col_offset;
    uint32_t file_inode;
    bool dirty;
    char status_msg[80];
    char filename[64];
} EditorState;

static EditorState E;

static void str_copy(char* dest, const char* src) {
    while ((*dest++ = *src++));
}

static int str_len(const char* s) {
    int len = 0;
    while (*s++) len++;
    return len;
}

static void clear_screen_row(int y) {
    move_cursor(0, y);
    for (int i = 0; i < 80; i++) {
        terminal_putchar(' ');
    }
}

static void draw_status_bar() {
    move_cursor(0, STATUS_BAR_ROW);
    terminal_writestring("^G Help ^O WriteOut ^W Where Is ^K Cut Text ^J Justify ^C Cur Pos");
}

static void draw_shortcuts() {
    move_cursor(0, STATUS_BAR_ROW + 1);
    terminal_writestring("^X Exit   ^R Read File ^\\ Replace ^U Paste Text ^T To Spell ^_ Go To Line");
}

static void init_editor() {
    E.first_line = malloc(sizeof(Line));
    E.first_line->text[0] = '\0';
    E.first_line->length = 0;
    E.first_line->next = NULL;
    E.first_line->prev = NULL;
    E.current_line = E.first_line;
    E.cursor_x = 0;
    E.cursor_y = 0;
    E.row_offset = 0;
    E.col_offset = 0;
    E.file_inode = 0;
    E.dirty = false;
    E.status_msg[0] = '\0';
    E.filename[0] = '\0';
}

static void insert_line_after(Line* line) {
    Line* new_line = malloc(sizeof(Line));
    new_line->text[0] = '\0';
    new_line->length = 0;
    new_line->prev = line;
    new_line->next = line->next;
    if (line->next) line->next->prev = new_line;
    line->next = new_line;
}

static void insert_char(int c) {
    if (E.current_line->length >= MAX_LINE_LEN-1) return;
    
    for (int i = E.current_line->length; i > E.cursor_x; i--) {
        E.current_line->text[i] = E.current_line->text[i-1];
    }
    E.current_line->text[E.cursor_x] = c;
    E.current_line->length++;
    E.current_line->text[E.current_line->length] = '\0';
    E.cursor_x++;
    E.dirty = true;
}

static void delete_char() {
    if (E.cursor_x == 0) {
        if (E.current_line->prev) {
            Line* prev = E.current_line->prev;
            int prev_len = prev->length;
            
            if (prev_len + E.current_line->length < MAX_LINE_LEN) {
                str_copy(prev->text + prev_len, E.current_line->text);
                prev->length += E.current_line->length;
                prev->next = E.current_line->next;
                if (E.current_line->next) {
                    E.current_line->next->prev = prev;
                }
                free(E.current_line);
                E.current_line = prev;
                E.cursor_x = prev_len;
                E.cursor_y--;
            }
        }
        return;
    }
    
    for (int i = E.cursor_x - 1; i < E.current_line->length; i++) {
        E.current_line->text[i] = E.current_line->text[i+1];
    }
    E.current_line->length--;
    E.cursor_x--;
    E.dirty = true;
}

static void load_file(uint32_t inode_num) {
    char buffer[4096];
    int size = fs_read_file(inode_num, buffer, sizeof(buffer));
    if (size <= 0) return;
    
    char* line_start = buffer;
    Line* current = E.first_line;
    
    for (char* p = buffer; p < buffer + size; p++) {
        if (*p == '\n') {
            *p = '\0';
            str_copy(current->text, line_start);
            current->length = str_len(line_start);
            insert_line_after(current);
            current = current->next;
            line_start = p + 1;
        }
    }
    
    if (line_start < buffer + size) {
        str_copy(current->text, line_start);
        current->length = str_len(line_start);
    }
    
    E.file_inode = inode_num;
}

static void save_file() {
    int total_size = 0;
    for (Line* line = E.first_line; line; line = line->next) {
        total_size += line->length + 1;
    }
    
    char* buffer = malloc(total_size);
    char* ptr = buffer;
    for (Line* line = E.first_line; line; line = line->next) {
        str_copy(ptr, line->text);
        ptr += line->length;
        *ptr++ = '\n';
    }
    
    fs_write_file(E.file_inode, buffer, total_size);
    free(buffer);
    E.dirty = false;
    
    str_copy(E.status_msg, "File saved successfully");
}

static void draw_text() {
    Line* line = E.first_line;
    for (int y = 0; y < EDITOR_ROWS && line; y++) {
        move_cursor(0, y);
        terminal_writestring(line->text);
        line = line->next;
    }
}

static void refresh_screen() {
    clear_screen_row(STATUS_BAR_ROW);
    clear_screen_row(STATUS_BAR_ROW + 1);
    
    draw_text();
    draw_status_bar();
    draw_shortcuts();
    
    if (E.status_msg[0]) {
        move_cursor(0, STATUS_BAR_ROW);
        terminal_writestring(E.status_msg);
        E.status_msg[0] = '\0';
    }
    
    move_cursor(E.cursor_x, E.cursor_y);
}

void code_ed_run(uint32_t inode_num, const char* filename) {
    init_editor();
    if (inode_num != 0) {
        str_copy(E.filename, filename);
        load_file(inode_num);
    }
    
    while (1) {
        refresh_screen();
        char c = keyboard_getchar();
        
        switch (c) {
            case CTRL('X'):
                if (E.dirty) {
                    str_copy(E.status_msg, "Save modified buffer? (Y/N)");
                    refresh_screen();
                    c = keyboard_getchar();
                    if (c == 'y' || c == 'Y') {
                        save_file();
                    }
                }
                return;
                
            case CTRL('O'):
                save_file();
                break;
                
            case '\n':
                insert_line_after(E.current_line);
                str_copy(E.current_line->next->text, E.current_line->text + E.cursor_x);
                E.current_line->next->length = str_len(E.current_line->next->text);
                E.current_line->length = E.cursor_x;
                E.current_line->text[E.cursor_x] = '\0';
                E.current_line = E.current_line->next;
                E.cursor_x = 0;
                E.cursor_y++;
                E.dirty = true;
                break;
                
            case 127:
                delete_char();
                break;
                
            case CTRL('G'):
                str_copy(E.status_msg, "Help: ^X:Exit ^O:Save ^W:Search ^K:Cut ^J:Format ^C:CursorPos");
                break;
                
            default:
                if (c >= 32 && c < 127) {
                    insert_char(c);
                }
                break;
        }
    }
}

#endif
