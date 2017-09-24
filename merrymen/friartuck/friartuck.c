#include <windows.h>
#include <winsock.h>
#include <stdio.h>
#include <string.h>
#include "sayings.h"

#define RCVBUFSIZE 32   /* Size of receive buffer */
#define MAXPENDING 5    /* Maximum outstanding connection requests */

void DieWithError(char *errorMessage)
{
	fprintf(stderr,"%s: %d\n", errorMessage, WSAGetLastError());
    exit(1);
}

void HandleTCPClient(int clntSocket, struct sockaddr_in echoClntAddr)
{
    char echoBuffer[RCVBUFSIZE];        /* Buffer for echo string */
    int recvMsgSize;                    /* Size of received message */
	int saying = 0;

    /* Receive message from client */
    if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE, 0)) < 0)
        DieWithError("recv() failed");

    /* Send received string and receive again until end of transmission */
    while (recvMsgSize > 0)      /* zero indicates end of transmission */
    {
        /* Echo message back to client */
        if (send(clntSocket, sayings[saying], strlen(sayings[saying]), 0)
				!= strlen(sayings[saying]))
            DieWithError("send() failed");

		saying++;
		if (saying > NUM_SAYINGS) saying = 0;

        /* See if there is more data to receive */
        if ((recvMsgSize = recv(clntSocket, echoBuffer, RCVBUFSIZE,0)) < 0)
            DieWithError("recv() failed");
    }

	printf("Closing connection to client %s\n", inet_ntoa(echoClntAddr.sin_addr));
    closesocket(clntSocket);    /* Close client socket */
}

int main(int argc, char **argv)
{
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in echoServAddr; /* Local address */
    struct sockaddr_in echoClntAddr; /* Client address */
    unsigned short echoServPort=5555;/* Server port */
    unsigned int clntLen;            /* Length of client address data structure */
    WSADATA wsaData;                 /* Structure for WinSock setup communication */

    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) /* Load Winsock 2.0 DLL */
    {
        DieWithError("WSAStartup() failed");
    }

    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");

    /* Construct local address structure */
    memset(&echoServAddr, 0, sizeof(echoServAddr));   /* Zero out structure */
    echoServAddr.sin_family = AF_INET;                /* Internet address family */
    echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    echoServAddr.sin_port = htons(echoServPort);      /* Local port */

    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
        DieWithError("bind() failed");

    /* Mark the socket so it will listen for incoming connections */
    if (listen(servSock, MAXPENDING) < 0)
        DieWithError("listen() failed");

	while (1)
	{
        /* Set the size of the in-out parameter */
        clntLen = sizeof(echoClntAddr);

        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
            DieWithError("accept() failed");

        /* clntSock is connected to a client! */

        printf("Handling client %s\n", inet_ntoa(echoClntAddr.sin_addr));

        HandleTCPClient(clntSock, echoClntAddr);
	}

	/* your cleanup goes here */
    return 0;
}
