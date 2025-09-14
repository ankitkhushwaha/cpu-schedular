#include <stdio.h> 
#include <stdlib.h> 

// typedef struct node { 
//     int data; 
//     int priority; 
//     struct node* next; 
// } Node; 

Node* newNode(int d, int p) { 
    Node* temp = (Node*)malloc(sizeof(Node)); 
    temp->data = d; 
    temp->priority = p; 
    temp->next = NULL; 
    return temp; 
} 

int peek(Node* head) {
    return head->data; 
} 

Node* pop(Node* head) {
    Node* temp = head; 
    head = head->next; 
    free(temp); 
    return head; 
} 

Node* push(Node* head, int d, int p) { 
    Node* start = head; 
    Node* temp = newNode(d, p); 

    if (head->priority > p) { 
        temp->next = head; 
        head = temp; 
    } 
    else { 
        while (start->next != NULL && start->next->priority < p) { 
            start = start->next; 
        } 
        temp->next = start->next; 
        start->next = temp; 
    } 
    return head; 
} 

int isEmpty(Node* head) { 
    return (head == NULL); 
} 

// int main() { 
//     Node* pq = newNode(4, 1); 
//     pq = push(pq, 5, 2); 
//     pq = push(pq, 6, 3); 
//     pq = push(pq, 7, 0); 

//     while (!isEmpty(pq)) { 
//         printf("%d ", peek(pq)); 
//         pq = pop(pq); 
//     } 

//     return 0; 
// }
