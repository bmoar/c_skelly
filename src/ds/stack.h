#include <ds/list.h>

typedef struct Stack {
    int count;
    ListNode *first;
    ListNode *last;
} Stack;

#define STACK_FOREACH(S, V) \
    ListNode *V = NULL;\
    ListNode *_node = NULL;\
    for(V = _node = S->last; _node != NULL; V = _node = _node->prev)

#define Stack_create() ((Stack *)List_create())

#define Stack_destroy(A) (List_destroy((List *)A))

#define Stack_push(A, D) (List_push((List *)A, D))

#define Stack_pop(A) (List_pop((List *)A))

#define Stack_peek(A) (List_last(A))

#define Stack_count(A) (List_count(A))
