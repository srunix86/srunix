#include <stddef.h>
#include "../lib/pring.h"
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

