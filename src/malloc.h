#ifndef MALLOC_H
#define MALLOC_H

#define MALLOC_MAGIC 0xabcdefff
#define BLOCK_FREE 0xf4ee
#define BLOCK_USED 0x25ed

struct malloc_header {
  unsigned int magic;
  unsigned short type;
  unsigned int length;
  void *mem;
  void (*owner)();
  struct malloc_header *next;
};

void init_malloc();
void page_all_allocations();
void *malloc(unsigned int size);
void *malloc_a(unsigned int size, int align);
void free(void *mem);

#endif
