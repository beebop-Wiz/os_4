#include "paging.h"
#include "vgatext.h"
#include "malloc.h"
#include "debug.h"

#ifdef DEBUG_PAGING
#define printd(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define printd(fmt, ...) dummy_print(fmt, ##__VA_ARGS__)
#endif

unsigned int *pagedir;
struct mem_tree phy;


void init_paging() {
  pagedir = malloc_a(0x4000, 4096);
  phy.bitmap = malloc(18 * sizeof(unsigned int *));
  phy.bitmap[0] = malloc(131072 * 4); // 4k
  phy.bitmap[1] = malloc(65536 * 4);  // 8k
  phy.bitmap[2] = malloc(32768 * 4);  // 16k
  phy.bitmap[3] = malloc(16384 * 4);  // 32k
  phy.bitmap[4] = malloc(8192 * 4);   // 64k
  phy.bitmap[5] = malloc(4096 * 4);   // 128k
  phy.bitmap[6] = malloc(2048 * 4);   // 256k
  phy.bitmap[7] = malloc(1024 * 4);   // 512k
  phy.bitmap[8] = malloc(512 * 4);    // 1m
  phy.bitmap[9] = malloc(256 * 4);    // 2m
  phy.bitmap[10] = malloc(128 * 4);   // 4m
  phy.bitmap[11] = malloc(64 * 4);    // 8m
  phy.bitmap[12] = malloc(32 * 4);    // 16m
  phy.bitmap[13] = malloc(16 * 4);    // 32m
  phy.bitmap[14] = malloc(8 * 4);     // 64m
  phy.bitmap[15] = malloc(4 * 4);     // 128m
  phy.bitmap[16] = malloc(2 * 4);     // 256m
  phy.bitmap[17] = malloc(4);         // 512m
  int i, j;
  for(i = 0; i < 18; i++)
    for(j = 0; j < (131072 >> i); j++)
      phy.bitmap[i][j] = 0;
}

void load_page_directory() {
  asm("mov %0, %%cr3" : : "r" (pagedir));
  int cr0;
  asm("mov %%cr0, %0" : "=r" (cr0));
  cr0 |= 0x80000000;
  asm("mov %0, %%cr0" : : "r" (cr0));
}

void mark_block(int gran, int off) {
  //  printd("Marked block %d:%x (%x/%d)\n", gran, off, off / 32, off % 32);
  phy.bitmap[gran][off / 32] |= 1 << (off % 32);
  if(gran) {
    mark_block(gran - 1, off * 2);
    mark_block(gran - 1, off * 2 + 1);
  }
  for(gran++; gran < 18; gran++) {
    off /= 2;
    phy.bitmap[gran][off / 32] |= 1 << (off % 32);
  }
}

unsigned int get_page_block(int gran) {
  int l = (1 << gran) * 4096;
  int bs = (131072 >> gran);
  printd("Finding block, granularity %d (block size %x, iter %x)\n", gran, l, bs);
  int i, j;
  for(i = 0; i < bs; i++) {
    if(phy.bitmap[gran][i] == 0xFFFFFFFF) {
      continue;
    }
    for(j = 0; j < 32; j++) {
      if(!(phy.bitmap[gran][i] & (1 << j))) goto found;
    }
  }
 found:
  printd("Found block at %x:%d\n", i, j);
  mark_block(gran, i * 32 + j);
  return (i * 32 + j) * (1 << gran);
}

void register_page_table(page_table_t pt) {
  pagedir[pt->idx] = (unsigned int) pt->table | 0x7;
}

void nonid_page(page_table_t pt, unsigned int offset) {
  while(pt) {
    if(!pt->table) {
      pt->table = malloc_a(4 * 4096, 4096);
      pt->idx = offset / 1024;
      register_page_table(pt);
    }
    if(pt->idx == offset / 1024) {
      pt->table[offset % 1024] = (get_page_block(0) * 4096) | 0x7;
      break;
    }
    if(!pt->next) {
      pt->next = malloc(sizeof(struct page_table));
      pt->next->table = malloc_a(4 * 4096, 4096);
      pt->next->idx = offset / 1024;
      register_page_table(pt->next);
      pt->next->table[offset % 1024] = (get_page_block(0) * 4096) | 0x7;
      return;
    }
    pt = pt->next;
  }
}

void id_page(page_table_t pt, unsigned int offset) {
  printd("Identity paging page %x (vaddr %x PDI %x)\n", offset, offset * 4096, offset / 1024);
  while(pt) {
    if(!pt->table) {
      pt->table = malloc_a(4 * 4096, 4096);
      pt->idx = offset / 1024;
      register_page_table(pt);
    }
    if(pt->idx == offset / 1024) {
      pt->table[offset % 1024] = (offset * 4096) | 0x7;
      mark_block(0, offset);
      return;
    }
    if(!pt->next) {
      pt->next = malloc(sizeof(struct page_table));
      pt->next->table = malloc_a(4 * 4096, 4096);
      pt->next->idx = offset / 1024;
      register_page_table(pt->next);
      pt->next->table[offset % 1024] = (offset * 4096) | 0x7;
      mark_block(0, offset);
      return;
    }
    pt = pt->next;
  }
}
