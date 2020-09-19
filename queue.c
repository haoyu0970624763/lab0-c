#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "harness.h"
#include "queue.h"

/*
 * Create empty queue.
 * Return NULL if could not allocate space.
 */
queue_t *q_new()
{
    queue_t *q = malloc(sizeof(queue_t));
    if (q) {
        q->head = NULL;
        q->tail = NULL;
        q->size = 0;
    }
    return q;
}

/* Free all storage used by queue */
void q_free(queue_t *q)
{
    while (q->head) {
        list_ele_t *tmp = q->head;
        q->head = q->head->next;
        free(tmp->value);
        free(tmp);
    }
    free(q);
}

/*
 * Attempt to insert element at head of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_head(queue_t *q, char *s)
{
    if (!q)
        return false;

    list_ele_t *newHead;  // newh means new element in head
    newHead = malloc(sizeof(list_ele_t));
    if (!newHead) {
        return false;
    }

    // Allocate space and copy the string
    newHead->value = malloc(sizeof(char) * (strlen(s) + 1));
    // check malloc
    if (!newHead->value) {
        free(newHead);
        return false;
    }
    memset(newHead->value, '\0', strlen(s) + 1);
    strncpy(newHead->value, s, strlen(s));

    newHead->next = q->head;
    q->head = newHead;

    if (!q->tail) {
        q->tail = q->head;
    }

    q->size += 1;
    return true;
}

/*
 * Attempt to insert element at tail of queue.
 * Return true if successful.
 * Return false if q is NULL or could not allocate space.
 * Argument s points to the string to be stored.
 * The function must explicitly allocate space and copy the string into it.
 */
bool q_insert_tail(queue_t *q, char *s)
{
    if (!q)
        return false;

    list_ele_t *tail;  // newt means new element in tail
    tail = malloc(sizeof(list_ele_t));
    if (!tail)
        return false;

    tail->value = malloc(sizeof(char) * strlen(s) + 1);
    if (!tail->value) {
        free(tail);
        return false;
    }
    memset(tail->value, '\0', strlen(s) + 1);
    strncpy(tail->value, s, strlen(s));
    tail->next = NULL;

    if (!q->tail) {
        q->tail = q->head = tail;
    } else {
        q->tail->next = tail;
        q->tail = tail;
    }

    q->size += 1;
    return true;
}

/*
 * Attempt to remove element from head of queue.
 * Return true if successful.
 * Return false if queue is NULL or empty.
 * If sp is non-NULL and an element is removed, copy the removed string to *sp
 * (up to a maximum of bufsize-1 characters, plus a null terminator.)
 * The space used by the list element and the string should be freed.
 */
bool q_remove_head(queue_t *q, char *sp, size_t bufsize)
{
    if (!q || !q->head) {
        return false;
    }
    if (sp) {
        // Insure copy size is right
        size_t realbufsize = (bufsize > strlen(q->head->value))
                                 ? strlen(q->head->value)
                                 : bufsize - 1;
        memset(sp, '\0', realbufsize + 1);
        strncpy(sp, q->head->value, realbufsize);
    }

    list_ele_t *tmp;

    tmp = q->head;
    q->head = q->head->next;

    free(tmp->value);
    free(tmp);

    q->size -= 1;
    return true;
}


/*
 * Return number of elements in queue.
 * Return 0 if q is NULL or empty
 */
int q_size(queue_t *q)
{
    return !q ? 0 : q->size;
}

/*
 * Reverse elements in queue
 * No effect if q is NULL or empty
 * This function should not allocate or free any list elements
 * (e.g., by calling q_insert_head, q_insert_tail, or q_remove_head).
 * It should rearrange the existing ones.
 */
void q_reverse(queue_t *q)
{
    if (!q || !q->head)
        return;

    list_ele_t *tmp = q->head;
    list_ele_t *prev = NULL;

    while (q->head != NULL) {
        list_ele_t *next = q->head->next;
        q->head->next = prev;

        prev = q->head;
        q->head = next;
    }

    q->head = q->tail;
    q->tail = tmp;
}


/*
 * Sort elements of queue in ascending order
 * No effect if q is NULL or empty. In addition, if q has only one
 * element, do nothing.
 */
list_ele_t *merge(list_ele_t *l1, list_ele_t *l2)
{
    if (!l2)
        return l1;
    if (!l1)
        return l2;

    list_ele_t *tmp, *head;
    if (!(strcmp(l1->value, l2->value) >= 0)) {
        tmp = l1;
        l1 = l1->next;
    } else {
        tmp = l2;
        l2 = l2->next;
    }
    head = tmp;

    while (l1 && l2) {
        if (!(strcmp(l1->value, l2->value) >= 0)) {  // compare 2 lists
            tmp->next = l1;
            l1 = l1->next;
        } else {
            tmp->next = l2;
            l2 = l2->next;
        }
        tmp = tmp->next;
    }

    if (!l1)
        tmp->next = l2;
    if (!l2)
        tmp->next = l1;

    return head;
}

list_ele_t *merge_sort(list_ele_t *head)
{
    if (!head || !head->next)
        return head;

    list_ele_t *fast = head->next;
    list_ele_t *slow = head;

    // split list
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }
    fast = slow->next;
    slow->next = NULL;

    // sort each list
    list_ele_t *l1 = merge_sort(head);
    list_ele_t *l2 = merge_sort(fast);

    // merge sorted list l1 and l2
    return merge(l1, l2);
}

void q_sort(queue_t *q)
{
    if (!q || !q->head || !q->head->next)
        return;

    q->head = merge_sort(q->head);
    list_ele_t *tmp = q->head;
    while (tmp->next) {
        tmp = tmp->next;
    }
    q->tail = tmp;
}
