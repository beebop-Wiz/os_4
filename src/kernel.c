#include "vga.h"
#include "vgatext.h"
#include "version.h"
#include "idt.h"
#include "vbe.h"
#include "malloc.h"
#include "paging.h"
#include "gdt.h"
#include "user.h"
#include "timer.h"
#include "mt.h"
#include "ext2.h"
#include "util.h"

page_table_t kernel_pages = 0;

// for init loading
struct elf_header {
  char magic[4];
  char bits;
  char endian;
  char version;
  char abi;
  unsigned int pad[2];
  short type;
  short iset;
  int version2;
  unsigned int entry;
  unsigned int header_pos;
  unsigned int section_pos;
  unsigned int flags;
  unsigned short hsize;
  unsigned short ptsize;
  unsigned short ptidx;
  unsigned short stsize;
  unsigned short stidx;
  unsigned short stnam;
} __attribute__ ((packed));

struct pheader {
  unsigned int type;
  unsigned int p_offset;
  unsigned int p_vaddr;
  unsigned int res;
  unsigned int p_filesz;
  unsigned int p_memsz;
  unsigned int flags;
  unsigned int align;
} __attribute__ ((packed));

void kernel_main(unsigned int **bdata) {
  vbe_load_data();
  init_vga();
  init_vgatext();
  printf("Hello, World!\n");
  init_malloc();
  setup_idt();
  PIC_init();
  init_paging();
  kernel_pages = malloc(sizeof(struct page_table));
  unsigned int i;
  for(i = 0; i < 0x2000; i++) {
    id_page(kernel_pages, i);
  }
  for(i = 0xFD000; i < 0xFFFFF; i++) {
    id_page(kernel_pages, i);
  }
  // now to set up the TSS for multitasking
  load_bios_gdt(bdata[1]);
  page_all_allocations();
  load_page_directory();
  timer_init(1);
  printf("Booted os_4 %s\n", VERSION);
  init_mt();
  //  new_process((unsigned int) test_mt);
  //  new_process((unsigned int) test_mt_2);
  //  printf("Enabling MT\n");
  //  enable_mt();
  struct ext2_superblock s;
  struct ext2_bg_desc desc;
  struct ext2_inode rd, sub;
  read_superblock(&s);
  printf("Read EXT2 superblock\n");
  printf("\tversion %d.%d magic %x n_blocks %d\n\tblock size %d (%d sectors)\n", s.vers_major, s.vers_minor, s.magic, s.n_blocks, s.block_size, s.block_size / 2);
  printf("Attempting to read BGD 0\n");
  read_block_group(&s, &desc, 0);
  printf("\titab idx %d\n", desc.inode_table);
  printf("Attempting to read inode 2\n");
  read_inode(&s, &rd, 2);
  char out[11];
  parse_inode_type(rd.type, out);
  printf("\ttype %s atime %d n_sectors %d\n", out, rd.atime, rd.n_sectors);
  printf("\n\nListing:\n");
  ext2_dirstate_t root = opendir(&rd);
  ext2_dirent_t d;
  while((d = dirwalk(root))) {
    if(d->nlen) {
      read_inode(&s, &sub, d->inode);
      parse_inode_type(sub.type, out);
      printf("%s %s %d\n", out, d->name, sub.size_low);
      if(streq(d->name, "init.exe")) break;
    }
  }
  closedir(root);
  int init_pid = new_process(0);
  printf("\nInit size: %d\n", sub.size_low);
  unsigned char *init_buf = malloc(sub.n_sectors * 512);
  for(i = 0; i < sub.size_low; i += 1024) {
    get_block(&sub, i / 1024, init_buf + i);
  }
  struct elf_header init_header;
  struct pheader ph;
  memcpy(&init_header, init_buf, sizeof(struct elf_header));
  printf("Read init ELF header\n");
  for(i = 0; i < init_header.ptidx; i++) {
    memcpy(&ph, init_buf + init_header.header_pos + i * init_header.ptsize, init_header.ptsize);
    printf("\tProgram header %d: type %d memsz %x vaddr %x\n", i, ph.type, ph.p_memsz, ph.p_vaddr);
    unsigned int j;
    for(j = 0; j < ph.p_memsz; j += 4096) {
      nonid_page(get_process_pt(init_pid), (ph.p_vaddr + j) / 4096, 1);
      printf("Paging in page %x (idx %x)\n", (j + ph.p_vaddr) / 4096, (j + ph.p_vaddr) / 4096 / 1024);
    }
    memcpy((void *) ph.p_vaddr, init_buf + ph.p_offset, ph.p_filesz);
  }
  set_process_entry(init_pid, init_header.entry);
  enable_mt();
  for(;;) {
    asm("hlt");
    asm("cli");
  }
}
