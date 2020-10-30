// CS 2690 Program 1 
// Simple Windows Sockets Echo Client (IPv6)
// Last update: 2/12/19
//Blake Thompson CS2690 Section 001 10/09/2020
// Windows 10, made in Visual Studio Community Edition v2019
//
// Usage: WSEchoClientv6 <server IPv6 address> <server port> <"message to echo">
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

// Minimum required header files for C Winsock program
#include <stdio.h>       // for print functions
#include <stdlib.h>      // for exit() 
#include <winsock2.h>	 // for Winsock2 functions
#include <ws2tcpip.h>    // adds support for getaddrinfo & getnameinfo for v4+6 name resolution
#include <Ws2ipdef.h>    // optional - needed for MS IP Helper

// #define ALL required constants HERE, not inline 
// #define is a macro, don't terminate with ';'  For example...
#define RCVBUFSIZ 50

// declare any functions located in other .c files here
void DisplayFatalErr(char* errMsg); // writes error message before abnormal termination

void main(int argc, char* argv[])
{
	// Declare ALL variables and structures for main() HERE, NOT INLINE (including the following...)
	WSADATA wsaData;                // contains details about WinSock DLL implementation
	struct sockaddr_in6 serverInfo;	// standard IPv6 structure that holds server socket info

	// Verify correct number of command line arguments
	if (argc != 4) {
		fprintf(stderr, "Invalid number of arguments!\n");
		exit(1); // ...and terminate with abnormal termination code (1)
	}

	// Retrieve the command line arguments. (Sanity checks not required, but port and IP addr will need
	// to be converted from char to int.  See slides 11-15 & 12-3 for details.)
	int port = 0;
	int ip = 0;
	__try {
		ip = atoi(argv[1]);
		port = atoi(argv[2]);
	}
	__except (printf("bad IP or Port")) { exit(1); }

	// Initialize Winsock 2.0 DLL. Returns 0 if ok. If this fails, fprint error message to stderr as above & exit(1).  
	WSAStartup(MAKEWORD(2, 0), &wsaData);
	// Create an IPv6 TCP stream socket.  
	int sock;
	__try { sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP); }
	//Now that Winsock DLL is loaded, we can signal any errors as shown on next line :
	__except (INVALID_SOCKET) {
		DisplayFatalErr("socket() function failed.");
	}
	// Display helpful confirmation messages after key socket calls like this:
	printf("Socket created successfully.\n");
	int perrno = -1;
	int v6Only = 0;
	// If doing extra credit IPv4 address handling option, add the setsockopt() call as follows...
	 if (perrno = setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, (char *)&v6Only, sizeof(v6Only)) != 0)
	     DisplayFatalErr("setsockopt() function failed.");  

	// Zero out the sockaddr_in6 structure and load server info into it.  See slide 11-15.
	// Don't forget any necessary format conversions.
	struct sockaddr_in6 serverinfo;

	// Server IP address and port number come from the client command line
	char* serverIPaddr = argv[1];//argv[1] is 1 in our test case looping back locally for ipv6.
	unsigned short serverPort = atoi(argv[2]);
	memset(&serverInfo, 0, sizeof(serverInfo));

	// Load server info into sockaddr_in6
	serverInfo.sin6_family = AF_INET6;
	serverInfo.sin6_port = htons(serverPort);
	// Convert cmd line server addr from char to ntwk form, load into sockaddr_in6
	inet_pton(AF_INET6, serverIPaddr, &serverInfo.sin6_addr);
	int msgLen = strlen(argv[3]);
	// Attempt connection to the server.  If it fails, call DisplayFatalErr() with appropriate message,
	// otherwise printf() confirmation message
	if (connect(sock, (struct sockaddr*) & serverInfo, sizeof(serverInfo)) < 0) {
		DisplayFatalErr("connect() function failed.");
	}
	else { printf("Connection succcessful\n"); }

	// Send message to server (without '\0' null terminator). Check for null msg (length=0) & verify all bytes were sent...
	int sent = send(sock, argv[3], msgLen, 0);
	if (sent != msgLen) {
		DisplayFatalErr("send failed.");
	}
	else {
		printf("trying to receive from server\n");
		int bytesRead = 0;
		int buffLength = (int)strlen(argv[3]);
		char rcvBuffer[RCVBUFSIZ];
		int byteTracker = 1;
		memset(rcvBuffer, 0, strlen(rcvBuffer));
		printf("reading back message:\n");
		// Retrieve the message returned by server.  Be sure you've read the whole thing (could be multiple segments). 
		// Manage receive buffer to prevent overflow with a big message.


		// Display ALL of the received message, in printable C string format.
		while (bytesRead != msgLen) {
			// if recv fails, DisplayFatalErr() with appropriate message as before
			byteTracker = recv(sock, rcvBuffer, RCVBUFSIZ - 1, 0);
			// Call DisplayFatalErr() if this fails.  (Lots can go wrong here, see slides.)
			if (byteTracker < 0) {
				DisplayFatalErr("Receive failed.");
			}
				bytesRead += byteTracker;
				//routine 1: get entire string in 1 try:
				if (byteTracker == msgLen) {
					rcvBuffer[strlen(rcvBuffer)] = 0;
					printf(rcvBuffer);
				}
				//routine 2: get less than entire string in 1 try:
				else if (bytesRead <= msgLen) {
					rcvBuffer[strlen(rcvBuffer)] = 0;
					printf(rcvBuffer);
				}
				memset(rcvBuffer, 0, strlen(rcvBuffer));
		}
		printf("\nBytes read: %d\n", bytesRead);
	}
	// Close the TCP connection (send a FIN) & print appropriate message.
	closesocket(sock);

	// Release the Winsock DLL

	WSACleanup();
	exit(0);
}

