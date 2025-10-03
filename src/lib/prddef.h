#include "pring.h"
#include <stddef.h>

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

