#include "malloc.h"
#include "vgatext.h"

#define MALLOC_ARENA ((void *) 0x4000000)
#define MALLOC_FIRST_BLOCK ((void *) MALLOC_ARENA + sizeof(struct malloc_header))

void init_malloc() {
  struct malloc_header *ptr = MALLOC_ARENA;
  ptr->magic = MALLOC_MAGIC;
  ptr->type = BLOCK_USED;
  ptr->length = 0;
  ptr->owner = 0;
  ptr->next = 0;
}

void *malloc(unsigned int size) {
  // Simple first fit allocation.
  struct malloc_header *ptr = MALLOC_ARENA;
  vga_puts("Beginning allocation of ");
  vga_itoa(size);
  vga_puts(" bytes.\n");
  // traverse the array looking for free blocks
  while(ptr->next) {
    if(ptr->type == BLOCK_FREE && ptr->length < size + sizeof(struct malloc_header)) {
      // found a free block
      vga_puts("Found a free, allocated block at offset ");
      vga_itoa((unsigned int) ptr - (unsigned int) MALLOC_ARENA);
      vga_puts(".\n");
      ptr->type = BLOCK_USED;
      ptr->length = size;
      ptr->owner = __builtin_return_address(0);
      return (void *) ((void *) ptr) + sizeof(struct malloc_header);
    }
    ptr = ptr->next;
  }
  // if we're here now, there was either no free block, or it was the
  // last block.
  if(ptr->type == BLOCK_FREE && ptr->length < size + sizeof(struct malloc_header)) {
    // found a free block
    vga_puts("Found a free, allocated block at offset ");
    vga_itoa((unsigned int) ptr - (unsigned int) MALLOC_ARENA);
    vga_puts(". (last block)\n");
    ptr->type = BLOCK_USED;
    ptr->length = size;
    ptr->owner = __builtin_return_address(0);
    return (void *) ((void *) ptr) + sizeof(struct malloc_header);
  }
  vga_puts("Found no free blocks.\n");
  // ok, now we definitely have to allocate one
  ptr->next = (struct malloc_header *) ((void *) ptr) + sizeof(struct malloc_header) + ptr->length;
  ptr->next->magic = MALLOC_MAGIC;
  ptr->next->type = BLOCK_USED;
  ptr->next->length = size;
  ptr->next->owner = __builtin_return_address(0);
  vga_puts("New block allocated.\n");
  return (void *) ((void *) ptr->next) + sizeof(struct malloc_header);
}

void free(void *mem) {
  struct malloc_header *ptr = (struct malloc_header *) mem - 1;
  ptr->type = BLOCK_FREE;
  ptr->owner = 0;
  vga_puts("Freed ");
  vga_itoa(ptr->length);
  vga_puts(" bytes.\n");
}

