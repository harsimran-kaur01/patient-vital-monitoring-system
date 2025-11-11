#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include <stdio.h>

#define MAX_QUEUE_SIZE 100

// Structure for real-time vital alerts
typedef struct Alert {
    int patient_id;
    char timestamp[20];
    char vital_type[20];
    float value;
    char message[100];
} Alert;

typedef struct Queue {
    Alert* alerts[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int size;
} Queue;

// Queue operations
Queue* createQueue();
int isQueueEmpty(Queue* q);
int isQueueFull(Queue* q);
void enqueue(Queue* q, Alert* alert);
Alert* dequeue(Queue* q);
Alert* peek(Queue* q);
void displayQueue(Queue* q);
void freeQueue(Queue* q);

#endif