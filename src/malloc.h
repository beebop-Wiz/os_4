#ifndef MALLOC_H
#define MALLOC_H

#define MALLOC_MAGIC 0xa10ca7ed
#define BLOCK_FREE 0xf4ee
#define BLOCK_USED 0x25ed

struct malloc_header {
  unsigned int magic;
  unsigned short type;
  unsigned int length;
  void (*owner)();
  struct malloc_header *next;
};

void *malloc(unsigned int size);
void free(void *mem);

#endif