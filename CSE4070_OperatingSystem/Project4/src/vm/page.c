#include <stdio.h>
#include <stdlib.h>
#include "vm/page.h"
#include "vm/frame.h"
#include "vm/swap.h"
#include "threads/thread.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"

bool page_compare_less(const struct hash_elem *p1, const struct hash_elem *p2, void *aux)
{
	struct page_entry* page_entry1 = hash_entry(p1, struct page_entry, elem);
	struct page_entry* page_entry2 = hash_entry(p2, struct page_entry, elem);

	return page_entry1->virtual_address < page_entry2->virtual_address;
}

unsigned page_hash(const struct hash_elem *p, void *aux)
{
	struct page_entry* page_entry = hash_entry(p, struct page_entry, elem);
	// size_t virtual_address_size = sizeof(page_entry->virtual_address);
	return hash_int((int)page_entry->virtual_address);
}

bool page_insert(void *virtual_address, void *physical_address, bool writable)
{
	struct page_entry *page = (struct page_entry*)malloc(sizeof(struct page_entry));
	void *rounded_vaddr = pg_round_down(virtual_address);
	void *rounded_paddr = pg_round_down(physical_address);

	page->virtual_address = rounded_vaddr;
	page->physical_address = rounded_paddr;
	
	page->loaded = true;
	page->writable = writable;
	page->swap_index = -1;

	struct hash *page_table = &(thread_current()->page_table);
	struct hash_elem *element = hash_find(page_table, &page->elem);

	if (element != NULL)
	{
		struct page_entry* ptr = page;
		page = hash_entry(element, struct page_entry, elem);
		page->swap_index = -1;
		free(ptr);
	}

	hash_insert(page_table, &page->elem);
	return true;
}

void page_free(struct hash_elem* e, void* aux)
{
	struct page_entry* page = hash_entry(e, struct page_entry, elem);
	free(page);
}

struct page_entry* page_lookup(const void *virtual_address)
{
	struct hash *page_table = &(thread_current()->page_table);
	struct page_entry page;
	page.virtual_address = pg_round_down(virtual_address);
	struct hash_elem *element = hash_find(page_table, &page.elem);

	if (element == NULL)
		return NULL;

	return hash_entry(element, struct page_entry, elem);
}

struct page_entry* page_lookup_with_table(struct hash *page_table, void *vaddr)
{
	struct page_entry page;
	page.virtual_address = pg_round_down(vaddr);
	struct hash_elem *element = hash_find(page_table, &page.elem);
	
	if(element == NULL){
		return NULL;
	}
	
	return hash_entry(element, struct page_entry, elem);
}


void page_table_init(struct hash *page_table)
{
	hash_init(page_table, page_hash, page_compare_less, NULL);
}


void page_table_destroy(struct hash *page_table)
{
	hash_destroy(page_table, page_free);
}

void page_pin_buffer(void *buffer, size_t size)
{
	void *vaddr = pg_round_down(buffer);
	void *vaddr_end = pg_round_down(buffer + size - 1);

	while (vaddr <= vaddr_end)
	{
		struct page_entry *page = page_lookup(vaddr);
		if (page != NULL)
			page->pinned = true;
		vaddr += PGSIZE;
	}
}

void page_unpin_buffer(void *buffer, size_t size)
{
	void *vaddr = pg_round_down(buffer);
	void *vaddr_end = pg_round_down(buffer + size - 1);

	while (vaddr <= vaddr_end)
	{
		struct page_entry *page = page_lookup(vaddr);
		if (page != NULL)
			page->pinned = false;
		vaddr += PGSIZE;
	}
}