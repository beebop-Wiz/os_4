#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

unsigned int *malloc_arena = 0;
unsigned int *endptr = 0;

#define MAGIC 0xA10C2534

struct malloc_header {
  unsigned int magic;
  size_t size;
  char used : 1;
};

void *malloc(size_t size) {
  if(!malloc_arena) {
    malloc_arena = endptr = sbrk(sizeof(struct malloc_header));
  }
  struct malloc_header *h = (struct malloc_header *) malloc_arena;
  while(1) {
    if(h->magic != MAGIC) goto new_allocation;
    if(!h->used && h->size >= size) return (void *) (h + 1);
    h = ((void *) h) + h->size + sizeof(struct malloc_header);
  }
 new_allocation:
  h->magic = MAGIC;
  h->used = 1;
  h->size = size;
  if(((unsigned int *) h) + size + 2 * sizeof(struct malloc_header) >= endptr) {
    brk(((unsigned int *) h) + size + 2 * sizeof(struct malloc_header));
    endptr = ((unsigned int *) h) + size + 2 * sizeof(struct malloc_header);
  }

  return (void *) (h + 1);
}

void free(void *ptr) {
  struct malloc_header *h = ((struct malloc_header *) ptr) - 1;
  h->used = 0;
}
