#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
  int fd = open("home/beebop/test.txt", O_RDONLY);
  printf("Opened fd %d\n", fd);
  return 0;
}
