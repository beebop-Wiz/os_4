#ifndef PAGING_H
#define PAGING_H

struct page_table {
  struct page_table *next;
  unsigned int idx;
  unsigned int *table;
};

struct mem_tree {
  unsigned int **bitmap;
};

typedef struct page_table * page_table_t;

void init_paging();
unsigned int get_page_block(int gran);
void swap_page_table(page_table_t old, page_table_t new);
unsigned int nonid_page(page_table_t pt, unsigned int offset);
void id_page(page_table_t pt, unsigned int offset);
void mapped_page(page_table_t pt, unsigned int virt, unsigned int phy);
void load_page_directory();

#endif
