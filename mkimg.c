#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define BOOTLOADER_ASM "src/bootloader.nasm"
#define BOOTLOADER_OBJ "bin/bootloader"
#define OS_BINARY "bin/os_4.bin"
#define OUTPUT "os_4.img"

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
  write(devfd, buf, 512);
  for(i = 0; i < 512; i++) buf[i] = 0;
  printf("Wrote bootloader.\n");
  
  // Next, write the OS information (currently just sector count)
  buf[0] = nsectors;
  write(devfd, buf, 512);
  for(i = 0; i < 512; i++) buf[i] = 0;
  printf("Wrote OS info\n");
  
  // Next, copy over the OS proper. This will be loaded on top of the
  // OS info in RAM.
  for(i = 0; i < nsectors; i++) {
    read(osfd, buf, 512);
    write(devfd, buf, 512);
  }
  printf("Wrote OS\n");
  
  close(loaderfd);
  close(osfd);
  close(devfd);
}
