#ifndef ASYNC_H
#define ASYNC_H

struct async_event {
  unsigned int id;
  unsigned int data;
};

struct async_queue {
  struct async_queue *next;
  unsigned int cur_id;
  void (*flush)();
  int queue_type;
  struct async_event *evt;
};

struct async_closure {
  int type;
  void (*callback)(unsigned int, unsigned int);
  unsigned int id;
  unsigned int data;
};

struct callback_queue {
  struct callback_queue *next;
  struct async_closure *evt;
};

typedef struct async_queue *async_queue_t;
typedef struct async_event *async_event_t;
typedef struct async_closure *async_closure_t;
typedef struct callback_queue *callback_queue_t;

enum async_types {
  ASYNC_TYPE_DUMMY,
  ASYNC_TYPE_IO,
  ASYNC_TYPE_MAX
};

#ifndef ASYNC_C
extern async_queue_t async_queues[ASYNC_TYPE_MAX];
#endif

void init_async();
async_queue_t new_queue(int type);
void register_queue_raw(async_queue_t q, void (*flush_queue)());
#define register_queue(t, f) (register_queue_raw(async_queues[t], (f)))
int queue_event_raw(async_queue_t q, unsigned int data); // Adds a new event to the queue with a generated ID.
#define queue_event(t, d) (queue_event_raw(async_queues[t], (d)))
async_event_t get_next_event_raw(async_queue_t *q); // Returns either the first-added event from q or 0 if no such event exists.
#define get_next_event(t) (get_next_event_raw(&async_queues[t]))
#define flush_queue(t) (async_queues[t]->flush())

void queue_user_callback(int process, int type, void (*callback)(unsigned int, unsigned int), unsigned int id, unsigned int data);
async_closure_t get_next_callback(int process);

void call_usermode(unsigned int stack, void (*func)(unsigned int, unsigned int), unsigned int id, unsigned int data);

#endif
