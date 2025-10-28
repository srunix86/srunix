#include "srunix86_api.h"
#include "../lib/pring.h"
#include "../lib/prdio.h"
#include "../fs/bkfs.h"

srunix86_api_t srunix86_api;

static void* api_malloc(size_t size) { return malloc(size); }
static void api_free(void* ptr) { free(ptr); }
static void* api_calloc(size_t num, size_t size) { return calloc(num, size); }
static void* api_realloc(void* ptr, size_t size) { return realloc(ptr, size); }

static uint32_t api_fork(void) { return sys_fork(); }
static void api_exit(uint32_t status) { sys_exit(status); }
static void api_sleep(uint32_t ms) { 
    uint32_t target = timer_ticks + (ms * TIMER_HZ / 1000);
    while (timer_ticks < target) asm volatile("pause");
}

static int api_open(const char* path, int flags) {
    for (int i = 0; i < file_count; i++) {
        if (strcmp(files[i].name, path) == 0) {
            return files[i].inode;        }
    }
    return -1;
}

static int api_close(int fd) { return 0; }

static int api_read(int fd, void* buf, size_t count) {
    return fs_read_file(fd, buf, count);
}

static int api_write(int fd, const void* buf, size_t count) {
    return fs_write_file(fd, (char*)buf, count);
}

static void api_print(const char* str) { terminal_writestring(str); }
static void api_println(const char* str) { 
    terminal_writestring(str); 
    terminal_writestring("\n");
}
static char api_getchar(void) { return keyboard_getchar(); }
static void api_set_color(uint8_t fg, uint8_t bg) { terminal_setcolor(fg, bg); }
static void api_clear_screen(void) { terminal_clear(); }

static uint32_t api_get_memory_total(void) { return TOTAL_MEMORY_KB; }
static uint32_t api_get_memory_used(void) { 
    return (next_node_addr - 0x100000) / 1024; 
}
static uint32_t api_get_uptime(void) { return timer_ticks / TIMER_HZ; }
static const char* api_get_kernel_version(void) { return SRUNIX86_KERNEL_VERSION; }

void init_srunix86_api() {
    srunix86_api.malloc = api_malloc;
    srunix86_api.free = api_free;
    srunix86_api.calloc = api_calloc;
    srunix86_api.realloc = api_realloc;
    
    srunix86_api.fork = api_fork;
    srunix86_api.exit = api_exit;
    srunix86_api.sleep = api_sleep;
    
    srunix86_api.open = api_open;
    srunix86_api.close = api_close;
    srunix86_api.read = api_read;
    srunix86_api.write = api_write;
    srunix86_api.mkdir = execute_mkdir;
    srunix86_api.unlink = execute_rm;
    
    srunix86_api.print = api_print;
    srunix86_api.println = api_println;
    srunix86_api.getchar = api_getchar;
    srunix86_api.set_color = api_set_color;
    srunix86_api.clear_screen = api_clear_screen;
    
    srunix86_api.get_memory_total = api_get_memory_total;
    srunix86_api.get_memory_used = api_get_memory_used;
    srunix86_api.get_uptime = api_get_uptime;
    srunix86_api.get_kernel_version = api_get_kernel_version;
    
    srunix86_api.net_connect = NULL;
    srunix86_api.net_send = NULL;
    srunix86_api.net_recv = NULL;
    srunix86_api.net_close = NULL;
}
