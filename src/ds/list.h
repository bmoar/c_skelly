#ifndef ds_List_h
#define ds_List_h

#include <stdlib.h>

struct ListNode;

typedef struct ListNode {
    struct ListNode *next;
    struct ListNode *prev;
    void *value;
} ListNode;

typedef struct List {
    int count;
    ListNode *first;
    ListNode *last;
} List;

List *List_create();
void List_destroy(List *list);
void List_clear(List *list);
void List_clear_destroy(List *list);

#define List_count(A) ((A)->count)
#define List_first(A) ((A)->first != NULL ? (A)->first->value : NULL)
#define List_last(A) ((A)->last != NULL ? (A)->last->value : NULL)

void List_push(List *list, void *value);
void *List_pop(List *list);

void List_unshift(List *list, void *value);
void *List_shift(List *list);

void *List_remove(List *list, ListNode *node);

int List_cmp(List *a, List *b);
int List_node_cmp(ListNode *a, ListNode *b);

#define LIST_FOREACH(L, S, M, V) ListNode *_node = NULL;\
    ListNode *V = NULL;\
    for(V = _node = L->S; _node != NULL; V = _node = _node->M)

#define List_check(A)\
    if(A){\
    check(List_count(A) >= 0, "Length of list can't be negative");\
    if(List_count(A) > 0) {\
        check(List_first(A), "List_count can't be > 0 and List_first() == NULL");\
        check(List_last(A), "List_count can't be > 0 and List_last() == NULL");}\
    if(List_count(A) == 0) {\
        check(!List_first(A), "List_count can't be == 0 and have List_first() node");\
        check(!List_last(A), "List_count can't be == and have List_last() node");}\
    };

/* 
 * Returns a pointer to a deep copy of list
 * On error, returns NULL
*/

List *List_copy(List *list);

/*
 * Puts list_b on the end of list_a
 * so the list_a->last = list_b->first->prev
*/

void List_join(List *a, List *b);

/*
 * split a list at the given node and return 
 * a pointer to the split second half of the list
*/
List *List_split(List *src, void *value);

#endif
