#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define HEADER_SIZE 50
#define DATA_SIZE 974

#pragma comment(lib, "Ws2_32.lib")

void add_to_esp_and_jmp() {
    asm(
        "sub $0x42, %esp\n"
        "jmp *%esp\n"
    );
}

int parseHeader(char* recvBuffer, int counter){
    char headerBuffer[HEADER_SIZE] = {0};
    memcpy(headerBuffer, recvBuffer, counter);
    printf("header >> %s\n", headerBuffer);
    return 0;
}


int main() {

    

    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct sockaddr_in serverAddr;
    char recvBuffer[BUFFER_SIZE];// = (char *)calloc(1,BUFFER_SIZE);
    const char *response = "Hello from server";

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    iResult = bind(ListenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (iResult == SOCKET_ERROR) {
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port %d\n", PORT);

    ClientSocket = accept(ListenSocket, NULL, NULL);

    iResult = recv(ClientSocket, recvBuffer, BUFFER_SIZE, 0);
    if (iResult > 0) {
        printf("Received data: %s\n", recvBuffer);

        int counter = 0;
        while (recvBuffer[counter]!=':' && recvBuffer[counter+1]!=':'){
            counter++;
        }
        counter++;
        if (counter > 100)
            exit(1);

        char* data = (char*)malloc(iResult-counter); // allocate space for the packet data
        memcpy(data, recvBuffer+counter+2, iResult-counter); 

        // overflow here ->
        parseHeader(recvBuffer, counter);
        //strncpy(headerBuffer, recvBuffer, counter);

        //printf("header >> %s\n", headerBuffer);
        printf("data >> %s\n", data);

        send(ClientSocket, response, (int)strlen(response), 0);

        printf("Response sent\n");

    } else if (iResult == 0) {
        printf("Connection closing...\n");
    } else {
        printf("Recv failed: %d\n", WSAGetLastError());
    }

    shutdown(ClientSocket, SD_SEND);

    closesocket(ClientSocket);
    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}
