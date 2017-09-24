#include <windows.h>
#include <winsvc.h>

/* specific to this service */
#include <winsock.h>

#define RCVBUFSIZE 32   /* Size of receive buffer */
#define MAXPENDING 5    /* Maximum outstanding connection requests */

extern BOOL ReportStatusToSCMgr ( DWORD, DWORD, DWORD );
extern void AddToMessageLog( LPTSTR, WORD );

int stop=0;

BOOL StartServer(char *command)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	memset(&pi, 0, sizeof(pi));
	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);

	return CreateProcess(NULL,command,NULL,NULL,0,NORMAL_PRIORITY_CLASS,NULL,NULL,&si,&pi);
}

void DieWithError(char *errorMessage)
{
	AddToMessageLog(TEXT(errorMessage), EVENTLOG_INFORMATION_TYPE);
	ReportStatusToSCMgr(SERVICE_STOPPED, NO_ERROR, 0);
    exit(0);
}

VOID ServiceStart(DWORD dwArgc, LPTSTR *lpszArgv)
{
    int sock;                        /* Socket descriptor */
    struct sockaddr_in servAddr;     /* Server address */
    unsigned short servPort=5555;    /* Server port */
    char *servIP="127.0.0.1";        /* Server IP address (dotted quad) */
    WSADATA wsaData;                 /* Structure for WinSock setup communication */

	ReportStatusToSCMgr ( SERVICE_START_PENDING, NO_ERROR, 3000 );

	Sleep(10);

	/* Report running status to Service Control Manage (SCM) */
	ReportStatusToSCMgr ( SERVICE_RUNNING, NO_ERROR, 0 );

	/* Load Winsock 2.0 DLL */
    if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) {
        DieWithError("WSAStartup() failed");
    }

	while (!stop)
	{
		/* Create a reliable, stream socket using TCP */
	    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    	    DieWithError("socket() failed");

	    /* Construct the server address structure */
    	memset(&servAddr, 0, sizeof(servAddr));         /* Zero out structure */
    	servAddr.sin_family      = AF_INET;             /* Internet address family */
   		servAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
    	servAddr.sin_port        = htons(servPort);     /* Server port */

    	if (connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
			AddToMessageLog(TEXT("server not responding, restarting"),
				EVENTLOG_INFORMATION_TYPE);
			if (!StartServer("friartuck")) {
				AddToMessageLog(TEXT("server cannot start"),
					EVENTLOG_INFORMATION_TYPE );
			}
		} else {
			closesocket(sock);
		}
		Sleep(1000);
	}

	Sleep(1);
	AddToMessageLog ( TEXT("stopping service"), EVENTLOG_INFORMATION_TYPE );
	/* Cleanup Winsock */
	WSACleanup();
	ReportStatusToSCMgr ( SERVICE_STOPPED, NO_ERROR, 0 );
	exit(0);
}


void ServiceStop ( void )
{
	stop = 1;
}


