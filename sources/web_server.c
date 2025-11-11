#include "../headers/web_server.h"
#include "../headers/vital_monitor.h"
#include <windows.h>

// These are now properly declared as extern and defined in main.c
extern VitalRecord* root;
extern Queue* alertQueue;

// Function declarations
void sendDynamicViewRecords(SOCKET client_socket);
void sendDynamicAlerts(SOCKET client_socket);
void sendDynamicTrends(SOCKET client_socket);

char* readHtmlFile(const char* filename) {
    printf("Attempting to read file: %s\n", filename);
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Failed to open file: %s\n", filename);
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    printf("File size: %ld bytes\n", length);
    
    char* buffer = malloc(length + 1);
    if (buffer) {
        size_t bytes_read = fread(buffer, 1, length, file);
        buffer[bytes_read] = '\0';
        printf("✓ Successfully read %zu bytes from %s\n", bytes_read, filename);
    } else {
        printf("Memory allocation failed for file: %s\n", filename);
    }
    
    fclose(file);
    return buffer;
}

void sendHttpResponse(SOCKET client_socket, const char* content_type, const char* content) {
    int content_length = strlen(content);
    
    // Use dynamic allocation for responses
    int header_size = 200;
    int total_size = content_length + header_size;
    
    char* response = malloc(total_size);
    if (!response) {
        printf("Memory allocation failed for response\n");
        return;
    }
    
    sprintf(response, 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s; charset=utf-8\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s", content_type, content_length, content);
    
    int bytes_sent = send(client_socket, response, strlen(response), 0);
    printf("Sent %d bytes to client\n", bytes_sent);
    
    free(response);
}

void sendHtmlPage(SOCKET client_socket, const char* html_content) {
    if (html_content) {
        sendHttpResponse(client_socket, "text/html", html_content);
    } else {
        send404(client_socket);
    }
}

void send404(SOCKET client_socket) {
    const char* not_found = 
        "<html><body style='font-family: Arial; margin: 40px;'>"
        "<h1>404 - Page Not Found</h1>"
        "<p>The requested page was not found on this server.</p>"
        "<a href='/'>Go Back to Home</a>"
        "</body></html>";
    sendHttpResponse(client_socket, "text/html", not_found);
}

void sendSuccessResponse(SOCKET client_socket, int patient_id, const char* timestamp, 
                        float heart_rate, float blood_pressure, float temperature, float oxygen_saturation) {
    char success_html[2048];
    sprintf(success_html,
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<title>Record Added Successfully</title>"
        "<style>"
        "body { font-family: Arial, sans-serif; margin: 40px; background: #f5f5f5; }"
        ".container { max-width: 600px; margin: 0 auto; background: white; padding: 30px; border-radius: 10px; box-shadow: 0 5px 15px rgba(0,0,0,0.1); text-align: center; }"
        ".success { color: #27ae60; font-size: 2em; margin-bottom: 20px; }"
        ".btn { display: inline-block; margin: 10px; padding: 10px 20px; background: #3498db; color: white; text-decoration: none; border-radius: 5px; }"
        ".btn:hover { background: #2980b9; }"
        ".record-details { background: #f8f9fa; padding: 15px; border-radius: 5px; margin: 20px 0; text-align: left; }"
        "</style>"
        "</head>"
        "<body>"
        "<div class='container'>"
        "<div class='success'>&#10004; Record Added Successfully!</div>"
        "<div class='record-details'>"
        "<p><strong>Patient ID:</strong> %d</p>"
        "<p><strong>Timestamp:</strong> %s</p>"
        "<p><strong>Heart Rate:</strong> %.1f bpm</p>"
        "<p><strong>Blood Pressure:</strong> %.1f mmHg</p>"
        "<p><strong>Temperature:</strong> %.1f °F</p>"
        "<p><strong>Oxygen Saturation:</strong> %.1f%%</p>"
        "</div>"
        "<div style='margin-top: 30px;'>"
        "<a href='/add_record' class='btn'>Add Another Record</a>"
        "<a href='/view_records' class='btn'>View All Records</a>"
        "<a href='/' class='btn'>Back to Dashboard</a>"
        "</div>"
        "</div>"
        "</body>"
        "</html>", patient_id, timestamp, heart_rate, blood_pressure, temperature, oxygen_saturation);
    
    sendHtmlPage(client_socket, success_html);
}

void handleAddRecordPost(SOCKET client_socket, char* request_buffer) {
    printf("=== FORM SUBMISSION RECEIVED ===\n");
    
    // Parse form data directly from the buffer
    int patient_id = 109;
    char timestamp[50] = "2024-01-15 15:45";
    float heart_rate = 88.0, blood_pressure = 122.0, temperature = 99.2, oxygen_saturation = 96.0;
    
    char* field = NULL;
    
    field = strstr(request_buffer, "patient_id=");
    if (field) patient_id = atoi(field + 11);
    
    field = strstr(request_buffer, "timestamp=");
    if (field) {
        char* end = strchr(field + 10, '&');
        if (end) {
            strncpy(timestamp, field + 10, end - (field + 10));
            timestamp[end - (field + 10)] = '\0';
        } else {
            strcpy(timestamp, field + 10);
        }
        for (char* p = timestamp; *p; p++) {
            if (*p == '+') *p = ' ';
            if (*p == '%' && *(p+1) == '3' && *(p+2) == 'A') {
                *p = ':';
                memmove(p+1, p+3, strlen(p+3)+1);
            }
        }
    }
    
    field = strstr(request_buffer, "heart_rate=");
    if (field) heart_rate = atof(field + 11);
    field = strstr(request_buffer, "blood_pressure=");
    if (field) blood_pressure = atof(field + 15);
    field = strstr(request_buffer, "temperature=");
    if (field) temperature = atof(field + 12);
    field = strstr(request_buffer, "oxygen_saturation=");
    if (field) oxygen_saturation = atof(field + 18);
    
    printf("Final data: Patient %d, Time: %s, HR: %.1f, BP: %.1f, Temp: %.1f, O2: %.1f\n",
           patient_id, timestamp, heart_rate, blood_pressure, temperature, oxygen_saturation);
    
    VitalRecord* newRecord = createVitalRecord(patient_id, timestamp, heart_rate, 
                                              blood_pressure, temperature, oxygen_saturation);
    if (newRecord) {
        root = insertVitalRecord(root, newRecord);
        printf("✓ Record added to AVL tree successfully! Patient ID: %d\n", patient_id);
        
        char message[100];
        if (checkVitalRanges(heart_rate, blood_pressure, temperature, oxygen_saturation, message)) {
            generateAlert(alertQueue, patient_id, timestamp, "Multiple", 0, message);
            printf("Alert generated: %s\n", message);
        }
        
        sendSuccessResponse(client_socket, patient_id, timestamp, heart_rate, blood_pressure, temperature, oxygen_saturation);
    } else {
        printf("Failed to create record\n");
        send404(client_socket);
    }
    
    printf("=== FORM PROCESSING COMPLETE ===\n\n");
}

void serveHtmlFile(SOCKET client_socket, const char* filename) {
    printf("Serving file: %s\n", filename);
    
    char full_path[256];
    sprintf(full_path, "web_pages/%s", filename);
    
    printf("Looking for file at: %s\n", full_path);
    
    char* html = readHtmlFile(full_path);
    if (html) {
        size_t html_size = strlen(html);
        printf("HTML content size: %zu bytes\n", html_size);
        
        sendHtmlPage(client_socket, html);
        printf("✓ Successfully served: %s\n", full_path);
        
        free(html);
    } else {
        printf("Failed to serve: %s - sending 404\n", full_path);
        send404(client_socket);
    }
}

// SIMPLE ITERATIVE SOLUTION: Collect all records into an array first
void sendDynamicViewRecords(SOCKET client_socket) {
    printf("Generating dynamic view records page...\n");
    
    // First, collect all records into an array (simple approach)
    VitalRecord* records[1000];
    int record_count = 0;
    
    // Simple iterative collection using a stack (avoid recursion)
    VitalRecord* stack[1000];
    int stack_size = 0;
    VitalRecord* current = root;
    
    // In-order traversal without recursion
    while (current != NULL || stack_size > 0) {
        // Reach the leftmost node of current node
        while (current != NULL) {
            stack[stack_size++] = current;
            current = current->left;
        }
        
        // Current must be NULL at this point
        current = stack[--stack_size];
        
        // Add to our records array
        if (record_count < 1000) {
            records[record_count++] = current;
        }
        
        // Visit the right subtree
        current = current->right;
    }
    
    printf("Collected %d records from AVL tree\n", record_count);
    
    // Now build the HTML
    char* html_start = 
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<title>Patient Records</title>"
        "<style>"
        "body { font-family: Arial, sans-serif; margin: 20px; background: #f5f5f5; }"
        ".container { max-width: 1200px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }"
        "h1 { color: #2c3e50; text-align: center; margin-bottom: 30px; }"
        ".record-table { width: 100%; border-collapse: collapse; margin: 20px 0; }"
        ".record-table th, .record-table td { padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }"
        ".record-table th { background-color: #3498db; color: white; }"
        ".record-table tr:hover { background-color: #f5f5f5; }"
        ".critical { background-color: #ffebee; color: #c62828; font-weight: bold; }"
        ".normal { background-color: #e8f5e8; color: #2e7d32; }"
        ".btn { display: inline-block; padding: 10px 20px; background: #3498db; color: white; text-decoration: none; border-radius: 5px; margin: 5px; }"
        ".btn:hover { background: #2980b9; }"
        ".nav { text-align: center; margin: 20px 0; }"
        "</style>"
        "</head>"
        "<body>"
        "<div class='container'>"
        "<h1>Patient Vital Records</h1>"
        "<div class='nav'>"
        "<a href='/' class='btn'>Dashboard</a>"
        "<a href='/add_record' class='btn'>Add Record</a>"
        "<a href='/alerts' class='btn'>View Alerts</a>"
        "<a href='/trends' class='btn'>Trends</a>"
        "</div>"
        "<table class='record-table'>"
        "<thead>"
        "<tr>"
        "<th>Patient ID</th>"
        "<th>Timestamp</th>"
        "<th>Heart Rate</th>"
        "<th>Blood Pressure</th>"
        "<th>Temperature</th>"
        "<th>Oxygen Saturation</th>"
        "<th>Status</th>"
        "</tr>"
        "</thead>"
        "<tbody>";

    // Calculate buffer size
    int total_size = strlen(html_start) + (record_count * 500) + 1000;
    char* html = malloc(total_size);
    if (!html) {
        printf("Memory allocation failed for dynamic page\n");
        send404(client_socket);
        return;
    }
    
    strcpy(html, html_start);
    
    // Add all records to HTML
    for (int i = 0; i < record_count; i++) {
        VitalRecord* record = records[i];
        char message[100];
        int is_critical = checkVitalRanges(record->heart_rate, record->blood_pressure, 
                                         record->temperature, record->oxygen_saturation, message);
        
        const char* row_class = is_critical ? "critical" : "normal";
        const char* status_text = is_critical ? "CRITICAL" : "Normal";
        
        char row_buffer[512];
        sprintf(row_buffer,
            "<tr class='%s'>"
            "<td>%d</td>"
            "<td>%s</td>"
            "<td>%.1f bpm</td>"
            "<td>%.1f mmHg</td>"
            "<td>%.1f °F</td>"
            "<td>%.1f%%</td>"
            "<td>%s</td>"
            "</tr>",
            row_class, record->patient_id, record->timestamp,
            record->heart_rate, record->blood_pressure, 
            record->temperature, record->oxygen_saturation,
            status_text);
        
        strcat(html, row_buffer);
    }
    
    // If no records found
    if (record_count == 0) {
        strcat(html, "<tr><td colspan='7' style='text-align: center;'>No records found</td></tr>");
    }
    
    // Close HTML
    char* html_end = 
        "</tbody>"
        "</table>"
        "<div style='text-align: center; margin-top: 20px;'>"
        "<p>Total records: <strong>%d</strong></p>"
        "</div>"
        "</div>"
        "</body>"
        "</html>";
    
    char count_buffer[100];
    sprintf(count_buffer, html_end, record_count);
    strcat(html, count_buffer);
    
    printf("Generated dynamic page with %d records\n", record_count);
    sendHttpResponse(client_socket, "text/html", html);
    free(html);
}

void sendDynamicAlerts(SOCKET client_socket) {
    printf("Generating dynamic alerts page...\n");
    
    char* html_start = 
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<title>Patient Alerts</title>"
        "<style>"
        "body { font-family: Arial, sans-serif; margin: 20px; background: #f5f5f5; }"
        ".container { max-width: 1200px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }"
        "h1 { color: #c62828; text-align: center; margin-bottom: 30px; }"
        ".alert-table { width: 100%; border-collapse: collapse; margin: 20px 0; }"
        ".alert-table th, .alert-table td { padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }"
        ".alert-table th { background-color: #c62828; color: white; }"
        ".alert-table tr { background-color: #ffebee; }"
        ".alert-table tr:hover { background-color: #ffcdd2; }"
        ".btn { display: inline-block; padding: 10px 20px; background: #3498db; color: white; text-decoration: none; border-radius: 5px; margin: 5px; }"
        ".btn:hover { background: #2980b9; }"
        ".nav { text-align: center; margin: 20px 0; }"
        ".no-alerts { text-align: center; padding: 40px; color: #666; }"
        "</style>"
        "</head>"
        "<body>"
        "<div class='container'>"
        "<h1>Critical Patient Alerts</h1>"
        "<div class='nav'>"
        "<a href='/' class='btn'>Dashboard</a>"
        "<a href='/view_records' class='btn'>View Records</a>"
        "<a href='/add_record' class='btn'>Add Record</a>"
        "<a href='/trends' class='btn'>Trends</a>"
        "</div>";

    int max_alerts = 50;
    int alert_size = 400;
    int total_size = strlen(html_start) + (max_alerts * alert_size) + 1000;
    
    char* html = malloc(total_size);
    if (!html) {
        printf("Memory allocation failed for alerts page\n");
        send404(client_socket);
        return;
    }
    
    strcpy(html, html_start);
    
    // Display alerts from queue
    if (isQueueEmpty(alertQueue)) {
        strcat(html, "<div class='no-alerts'><h2>&#10004; No Critical Alerts</h2><p>All patients are stable</p></div>");
    } else {
        strcat(html, "<table class='alert-table'><thead><tr><th>Patient ID</th><th>Timestamp</th><th>Alert Message</th></tr></thead><tbody>");
        
        // Create a temporary queue to preserve original
        Queue* tempQueue = createQueue();
        int alert_count = 0;
        char alert_buffer[400];
        
        // Display all alerts
        while (!isQueueEmpty(alertQueue)) {
            Alert* alert = dequeue(alertQueue);
            if (alert) {
                sprintf(alert_buffer,
                    "<tr>"
                    "<td>%d</td>"
                    "<td>%s</td>"
                    "<td><strong>%s</strong></td>"
                    "</tr>",
                    alert->patient_id, alert->timestamp, alert->message);
                
                strcat(html, alert_buffer);
                alert_count++;
                
                // Add back to original queue
                enqueue(tempQueue, alert);
            }
        }
        
        // Restore original queue
        while (!isQueueEmpty(tempQueue)) {
            Alert* alert = dequeue(tempQueue);
            enqueue(alertQueue, alert);
        }
        freeQueue(tempQueue);
        
        strcat(html, "</tbody></table>");
        
        char count_buffer[100];
        sprintf(count_buffer, "<div style='text-align: center; margin-top: 20px;'><p>Total alerts: <strong>%d</strong></p></div>", alert_count);
        strcat(html, count_buffer);
    }
    
    // Close HTML
    char* html_end = "</div></body></html>";
    strcat(html, html_end);
    
    printf("Generated dynamic alerts page\n");
    sendHttpResponse(client_socket, "text/html", html);
    free(html);
}

// NEW FUNCTION: Dynamic trends analysis
void sendDynamicTrends(SOCKET client_socket) {
    printf("Generating dynamic trends analysis page...\n");
    
    // Collect all records
    VitalRecord* records[1000];
    int record_count = 0;
    
    VitalRecord* stack[1000];
    int stack_size = 0;
    VitalRecord* current = root;
    
    // In-order traversal without recursion
    while (current != NULL || stack_size > 0) {
        while (current != NULL) {
            stack[stack_size++] = current;
            current = current->left;
        }
        current = stack[--stack_size];
        if (record_count < 1000) {
            records[record_count++] = current;
        }
        current = current->right;
    }
    
    printf("Collected %d records for trend analysis\n", record_count);
    
    // Calculate statistics
    int critical_count = 0;
    int normal_count = 0;
    float avg_heart_rate = 0, avg_blood_pressure = 0, avg_temperature = 0, avg_oxygen = 0;
    
    for (int i = 0; i < record_count; i++) {
        VitalRecord* record = records[i];
        char message[100];
        int is_critical = checkVitalRanges(record->heart_rate, record->blood_pressure, 
                                         record->temperature, record->oxygen_saturation, message);
        
        if (is_critical) {
            critical_count++;
        } else {
            normal_count++;
        }
        
        avg_heart_rate += record->heart_rate;
        avg_blood_pressure += record->blood_pressure;
        avg_temperature += record->temperature;
        avg_oxygen += record->oxygen_saturation;
    }
    
    if (record_count > 0) {
        avg_heart_rate /= record_count;
        avg_blood_pressure /= record_count;
        avg_temperature /= record_count;
        avg_oxygen /= record_count;
    }
    
    // Build HTML
    char* html_start = 
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<title>Trend Analysis</title>"
        "<style>"
        "body { font-family: Arial, sans-serif; margin: 20px; background: #f5f5f5; }"
        ".container { max-width: 1200px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }"
        "h1 { color: #2c3e50; text-align: center; margin-bottom: 30px; }"
        ".stats-grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap: 20px; margin: 30px 0; }"
        ".stat-card { background: #f8f9fa; padding: 20px; border-radius: 10px; text-align: center; border-left: 5px solid #3498db; }"
        ".stat-card.critical { border-left-color: #c62828; background: #ffebee; }"
        ".stat-card.normal { border-left-color: #27ae60; background: #e8f5e8; }"
        ".stat-number { font-size: 2.5em; font-weight: bold; margin: 10px 0; }"
        ".stat-label { color: #666; font-size: 1.1em; }"
        ".trend-table { width: 100%; border-collapse: collapse; margin: 20px 0; }"
        ".trend-table th, .trend-table td { padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }"
        ".trend-table th { background-color: #3498db; color: white; }"
        ".btn { display: inline-block; padding: 10px 20px; background: #3498db; color: white; text-decoration: none; border-radius: 5px; margin: 5px; }"
        ".btn:hover { background: #2980b9; }"
        ".nav { text-align: center; margin: 20px 0; }"
        ".no-data { text-align: center; padding: 40px; color: #666; }"
        "</style>"
        "</head>"
        "<body>"
        "<div class='container'>"
        "<h1>Patient Vital Trends Analysis</h1>"
        "<div class='nav'>"
        "<a href='/' class='btn'>Dashboard</a>"
        "<a href='/view_records' class='btn'>View Records</a>"
        "<a href='/alerts' class='btn'>View Alerts</a>"
        "<a href='/add_record' class='btn'>Add Record</a>"
        "</div>";

    int total_size = strlen(html_start) + 5000;
    char* html = malloc(total_size);
    if (!html) {
        printf("Memory allocation failed for trends page\n");
        send404(client_socket);
        return;
    }
    
    strcpy(html, html_start);
    
    if (record_count == 0) {
        strcat(html, "<div class='no-data'><h2>No data available for analysis</h2><p>Add some patient records to see trends</p></div>");
    } else {
        // Add statistics cards
        strcat(html, "<div class='stats-grid'>");
        
        char stat_buffer[512];
        
        sprintf(stat_buffer,
            "<div class='stat-card'>"
            "<div class='stat-label'>Total Records</div>"
            "<div class='stat-number'>%d</div>"
            "</div>",
            record_count);
        strcat(html, stat_buffer);
        
        sprintf(stat_buffer,
            "<div class='stat-card normal'>"
            "<div class='stat-label'>Normal Records</div>"
            "<div class='stat-number'>%d</div>"
            "</div>",
            normal_count);
        strcat(html, stat_buffer);
        
        sprintf(stat_buffer,
            "<div class='stat-card critical'>"
            "<div class='stat-label'>Critical Records</div>"
            "<div class='stat-number'>%d</div>"
            "</div>",
            critical_count);
        strcat(html, stat_buffer);
        
        sprintf(stat_buffer,
            "<div class='stat-card'>"
            "<div class='stat-label'>Critical Rate</div>"
            "<div class='stat-number'>%.1f%%</div>"
            "</div>",
            record_count > 0 ? ((float)critical_count / record_count * 100) : 0);
        strcat(html, stat_buffer);
        
        strcat(html, "</div>");
        
        // Add average values table
        strcat(html, 
            "<h2 style='text-align: center; margin-top: 40px;'>Average Vital Values</h2>"
            "<table class='trend-table'>"
            "<thead>"
            "<tr>"
            "<th>Vital Sign</th>"
            "<th>Average Value</th>"
            "<th>Normal Range</th>"
            "<th>Status</th>"
            "</tr>"
            "</thead>"
            "<tbody>");
        
        // Check each average against normal ranges
        char message[100];
        int hr_ok = !checkVitalRanges(avg_heart_rate, 120, 98.6, 98, message); // Use dummy values for other vitals
        int bp_ok = !checkVitalRanges(80, avg_blood_pressure, 98.6, 98, message);
        int temp_ok = !checkVitalRanges(80, 120, avg_temperature, 98, message);
        int oxy_ok = !checkVitalRanges(80, 120, 98.6, avg_oxygen, message);
        
        sprintf(stat_buffer,
            "<tr><td>Heart Rate</td><td>%.1f bpm</td><td>60-100 bpm</td><td style='color: %s;'>%s</td></tr>",
            avg_heart_rate, hr_ok ? "#27ae60" : "#c62828", hr_ok ? "Normal" : "Abnormal");
        strcat(html, stat_buffer);
        
        sprintf(stat_buffer,
            "<tr><td>Blood Pressure</td><td>%.1f mmHg</td><td>90-120 mmHg</td><td style='color: %s;'>%s</td></tr>",
            avg_blood_pressure, bp_ok ? "#27ae60" : "#c62828", bp_ok ? "Normal" : "Abnormal");
        strcat(html, stat_buffer);
        
        sprintf(stat_buffer,
            "<tr><td>Temperature</td><td>%.1f °F</td><td>97-99 °F</td><td style='color: %s;'>%s</td></tr>",
            avg_temperature, temp_ok ? "#27ae60" : "#c62828", temp_ok ? "Normal" : "Abnormal");
        strcat(html, stat_buffer);
        
        sprintf(stat_buffer,
            "<tr><td>Oxygen Saturation</td><td>%.1f%%</td><td>>95%%</td><td style='color: %s;'>%s</td></tr>",
            avg_oxygen, oxy_ok ? "#27ae60" : "#c62828", oxy_ok ? "Normal" : "Abnormal");
        strcat(html, stat_buffer);
        
        strcat(html, "</tbody></table>");
        
        // Add recent records summary
        strcat(html, "<h2 style='text-align: center; margin-top: 40px;'>Recent Patient Overview</h2>");
        
        // Show last 5 records
        int show_count = record_count < 5 ? record_count : 5;
        strcat(html, "<table class='trend-table'><thead><tr><th>Patient ID</th><th>Timestamp</th><th>Heart Rate</th><th>Status</th></tr></thead><tbody>");
        
        for (int i = record_count - show_count; i < record_count; i++) {
            VitalRecord* record = records[i];
            char message[100];
            int is_critical = checkVitalRanges(record->heart_rate, record->blood_pressure, 
                                             record->temperature, record->oxygen_saturation, message);
            
            sprintf(stat_buffer,
                "<tr><td>%d</td><td>%s</td><td>%.1f bpm</td><td style='color: %s;'>%s</td></tr>",
                record->patient_id, record->timestamp, record->heart_rate,
                is_critical ? "#c62828" : "#27ae60", is_critical ? "Critical" : "Normal");
            strcat(html, stat_buffer);
        }
        
        strcat(html, "</tbody></table>");
    }
    
    // Close HTML
    char* html_end = "</div></body></html>";
    strcat(html, html_end);
    
    printf("Generated dynamic trends page with %d records\n", record_count);
    sendHttpResponse(client_socket, "text/html", html);
    free(html);
}

void handleClientRequest(SOCKET client_socket) {
    char buffer[BUFFER_SIZE] = {0};
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
    
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        
        char buffer_copy[BUFFER_SIZE] = {0};
        strncpy(buffer_copy, buffer, BUFFER_SIZE - 1);
        
        char* method = strtok(buffer_copy, " ");
        char* path = strtok(NULL, " ");
        
        if (!method || !path) {
            printf("Invalid request format\n");
            send404(client_socket);
            closesocket(client_socket);
            return;
        }
        
        printf("Request: %s %s\n", method, path);
        
        if (strcmp(method, "GET") == 0) {
            if (strcmp(path, "/") == 0 || strcmp(path, "/index.html") == 0) {
                serveHtmlFile(client_socket, "index.html");
            }
            else if (strcmp(path, "/add_record") == 0) {
                serveHtmlFile(client_socket, "add_record.html");
            }
            else if (strcmp(path, "/view_records") == 0) {
                sendDynamicViewRecords(client_socket);
            }
            else if (strcmp(path, "/alerts") == 0) {
                sendDynamicAlerts(client_socket);
            }
            else if (strcmp(path, "/trends") == 0) {
                sendDynamicTrends(client_socket);
            }
            else if (strcmp(path, "/favicon.ico") == 0) {
                printf("Ignoring favicon request\n");
                send404(client_socket);
            }
            else {
                printf("404 - Unknown path: %s\n", path);
                send404(client_socket);
            }
        }
        else if (strcmp(method, "POST") == 0) {
            if (strcmp(path, "/add_record") == 0) {
                printf("Processing POST to /add_record\n");
                handleAddRecordPost(client_socket, buffer);
            }
            else {
                printf("404 - Unknown POST path: %s\n", path);
                send404(client_socket);
            }
        }
        else {
            printf("405 - Method not allowed: %s\n", method);
            send404(client_socket);
        }
    } else {
        printf("No data received from client or connection closed\n");
    }
    
    closesocket(client_socket);
}

int initializeWebServer() {
    WSADATA wsa;
    SOCKET server_socket;
    struct sockaddr_in server;
    
    printf("Initializing Winsock...\n");
    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("Failed. Error Code: %d\n", WSAGetLastError());
        return 1;
    }
    
    printf("Creating socket...\n");
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    
    printf("Binding socket to port %d...\n", PORT);
    if (bind(server_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
    
    listen(server_socket, 3);
    printf("✓ Web server started successfully!\n");
    printf("Open your browser and go to: http://localhost:%d\n", PORT);
    printf("Server is listening for connections...\n\n");
    
    while (1) {
        SOCKET client_socket;
        struct sockaddr_in client;
        int client_len = sizeof(client);
        
        client_socket = accept(server_socket, (struct sockaddr*)&client, &client_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed: %d\n", WSAGetLastError());
            continue;
        }
        
        printf("=== New connection from %s ===\n", inet_ntoa(client.sin_addr));
        handleClientRequest(client_socket);
        printf("=== Request completed ===\n\n");
    }
    
    closesocket(server_socket);
    WSACleanup();
    return 0;
}