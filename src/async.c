#include "async.h"
#include "malloc.h"
#include "mt.h"

async_queue_t async_queues[ASYNC_TYPE_MAX];
extern struct process *ptab[65536];

void init_async() {
  int i;
  for(i = 0; i < ASYNC_TYPE_MAX; i++) {
    async_queues[i] = new_queue(i);
  }
}

async_queue_t new_queue(int type) {
  async_queue_t r = malloc(sizeof(struct async_queue));
  r->next = 0;
  r->cur_id = 0;
  r->queue_type = type;
  r->evt = 0;
  return r;
}

void register_queue_raw(async_queue_t q, void (*flush_queue)()) {
  q->flush = flush_queue;
}

int queue_event_raw(async_queue_t q, unsigned int data) {
  async_queue_t oq;
  while(q) {
    if(!q->evt) break;
    oq = q;
    q = q->next;
  }
  if(!q) {
    oq->next = malloc(sizeof(struct async_queue));
    oq->next->flush = oq->flush;	
    q = oq->next;
  }
  q->evt = malloc(sizeof(struct async_event));
  q->evt->id = q->cur_id;
  q->evt->data = data;
  return q->cur_id++;
}

async_event_t get_next_event_raw(async_queue_t *q) {
  async_event_t r = 0;
  if((*q)->evt) {
    r = (*q)->evt;
    (*q)->evt = 0;
  }
  if((*q)->next) *q = (*q)->next;
  return r;
}

void queue_user_callback(int process, int type, void (*callback)(unsigned int, unsigned int), unsigned int id, unsigned int data) {
  callback_queue_t q, oq;
  oq = q = ptab[process]->cb_queue;
  while(q) {
    if(!q->evt) break;
    oq = q;
    q = q->next;
  }
  if(!q) {
    oq->next = malloc(sizeof(struct callback_queue));
    q = oq->next;
  }
  q->evt = malloc(sizeof(struct async_closure));
  q->evt->type = type;
  q->evt->callback = callback;
  q->evt->id = id;
  q->evt->data = data;
}

async_closure_t get_next_callback(int process) {
  async_closure_t r = ptab[process]->cb_queue->evt;
  if(ptab[process]->cb_queue->next)
    ptab[process]->cb_queue = ptab[process]->cb_queue->next;
  else
    ptab[process]->cb_queue->evt = 0;
  return r;
}
