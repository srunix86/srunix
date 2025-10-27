#include <stddef.h>
#include "../lib/pring.h"

void terminal_writestring(const char* data);

static void zov() {
	    terminal_setcolor(COLOR_WHITE, COLOR_GRAY);
	    terminal_writestring("\n\n\n\n\n\n\n\n\n");
	    terminal_setcolor(COLOR_WHITE, COLOR_YELLOW);
	    terminal_writestring("\n\n\n\n\n\n\n\n");
	    terminal_setcolor(COLOR_WHITE, COLOR_WHITE);
	    terminal_writestring("\n\n\n\n\n\n\n\n");
}
