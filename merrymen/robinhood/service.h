// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (C) 1993-1995  Microsoft Corporation.  All Rights Reserved.
//
//  MODULE: service.h
//
//
//  Comments:  The use of this header file and the accompanying service.c
//  file simplifies the process of writting a service.  You as a developer
//  simply need to follow the TODO's outlined in this header file, and
//  implement the ServiceStart() and ServiceStop() functions.
//
//  There is no need to modify the code in service.c.  Just add service.c
//  to your project and link with the following libraries...
//
//  libcmt.lib kernel32.lib advapi.lib shell32.lib
//
//  This code also supports unicode.  Be sure to compile both service.c and
//  and code #include "service.h" with the same Unicode setting.
//
//  Upon completion, your code will have the following command line interface
//
//  <service exe> -?                to display this list
//  <service exe> -install          to install the service
//  <service exe> -remove           to remove the service
//  <service exe> -debug <params>   to run as a console app for debugging
//
//  Note: This code also implements Ctrl+C and Ctrl+Break handlers
//        when using the debug option.  These console events cause
//        your ServiceStop routine to be called
//
//        Also, this code only handles the OWN_SERVICE service type
//        running in the LOCAL_SYSTEM security context.
//
//        To control your service ( start, stop, etc ) you may use the
//        Services control panel applet or the NET.EXE program.
//
//        To aid in writing/debugging service, the
//        SDK contains a utility (MSTOOLS\BIN\SC.EXE) that
//        can be used to control, configure, or obtain service status.
//        SC displays complete status for any service/driver
//        in the service database, and allows any of the configuration
//        parameters to be easily changed at the command line.
//        For more information on SC.EXE, type SC at the command line.
//

#ifndef _SERVICE_H
#define _SERVICE_H


#ifdef __cplusplus
extern "C" {
#endif


// internal name of the service
#define SZSERVICENAME        "RobinhoodTest"  /* this is the name used
                                               to programaticaly
                                               stop and start service */

// displayed name of the service
#define SZSERVICEDISPLAYNAME "Robinhood"    /* this is the name shown in the
                                               service manager for NT */

// list of service dependencies - "dep1\0dep2\0\0"
#define SZDEPENDENCIES       ""   // For NT 4.0  /* no idea what this is for, any one?  */

void ServiceStart(DWORD dwArgc, LPTSTR *lpszArgv);
void ServiceStop( void );
//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
//// The following are procedures which
//// may be useful to call within the above procedures,
//// but require no implementation by the user.
//// They are implemented in service.c

//
//  FUNCTION: ReportStatusToSCMgr()
//
//  PURPOSE: Sets the current status of the service and
//           reports it to the Service Control Manager
//
//  PARAMETERS:
//    dwCurrentState - the state of the service
//    dwWin32ExitCode - error code to report
//    dwWaitHint - worst case estimate to next checkpoint
//
//  RETURN VALUE:
//    TRUE  - success
//    FALSE - failure
//
//int ReportStatusToSCMgr( long dwCurrentState, long dwWin32ExitCode, long dwWaitHint);


//
//  FUNCTION: AddToMessageLog(LPTSTR lpszMsg)
//
//  PURPOSE: Allows any thread to log an error message
//
//  PARAMETERS:
//    lpszMsg - text for message
//
//  RETURN VALUE:
//    none
//
extern void AddToMessageLog(LPTSTR lpszMsg, WORD);
//////////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
}
#endif

#endif
