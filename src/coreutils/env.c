#include <stdio.h>
#include <unistd.h>

int main(void) {
  char *ep, **temp_environ = environ;
  for(ep = *temp_environ; ep; ep = *++temp_environ) {
    printf("%s\n", ep);
  }
  return 0;
}
