// CS 2690 Program 2
// Simple Windows Sockets Echo Server(IPv6)
// Last update: 10/20/2020
//Blake Thompson CS2690 Section 001 10/20/2020
// Windows 10, made in Visual Studio Community Edition v2019
//
// Usage: WSEchoServer6 <server port> <"message to echo">
// Companion server is WSEchoServerv6
// Server usage: WSEchoServerv6 <server port>
//
// This program is coded in conventional C programming style, with the 
// exception of the C++ style comments.
//
// I declare that the following source code was written by me or provided
// by the instructor. I understand that copying source code from any other 
// source or posting solutions to programming assignments (code) on public
// Internet sites constitutes cheating, and that I will receive a zero 
// on this project if I violate this policy.
// ----------------------------------------------------------------------------


#include <stdio.h>      // for print functions
#include <stdlib.h>     // for exit() 
#include <winsock2.h>    // for WSAGetLastError() 
#include <WS2tcpip.h>

#define MAXQUEUED 3

void DisplayFatalErr(char* errMsg);
void ProcessClient(SOCKET *clientSock);

int main(int argc, char *argv[]) {

	unsigned short serverPort;     // 16-bit int port number    

	// argv[] is an array of pointers to those argument strings    
	serverPort = atoi(argv[1]);

	//Initializethe WinSock DLL.After a successful call to WSAStartup(), handle any errors by callingDisplayFatalErr().
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
		DisplayFatalErr("WSAStartup failed.");
	}
	//Create the server socket.
	SOCKET serverSocket;
	serverSocket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	// This references the 2nd string after the program name ("12345"),     
	// and converts it from a char string to a 16-bit unsigned int    

	struct sockaddr_in6 serverInfo;
	memset(&serverInfo, 0, sizeof(serverInfo)); // zero out the structure

	serverInfo.sin6_family = AF_INET6; // address family = IPv6
	serverInfo.sin6_port = htons(serverPort); //convert local port to big endian
	//Use "wildcard" (any server IPv6 interface address)
	serverInfo.sin6_addr = in6addr_any;

	bind(serverSocket, (struct sockaddr*) & serverInfo, sizeof(serverInfo));

	listen(serverSocket, MAXQUEUED);
	printf("BT's IPv6 echo server is ready for client connection\n");
	struct sockaddr_in6 clientInfo;
	int clientInfoLen = sizeof(clientInfo);

	//Enter a “forever” loop
	for (;;) {
		SOCKET clientSock = accept(serverSocket, (struct sockaddr*) & clientInfo, &clientInfoLen);
		char ipv6Array[INET6_ADDRSTRLEN];
		const IN6_ADDR clientAddr = clientInfo.sin6_addr;
		char* goodIp = inet_ntop(AF_INET6, &clientAddr, ipv6Array, sizeof(ipv6Array));
		printf("Processing the client at %s, client port %i\n", goodIp, ntohs(clientInfo.sin6_port));
		ProcessClient(*&clientSock);
	}
	 // For the overall size of a data type or structure in bytes (like     
	 // serverInfo), rather than the length of its contents, use the sizeof()// operator    



	printf("it compiled");
}
