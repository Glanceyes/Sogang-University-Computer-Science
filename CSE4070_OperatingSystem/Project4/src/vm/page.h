#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <hash.h>

struct page_entry{
	struct hash_elem elem;

	void *virtual_address;
	void *physical_address;

	int swap_index;

	bool writable;
	bool loaded;
	bool pinned;
};

bool page_compare_less(const struct hash_elem *p1, const struct hash_elem *p2, void *aux);
unsigned page_hash(const struct hash_elem *p, void *aux);

void page_table_init(struct hash *page_table);
bool page_insert(void *vaddr, void *paddr, bool writable);

struct page_entry* page_lookup(const void *vaddr);
struct page_entry *page_lookup_with_table(struct hash *page_table, void *vaddr);

void page_table_destroy(struct hash *page_table);

void page_pin_buffer(void *buffer, size_t size);
void page_unpin_buffer(void *buffer, size_t size);

#endif
