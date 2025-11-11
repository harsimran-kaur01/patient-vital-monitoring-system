#include <stdio.h>
#include <stdlib.h>
#include "../headers/vital_monitor.h"
#include "../headers/web_server.h"

// Global variables for web server access - REMOVE 'extern' and PROPERLY DEFINE them
VitalRecord* root = NULL;
Queue* alertQueue = NULL;

int main() {
    printf("==============================================\n");
    printf("   PATIENT VITAL MONITORING SYSTEM\n");
    printf("           (Web Server Version)\n");
    printf("==============================================\n\n");
    
    printf("Starting system initialization...\n");
    
    // Initialize data structures
    alertQueue = createQueue();
    if (!alertQueue) {
        printf("❌ Failed to create alert queue!\n");
        return 1;
    }
    
    // Add sample data for demonstration
    printf("Loading sample data...\n");
    VitalRecord* sample1 = createVitalRecord(101, "2024-01-15 10:30", 75, 110, 98.6, 98);
    VitalRecord* sample2 = createVitalRecord(102, "2024-01-15 11:00", 85, 115, 99.1, 97);
    VitalRecord* sample3 = createVitalRecord(103, "2024-01-15 11:30", 110, 125, 100.2, 92);
    
    if (sample1 && sample2 && sample3) {
        root = insertVitalRecord(root, sample1);
        root = insertVitalRecord(root, sample2);
        root = insertVitalRecord(root, sample3);
        
        generateAlert(alertQueue, 103, "2024-01-15 11:30", "Multiple", 0, 
                     "CRITICAL: Multiple abnormal vitals detected!");
        
        printf("&#10004; Sample data loaded successfully!\n");
        printf("   - 3 patient records added\n");
        printf("   - 1 critical alert generated\n");
    } else {
        printf("❌ Failed to create sample data!\n");
    }
    
    printf("\nStarting web server on port %d...\n", PORT);
    printf("Please wait...\n\n");
    
    // Start web server (this function runs indefinitely)
    int result = initializeWebServer();
    
    // Cleanup (this won't be reached until server stops)
    printf("Cleaning up resources...\n");
    freeAVLTree(root);
    freeQueue(alertQueue);
    
    printf("System shutdown complete.\n");
    return result;
}