#include <lcthw/list.h>
#include <debug.h>

List *List_create() {
    return calloc(1, sizeof(List));
}

void List_destroy(List *list) {
    List_check(list);

    LIST_FOREACH(list, first, next, cur) {
        if(cur->prev) {
            free(cur->prev);
        }
    }

    free(list->last);
    free(list);

error:
    return;
}

void List_clear(List *list) {
    List_check(list);

    LIST_FOREACH(list, first, next, cur) {
        if(cur->value) {
            free(cur->value);
        }
    }

error:
    return;
}

void List_clear_destroy(List *list) {
    List_check(list);

    LIST_FOREACH(list, first, next, cur) {
        //free(cur->value);
        if(cur->value) {
            free(cur->value);
        }
        if(cur->prev) {
            free(cur->prev);
        }
    }

    free(list->last);
    free(list);

error:
    return;
}

int List_cmp(List *a, List *b) {
    List_check(a);
    List_check(b);

    int equal = 0;
    int rc, i;

    if(a == NULL && b == NULL) {
        equal = 1;
    } else if(a->count == 0 && b->count == 0) {
        equal = 1;
    } else if(a->count == b->count){
        equal = 1;
        ListNode *a_cur = List_first(a);
        ListNode *b_cur = List_first(b);
        for (i = 0; i < a->count - 1; i++) {
            if(List_node_cmp(a_cur, b_cur)) {
                a_cur = a_cur->next;
                b_cur = b_cur->next;
            } else {
                equal = 0;
                break;
            }
        }
    }

    return equal;

error:
    return -1;
}

int List_node_cmp(ListNode *a, ListNode *b) {
    int equal = 0;

    if(a == NULL && b == NULL) {
        equal = 1;
    }

    if(a->next == b->next &&
       a->prev == b->prev &&
       a->value == b->value) {
        equal = 1;
    }

    return equal;

error:
    return -1;
}



void List_push(List *list, void *value) {
    List_check(list);
    ListNode *node = calloc(1, sizeof(ListNode));
    check_mem(node);

    node->value = value;

    if(list->last == NULL) {
        list->first = node;
        list->last = node;
    } else {
        list->last->next = node;
        node->prev = list->last;
        list->last = node;
    }
    
    list->count++;

error:
    return;
}

void *List_pop(List *list) {
    List_check(list);

    ListNode *node = list->last;
    return node != NULL ? List_remove(list, node) : NULL;

error:
    return NULL;
}

void List_unshift(List *list, void *value) {
    List_check(list);
    ListNode *node = calloc(1, sizeof(ListNode));
    check_mem(node);

    node->value = value;

    if(list->first == NULL) {
        list->first = node;
        list->last = node;
    } else {
        node->next = list->first;
        list->first->prev = node;
        list->first = node;
    }

    list->count++;

error:
    return;
}

void *List_shift(List *list) {
    List_check(list);
    ListNode *node = list->first;
    return node != NULL ? List_remove(list, node) : NULL;

error:
    return NULL;
}

void *List_remove(List *list, ListNode *node) {
    List_check(list);
    void *result = NULL;

    check(node, "node can't be NULL");

    if(node == list->first && node == list->last) {
        // This node is the only element in the list
        list->first = NULL;
        list->last = NULL;
    } else if(node == list->first) {
        list->first = node->next;
        check(list->first != NULL, "Somehow got a NULL as the first node");
        list->first->prev = NULL;
    } else if(node == list->last) {
        list->last = node->prev;
        check(list->last != NULL, "Somehow got a NULL as the last node");
        list->last->next = NULL;
    } else {
        // node falls somewhere in the middle of the list
        ListNode *after = node->next;
        ListNode *before = node->prev;
        after->prev = before;
        before->next = after;
    }

    list->count--;
    result = node->value;
    free(node);

error:
    return result;
}

List *List_copy(List *list) {
    List_check(list);

    List *dest = List_create();
    check_mem(dest);

    LIST_FOREACH(list, first, next, cur) {
        List_push(dest, cur->value);
    }

    return dest;

error:
    List_clear_destroy(dest);
    return NULL;
}

void List_join(List *a, List *b) {
    List_check(a);
    List_check(b);

    LIST_FOREACH(b, first, next, cur) {
        List_push(a, cur->value);
    }

error:
    return;
}

List *List_split(List *list, ListNode *node) {
    List_check(list);
    int split = 0;

    LIST_FOREACH(list, first, next, cur) {
        // add comparison function for Lists and Nodes
    }


error:
    return NULL;

}
