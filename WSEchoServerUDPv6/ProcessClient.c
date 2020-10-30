
#include <stdio.h>      // for print functions
#include <stdlib.h>     // for exit() 
#include <winsock2.h>    // for WSAGetLastError() 
#include <WS2tcpip.h>
#define CLIENTBUFF 20
void DisplayFatalErr(char* message);
void ProcessClient(SOCKET* clientSock) {
	char clientBuffer[CLIENTBUFF + 1];
	int receiver = 1;
	while (receiver != 0) {
		receiver = recvfrom(clientSock, clientBuffer, CLIENTBUFF, 0);
		if (receiver < 0) {
			DisplayFatalErr("Receive failed.");
		}

		clientBuffer[receiver] = 0;

		send(clientSock, clientBuffer, receiver, 0);
		printf(clientBuffer);
		memset(clientBuffer, '\0', CLIENTBUFF);
		if (receiver < CLIENTBUFF) {
			break;
		}
	}
	printf("\n");
	closesocket(clientSock);
}