#include "paging.h"
#include "vgatext.h"
#include "malloc.h"

unsigned int *pagedir;
struct mem_tree phy;


void init_paging() {
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

void mark_block(int gran, int off) {
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
  printf("Finding block, granularity %d (block size %x, iter %x)\n", gran, l, bs);
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
  printf("Found block at %x:%d\n", i, j);
  mark_block(gran, i * 32 + j);
  return 0;
}

void nonid_page(page_table_t pt) {
  while(pt->next) {
    if(!pt->table) pt->table = malloc_a(4096, 4096);
    int i;
    for(i = 0; i < 1024; i++) {
      if(!(pt->table[i] & 0x01)) {
	// TODO
      }
    }
  }
}
