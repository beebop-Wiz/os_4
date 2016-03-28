#ifndef LOG_H
#define LOG_H

#define LOG_CRITICAL 0
#define LOG_FAILURE 1
#define LOG_INFO 2
#define LOG_DEBUG 3

#define LOG_GENERAL 0
#define LOG_MALLOC 1
#define LOG_PAGING 2
#define LOG_PAGING_MEMORY 3
#define LOG_MT 4
#define LOG_SYSCALL 5
#define LOG_BOOT 6

#define LOGLEVELS {LOG_FAILURE, LOG_FAILURE, LOG_FAILURE, LOG_FAILURE, LOG_FAILURE, LOG_INFO, LOG_INFO}

#define LOG_MAX LOG_INFO

#define log(c, l, f, ...) do{_log(c,l)?printf(f,##__VA_ARGS__):0;}while(0);

int _log(int channel, int level);

#endif
