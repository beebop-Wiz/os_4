#include "log.h"
#include "debug.h"
#include "vgatext.h"

extern volatile unsigned long long jiffy_clock;

const char *cstr[] = {
  "GENERAL", "MALLOC", "PAGING", "PGMEM", "MT", "SYSCALL", "BOOT"
};

const char *lstr[] = {
  "CRITICAL", "FAILURE", "INFO", "DEBUG"
};

int levels[] = LOGLEVELS;

int _log(int channel, int level) {
  if(levels[channel] >= level && LOG_MAX >= level) {
    printf("[%d] <%x> %s %s ", (int) jiffy_clock, __builtin_return_address(0), cstr[channel], lstr[level]);
    return 1;
  }
  return 0;
}
