#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/wait.h>

#define COMMAND_LEN 1024

#define FLAG_VIDEO 0x01
#define FLAG_DEBUG 0x02

int main(int argc, char **argv) {
  if(argc < 2) {
    fprintf(stderr, "Usage: %s <test> [options]\n"
	    "Options:\n"
	    "\t-v: Start QEMU with video enabled.\n"
	    "\t-d: Start QEMU in debug mode (implies -v).\n"
	    , argv[0]);
    return 1;
  }
  int flags = 0, opt;
  while((opt = getopt(argc, argv, "vd")) != -1) {
    switch(opt) {
    case 'd':
      flags |= FLAG_DEBUG;
      // fall through
    case 'v':
      flags |= FLAG_VIDEO;
      break;
    }
  }
  char template[16];
  memcpy(template, "/tmp/fos_XXXXXX", 16);
  int tf = mkstemp(template);
  char *command = malloc(COMMAND_LEN);
  memset(command, 0, COMMAND_LEN);
  sprintf(command, "qemu-system-i386 -kernel tests/bin/%s.bin -hda disk.img -serial stdio -no-reboot %s %s 1>&%d", argv[optind], (flags & FLAG_DEBUG) ? "-s -S" : "", (flags & FLAG_VIDEO) ? "" : "-display none", tf);
  system(command);
  close(tf);
  FILE *f = fopen(template, "r");
  int retval;
  fscanf(f, " %d", &retval);
  printf("%d\n", retval);
  fclose(f);
  if(!retval)
    remove(template);
  free(command);
  return retval;
}
