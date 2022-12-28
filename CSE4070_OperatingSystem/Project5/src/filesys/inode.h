#ifndef FILESYS_INODE_H
#define FILESYS_INODE_H

#include "devices/block.h"
#include "filesys/off_t.h"
#include <list.h>
#include <stdbool.h>
#include "threads/synch.h"

/* Identifies an inode. */
#define INODE_MAGIC 0x494e4f44
#define DIRECT_BLOCKS_NUM 123
#define INDIRECT_BLOCKS_NUM 128

struct inode_indirect_block
{
    block_sector_t blocks[INDIRECT_BLOCKS_NUM];
};

/* On-disk inode.
   Must be exactly BLOCK_SECTOR_SIZE bytes long. */
struct inode_disk
{
    block_sector_t direct_blocks[DIRECT_BLOCKS_NUM];
    block_sector_t indirect_block;
    block_sector_t doubly_indirect_block;
    off_t length;   /* File size in bytes. */
    unsigned magic; /* Magic number. */
    bool is_dir;
};

/* In-memory inode. */
struct inode
{
    struct list_elem elem; /* Element in inode list. */
    block_sector_t sector; /* Sector number of disk location. */
    int open_cnt;          /* Number of openers. */
    bool removed;          /* True if deleted, false otherwise. */
    int deny_write_cnt;    /* 0: writes ok, >0: deny writes. */
    struct inode_disk data;/* Inode content. */
};

void inode_init(void);
bool inode_create(block_sector_t, off_t, bool);
struct inode *inode_open(block_sector_t);
struct inode *inode_reopen(struct inode *);
block_sector_t inode_get_inumber(const struct inode *);
void inode_close(struct inode *);
void inode_remove(struct inode *);
off_t inode_read_at(struct inode *, void *, off_t, off_t);
off_t inode_write_at(struct inode *, const void *, off_t, off_t);
void inode_deny_write(struct inode *);
void inode_allow_write(struct inode *);
off_t inode_length(struct inode *);
bool inode_is_removed(struct inode *);
bool inode_is_dir(struct inode *);

bool sector_in_buffer_cache(block_sector_t*, struct inode_indirect_block *);
bool sector_allocate(struct inode_disk *, block_sector_t, off_t);
void sectors_deallocate(struct inode_disk *);

#endif /* filesys/inode.h */
