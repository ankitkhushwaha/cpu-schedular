#include <stdio.h>
#include <stdlib.h>
#include "linkedList.h"
#include "dbg.h"

linkedList *list_create()
{
    linkedList *list = (linkedList *)calloc(1, sizeof(linkedList));
    check_mem(list);
    return list;
error:
    exit(EXIT_FAILURE);
}

node *list_add(linkedList *list, int data)
{
    node *newnode = (node *)calloc(1, sizeof(node));
    newnode->data = data;
    check_mem(newnode);

    if (!list->front)
    {
        list->front = newnode;
        return list->front;
    }
    node *tnode;
    Traverse(tnode, list);
    tnode->next = newnode;
    list->rear = tnode->next;

    return list->front;
error:
    return NULL;
}

void list_print(linkedList *list)
{
    node *tnode;
    Traverse(tnode, list)
    {
        printf("%d ", tnode->data);
    }
    printf("%d\n", tnode->data);
}

node *list_RemoveFromEnd(linkedList *list)
{
    node *tnode;
    Traverse(tnode, list)
    {
        if (tnode->next->next == NULL)
            break;
    }
    list->rear = tnode;
    node *snode = tnode->next;
    tnode->next = NULL;
    free(snode);

    return list->front;
}

node *list_RemoveFromFront(linkedList *list){
    node *tnode, *snode;
    tnode = snode = list->front;

    tnode = tnode->next;
    list->front = tnode;
    
    free(snode);
    return list->front;
}


// For testing
void validate_list(linkedList *list, int *arr, int size)
{
    int i;
    node *tnode = list->front;
    for (i = 0; i < size; i++)
    {
        if (arr[i] != tnode->data)
        {
            fprintf(stderr, "%dth index: %d != %d\n",
                    i, arr[i], tnode->data);
        }
        tnode = tnode->next;
    }
}

int main(int argc, char *argv[])
{
    linkedList *list = list_create();
    list_add(list, 2);
    list_add(list, 3);
    list_add(list, 4);

    int arr[] = {2, 3, 4};
    validate_list(list, arr, 3);
    
    list_print(list);
    list_RemoveFromEnd(list);
    list_print(list);
    list_add(list, 5);
    
    int arr2[] = {2, 3, 5};
    validate_list(list, arr2, 3);
    
    list_RemoveFromFront(list);
    list_add(list, 1);
    
    int arr3[] = {3, 5, 1};
    validate_list(list, arr3, 3);

    list_print(list);
}