#include "../headers/vital_monitor.h"
#include<string.h>
// Check if vital signs are within normal ranges
int checkVitalRanges(float hr, float bp, float temp, float oxy, char* message) {
    int abnormal = 0;
    strcpy(message, "Abnormal vitals: ");
    
    if (hr < MIN_HEART_RATE || hr > MAX_HEART_RATE) {
        char tempMsg[50];
        sprintf(tempMsg, "Heart Rate(%.1f) ", hr);
        strcat(message, tempMsg);
        abnormal = 1;
    }
    
    if (bp < MIN_BLOOD_PRESSURE || bp > MAX_BLOOD_PRESSURE) {
        char tempMsg[50];
        sprintf(tempMsg, "Blood Pressure(%.1f) ", bp);
        strcat(message, tempMsg);
        abnormal = 1;
    }
    
    if (temp < MIN_TEMPERATURE || temp > MAX_TEMPERATURE) {
        char tempMsg[50];
        sprintf(tempMsg, "Temperature(%.1f) ", temp);
        strcat(message, tempMsg);
        abnormal = 1;
    }
    
    if (oxy < MIN_OXYGEN_SAT) {
        char tempMsg[50];
        sprintf(tempMsg, "Oxygen Saturation(%.1f) ", oxy);
        strcat(message, tempMsg);
        abnormal = 1;
    }
    
    if (!abnormal) {
        strcpy(message, "All vitals normal");
    }
    
    return abnormal;
}

// Generate alert and add to queue
void generateAlert(Queue* alertQueue, int patient_id, const char* timestamp, 
                   const char* vital_type, float value, const char* message) {
    Alert* newAlert = (Alert*)malloc(sizeof(Alert));
    if (newAlert == NULL) {
        printf("Failed to create alert!\n");
        return;
    }
    
    newAlert->patient_id = patient_id;
    strcpy(newAlert->timestamp, timestamp);
    strcpy(newAlert->vital_type, vital_type);
    newAlert->value = value;
    strcpy(newAlert->message, message);
    
    enqueue(alertQueue, newAlert);
    printf("ALERT GENERATED: Patient %d - %s\n", patient_id, message);
}

// Display patient history
void displayPatientHistory(VitalRecord* root, int patient_id) {
    printf("\n=== PATIENT %d HISTORY ===\n", patient_id);
    
    // Simple search - in real scenario, we'd have separate patient history structure
    VitalRecord* current = root;
    int found = 0;
    
    // Using in-order traversal to find all records for this patient
    // This is simplified - in practice, you'd want a better data structure for patient history
    while (current != NULL) {
        if (patient_id == current->patient_id) {
            printf("Time: %s, HR: %.1f, BP: %.1f, Temp: %.1f, O2: %.1f\n",
                   current->timestamp, current->heart_rate, current->blood_pressure,
                   current->temperature, current->oxygen_saturation);
            found = 1;
        }
        
        if (patient_id < current->patient_id)
            current = current->left;
        else
            current = current->right;
    }
    
    if (!found) {
        printf("No records found for Patient %d\n", patient_id);
    }
}

// Compare trends between two patients
void comparePatientTrends(VitalRecord* root, int patient_id1, int patient_id2) {
    printf("\n=== COMPARING PATIENT TRENDS ===\n");
    printf("Patient %d vs Patient %d\n", patient_id1, patient_id2);
    
    VitalRecord* patient1 = searchVitalRecord(root, patient_id1);
    VitalRecord* patient2 = searchVitalRecord(root, patient_id2);
    
    if (patient1 && patient2) {
        printf("Patient %d - HR: %.1f, BP: %.1f, Temp: %.1f, O2: %.1f\n",
               patient_id1, patient1->heart_rate, patient1->blood_pressure,
               patient1->temperature, patient1->oxygen_saturation);
        
        printf("Patient %d - HR: %.1f, BP: %.1f, Temp: %.1f, O2: %.1f\n",
               patient_id2, patient2->heart_rate, patient2->blood_pressure,
               patient2->temperature, patient2->oxygen_saturation);
        
        // Simple trend comparison
        if (patient1->heart_rate > patient2->heart_rate) {
            printf("Patient %d has higher heart rate\n", patient_id1);
        } else {
            printf("Patient %d has higher heart rate\n", patient_id2);
        }
    } else {
        printf("One or both patients not found in records.\n");
    }
}

// Real-time monitoring simulation
void realTimeMonitoring(VitalRecord** root, Queue* alertQueue) {
    printf("\n=== REAL-TIME MONITORING ===\n");
    
    int patient_id;
    char timestamp[20];
    float hr, bp, temp, oxy;
    char message[100];
    
    printf("Enter Patient ID: ");
    scanf("%d", &patient_id);
    
    printf("Enter Timestamp (YYYY-MM-DD HH:MM): ");
    scanf(" %[^\n]", timestamp);
    
    printf("Enter Heart Rate: ");
    scanf("%f", &hr);
    
    printf("Enter Blood Pressure: ");
    scanf("%f", &bp);
    
    printf("Enter Temperature: ");
    scanf("%f", &temp);
    
    printf("Enter Oxygen Saturation: ");
    scanf("%f", &oxy);
    
    // Create new vital record
    VitalRecord* newRecord = createVitalRecord(patient_id, timestamp, hr, bp, temp, oxy);
    if (newRecord != NULL) {
        *root = insertVitalRecord(*root, newRecord);
        printf("Vital record added successfully!\n");
    }
    
    // Check for abnormalities
    if (checkVitalRanges(hr, bp, temp, oxy, message)) {
        generateAlert(alertQueue, patient_id, timestamp, "Multiple", 0, message);
    }
}