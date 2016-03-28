#ifndef ASYNC_H
#define ASYNC_H

union callback_data {
  unsigned int i;
  char *buf;
};

typedef union callback_data callback_data_t;

struct queued_callback {
  int reqid;
  void (*fn)(int reqid, callback_data_t data);
};

typedef struct queued_callback *queued_callback_t;

struct callback_list {
  struct callback_list *next;
  queued_callback_t callback;
};

#include "idt.h"
void fulfill_event(struct registers *r, int etype, callback_data_t data);

#define ASYNC_TYPE_DUMMY 0
#define ASYNC_TYPE_MAX 1

void init_async();

#endif
