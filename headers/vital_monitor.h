#ifndef VITAL_MONITOR_H
#define VITAL_MONITOR_H

#include "avl_tree.h"
#include "queue.h"

// Vital range constants
#define MIN_HEART_RATE 60
#define MAX_HEART_RATE 100
#define MIN_BLOOD_PRESSURE 90
#define MAX_BLOOD_PRESSURE 120
#define MIN_TEMPERATURE 97.0
#define MAX_TEMPERATURE 99.5
#define MIN_OXYGEN_SAT 95.0

// Function declarations
int checkVitalRanges(float hr, float bp, float temp, float oxy, char* message);
void generateAlert(Queue* alertQueue, int patient_id, const char* timestamp, 
                   const char* vital_type, float value, const char* message);
void displayPatientHistory(VitalRecord* root, int patient_id);
void comparePatientTrends(VitalRecord* root, int patient_id1, int patient_id2);
void realTimeMonitoring(VitalRecord** root, Queue* alertQueue);

#endif