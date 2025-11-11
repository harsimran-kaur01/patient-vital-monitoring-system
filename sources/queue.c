#include "../headers/queue.h"

// Create queue
Queue* createQueue() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (q == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    q->front = 0;
    q->rear = -1;
    q->size = 0;
    return q;
}

// Check if queue is empty
int isQueueEmpty(Queue* q) {
    return q->size == 0;
}

// Check if queue is full
int isQueueFull(Queue* q) {
    return q->size == MAX_QUEUE_SIZE;
}

// Enqueue alert
void enqueue(Queue* q, Alert* alert) {
    if (isQueueFull(q)) {
        printf("Alert queue is full! Cannot add more alerts.\n");
        return;
    }
    
    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    q->alerts[q->rear] = alert;
    q->size++;
}

// Dequeue alert
Alert* dequeue(Queue* q) {
    if (isQueueEmpty(q)) {
        printf("Alert queue is empty!\n");
        return NULL;
    }
    
    Alert* alert = q->alerts[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    q->size--;
    return alert;
}

// Peek at front alert
Alert* peek(Queue* q) {
    if (isQueueEmpty(q)) {
        return NULL;
    }
    return q->alerts[q->front];
}

// Display all alerts in queue
void displayQueue(Queue* q) {
    if (isQueueEmpty(q)) {
        printf("No alerts in the queue.\n");
        return;
    }
    
    printf("\n=== ALERT QUEUE ===\n");
    int i = q->front;
    int count = 0;
    
    while (count < q->size) {
        Alert* alert = q->alerts[i];
        printf("Alert %d: Patient %d | %s | %s: %.1f | %s\n",
               count + 1, alert->patient_id, alert->timestamp,
               alert->vital_type, alert->value, alert->message);
        i = (i + 1) % MAX_QUEUE_SIZE;
        count++;
    }
}

// Free queue memory
void freeQueue(Queue* q) {
    while (!isQueueEmpty(q)) {
        Alert* alert = dequeue(q);
        free(alert);
    }
    free(q);
}