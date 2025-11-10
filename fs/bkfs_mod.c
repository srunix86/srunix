#include <linux/fs.h>
#include <linux/types.h>
#include <linux/blkdev.h>
#include <linux/buffer_head.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/statfs.h>
#include <linux/mpage.h>
#include <linux/namei.h>
#include <linux/sched.h>
#include <linux/parser.h>
#include <linux/random.h>
#include <linux/version.h>
#include <linux/cred.h>
#include <linux/backing-dev.h>

#define BKFS_MAGIC 0xBACAF5
#define BKFS_SUPER_BLOCK 0
#define BKFS_ROOT_INO 2
#define BKFS_BLOCK_SIZE 4096
#define BKFS_FILENAME_LEN 255
#define BKFS_MAX_EXTENTS 12
#define BKFS_INODES_PER_BLOCK (BKFS_BLOCK_SIZE / sizeof(struct bkfs_inode))
#define BKFS_DEFAULT_MODE 0755

struct bkfs_sb_info {
    uint32_t free_blocks_count;
    uint32_t free_inodes_count;
    uint32_t blocks_count;
    uint32_t inodes_count;
    uint32_t inode_blocks;
};

struct bkfs_inode_info {
    uint32_t i_flags;
    struct inode vfs_inode;
};

struct bkfs_super_block {
    __le32 s_magic;
    __le32 s_block_size;
    __le32 s_blocks_count;
    __le32 s_free_blocks_count;
    __le32 s_free_inodes_count;
    __le32 s_inodes_count;
    __le32 s_first_data_block;
    __le32 s_first_ino;
    __le32 s_inode_blocks;
    __le64 s_mtime;
    __le32 s_state;
    __le32 s_errors;
    __le32 s_creator_os;
    __le32 s_rev_level;
    __le16 s_def_resuid;
    __le16 s_def_resgid;
    __le32 s_reserved[236];
};

struct bkfs_inode {
    __le16 i_mode;
    __le16 i_uid;
    __le16 i_gid;
    __le32 i_size;
    __le32 i_atime;
    __le32 i_ctime;
    __le32 i_mtime;
    __le32 i_dtime;
    __le16 i_links_count;
    __le32 i_blocks;
    __le32 i_flags;
    __le32 i_block[BKFS_MAX_EXTENTS];
    __le32 i_generation;
    __le32 i_reserved;
};

struct bkfs_dir_entry {
    __le32 inode;
    __le16 rec_len;
    __le16 name_len;
    char name[BKFS_FILENAME_LEN];
};

static struct kmem_cache *bkfs_inode_cachep;

static inline struct bkfs_inode_info *BKFS_I(struct inode *inode)
{
    return container_of(inode, struct bkfs_inode_info, vfs_inode);
}

static struct inode *bkfs_alloc_inode(struct super_block *sb)
{
    struct bkfs_inode_info *bi;
    bi = kmem_cache_alloc(bkfs_inode_cachep, GFP_KERNEL);
    if (!bi)
        return NULL;
    return &bi->vfs_inode;
}

static void bkfs_destroy_inode(struct inode *inode)
{
    struct bkfs_inode_info *bi = BKFS_I(inode);
    kmem_cache_free(bkfs_inode_cachep, bi);
}

static void bkfs_evict_inode(struct inode *inode)
{
    truncate_inode_pages_final(&inode->i_data);
    clear_inode(inode);
}

static int bkfs_write_inode(struct inode *inode, struct writeback_control *wbc)
{
    return 0;
}

static void bkfs_put_super(struct super_block *sb)
{
    struct bkfs_sb_info *sbi = sb->s_fs_info;
    sb->s_fs_info = NULL;
    kfree(sbi);
}

static int bkfs_statfs(struct dentry *dentry, struct kstatfs *buf)
{
    struct super_block *sb = dentry->d_sb;
    struct bkfs_sb_info *sbi = sb->s_fs_info;

    buf->f_type = BKFS_MAGIC;
    buf->f_bsize = BKFS_BLOCK_SIZE;
    buf->f_blocks = sbi->blocks_count;
    buf->f_bfree = sbi->free_blocks_count;
    buf->f_bavail = sbi->free_blocks_count;
    buf->f_files = sbi->inodes_count;
    buf->f_ffree = sbi->free_inodes_count;
    buf->f_namelen = BKFS_FILENAME_LEN;
    return 0;
}

static int bkfs_sync_fs(struct super_block *sb, int wait)
{
    struct buffer_head *bh;
    struct bkfs_super_block *disk_sb;
    struct bkfs_sb_info *sbi = sb->s_fs_info;

    bh = sb_bread(sb, BKFS_SUPER_BLOCK);
    if (!bh)
        return -EIO;

    disk_sb = (struct bkfs_super_block *)bh->b_data;
    disk_sb->s_free_blocks_count = cpu_to_le32(sbi->free_blocks_count);
    disk_sb->s_free_inodes_count = cpu_to_le32(sbi->free_inodes_count);
    disk_sb->s_mtime = cpu_to_le64(ktime_get_real_seconds());
    mark_buffer_dirty(bh);
    if (wait)
        sync_dirty_buffer(bh);
    brelse(bh);
    return 0;
}

static struct super_operations bkfs_sops = {
    .alloc_inode = bkfs_alloc_inode,
    .destroy_inode = bkfs_destroy_inode,
    .write_inode = bkfs_write_inode,
    .evict_inode = bkfs_evict_inode,
    .put_super = bkfs_put_super,
    .sync_fs = bkfs_sync_fs,
    .statfs = bkfs_statfs,
};

static struct inode *bkfs_iget(struct super_block *sb, unsigned long ino)
{
    struct inode *inode;
    struct bkfs_inode *disk_inode;
    struct buffer_head *bh;
    uint32_t inode_block, inode_offset;

    inode = iget_locked(sb, ino);
    if (!inode)
        return ERR_PTR(-ENOMEM);
    if (!(inode->i_state & I_NEW))
        return inode;

    inode_block = 1 + (ino / BKFS_INODES_PER_BLOCK);
    inode_offset = ino % BKFS_INODES_PER_BLOCK;

    bh = sb_bread(sb, inode_block);
    if (!bh) {
        iget_failed(inode);
        return ERR_PTR(-EIO);
    }

    disk_inode = (struct bkfs_inode *)bh->b_data;
    disk_inode += inode_offset;

    inode->i_mode = le16_to_cpu(disk_inode->i_mode);
    inode->i_uid = le16_to_cpu(disk_inode->i_uid);
    inode->i_gid = le16_to_cpu(disk_inode->i_gid);
    inode->i_size = le32_to_cpu(disk_inode->i_size);
    inode->i_atime.tv_sec = le32_to_cpu(disk_inode->i_atime);
    inode->i_ctime.tv_sec = le32_to_cpu(disk_inode->i_ctime);
    inode->i_mtime.tv_sec = le32_to_cpu(disk_inode->i_mtime);
    set_nlink(inode, le16_to_cpu(disk_inode->i_links_count));
    inode->i_blocks = le32_to_cpu(disk_inode->i_blocks);

    if (S_ISREG(inode->i_mode)) {
        inode->i_op = &simple_dir_inode_operations;
        inode->i_fop = &simple_dir_operations;
    } else if (S_ISDIR(inode->i_mode)) {
        inode->i_op = &simple_dir_inode_operations;
        inode->i_fop = &simple_dir_operations;
        inode->i_size = BKFS_BLOCK_SIZE;
    }

    brelse(bh);
    unlock_new_inode(inode);
    return inode;
}

static int bkfs_fill_super(struct super_block *sb, void *data, int silent)
{
    struct bkfs_sb_info *sbi;
    struct bkfs_super_block *disk_sb;
    struct buffer_head *bh;
    struct inode *root;
    int ret = -EINVAL;

    sbi = kzalloc(sizeof(struct bkfs_sb_info), GFP_KERNEL);
    if (!sbi)
        return -ENOMEM;
    sb->s_fs_info = sbi;

    bh = sb_bread(sb, BKFS_SUPER_BLOCK);
    if (!bh)
        goto out;

    disk_sb = (struct bkfs_super_block *)bh->b_data;
    if (le32_to_cpu(disk_sb->s_magic) != BKFS_MAGIC) {
        if (!silent)
            printk(KERN_ERR "Wrong magic number\n");
        goto out_bh;
    }

    sbi->free_blocks_count = le32_to_cpu(disk_sb->s_free_blocks_count);
    sbi->free_inodes_count = le32_to_cpu(disk_sb->s_free_inodes_count);
    sbi->blocks_count = le32_to_cpu(disk_sb->s_blocks_count);
    sbi->inodes_count = le32_to_cpu(disk_sb->s_inodes_count);
    sbi->inode_blocks = le32_to_cpu(disk_sb->s_inode_blocks);

    sb->s_magic = BKFS_MAGIC;
    sb->s_op = &bkfs_sops;
    sb->s_maxbytes = BKFS_MAX_EXTENTS * BKFS_BLOCK_SIZE;

    root = bkfs_iget(sb, BKFS_ROOT_INO);
    if (IS_ERR(root)) {
        ret = PTR_ERR(root);
        goto out_bh;
    }

    sb->s_root = d_make_root(root);
    if (!sb->s_root) {
        ret = -ENOMEM;
        goto out_bh;
    }

    brelse(bh);
    return 0;

out_bh:
    brelse(bh);
out:
    kfree(sbi);
    return ret;
}

static struct dentry *bkfs_mount(struct file_system_type *fs_type,
        int flags, const char *dev_name, void *data)
{
    return mount_bdev(fs_type, flags, dev_name, data, bkfs_fill_super);
}

static void bkfs_kill_sb(struct super_block *sb)
{
    kill_block_super(sb);
    kfree(sb->s_fs_info);
}

static struct file_system_type bkfs_fs_type = {
    .owner = THIS_MODULE,
    .name = "bkfs",
    .mount = bkfs_mount,
    .kill_sb = bkfs_kill_sb,
    .fs_flags = FS_REQUIRES_DEV,
};
MODULE_ALIAS_FS("bkfs");

static int __init init_bkfs_fs(void)
{
    int err;

    bkfs_inode_cachep = kmem_cache_create("bkfs_inode_cache",
            sizeof(struct bkfs_inode_info),
            0, (SLAB_RECLAIM_ACCOUNT|SLAB_MEM_SPREAD),
            NULL);
    if (!bkfs_inode_cachep)
        return -ENOMEM;

    err = register_filesystem(&bkfs_fs_type);
    if (err) {
        kmem_cache_destroy(bkfs_inode_cachep);
        return err;
    }
    return 0;
}

static void __exit exit_bkfs_fs(void)
{
    unregister_filesystem(&bkfs_fs_type);
    kmem_cache_destroy(bkfs_inode_cachep);
}

module_init(init_bkfs_fs)
module_exit(exit_bkfs_fs)

MODULE_LICENSE("BSD");
MODULE_AUTHOR("BKFS Developer");
MODULE_DESCRIPTION("A Simple Robust Filesystem");
