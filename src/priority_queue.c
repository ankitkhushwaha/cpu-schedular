#include<stdio.h>
#include<stdlib.h>

struct PQNode {
    int data;
    int priority;
};

struct PriorityQueue {
    struct PQNode* array;
    int capacity;
    int size;
};

struct PriorityQueue* createPriorityQueue(int capacity) {
    struct PriorityQueue* pq = (struct PriorityQueue*) malloc(sizeof(struct PriorityQueue));
    if (!pq) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    pq->capacity = capacity;
    pq->size = 0;
    pq->array = (struct PQNode*) malloc(capacity * sizeof(struct PQNode));
    if (!pq->array) {
        printf("Memory allocation failed.\n");
        free(pq);
        exit(1);
    }
    return pq;
}

// Function to swap two priority queue nodes
void swap(struct PQNode* a, struct PQNode* b) {
    struct PQNode temp = *a;
    *a = *b;
    *b = temp;
}

// Function to heapify a subtree rooted with node i
void heapify(struct PriorityQueue* pq, int i) {
    int largest = i;  // Initialize largest as root
    int left = 2 * i + 1;  // Left child
    int right = 2 * i + 2;  // Right child

    // If left child is larger than root
    if (left < pq->size && pq->array[left].priority > pq->array[largest].priority)
        largest = left;

    // If right child is larger than largest so far
    if (right < pq->size && pq->array[right].priority > pq->array[largest].priority)
        largest = right;

    // If largest is not root, then swap and heapify the affected subtree
    if (largest != i) {
        swap(&pq->array[i], &pq->array[largest]);
        heapify(pq, largest);
    }
}

// Function to insert an element into the priority queue
void insert(struct PriorityQueue* pq, int data, int priority) {
    if (pq->size == pq->capacity) {
        printf("Priority queue is full. Cannot insert.\n");
        return;
    }

    // Create a new node
    struct PQNode newNode;
    newNode.data = data;
    newNode.priority = priority;

    // Insert the new node at the end of the queue
    pq->array[pq->size] = newNode;
    pq->size++;

    // Heapify the new node upwards to maintain heap property
    int i = pq->size - 1;
    while (i != 0 && pq->array[(i - 1) / 2].priority < pq->array[i].priority) {
        swap(&pq->array[i], &pq->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

// Function to extract the element with highest priority from the priority queue
struct PQNode extractMax(struct PriorityQueue* pq) {
    if (pq->size <= 0) {
        printf("Priority queue is empty. Cannot extract.\n");
        struct PQNode emptyNode = {0, 0};
        return emptyNode;
    }
    if (pq->size == 1) {
        pq->size--;
        return pq->array[0];
    }

    // Store the root node, and reduce the size of the queue
    struct PQNode root = pq->array[0];
    pq->array[0] = pq->array[pq->size - 1];
    pq->size--;

    // Heapify the root node downwards to maintain heap property
    heapify(pq, 0);

    return root;
}

// Function to display the elements of the priority queue
void displayPriorityQueue(struct PriorityQueue* pq) {
    printf("Priority Queue elements:\n");
    for (int i = 0; i < pq->size; ++i)
        printf("(%d, %d) ", pq->array[i].data, pq->array[i].priority);
    printf("\n");
}

// Function to free priority queue memory
void freePriorityQueue(struct PriorityQueue* pq) {
    if (pq) {
        if (pq->array)
            free(pq->array);
        free(pq);
    }
}

// Main function to demonstrate priority queue operations
// int main() {
//     int capacity, choice, data, priority;

//     printf("Enter the capacity of the priority queue: ");
//     scanf("%d", &capacity);

//     struct PriorityQueue* pq = createPriorityQueue(capacity);

//     while (1) {
//         printf("\nPriority Queue Operations:\n");
//         printf("1. Insert Element\n");
//         printf("2. Extract Maximum Element\n");
//         printf("3. Display Priority Queue\n");
//         printf("4. Exit\n");
//         printf("Enter your choice: ");
//         scanf("%d", &choice);

//         switch (choice) {
//             case 1:
//                 printf("Enter element and its priority to insert: ");
//                 scanf("%d %d", &data, &priority);
//                 insert(pq, data, priority);
//                 break;
//             case 2:
//                 printf("Extracting maximum priority element...\n");
//                 {
//                     struct PQNode node = extractMax(pq);
//                     if (node.priority != 0)
//                         printf("Extracted element: (%d, %d)\n", node.data, node.priority);
//                 }
//                 break;
//             case 3:
//                 displayPriorityQueue(pq);
//                 break;
//             case 4:
//                 printf("Exiting program.\n");
//                 freePriorityQueue(pq);
//                 exit(0);
//             default:
//                 printf("Invalid choice. Please try again.\n");
//         }
//     }

//     return 0;
// }


							

