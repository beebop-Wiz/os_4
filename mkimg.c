#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#define BOOTLOADER_ASM "bootloader.nasm"
#define BOOTLOADER_OBJ "../bin/bootloader"
#define OS_BINARY "../bin/os_4.bin"
#define OUTPUT "../os_4.img"

struct elf_header {
  char magic[4];
  int8_t bits;
  int8_t endian;
  int8_t version;
  int8_t abi;
  u_int64_t pad;
  int16_t type;
  int16_t iset;
  int32_t version2;
  u_int32_t entry;
  u_int32_t header_pos;
  u_int32_t section_pos;
  u_int32_t flags;
  u_int16_t hsize;
  u_int16_t ptsize;
  u_int16_t ptidx;
  u_int16_t stsize;
  u_int16_t stidx;
  u_int16_t stnam;
} __attribute__ ((packed));

struct pheader {
  u_int32_t type;
  u_int32_t p_offset;
  u_int32_t p_vaddr;
  u_int32_t res;
  u_int32_t p_filesz;
  u_int32_t p_memsz;
  u_int32_t flags;
  u_int32_t align;
} __attribute__ ((packed));

struct bheader {
  u_int16_t nsectors;
  u_int16_t memaddr_hi;
  u_int16_t memaddr_lo;
  u_int8_t pad[506];
} __attribute__ ((packed));

int main(void) {
  struct stat fs;
  stat(OS_BINARY, &fs);
  int nsectors = (fs.st_size / 512) + 1;
  printf("The OS occupies %d sectors.\n", nsectors);
  if(nsectors > 255) {
    printf("This is a problem. Time to stop putting off that loader restructure!\n");
    exit(1);
  }
  printf("Compiling bootloader...\n");
  system("nasm -f bin -o " BOOTLOADER_OBJ " " BOOTLOADER_ASM);
  printf("Opening files...\n");
  int devfd, loaderfd, osfd;
  char buf[512];
  int i;
  for(i = 0; i < 512; i++) buf[i] = 0;
  devfd = open(OUTPUT, O_RDWR | O_CREAT | O_TRUNC, 0644);
  loaderfd = open(BOOTLOADER_OBJ, O_RDONLY);
  osfd = open(OS_BINARY, O_RDONLY);

  // First, write the bootloader.
  read(loaderfd, buf, 512);
  buf[510] = 0x55;
  buf[511] = 0xaa;
  write(devfd, buf, 512);
  for(i = 0; i < 512; i++) buf[i] = 0;
  printf("Wrote bootloader.\n");

  // Next, parse the OS ELF file.
  struct elf_header osh;
  struct pheader ph;
  struct bheader bh;
  for(i = 0; i < sizeof(bh.pad); i++)
    bh.pad[i] = 0;
  u_int8_t *buf2;
  read(osfd, &osh, sizeof(osh));
  printf("Read OS header, bit %d, endian %d, PHT pos 0x%x, PT size %dx"
	 "%d (%d total), entry %x\n", osh.bits, osh.endian,
	 osh.header_pos, osh.ptsize, osh.ptidx, osh.ptsize * osh.ptidx,
	 osh.entry);

  for(i = 0; i < osh.ptidx; i++) {
    lseek(osfd, osh.header_pos + i * osh.ptsize, SEEK_SET);
    read(osfd, &ph, osh.ptsize);
    printf("\tProgram header %d: type %d filesz %x (%d sectors) vaddr "
	   "%x\n", i, ph.type, ph.p_filesz, (int)
	   ceil((ph.p_filesz + sizeof(bh)) / 512.0), ph.p_vaddr);
    bh.nsectors = (int) ceil((ph.p_filesz + sizeof(bh)) / 512.0) - 1;
    bh.memaddr_lo = ph.p_vaddr & 0xFFFF;
    bh.memaddr_hi = (ph.p_vaddr & 0xF0000) >> 4;
    printf("\t\tnsectors %x lo %x hi %x\n", bh.nsectors,
	   bh.memaddr_lo, bh.memaddr_hi);
    write(devfd, &bh, sizeof(bh));
    buf2 = malloc(ph.p_filesz);
    lseek(osfd, ph.p_offset, SEEK_SET);
    read(osfd, buf2, ph.p_filesz);
    write(devfd, buf2, ph.p_filesz);
    lseek(devfd, 512 - ((ph.p_filesz + sizeof(bh)) % 512), SEEK_CUR);
  }
  bh.nsectors = 0;
  bh.memaddr_lo = osh.entry & 0xFFFF;
  bh.memaddr_hi = (osh.entry & 0xF0000) >> 4;
  write(devfd, &bh, sizeof(bh));
  close(loaderfd);
  close(osfd);
  close(devfd);
}
