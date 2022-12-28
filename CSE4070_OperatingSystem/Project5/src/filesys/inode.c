#include "filesys/inode.h"
#include <debug.h>
#include <round.h>
#include <string.h>
#include "threads/malloc.h"
#include "filesys/cache.h"
#include "filesys/filesys.h"
#include "filesys/free-map.h"

static inline size_t
bytes_to_sectors (off_t size)
{
  return DIV_ROUND_UP (size, BLOCK_SECTOR_SIZE);
}

static bool indirect_block_allocate(block_sector_t *indirect_block_sector, struct inode_indirect_block *indirect_block, int index){
    if ((int)*indirect_block_sector == -1){
        return false;
    }

    buffer_cache_read(*indirect_block_sector, 0, indirect_block, 0, sizeof(struct inode_indirect_block));

    *indirect_block_sector = indirect_block->blocks[index];
    return true;
}

/* Returns the block device sector that contains byte offset POS
   within INODE.
   Returns -1 if INODE does not contain data for a byte at offset
   POS. */
static block_sector_t byte_to_sector(const struct inode_disk *inode_disk, off_t pos)
{
    /* without use locate_byte function */
    ASSERT(inode_disk != NULL);
    if (pos < inode_disk->length)
    {
      int index = pos / BLOCK_SECTOR_SIZE;
      if (index < DIRECT_BLOCKS_NUM) {
          return inode_disk->direct_blocks[index];
      }
      else if (index < DIRECT_BLOCKS_NUM + INDIRECT_BLOCKS_NUM){
          struct inode_indirect_block *indirect_block = malloc(BLOCK_SECTOR_SIZE);
          block_sector_t indirect_block_sector = inode_disk->indirect_block;

          if (!indirect_block_allocate(&indirect_block_sector, indirect_block, index - DIRECT_BLOCKS_NUM)){
              free(indirect_block);
              return -1;
          }

          free(indirect_block);
          return indirect_block_sector;
      }
      else if (index < DIRECT_BLOCKS_NUM + INDIRECT_BLOCKS_NUM + INDIRECT_BLOCKS_NUM * INDIRECT_BLOCKS_NUM){
          struct inode_indirect_block *doubly_indirect_block = malloc(BLOCK_SECTOR_SIZE);
          block_sector_t doubly_indirect_block_sector = inode_disk->doubly_indirect_block;

          if (!indirect_block_allocate(&doubly_indirect_block_sector, doubly_indirect_block, (index - DIRECT_BLOCKS_NUM - INDIRECT_BLOCKS_NUM) / INDIRECT_BLOCKS_NUM)){
              free(doubly_indirect_block);
              return -1;
          }

          if (!indirect_block_allocate(&doubly_indirect_block_sector, doubly_indirect_block, (index - DIRECT_BLOCKS_NUM - INDIRECT_BLOCKS_NUM) % INDIRECT_BLOCKS_NUM)){
              free(doubly_indirect_block);
              return -1;
          }

          free(doubly_indirect_block);
          return doubly_indirect_block_sector;
      }
  }

  // ASSERT(inode_disk != NULL);
  return -1;
}

/* List of open inodes, so that opening a single inode twice
   returns the same `struct inode'. */
static struct list open_inodes;
// static char global_buffer[BLOCK_SECTOR_SIZE];

/* Initializes the inode module. */
void inode_init(void)
{
    list_init(&open_inodes);

    // buffer_cache_init();
    // buffer_cache_read(0, global_buffer, 0, BLOCK_SECTOR_SIZE, 0);
}

/* Initializes an inode with LENGTH bytes of data and
   writes the new inode to sector SECTOR on the file system
   device.
   Returns true if successful.
   Returns false if memory or disk allocation fails. */
bool inode_create(block_sector_t sector, off_t length, bool is_dir)
{
    struct inode_disk *disk_inode = NULL;
    bool success = false;
    static char zeros[BLOCK_SECTOR_SIZE];

    ASSERT(length >= 0);

    /* If this assertion fails, the inode structure is not exactly
       one sector in size, and you should fix that. */
    ASSERT(sizeof *disk_inode == BLOCK_SECTOR_SIZE);

    disk_inode = malloc(sizeof *disk_inode);

    if (disk_inode != NULL){
        memset(disk_inode, 0xFF, sizeof *disk_inode);

        disk_inode->magic = INODE_MAGIC;
        disk_inode->is_dir = is_dir;
        disk_inode->length = length;

        for (off_t i = 0; i < length; i += BLOCK_SECTOR_SIZE){
            block_sector_t block_sector = byte_to_sector(disk_inode, i);
            if ((int)block_sector == -1){
                if (!free_map_allocate(1, &block_sector) || !sector_allocate(disk_inode, block_sector, i)){
                    free(disk_inode);

                    if ((int)block_sector != -1){
                        free_map_release(block_sector, 1);
                    }
                    success = false;
                    return success;
                }
                buffer_cache_write(block_sector, 0, zeros, 0, BLOCK_SECTOR_SIZE);
            }
        }

        buffer_cache_write(sector, 0, disk_inode, 0, BLOCK_SECTOR_SIZE);
        free(disk_inode);
        success = true;
    }
    return success;
}

/* Reads an inode from SECTOR
   and returns a `struct inode' that contains it.
   Returns a null pointer if memory allocation fails. */
struct inode *inode_open(block_sector_t sector)
{
    struct list_elem *e;
    struct inode *inode;

    /* Check whether this inode is already open. */
    for (e = list_begin(&open_inodes); e != list_end(&open_inodes);
         e = list_next(e))
    {
        inode = list_entry(e, struct inode, elem);
        if (inode->sector == sector)
        {
            inode_reopen(inode);
            return inode;
        }
    }

    /* Allocate memory. */
    inode = malloc(sizeof *inode);
    if (inode == NULL)
        return NULL;

    /* Initialize. */
    list_push_front(&open_inodes, &inode->elem);
    inode->sector = sector;
    inode->open_cnt = 1;
    inode->deny_write_cnt = 0;
    inode->removed = false;
    return inode;
}

/* Reopens and returns INODE. */
struct inode *
inode_reopen(struct inode *inode)
{
    if (inode != NULL)
        inode->open_cnt++;
    return inode;
}

/* Returns INODE's inode number. */
block_sector_t
inode_get_inumber(const struct inode *inode)
{
    return inode->sector;
}

/* Closes INODE and writes it to disk.
   If this was the last reference to INODE, frees its memory.
   If INODE was also a removed inode, frees its blocks. */
void inode_close(struct inode *inode)
{
    /* Ignore null pointer. */
    if (inode == NULL)
        return;

    /* Release resources if this was the last opener. */
    if (--inode->open_cnt == 0)
    {
        /* Remove from inode list and release lock. */
        list_remove(&inode->elem);

        /* Deallocate blocks if removed. */
        if (inode->removed)
        {
            struct inode_disk inode_disk;
            buffer_cache_read(inode->sector, 0, &inode_disk, 0, BLOCK_SECTOR_SIZE);
            sectors_deallocate(&inode_disk);
            free_map_release(inode->sector, 1);
        }

        free(inode);
    }
}

/* Marks INODE to be deleted when it is closed by the last caller who
   has it open. */
void inode_remove(struct inode *inode)
{
    ASSERT(inode != NULL);
    inode->removed = true;
}

/* Reads SIZE bytes from INODE into BUFFER, starting at position OFFSET.
   Returns the number of bytes actually read, which may be less
   than SIZE if an error occurs or end of file is reached. */
off_t inode_read_at(struct inode *inode, void *buffer_, off_t size, off_t offset)
{
    struct inode_disk inode_disk;
    uint8_t *buffer = buffer_;
    uint8_t *bounce = NULL;
    off_t bytes_read = 0;

    buffer_cache_read(inode->sector, 0, &inode_disk, 0, sizeof(struct inode_disk));

    while (size > 0)
    {
        /* Disk sector to read, starting byte offset within sector. */
        block_sector_t sector_idx = byte_to_sector(&inode_disk, offset);
        int sector_ofs = offset % BLOCK_SECTOR_SIZE;

        /* Bytes left in inode, bytes left in sector, lesser of the two. */
        off_t inode_left = inode_disk.length - offset;
        int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;
        int min_left = inode_left < sector_left ? inode_left : sector_left;

        /* Number of bytes to actually copy out of this sector. */
        int chunk_size = size < min_left ? size : min_left;
        if (chunk_size <= 0)
            break;

        // if (sector_ofs == 0 && chunk_size == BLOCK_SECTOR_SIZE)
        // {
        //     /* Read full sector directly into caller's buffer. */
        //     buffer_cache_read(sector_idx, 0, buffer + bytes_read, 0, BLOCK_SECTOR_SIZE);
        // }
        // else
        // {
        //     /* Read sector into bounce buffer, then partially copy
        //        into caller's buffer. */
        //     bounce = malloc(BLOCK_SECTOR_SIZE);
        //     if (bounce == NULL)
        //         break;
        //     buffer_cache_read(sector_idx, 0, bounce, 0, BLOCK_SECTOR_SIZE);
        //     memcpy(buffer + bytes_read, bounce + sector_ofs, chunk_size);
        // }
        buffer_cache_read(sector_idx, sector_ofs, buffer, bytes_read, chunk_size);

        /* Advance. */
        size -= chunk_size;
        offset += chunk_size;
        bytes_read += chunk_size;
    }
    free(bounce);
    return bytes_read;
}

/* Writes SIZE bytes from BUFFER into INODE, starting at OFFSET.
   Returns the number of bytes actually written, which may be
   less than SIZE if end of file is reached or an error occurs.
   (Normally a write at end of file would extend the inode, but
   growth is not yet implemented.) */
off_t inode_write_at(struct inode *inode, const void *buffer_, off_t size, off_t offset)
{
    struct inode_disk inode_disk;
    const uint8_t *buffer = buffer_;
    // uint8_t *bounce = NULL;
    off_t bytes_written = 0;

    if (inode->deny_write_cnt)
        return 0;

    buffer_cache_read(inode->sector, 0, &inode_disk, 0, sizeof(struct inode_disk));

    if (inode_disk.length < offset + size)
    {
        // write at the end of file
        off_t file_seek = inode_disk.length;
        inode_disk.length = offset + size;
        static char buffer_temp[BLOCK_SECTOR_SIZE];
        // char *buffer_temp = malloc(BLOCK_SECTOR_SIZE);

        if (file_seek % BLOCK_SECTOR_SIZE != 0){
            file_seek = (file_seek / BLOCK_SECTOR_SIZE) * BLOCK_SECTOR_SIZE;
        }

        for (off_t i = file_seek; i < offset + size; i += BLOCK_SECTOR_SIZE){
            block_sector_t sector = byte_to_sector(&inode_disk, i);
            if ((int)sector == -1){
                if (!free_map_allocate(1, &sector) || !sector_allocate(&inode_disk, sector, i)){
                    inode_disk.length = i;
                    break;
                }
                buffer_cache_write(sector, 0, buffer_temp, 0, BLOCK_SECTOR_SIZE);
            }
        }
        // free(buffer_temp);
        buffer_cache_write(inode->sector, 0, &inode_disk, 0, BLOCK_SECTOR_SIZE);
    }

    while (size > 0)
    {
        /* Sector to write, starting byte offset within sector. */
        block_sector_t sector_idx = byte_to_sector(&inode_disk, offset);
        int sector_ofs = offset % BLOCK_SECTOR_SIZE;

        /* Bytes left in inode, bytes left in sector, lesser of the two. */
        off_t inode_left = inode_disk.length - offset;
        int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;
        int min_left = inode_left < sector_left ? inode_left : sector_left;

        /* Number of bytes to actually write into this sector. */
        int chunk_size = size < min_left ? size : min_left;
        if (chunk_size <= 0)
            break;

        if (sector_ofs == 0 && chunk_size == BLOCK_SECTOR_SIZE){
            buffer_cache_write(sector_idx, sector_ofs, (void *)buffer, bytes_written, chunk_size);
        }
        else {
            // bounce = malloc(BLOCK_SECTOR_SIZE);
            static char buffer_temp[BLOCK_SECTOR_SIZE];
            /* If the sector contains data before or after the chunk
               we're writing, then we need to read in the sector
               first.  Otherwise we start with a sector of all zeros. */
            if (sector_ofs > 0 || chunk_size < sector_left) {
                // buffer_cache_read(sector_idx, 0, bounce, 0, BLOCK_SECTOR_SIZE);
                buffer_cache_read(sector_idx, 0, buffer_temp, 0, BLOCK_SECTOR_SIZE);
            }
            else {
                // memset(bounce, 0, BLOCK_SECTOR_SIZE);
                memset(buffer_temp, 0, BLOCK_SECTOR_SIZE);
            }
            // memcpy(bounce + sector_ofs, buffer + bytes_written, chunk_size);
            memcpy(buffer_temp + sector_ofs, buffer + bytes_written, chunk_size);
            // buffer_cache_write(sector_idx, 0, bounce, 0, BLOCK_SECTOR_SIZE);
            buffer_cache_write(sector_idx, 0, buffer_temp, 0, BLOCK_SECTOR_SIZE);
            // free(bounce);
        }
        // buffer_cache_write(sector_idx, sector_ofs, (void *)buffer, bytes_written, chunk_size);

        /* Advance. */
        size -= chunk_size;
        offset += chunk_size;
        bytes_written += chunk_size;
    }

    return bytes_written;
}

/* Disables writes to INODE.
   May be called at most once per inode opener. */
void inode_deny_write(struct inode *inode)
{
    inode->deny_write_cnt++;
    ASSERT(inode->deny_write_cnt <= inode->open_cnt);
}

/* Re-enables writes to INODE.
   Must be called once by each inode opener who has called
   inode_deny_write() on the inode, before closing the inode. */
void inode_allow_write(struct inode *inode)
{
    ASSERT(inode->deny_write_cnt > 0);
    ASSERT(inode->deny_write_cnt <= inode->open_cnt);
    inode->deny_write_cnt--;
}

bool sector_in_buffer_cache(block_sector_t* sector, struct inode_indirect_block *indirect_block)
{
    if ((int)*sector != -1){
        buffer_cache_read(*sector, 0, indirect_block, 0, sizeof(struct inode_indirect_block));
        return true;
    }

    if (free_map_allocate(1, sector)){
        memset(indirect_block, -1, sizeof(struct inode_indirect_block));

        buffer_cache_write(*sector, 0, indirect_block, 0, sizeof(struct inode_indirect_block));
        return true;
    }
    
    return false;
}

bool sector_allocate(struct inode_disk *inode_disk, block_sector_t new_sector, off_t pos)
{
    struct inode_indirect_block first, second;
    block_sector_t *sector;

    pos /= BLOCK_SECTOR_SIZE;

    if (pos < DIRECT_BLOCKS_NUM){
        inode_disk->direct_blocks[pos] = new_sector;
        return true;
    }
    else if (pos < INDIRECT_BLOCKS_NUM + DIRECT_BLOCKS_NUM){
        pos -= DIRECT_BLOCKS_NUM;
        sector = &inode_disk->indirect_block;

        if (!sector_in_buffer_cache(sector, &second)){
            return false;
        }

        if ((int)second.blocks[pos] == -1){
            second.blocks[pos] = new_sector;
        }

        buffer_cache_write(*sector, 0, &second, 0, sizeof(struct inode_indirect_block));
        return true;
    }
    else if (pos < INDIRECT_BLOCKS_NUM * INDIRECT_BLOCKS_NUM + INDIRECT_BLOCKS_NUM + DIRECT_BLOCKS_NUM)
    {
        pos -= INDIRECT_BLOCKS_NUM + DIRECT_BLOCKS_NUM;
        sector = &inode_disk->doubly_indirect_block;

        if (!sector_in_buffer_cache(sector, &first)){
            return false;
        }

        sector = &first.blocks[pos / INDIRECT_BLOCKS_NUM];

        if (!sector_in_buffer_cache(sector, &second)){
            return false;
        }

        if ((int)second.blocks[pos % INDIRECT_BLOCKS_NUM] == -1){
            second.blocks[pos % INDIRECT_BLOCKS_NUM] = new_sector;
        }

        buffer_cache_write(*sector, 0, &second, 0, sizeof(struct inode_indirect_block));        
        buffer_cache_write(inode_disk->doubly_indirect_block, 0, &first, 0, sizeof(struct inode_indirect_block));
        return true;
    }
    return false;
}

void sectors_deallocate(struct inode_disk *disk_inode){
    size_t sectors = bytes_to_sectors(disk_inode->length);
    size_t direct_blocks = sectors < DIRECT_BLOCKS_NUM ? sectors : DIRECT_BLOCKS_NUM;
    size_t indirect_blocks = sectors < DIRECT_BLOCKS_NUM + INDIRECT_BLOCKS_NUM ? sectors - DIRECT_BLOCKS_NUM : INDIRECT_BLOCKS_NUM;
    size_t doubly_indirect_blocks = sectors < DIRECT_BLOCKS_NUM + INDIRECT_BLOCKS_NUM ? 0 : sectors - DIRECT_BLOCKS_NUM - INDIRECT_BLOCKS_NUM;
    
    for (size_t i = 0; i < direct_blocks; i++){
        if ((int)disk_inode->direct_blocks[i] != -1){
            free_map_release(disk_inode->direct_blocks[i], 1);
            disk_inode->direct_blocks[i] = -1;
        }
    }

    if (indirect_blocks > 0){
        if ((int)disk_inode->indirect_block != -1){
            struct inode_indirect_block *indirect_block = malloc(sizeof(struct inode_indirect_block));
            buffer_cache_read(disk_inode->indirect_block, 0, indirect_block, 0, BLOCK_SECTOR_SIZE);
            
            for (size_t i = 0; i < indirect_blocks; i++){
                if ((int)indirect_block->blocks[i] != -1){
                    free_map_release(indirect_block->blocks[i], 1);
                    indirect_block->blocks[i] = -1;
                }
            }

            free(indirect_block);
            free_map_release(disk_inode->indirect_block, 1);
            disk_inode->indirect_block = -1;
        }
    }

    if (doubly_indirect_blocks > 0){
        if ((int)disk_inode->doubly_indirect_block != -1){
            struct inode_indirect_block *doubly_indirect_block = malloc(sizeof(struct inode_indirect_block));
            buffer_cache_read(disk_inode->doubly_indirect_block, 0, doubly_indirect_block, 0, BLOCK_SECTOR_SIZE);

            for (size_t i = 0; i < INDIRECT_BLOCKS_NUM; i++){
                if ((int)doubly_indirect_block->blocks[i] != -1){
                    struct inode_indirect_block *indirect_block = malloc(sizeof(struct inode_indirect_block));
                    buffer_cache_read(doubly_indirect_block->blocks[i], 0, indirect_block, 0, BLOCK_SECTOR_SIZE);
                    for (size_t j = 0; j < INDIRECT_BLOCKS_NUM; j++){
                        if ((int)indirect_block->blocks[j] != -1){
                            free_map_release(indirect_block->blocks[j], 1);
                            indirect_block->blocks[j] = -1;
                        }
                    }
                    free(indirect_block);
                    free_map_release(doubly_indirect_block->blocks[i], 1);
                    doubly_indirect_block->blocks[i] = -1;
                }
            }
            free(doubly_indirect_block);
            free_map_release(disk_inode->doubly_indirect_block, 1);
            disk_inode->doubly_indirect_block = -1;
        }
    }
}

off_t inode_length(struct inode *inode)
{
    struct inode_disk *disk_inode = (struct inode_disk *)malloc(BLOCK_SECTOR_SIZE);
    buffer_cache_read(inode->sector, 0, disk_inode, 0, BLOCK_SECTOR_SIZE);
    off_t length = disk_inode->length;
    free(disk_inode);
    return length;
}


bool inode_is_removed(struct inode *inode)
{
    return inode->removed;
}

bool inode_is_dir(struct inode *inode)
{
    if (inode == NULL){
        return false;
    }
    
    if (inode_is_removed(inode)){
        return false;
    }

    struct inode_disk *disk_inode = (struct inode_disk *)malloc(BLOCK_SECTOR_SIZE);
    buffer_cache_read(inode->sector, 0, disk_inode, 0, BLOCK_SECTOR_SIZE);
    bool is_dir = disk_inode->is_dir;
    free(disk_inode);
    return is_dir;
}
