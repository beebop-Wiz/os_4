#include "async.h"
#include "malloc.h"
#include "user.h"

struct callback_list *queues[ASYNC_TYPE_MAX];

void init_async() {
  int i;
  for(i = 0; i < ASYNC_TYPE_MAX; i++) {
    queues[i] = malloc(sizeof(struct callback_list));
  }
}

queued_callback_t pop_queue(int etype) {
  if(!queues[etype]) return 0;
  queued_callback_t r = queues[etype]->callback;
  if(queues[etype]->next) {
    queues[etype] = queues[etype]->next;
  } else {
    free(queues[etype]);
    queues[etype] = 0;
  }
  return r;
}

void fulfill_event(struct registers *r, int etype, callback_data_t data) {
  queued_callback_t c = pop_queue(etype);
  if(!c) return;
  r->eip = (int) c->fn;
  call_usermode(r, c->reqid, data.i);
}
