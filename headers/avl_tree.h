#ifndef AVL_TREE_H
#define AVL_TREE_H

typedef struct VitalRecord {
    int patient_id;
    char timestamp[20];
    float heart_rate;
    float blood_pressure;
    float temperature;
    float oxygen_saturation;
    struct VitalRecord* left;
    struct VitalRecord* right;
    int height;
} VitalRecord;

// Function declarations
VitalRecord* createVitalRecord(int id, const char* time, float hr, float bp, float temp, float oxy);
VitalRecord* insertVitalRecord(VitalRecord* node, VitalRecord* newRecord);
VitalRecord* searchVitalRecord(VitalRecord* root, int patient_id);
void inOrderTraversal(VitalRecord* root);
void traverseAndProcess(VitalRecord* root, void (*process)(VitalRecord*));  // ADD THIS LINE
void freeAVLTree(VitalRecord* root);
int getHeight(VitalRecord* node);
int getBalance(VitalRecord* node);

#endif