#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "avl_tree.h"
#include "queue.h"

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 8192

// Declare global variables that are defined in main.c
extern VitalRecord* root;
extern Queue* alertQueue;

// Web server functions
int initializeWebServer();
void handleClientRequest(SOCKET client_socket);
void sendHttpResponse(SOCKET client_socket, const char* content_type, const char* content);
void sendHtmlPage(SOCKET client_socket, const char* html_content);
void send404(SOCKET client_socket);
char* readHtmlFile(const char* filename);

// API endpoint handlers
void handleApiRecords(SOCKET client_socket);
void handleApiAddRecord(SOCKET client_socket, const char* request_body);

// Add these missing function declarations
int checkVitalRanges(float hr, float bp, float temp, float oxy, char* message);
void generateAlert(Queue* alertQueue, int patient_id, const char* timestamp, 
                   const char* vital_type, float value, const char* message);

#endif