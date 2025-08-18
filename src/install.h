#ifndef INSTALL_H
#define INSTALL_H

#include "port.h"

#define INSTALL_BG COLOR_BLUE
#define INSTALL_FG COLOR_WHITE
#define INSTALL_TITLE COLOR_YELLOW

#define INSTALL_WIDTH 60
#define INSTALL_HEIGHT 20

typedef struct {
    uint8_t drive_num;
    char name[8];
    uint32_t sectors;
} InstallDisk;

#define MAX_INSTALL_DISKS 4
InstallDisk install_disks[MAX_INSTALL_DISKS];
uint8_t install_disk_count = 0;


void wait_key() {
    while(!keyboard_getchar());
}

void clear_screen() {
    terminal_setcolor(INSTALL_FG, INSTALL_BG);
    terminal_clear();
}


void detect_disks() {
    install_disk_count = 2;
    
    install_disks[0].drive_num = 0x80;
    strcpy(install_disks[0].name, "hd0");
    install_disks[0].sectors = 62500000;
    
    install_disks[1].drive_num = 0x81;
    strcpy(install_disks[1].name, "hd1");
    install_disks[1].sectors = 31250000; 
}


void format_disk(uint8_t disk_num) {
    for(int i = 0; i <= 100; i += 10) {
        terminal_printf("\r Formatting... %d%% ", i);
        for(volatile int j = 0; j < 5000000; j++);
    }
    terminal_writestring("\n");
}

void copy_system_files(uint8_t disk_num) {
    for(int i = 0; i <= 100; i += 10) {
        terminal_printf("\r Copying files... %d%% ", i);
        for(volatile int j = 0; j < 5000000; j++);
    }
    terminal_writestring("\n");
}

void install_bootloader(uint8_t disk_num) {
    for(int i = 0; i <= 100; i += 10) {
        terminal_printf("\rInstalling bootloader... %d%% \n", i);
        for(volatile int j = 0; j < 5000000; j++);
    }
    terminal_writestring("\n");
}


void show_disk_menu() {
    clear_screen();
    
    terminal_setcolor(INSTALL_TITLE, INSTALL_BG);
    terminal_writestring("\n  SRUNIX INSTALLER\n");
    terminal_writestring("  ===============\n\n");
    
    terminal_setcolor(INSTALL_FG, INSTALL_BG);
    terminal_writestring("Available disks:\n");
    for(int i = 0; i < install_disk_count; i++) {
        terminal_printf("%d. %s (%d MB)\n", 
                      i+1, 
                      install_disks[i].name,
                      install_disks[i].sectors / 2048);
    }
    
    terminal_writestring("\nSelect disk [1-");
    terminal_printf("%d]: ", install_disk_count);
}

void show_confirmation(uint8_t disk_num) {
    clear_screen();
    terminal_writestring("\nWARNING: ALL DATA ON ");
    terminal_writestring(install_disks[disk_num].name);
    terminal_writestring(" WILL BE LOST!\n\n");
    terminal_writestring("Continue installation? (y/n): ");
}

void run_installation(uint8_t disk_num) {
    clear_screen();
    terminal_writestring("\nStarting installation on ");
    terminal_writestring(install_disks[disk_num].name);
    terminal_writestring("\n\n");
    
    format_disk(disk_num);
    copy_system_files(disk_num);
    install_bootloader(disk_num);
    
    terminal_writestring("\nInstallation complete!\n");
    terminal_writestring("Remove installation media and press any key to reboot...\n");
    wait_key();
}


void execute_install() {
    detect_disks();
    
    uint8_t selected = 0;
    while(1) {
        show_disk_menu();
        char input = keyboard_getchar();
        selected = input - '0';
        
        if(selected >= 1 && selected <= install_disk_count) {
            selected--;
            break;
        }
        
        terminal_writestring("\nInvalid selection! Try again.\n");
        for(volatile int i = 0; i < 5000000; i++);
    }
    
    while(1) {
        show_confirmation(selected);
        char input = keyboard_getchar();
        
        if(input == 'y' || input == 'Y') {
            break;
        } else if(input == 'n' || input == 'N') {
            terminal_writestring("\nInstallation cancelled.\n");
            return;
        }
        
        terminal_writestring("\nPlease answer 'y' or 'n'.\n");
        for(volatile int i = 0; i < 5000000; i++);
    }
    
    run_installation(selected);
}

#endif
