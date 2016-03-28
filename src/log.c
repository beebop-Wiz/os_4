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
    printf("[%d] %s %s ", (int) jiffy_clock, cstr[channel], lstr[level]);
    return 1;
  }
  return 0;
}
