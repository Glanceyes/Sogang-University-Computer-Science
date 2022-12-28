#include "filesys/cache.h"
#include <debug.h>
#include <string.h>
#include "devices/block.h"
#include "devices/timer.h"

void buffer_cache_init(void){
    lock_init(&buffer_cache_lock);
    for (int i = 0; i < NUM_CACHE; i++){
        cache[i].valid_bit = false;
        cache[i].dirty_bit = false;
        cache[i].reference_bit = false;
        cache[i].disk_sector = 0;
        cache[i].access_time = 0;
        lock_init(&cache[i].lock);
    }
    clock = &cache[0];
}

void buffer_cache_terminate(void){
    clock = NULL;
    buffer_cache_flush_all();
}

bool buffer_cache_read(block_sector_t disk_sector, off_t disk_sector_offset, void *buffer, off_t offset, int chunk_size){
    struct buffer_cache_entry *cache_entry = buffer_cache_lookup(disk_sector);
    
    if (cache_entry == NULL){
        lock_acquire(&buffer_cache_lock);
        // cache_entry = buffer_cache_select_victim_by_clock();
        cache_entry = buffer_cache_select_victim_by_lru();

        if (cache_entry == NULL){
            lock_release(&buffer_cache_lock);
            return false;
        }

        lock_acquire(&cache_entry->lock);

        if (cache_entry->dirty_bit){
            buffer_cache_flush_entry(cache_entry);
        }

        cache_entry->disk_sector = disk_sector;
        cache_entry->valid_bit = true;
        cache_entry->dirty_bit = false;
        cache_entry->reference_bit = true;
        /* Update access time to present tick */
        cache_entry->access_time = timer_ticks();

        block_read(fs_device, cache_entry->disk_sector, cache_entry->buffer);

        memcpy(buffer + offset, cache_entry->buffer + disk_sector_offset, chunk_size);

        lock_release(&cache_entry->lock);
        lock_release(&buffer_cache_lock);
    }
    else {
        lock_acquire(&cache_entry->lock);
        cache_entry->reference_bit = true;
        /* Update access time to present */
        cache_entry->access_time = timer_ticks();

        memcpy(buffer + offset, cache_entry->buffer + disk_sector_offset, chunk_size);
        lock_release(&cache_entry->lock);
    }
    return true;
}

bool buffer_cache_write(block_sector_t disk_sector, off_t disk_sector_offset, void *buffer, off_t offset, int chunk_size){
    struct buffer_cache_entry *cache_entry = buffer_cache_lookup(disk_sector);

    if (chunk_size > BLOCK_SECTOR_SIZE){
        return false;
    }
    
    if (cache_entry == NULL){
        lock_acquire(&buffer_cache_lock);
        // cache_entry = buffer_cache_select_victim_by_clock();
        cache_entry = buffer_cache_select_victim_by_lru();

        if (cache_entry == NULL){
            lock_release(&buffer_cache_lock);
            return false;
        }

        lock_acquire(&cache_entry->lock);

        if (cache_entry->dirty_bit){
            buffer_cache_flush_entry(cache_entry);
        }

        block_read(fs_device, disk_sector, cache_entry->buffer);
        memcpy(cache_entry->buffer + disk_sector_offset, buffer + offset, chunk_size);

        cache_entry->disk_sector = disk_sector;
        cache_entry->valid_bit = true;
        cache_entry->dirty_bit = true;
        cache_entry->reference_bit = true;
        /* Update access time to present */
        cache_entry->access_time = timer_ticks();

        lock_release(&cache_entry->lock);
        lock_release(&buffer_cache_lock);
    }
    else {
        lock_acquire(&cache_entry->lock);
        cache_entry->reference_bit = true;
        cache_entry->dirty_bit = true;
        /* Update access time to present */
        cache_entry->access_time = timer_ticks();
        memcpy(cache_entry->buffer + disk_sector_offset, buffer + offset, chunk_size);
        lock_release(&cache_entry->lock);
    }
    return true;
}

struct buffer_cache_entry *buffer_cache_lookup(block_sector_t disk_sector){
    lock_acquire(&buffer_cache_lock);
    struct buffer_cache_entry *entry;

    for (int i = 0; i < NUM_CACHE; i++){
        entry = &cache[i];
        if (entry->disk_sector == disk_sector && entry->valid_bit){
            lock_acquire(&entry->lock);
            entry->reference_bit = true;
            /* Update access time to present */
            // entry->access_time = timer_ticks();
            lock_release(&entry->lock);
            lock_release(&buffer_cache_lock);
            return entry;
        }
    }

    lock_release(&buffer_cache_lock);
    return NULL;
}

struct buffer_cache_entry *buffer_cache_select_victim_by_lru(){
    struct buffer_cache_entry *victim = NULL;
    int64_t min_access_time = INT64_MAX;

    for (int i = 0; i < NUM_CACHE; i++){
        lock_acquire(&cache[i].lock);
        if (cache[i].access_time < min_access_time){
            victim = &cache[i];
            min_access_time = cache[i].access_time;
        }
        lock_release(&cache[i].lock);
    }
    return victim;
}

struct buffer_cache_entry *buffer_cache_select_victim_by_clock(){
    struct buffer_cache_entry *victim = NULL;
    while (victim == NULL){
        /* Check if clock is end of the cache. */
        if (clock == &cache[NUM_CACHE]){
            clock = &cache[0];
        }
        lock_acquire(&clock->lock);

        if (clock->valid_bit && clock->reference_bit){
            clock->reference_bit = false;
        }
        else {
            victim = clock;
        }

        lock_release(&clock->lock);
        clock++;
    }

    if (victim->dirty_bit){
        buffer_cache_flush_entry(victim);
    }

    return victim;
}

void buffer_cache_flush_entry(struct buffer_cache_entry *cache_entry){
    if (!cache_entry->valid_bit || !cache_entry->dirty_bit){
        return;
    }

    block_write(fs_device, cache_entry->disk_sector, cache_entry->buffer);
    memset(cache_entry->buffer, 0, BLOCK_SECTOR_SIZE);
    
    cache_entry->valid_bit = false;
    cache_entry->dirty_bit = false;
    cache_entry->access_time = 0;
}

void buffer_cache_flush_all(void){
    lock_acquire(&buffer_cache_lock);
    for (int i = 0; i < NUM_CACHE; i++){
        lock_acquire(&cache[i].lock);
        buffer_cache_flush_entry(&cache[i]);
        lock_release(&cache[i].lock);
    }
    lock_release(&buffer_cache_lock);
}