#include <ds/list.h>

typedef struct Queue {
    int count;
    ListNode *first;
    ListNode *last;
} Queue;

#define QUEUE_FOREACH(Q, V) \
    ListNode *V = NULL;\
    ListNode *_node = NULL;\
    for(V = _node = Q->last; _node != NULL; V = _node = _node->prev)

#define Queue_create() ((Queue *)List_create())

#define Queue_destroy(Q) (List_destroy((List *)Q))

#define Queue_send(Q, D) (List_push((List *)Q, D))

#define Queue_recv(Q) (List_shift((List *)Q))

#define Queue_peek(Q) (List_first(Q))

#define Queue_count(Q) (List_count(Q))
