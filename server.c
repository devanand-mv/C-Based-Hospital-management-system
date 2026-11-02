/*
 * ============================================================================
 * Project Name : Hospital Management System - Lightweight C HTTP Server
 * Subject      : C Programming Mini-Project
 * Target Level : B.Tech 1st / 2nd Year
 * Description  : Serves the Web-based Hospital Management System (HTML, CSS, JS)
 *                directly using native C Windows Sockets (Winsock2).
 * Compilation  : gcc server.c -o server.exe -lws2_32
 * Execution    : .\server.exe
 * Port         : http://localhost:8080
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#define SOCKET int
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define closesocket close
#endif

#define PORT 8080
#define BUFFER_SIZE 8192

/* Function to return the MIME Content-Type based on file extension */
const char* getContentType(const char *path) {
    const char *dot = strrchr(path, '.');
    if (!dot) return "text/plain";
    if (strcmp(dot, ".html") == 0) return "text/html";
    if (strcmp(dot, ".css") == 0)  return "text/css";
    if (strcmp(dot, ".js") == 0)   return "application/javascript";
    if (strcmp(dot, ".png") == 0)  return "image/png";
    if (strcmp(dot, ".svg") == 0)  return "image/svg+xml";
    if (strcmp(dot, ".json") == 0) return "application/json";
    return "text/plain";
}

/* Function to send an HTTP response containing a static file */
void serveStaticFile(SOCKET clientSocket, const char *filePath) {
    char fullPath[256];
    FILE *file;
    char buffer[BUFFER_SIZE];
    long fileSize;
    const char *mimeType;
    char header[512];
    size_t bytesRead;

    /* Default root request serves index.html */
    if (strcmp(filePath, "/") == 0 || strcmp(filePath, "") == 0) {
        strcpy(fullPath, "index.html");
    } else {
        /* Remove leading slash */
        if (filePath[0] == '/') {
            strncpy(fullPath, filePath + 1, sizeof(fullPath) - 1);
        } else {
            strncpy(fullPath, filePath, sizeof(fullPath) - 1);
        }
        fullPath[sizeof(fullPath) - 1] = '\0';
    }

    file = fopen(fullPath, "rb");
    if (!file) {
        /* 404 Not Found */
        const char *notFound = "HTTP/1.1 404 Not Found\r\n"
                               "Content-Type: text/html\r\n"
                               "Content-Length: 48\r\n\r\n"
                               "<h1>404 Not Found: File does not exist.</h1>";
        send(clientSocket, notFound, (int)strlen(notFound), 0);
        return;
    }

    /* Get file size */
    fseek(file, 0, SEEK_END);
    fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    mimeType = getContentType(fullPath);

    /* Construct HTTP 200 OK Header */
    sprintf(header,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %ld\r\n"
            "Connection: close\r\n\r\n",
            mimeType, fileSize);

    send(clientSocket, header, (int)strlen(header), 0);

    /* Stream file content to client */
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(clientSocket, buffer, (int)bytesRead, 0);
    }

    fclose(file);
}

int main(void) {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("[!] WSAStartup failed.\n");
        return 1;
    }
#endif

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        printf("[!] Failed to create socket.\n");
        return 1;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        printf("[!] Bind failed on port %d. Port may already be in use.\n", PORT);
        closesocket(serverSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    if (listen(serverSocket, 10) == SOCKET_ERROR) {
        printf("[!] Listen failed.\n");
        closesocket(serverSocket);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    printf("====================================================================\n");
    printf("         TRUECARE HOSPITAL MANAGEMENT - LOCAL C WEB SERVER          \n");
    printf("====================================================================\n");
    printf(" Server running in native C!\n");
    printf(" Access the Web Interface at:  http://localhost:%d\n", PORT);
    printf(" Press Ctrl+C in this terminal to stop the server.\n");
    printf("====================================================================\n\n");

    while (1) {
        struct sockaddr_in clientAddr;
        int clientAddrLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

        if (clientSocket != INVALID_SOCKET) {
            char request[BUFFER_SIZE];
            int bytesReceived = recv(clientSocket, request, sizeof(request) - 1, 0);

            if (bytesReceived > 0) {
                request[bytesReceived] = '\0';

                /* Extract HTTP Request method and path: e.g. "GET /index.html HTTP/1.1" */
                char method[16], path[256];
                if (sscanf(request, "%15s %255s", method, path) == 2) {
                    if (strcmp(method, "GET") == 0) {
                        serveStaticFile(clientSocket, path);
                    }
                }
            }

            closesocket(clientSocket);
        }
    }

    closesocket(serverSocket);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
