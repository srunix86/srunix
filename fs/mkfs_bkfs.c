#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/fs.h>

#define BKFS_MAGIC 0xBACAF5
#define BKFS_ROOT_INO 2
#define BKFS_BLOCK_SIZE 4096
#define BKFS_FILENAME_LEN 255
#define BKFS_MAX_EXTENTS 12

struct mkfs_bkfs_super {
    uint32_t s_magic;
    uint32_t s_block_size;
    uint32_t s_blocks_count;
    uint32_t s_free_blocks_count;
    uint32_t s_free_inodes_count;
    uint32_t s_inodes_count;
    uint32_t s_first_data_block;
    uint32_t s_first_ino;
    uint32_t s_inode_blocks;
    uint64_t s_mtime;
    uint32_t s_state;
    uint32_t s_errors;
    uint32_t s_creator_os;
    uint32_t s_rev_level;
    uint16_t s_def_resuid;
    uint16_t s_def_resgid;
    uint32_t s_reserved[236];
};

struct mkfs_bkfs_inode {
    uint16_t i_mode;
    uint16_t i_uid;
    uint16_t i_gid;
    uint32_t i_size;
    uint32_t i_atime;
    uint32_t i_ctime;
    uint32_t i_mtime;
    uint32_t i_dtime;
    uint16_t i_links_count;
    uint32_t i_blocks;
    uint32_t i_flags;
    uint32_t i_block[BKFS_MAX_EXTENTS];
    uint32_t i_generation;
    uint32_t i_reserved;
};

struct mkfs_bkfs_dir_entry {
    uint32_t inode;
    uint16_t rec_len;
    uint16_t name_len;
    char name[BKFS_FILENAME_LEN];
};

static uint64_t get_device_size(int fd)
{
    uint64_t size = 0;
    
    if (ioctl(fd, BLKGETSIZE64, &size) == -1) {
        perror("ioctl BLKGETSIZE64 failed");
        return 0;
    }
    
    return size;
}

static void write_superblock(int fd, uint32_t blocks_count)
{
    struct mkfs_bkfs_super sb = {0};
    uint32_t inodes_count = 1024;
    uint32_t inode_blocks = (inodes_count * sizeof(struct mkfs_bkfs_inode) + BKFS_BLOCK_SIZE - 1) / BKFS_BLOCK_SIZE;

    sb.s_magic = BKFS_MAGIC;
    sb.s_block_size = BKFS_BLOCK_SIZE;
    sb.s_blocks_count = blocks_count;
    sb.s_free_blocks_count = blocks_count - inode_blocks - 2;
    sb.s_free_inodes_count = inodes_count - 1;
    sb.s_inodes_count = inodes_count;
    sb.s_first_data_block = 1;
    sb.s_first_ino = BKFS_ROOT_INO;
    sb.s_inode_blocks = inode_blocks;
    sb.s_mtime = time(NULL);
    sb.s_state = 1;
    sb.s_creator_os = 0;
    sb.s_rev_level = 1;
    sb.s_def_resuid = 0;
    sb.s_def_resgid = 0;

    if (pwrite(fd, &sb, sizeof(sb), 0) != sizeof(sb)) {
        perror("Failed to write superblock");
        exit(1);
    }
}

static void write_root_inode(int fd)
{
    struct mkfs_bkfs_inode inode = {0};
    uint32_t inode_block = 1;

    inode.i_mode = S_IFDIR | 0755;
    inode.i_uid = 0;
    inode.i_gid = 0;
    inode.i_size = BKFS_BLOCK_SIZE;
    inode.i_atime = time(NULL);
    inode.i_ctime = time(NULL);
    inode.i_mtime = time(NULL);
    inode.i_links_count = 2;
    inode.i_blocks = 1;
    inode.i_block[0] = inode_block + 1;

    if (pwrite(fd, &inode, sizeof(inode), BKFS_BLOCK_SIZE) != sizeof(inode)) {
        perror("Failed to write root inode");
        exit(1);
    }
}

static void write_root_dir_block(int fd)
{
    char block[BKFS_BLOCK_SIZE] = {0};
    struct mkfs_bkfs_dir_entry *de = (struct mkfs_bkfs_dir_entry *)block;
    uint32_t block_pos = 2;

    de->inode = BKFS_ROOT_INO;
    de->name_len = 1;
    de->rec_len = 12;
    strcpy(de->name, ".");
    
    struct mkfs_bkfs_dir_entry *de2 = (struct mkfs_bkfs_dir_entry *)(block + de->rec_len);
    de2->inode = BKFS_ROOT_INO;
    de2->name_len = 2;
    de2->rec_len = BKFS_BLOCK_SIZE - de->rec_len;
    strcpy(de2->name, "..");

    if (pwrite(fd, block, BKFS_BLOCK_SIZE, block_pos * BKFS_BLOCK_SIZE) != BKFS_BLOCK_SIZE) {
        perror("Failed to write root directory block");
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    int fd;
    uint64_t device_size;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s /dev/device\n", argv[0]);
        exit(1);
    }

    fd = open(argv[1], O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        exit(1);
    }

    device_size = get_device_size(fd);
    if (device_size == 0) {
        fprintf(stderr, "Failed to get device size\n");
        close(fd);
        exit(1);
    }

    printf("Device size: %lu bytes\n", device_size);
    printf("Block size: %d bytes\n", BKFS_BLOCK_SIZE);
    
    uint32_t blocks_count = device_size / BKFS_BLOCK_SIZE;
    printf("Total blocks: %u\n", blocks_count);
    
    if (blocks_count < 3) {
        fprintf(stderr, "Device too small. Need at least 3 blocks (%d bytes), got %lu bytes\n", 
                3 * BKFS_BLOCK_SIZE, device_size);
        close(fd);
        exit(1);
    }

    printf("Creating filesystem with %u blocks...\n", blocks_count);
    write_superblock(fd, blocks_count);
    write_root_inode(fd);
    write_root_dir_block(fd);

    close(fd);
    printf("bkfs filesystem created on %s\n", argv[1]);
    return 0;
}
