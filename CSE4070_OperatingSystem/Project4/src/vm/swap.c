#include <stdio.h>
#include <stdlib.h>
#include <bitmap.h>
#include "devices/block.h"
#include "threads/palloc.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "vm/page.h"
#include "vm/frame.h"
#include "vm/swap.h"

const int SECTORS_PER_PAGE = PGSIZE / BLOCK_SECTOR_SIZE;

struct block *swap_block;
struct bitmap *swap_bitmap;

void swap_bitmap_init()
{
	swap_block = block_get_role(BLOCK_SWAP);
	
	if (swap_block == NULL){
		return;
	}

	int block_count = block_size(swap_block) / SECTORS_PER_PAGE;

	swap_bitmap = bitmap_create(block_count);

	if (swap_bitmap == NULL){
		return;
	}

	bitmap_set_all(swap_bitmap, false);
}

void swap_in(void *virtual_address, void *physical_address, int swap_index)
{
	for (int i = 0; i < SECTORS_PER_PAGE; i++){
		block_read(swap_block, swap_index * SECTORS_PER_PAGE + i, physical_address + i * BLOCK_SECTOR_SIZE);
	}

	bitmap_set(swap_bitmap, swap_index, false);
	
	struct hash *page_table = &(thread_current()->page_table);
	struct page_entry *page = page_lookup_with_table(page_table, virtual_address);

	page->swap_index = -1;
	page->loaded = true;

	pagedir_set_page(thread_current()->pagedir, virtual_address, physical_address, page->writable);

	return;
}

int32_t swap_out(void *virtual_address, void *physical_address)
{
	int swap_index = -1;
	int block_count = block_size(swap_block) / SECTORS_PER_PAGE;
	for (int i = 0; i < block_count; i++){
		if (bitmap_test(swap_bitmap, i) == false){
			swap_index = i;
			break;
		}
	}

	if (swap_index == -1){
		return -1;
	}

	for (int i = 0; i < SECTORS_PER_PAGE; i++){
		block_write(swap_block, swap_index * SECTORS_PER_PAGE + i, physical_address + i * BLOCK_SECTOR_SIZE);
	}

	bitmap_set(swap_bitmap, swap_index, true);

	return swap_index;
}