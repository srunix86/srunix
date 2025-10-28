#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

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
#define MAX_PIPES 10
#define HISTORY_SIZE 100

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

#define LOG_LEVEL_DEBUG 0
#define LOG_LEVEL_INFO  1
#define LOG_LEVEL_WARN  2
#define LOG_LEVEL_ERROR 3

#define SIGINT  2
#define SIGKILL 9
#define SIGSTOP 19
#define SIGCONT 18


