#include "malloc.h"
#include "vgatext.h"
#include "debug.h"
#include "paging.h"

#ifdef DEBUG_MALLOC
#define printd(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define printd(fmt, ...)
#endif

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

extern page_table_t kernel_pages;
extern unsigned char paging_enabled;

void page_all_allocations() {
  struct malloc_header *ptr = MALLOC_ARENA;
  while(ptr) {
    printd("Paging allocation for %x-%x\n", ptr, ptr + ptr->length);
    id_page(kernel_pages, ((unsigned int) ptr) / 4096);
    unsigned int i;
    for(i = 0; i < (ptr->length / 4096); i++) {
      id_page(kernel_pages, (((unsigned int) ptr) / 4096) + i);
    }
    ptr = ptr->next;
  }
}

#define PTR_ADD(a, b, t) ((t) (((unsigned int) (a)) + ((unsigned int) (b))))
#define PTR_SUB(a, b, t) PTR_ADD((a), -((unsigned int) (b)), t)

void *malloc(unsigned int size) {
  return malloc_a(size, 0);
}

void *align_address(void *addr, int align) {
  struct malloc_header *ptr = (struct malloc_header *) addr - 1;
  ptr->mem = addr;
  if(!align) return addr;
  unsigned int off = (unsigned int) addr;
  int pad = (align - (off % align)) % align;
  ptr->mem = (void *) (off + pad);
  return (void *) (off + pad);
}

void *page_allocation(void *addr, unsigned int size) {
  printd("Allocating %x + %x (%d)\n", addr, size, paging_enabled);
  if(!paging_enabled) return addr;
  printd("Paging new memory for allocation at %x (page %x, dir %x)\n", (unsigned int) addr, (unsigned int) addr / 4096 , ((unsigned int) addr / 4096) / 1024);
  unsigned int i;
  id_page(kernel_pages, ((unsigned int) addr) / 4096);
  for(i = 0; i < (size / 4096) + 2; i++) {
    id_page(kernel_pages, (((unsigned int) addr) / 4096) + i);
  }
  for(i = 0; i < (size / 4); i++) {
    ((int *) addr)[i]= 0;
  }
  return addr;
}

void page_header(struct malloc_header *h) {
  if(!paging_enabled) return;
  id_page(kernel_pages, ((unsigned int) h) / 4096);
}

unsigned int header_diff(struct malloc_header *a) {
  return PTR_SUB(a->next, a, unsigned int);
}

void *malloc_a(unsigned int size, int align) {
  // Simple first fit allocation.
  struct malloc_header *ptr = MALLOC_ARENA;
  printd("Beginning allocation of %d bytes (up to %d).\n", size, size + align);
  // traverse the array looking for free blocks
  while(ptr->next) {
    printd("Checking block at %x + %x\n", ptr, ptr->length);
    if(ptr->type == BLOCK_FREE && ptr->length > size + sizeof(struct malloc_header)) {
      // found a free block
      printd("Found a free, allocated block at offset %x\n",
	     PTR_SUB(ptr, MALLOC_ARENA, unsigned int));
      if(align) {
	printd("\tChecking alignment...\n");
	if(header_diff(ptr) < (unsigned int) align_address(ptr, align) + size) {
	  ptr = ptr->next;
	  continue;
	}
      }
      ptr->type = BLOCK_USED;
      ptr->length = size;
      ptr->owner = (void (*)()) __builtin_return_address(0);
      return page_allocation(align_address(PTR_ADD(ptr, sizeof(struct malloc_header), void *), align), size);
    }
    ptr = ptr->next;
  }
  // if we're here now, there was either no free block, or it was the
  // last block.
  if(ptr->type == BLOCK_FREE && ptr->length > size + sizeof(struct malloc_header)) {
    // found a free block
    printd("Found a free, allocated block at offset %x (last block)\n",
	   PTR_SUB(ptr, MALLOC_ARENA, unsigned int));
    if(align) {
      printd("\tChecking alignment...\n");
      if(header_diff(ptr) < (unsigned int) align_address(ptr, align) + size) goto nofree;
    }
    ptr->type = BLOCK_USED;
    ptr->length = size;
    ptr->owner = (void (*)()) __builtin_return_address(0);
    return page_allocation(align_address(PTR_ADD(ptr, sizeof(struct malloc_header), void *), align), size);
  }
 nofree:
  printd("Found no free blocks. Putting a new one at %x (prev %x + %x)\n", PTR_ADD(ptr, sizeof(struct malloc_header) + ptr->length, struct malloc_header *), ptr, ptr->length);
  // ok, now we definitely have to allocate one
  ptr->next = PTR_ADD(ptr, sizeof(struct malloc_header) + ptr->length, struct malloc_header *);
  page_header(ptr->next);
  ptr->next->magic = MALLOC_MAGIC;
  ptr->next->type = BLOCK_USED;
  if(align)
    ptr->next->length = size + (align - ((unsigned int) ptr % align)) % align;
  else
    ptr->next->length = size;
  ptr->next->owner = (void (*)()) __builtin_return_address(0);
  ptr->next->next = 0;
  printd("New block allocated, size %d loc %x.\n", ptr->next->length, ptr->next);
  return page_allocation(align_address(PTR_ADD(ptr->next, sizeof(struct malloc_header), void *), align), size);
}

void free(void *mem) {
  struct malloc_header *ptr = (struct malloc_header *) mem - 1;
  if(ptr->magic != MALLOC_MAGIC) {
    // check if it's an aligned block
    ptr = MALLOC_ARENA;
    while(ptr && ptr->mem != mem) ptr = ptr->next;
    if(!ptr)
      printf("Something very bad has happened when freeing %x\n", mem);
  }
  ptr->type = BLOCK_FREE;
  ptr->owner = 0;
  printd("Freed %x +%x bytes\n", ptr, ptr->length);
}

