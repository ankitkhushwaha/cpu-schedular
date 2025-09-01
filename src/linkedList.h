#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct Node
{
    int data;
    struct Node *next;
} node;

typedef struct linkedList
{
    struct Node *front;
    struct Node *rear;
} linkedList; 

linkedList *list_create();
node *list_add(linkedList *, int);
node *list_RemoveFromEnd(linkedList *);
node *list_RemoveFromFront(linkedList *);
void list_print(linkedList *);

#define Traverse(tnode, list) \
    for (tnode = list->front; \
         tnode->next != NULL; \
         tnode = tnode->next)

#endif
