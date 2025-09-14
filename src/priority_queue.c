#include "dbg.h"
#include "process.h"
#include "queue.h"
#include <stdlib.h>

Queue *p_queue_create() { return queue_create(); }

node *p_newNode(process_t *pd, int priority) {
    node *temp = (node *)malloc(sizeof(node));
    temp->data = pd;
    temp->priority = priority;
    temp->next = NULL;
    return temp;
}

process_t *p_dequeue(Queue *queue) { return dequeue(queue); }

bool p_isEmpty(Queue *queue) { return isEmpty(queue); }

process_t *p_remove_node_by_pid(Queue *queue, int pid) { return remove_node_by_pid(queue, pid); }

void p_isValidQueue(Queue *queue) { isValidQueue(queue); }

node *p_enqueue(Queue *queue, process_t *pd, int priority) {
    isValidQueue(queue);
    node *newnode = p_newNode(pd, priority);
    if (isEmpty(queue)) {
        queue->front = queue->rear = newnode;
        queue->len++;

        assert_t(queue->front == queue->rear);
        assert_t(queue->front == newnode);
        return queue->front;
    }
    // check for front pid
    if (queue->front->priority > priority) {
        newnode->next = queue->front;
        queue->front = newnode;
        queue->len++;

        assert_t(queue->front == newnode);
        return newnode;
    }
    node *tnode, *snode;
    Traverse(tnode, queue) {
        snode = tnode->next;
        if (!snode)
            break;
        debug("snode->priority: %d, Node priority: %d", snode->priority, priority);
        if (snode->priority > priority) {
            break;
        }
    }
    if (!snode) {
        assert_t(queue->rear == tnode);
        queue->rear->next = newnode;
        queue->rear = newnode;
        queue->len++;
        assert_t(queue->rear == newnode);
        return queue->front;
    }
    tnode->next = newnode;
    newnode->next = snode;
    queue->len++;
    return queue->front;
}

void p_queue_print(Queue *queue) {
    isValidQueue(queue);
    node *tnode;
    debug("printing priority queue");
    Traverse(tnode, queue) { printf("(%d | %d) ", tnode->data->pid, tnode->priority); }
    printf("%d | %d\n", tnode->data->pid, tnode->priority);
}
// int main() {
//     node* pq = newNode(4, 1);
//     pq = push(pq, 5, 2);
//     pq = push(pq, 6, 3);
//     pq = push(pq, 7, 0);

//     while (!isEmpty(pq)) {
//         printf("%d ", peek(pq));
//         pq = pop(pq);
//     }

//     return 0;
// }
