#include "../headers/avl_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Create a new vital record
VitalRecord* createVitalRecord(int id, const char* time, float hr, float bp, float temp, float oxy) {
    VitalRecord* newRecord = (VitalRecord*)malloc(sizeof(VitalRecord));
    if (newRecord == NULL) {
        printf("Memory allocation failed!\n");
        return NULL;
    }
    
    newRecord->patient_id = id;
    strcpy(newRecord->timestamp, time);
    newRecord->heart_rate = hr;
    newRecord->blood_pressure = bp;
    newRecord->temperature = temp;
    newRecord->oxygen_saturation = oxy;
    newRecord->left = NULL;
    newRecord->right = NULL;
    newRecord->height = 1;
    
    return newRecord;
}

// Get height of node
int getHeight(VitalRecord* node) {
    if (node == NULL)
        return 0;
    return node->height;
}

// Get maximum of two integers - CHANGED FROM 'max' TO 'maximum'
int maximum(int a, int b) {
    return (a > b) ? a : b;
}

// Get balance factor
int getBalance(VitalRecord* node) {
    if (node == NULL)
        return 0;
    return getHeight(node->left) - getHeight(node->right);
}

// Right rotation
VitalRecord* rightRotate(VitalRecord* y) {
    VitalRecord* x = y->left;
    VitalRecord* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = maximum(getHeight(y->left), getHeight(y->right)) + 1;  // UPDATED
    x->height = maximum(getHeight(x->left), getHeight(x->right)) + 1;  // UPDATED

    return x;
}

// Left rotation
VitalRecord* leftRotate(VitalRecord* x) {
    VitalRecord* y = x->right;
    VitalRecord* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = maximum(getHeight(x->left), getHeight(x->right)) + 1;  // UPDATED
    y->height = maximum(getHeight(y->left), getHeight(y->right)) + 1;  // UPDATED

    return y;
}

// Insert vital record into AVL tree
VitalRecord* insertVitalRecord(VitalRecord* node, VitalRecord* newRecord) {
    if (node == NULL)
        return newRecord;

    if (newRecord->patient_id < node->patient_id)
        node->left = insertVitalRecord(node->left, newRecord);
    else if (newRecord->patient_id > node->patient_id)
        node->right = insertVitalRecord(node->right, newRecord);
    else {
        // ✅ CHANGED: Update existing record with new values
        strcpy(node->timestamp, newRecord->timestamp);
        node->heart_rate = newRecord->heart_rate;
        node->blood_pressure = newRecord->blood_pressure;
        node->temperature = newRecord->temperature;
        node->oxygen_saturation = newRecord->oxygen_saturation;
        free(newRecord);  // Free the new record since we updated the existing one
        return node;      // Return same node (no structural changes needed)
    }

    node->height = 1 + maximum(getHeight(node->left), getHeight(node->right));  // UPDATED

    int balance = getBalance(node);

    // Left Left Case
    if (balance > 1 && newRecord->patient_id < node->left->patient_id)
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && newRecord->patient_id > node->right->patient_id)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && newRecord->patient_id > node->left->patient_id) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && newRecord->patient_id < node->right->patient_id) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// Search for patient records
VitalRecord* searchVitalRecord(VitalRecord* root, int patient_id) {
    if (root == NULL || root->patient_id == patient_id)
        return root;

    if (patient_id < root->patient_id)
        return searchVitalRecord(root->left, patient_id);

    return searchVitalRecord(root->right, patient_id);
}

// In-order traversal
void inOrderTraversal(VitalRecord* root) {
    if (root != NULL) {
        inOrderTraversal(root->left);
        printf("Patient ID: %d, Time: %s, HR: %.1f, BP: %.1f, Temp: %.1f, O2: %.1f\n",
               root->patient_id, root->timestamp, root->heart_rate,
               root->blood_pressure, root->temperature, root->oxygen_saturation);
        inOrderTraversal(root->right);
    }
}

// NEW FUNCTION: Traverse and process each node with a callback function
void traverseAndProcess(VitalRecord* root, void (*process)(VitalRecord*)) {
    if (root != NULL) {
        traverseAndProcess(root->left, process);
        process(root);
        traverseAndProcess(root->right, process);
    }
}

// Free AVL tree memory
void freeAVLTree(VitalRecord* root) {
    if (root != NULL) {
        freeAVLTree(root->left);
        freeAVLTree(root->right);
        free(root);
    }
}