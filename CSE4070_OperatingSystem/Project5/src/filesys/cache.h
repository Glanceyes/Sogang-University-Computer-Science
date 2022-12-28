#ifndef FILESYS_CACHE_H
#define FILESYS_CACHE_H

#include "filesys/filesys.h"
#include "filesys/inode.h"
#include "threads/synch.h"

#define NUM_CACHE 64

struct buffer_cache_entry{
    uint8_t buffer[BLOCK_SECTOR_SIZE];
    bool valid_bit;
    bool dirty_bit;
    bool reference_bit;
    block_sector_t disk_sector;
    struct lock lock;
    int access_time;
};

static struct buffer_cache_entry cache[NUM_CACHE];
static struct buffer_cache_entry *clock;
static struct lock buffer_cache_lock;

void buffer_cache_init(void);
void buffer_cache_terminate(void);
bool buffer_cache_read(block_sector_t, off_t, void *, off_t, int);
bool buffer_cache_write(block_sector_t, off_t, void *, off_t, int);
void buffer_cache_flush_entry(struct buffer_cache_entry *);
void buffer_cache_flush_all(void);
struct buffer_cache_entry *buffer_cache_lookup(block_sector_t);
struct buffer_cache_entry *buffer_cache_select_victim_by_clock(void);
struct buffer_cache_entry *buffer_cache_select_victim_by_lru(void);

#endif