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

#define PTR_ADD(a, b, t) ((t) (((unsigned int) (a)) + ((unsigned int) (b))))
#define PTR_SUB(a, b, t) PTR_ADD((a), -((unsigned int) (b)), t)

void *malloc(unsigned int size) {
  return malloc_a(size, 0);
}

void *align_address(void *addr, int align) {
  if(!align) return addr;
  unsigned int off = (unsigned int) addr;
  int pad = (align - (off % align)) % align;
  return (void *) (off + pad);
}

unsigned int header_diff(struct malloc_header *a) {
  return PTR_SUB(a->next, a, unsigned int);
}

void *malloc_a(unsigned int size, int align) {
  // Simple first fit allocation.
  struct malloc_header *ptr = MALLOC_ARENA;
  printf("Beginning allocation of %d bytes (up to %d).\n", size, size + align);
  // traverse the array looking for free blocks
  while(ptr->next) {
    if(ptr->type == BLOCK_FREE && ptr->length < size + sizeof(struct malloc_header)) {
      // found a free block
      printf("Found a free, allocated block at offset %x\n",
	     PTR_SUB(ptr, MALLOC_ARENA, unsigned int));
      if(align) {
	printf("\tChecking alignment...\n");
	if(header_diff(ptr) < (unsigned int) align_address(ptr, align) + size) continue;
      }
      ptr->type = BLOCK_USED;
      ptr->length = size;
      ptr->owner = (void (*)()) __builtin_return_address(0);
      return align_address(PTR_ADD(ptr, sizeof(struct malloc_header), void *), align);
    }
    ptr = ptr->next;
  }
  // if we're here now, there was either no free block, or it was the
  // last block.
  if(ptr->type == BLOCK_FREE && ptr->length < size + sizeof(struct malloc_header)) {
    // found a free block
    printf("Found a free, allocated block at offset %x (last block)\n",
	   PTR_SUB(ptr, MALLOC_ARENA, unsigned int));
    if(align) {
      printf("\tChecking alignment...\n");
      if(header_diff(ptr) < (unsigned int) align_address(ptr, align) + size) goto nofree;
    }
    ptr->type = BLOCK_USED;
    ptr->length = size;
    ptr->owner = (void (*)()) __builtin_return_address(0);
    return align_address(PTR_ADD(ptr, sizeof(struct malloc_header), void *), align);
  }
 nofree:
  printf("Found no free blocks.\n");
  // ok, now we definitely have to allocate one
  ptr->next = PTR_ADD(ptr, sizeof(struct malloc_header) + ptr->length, struct malloc_header *);
  ptr->next->magic = MALLOC_MAGIC;
  ptr->next->type = BLOCK_USED;
  if(align)
    ptr->next->length = size + (align - ((unsigned int) ptr % align)) % align;
  else
    ptr->next->length = size;
  ptr->next->owner = (void (*)()) __builtin_return_address(0);
  printf("New block allocated, size %d.\n", ptr->next->length);
  return align_address(PTR_ADD(ptr->next, sizeof(struct malloc_header), void *), align);
}

void free(void *mem) {
  struct malloc_header *ptr = (struct malloc_header *) mem - 1;
  ptr->type = BLOCK_FREE;
  ptr->owner = 0;
  printf("Freed %d bytes\n", ptr->length);
}

