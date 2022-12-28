#ifndef VM_FRAME_H
#define VM_FRAME_H
#include <list.h>
#include "vm/page.h"

struct list frame_table;
struct lock frame_evict_lock;
struct lock frame_table_lock;

struct frame_entry
{
	void *virtual_address;
	void *physical_address;
	struct list_elem elem;
	struct thread *owner_thread;
	int unused_count;
};

void frame_table_init();

bool frame_evict();
bool frame_delete(struct frame_entry* frame);
struct frame_entry* frame_lookup(void *paddr);
struct frame_entry *frame_allocate(void *virtual_address, void *physical_address, bool writable);
struct frame_entry* select_victim_frame();

#endif

