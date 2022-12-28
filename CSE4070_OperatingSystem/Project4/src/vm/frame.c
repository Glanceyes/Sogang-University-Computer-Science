#include <stdio.h>
#include <stdlib.h>
#include "threads/thread.h"
#include "threads/palloc.h"
#include "threads/vaddr.h"
#include "vm/page.h"
#include "vm/frame.h"
#include "vm/swap.h"


static struct list_elem *lru_element;

void frame_table_init()
{
	list_init(&frame_table);
	lock_init(&frame_table_lock);
	lock_init(&frame_evict_lock);
	lru_element = NULL;
}

struct list_elem* next_lru_element()
{
	if (list_empty(&frame_table)){
		return NULL;
	}
	
	if (lru_element == NULL || lru_element == list_end(&frame_table)){
		return list_begin(&frame_table);
	}
	
	if (list_next(lru_element) == list_end(&frame_table)){
		return list_begin(&frame_table);
	}
	else {
		return list_next(lru_element);
	}
	return lru_element;
}

void frame_table_insert(struct frame_entry *frame){
	lock_acquire(&frame_table_lock);
	list_push_back(&frame_table, &frame->elem);
	lock_release(&frame_table_lock);
}

struct frame_entry* frame_allocate(void *virtual_address, void *physical_address, bool writable)
{
	struct frame_entry *frame = (struct frame_entry*)malloc(sizeof(struct frame_entry));
	struct thread* current_thread = thread_current();

	if (physical_address != NULL) {
		frame->virtual_address = pg_round_down(virtual_address);
		frame->physical_address = pg_round_down(physical_address);
		frame->owner_thread = current_thread;
		frame_table_insert(frame);
		return frame;
	}

	physical_address = palloc_get_page(PAL_USER);

	if (physical_address == NULL){
		if (!frame_evict()){
			free(frame);
			return NULL;
		}	
		physical_address = palloc_get_page(PAL_USER);
	}

	frame->owner_thread = current_thread;
	frame->physical_address = pg_round_down(physical_address);
	frame->virtual_address = pg_round_down(virtual_address);

	frame_table_insert(frame);
	return frame;
}

struct frame_entry* frame_lookup(void *physical_address)
{
	struct list_elem *element;
	struct frame_entry *frame;

	for (element = list_begin(&frame_table); element != list_end(&frame_table); element = list_next(element))
	{
		frame = list_entry(element, struct frame_entry, elem);
		if (frame->physical_address == physical_address)
		{
			return frame;
		}
	}
	return NULL;
}

bool frame_delete(struct frame_entry * frame)
{
	struct page_entry *page = page_lookup_with_table(frame->owner_thread, frame->virtual_address);
	if (page != NULL){
		page_free(page, NULL);
	}
	pagedir_clear_page(frame->owner_thread->pagedir, frame->virtual_address);

	list_remove(&frame->elem);
}

bool frame_evict(){
	bool success = false;

	/* Find a victim frame. */
	struct list_elem *element = list_begin(&frame_table);
	struct frame_entry *victim_frame = list_entry(element, struct frame_entry, elem);
	// struct frame_entry *victim_frame = select_victim_frame();

	if (victim_frame != NULL){
		struct thread *victim_thread = victim_frame->owner_thread;
		struct thread *victim_page_table = &victim_thread->page_table;
		struct page_entry *victim_page = page_lookup_with_table(victim_page_table, victim_frame->virtual_address);

		if (victim_page != NULL){
			victim_page->swap_index = swap_out(victim_frame->virtual_address, victim_frame->physical_address);
			victim_page->physical_address = NULL;

			if (lru_element == &victim_frame->elem){
				lru_element = list_remove(&victim_frame->elem);
				if (lru_element == list_end(&frame_table)){
					lru_element = list_begin(&frame_table);
				}
			}
			else {
				list_remove(&victim_frame->elem);
			}
			
			pagedir_clear_page(victim_thread->pagedir, victim_frame->virtual_address);

			/* Free the page corresponding to the victim frame. */
			palloc_free_page(victim_frame->physical_address);

			free(victim_frame);

			success = true;
		}
	}

	return success;
}

void frame_free(void *physical_address)
{
	lock_acquire(&frame_table_lock);
	struct list_elem *element = list_begin(&frame_table);
	struct frame_entry *frame = list_entry(element, struct frame_entry, elem);

	while (frame->physical_address != physical_address){
		element = list_next(element);
		frame = list_entry(element, struct frame_entry, elem);
	}
	
	if (lru_element == &frame->elem){
		lru_element = list_remove(&frame->elem);
		if (lru_element == list_end(&frame_table)){
			lru_element = list_begin(&frame_table);
		}
	}
	else {
		list_remove(&frame->elem);
	}

	pagedir_clear_page(frame->owner_thread->pagedir, frame->virtual_address);
	palloc_free_page(physical_address);
	free(frame);

	lock_release(&frame_table_lock);
}

struct frame_entry* select_victim_frame()
{
	lru_element = next_lru_element();
	struct thread *owner_thread = list_entry(lru_element, struct frame_entry, elem)->owner_thread;
	struct frame_entry *victim_frame;

	lock_acquire(&frame_table_lock);

	while (pagedir_is_accessed(owner_thread->pagedir, list_entry(lru_element, struct frame_entry, elem)->virtual_address)){
		pagedir_set_accessed(owner_thread->pagedir, list_entry(lru_element, struct frame_entry, elem)->virtual_address, false);
		lru_element = next_lru_element();
		owner_thread = list_entry(lru_element, struct frame_entry, elem)->owner_thread;
	}

	victim_frame = list_entry(lru_element, struct frame_entry, elem);

	lock_release(&frame_table_lock);

	return victim_frame;
}

