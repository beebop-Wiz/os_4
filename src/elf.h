#ifndef ELF_H
#define ELF_H

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


#endif
