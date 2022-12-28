#ifndef VM_SWAP_H
#define VM_SWAP_H

void swap_bitmap_init();
void swap_in(void *virtual_address, void *physical_address, int swap_index);
int32_t swap_out(void *virtual_address, void *physical_address);

#endif
