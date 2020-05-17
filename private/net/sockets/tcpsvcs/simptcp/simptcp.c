/*++

Copyright (c) 1992 Microsoft Corporation

Module Name:

    Simpsvc.c

Abstract:

    Supports several simple TCP/IP services in a single thread: TCP Echo,
    UDP Echo, Daytime, Null, Chargen.

Author:

    David Treadwell (davidtr)    3-Mar-1993

Revision History:

--*/

#define FD_SETSIZE 65536

#include "simptcp.h"

#define MAX_UDP_CHARGEN_RESPONSE 7030

DWORD IoBufferSize = 4096;
PCHAR IoBuffer = NULL;

WSADATA WsaData;

RTL_CRITICAL_SECTION CriticalSection;
BOOL InitializedCriticalSection = FALSE;

typedef struct _TCP_CLIENT_INFO {
    SOCKET SocketHandle;
    SOCKADDR_IN RemoteAddress;
    HANDLE ThreadHandle;
    SHORT ServicePort;
} TCP_CLIENT_INFO, *PTCP_CLIENT_INFO;

#define MAX_TCP_CLIENTS 1000
PTCP_CLIENT_INFO TcpClients = NULL;

#define LISTEN_BACKLOG 5

#define MAX_IDLE_TICKS 10 * 60 * 1000    // 10 minutes
#define SELECT_TIMEOUT 5 * 60            // 5 minutes

DWORD MaxTcpClients = MAX_TCP_CLIENTS;
DWORD MaxIdleTicks = MAX_IDLE_TICKS;
DWORD SelectTimeout = SELECT_TIMEOUT;

PFD_SET ReadfdsStore, Readfds, WritefdsStore, Writefds;

SHORT TcpEchoPort;
SHORT UdpEchoPort;
SHORT TcpDiscardPort;
SHORT UdpDiscardPort;
SHORT TcpChargenPort;
SHORT UdpChargenPort;
SHORT TcpDaytimePort;
SHORT UdpDaytimePort;
SHORT TcpQotdPort;
SHORT UdpQotdPort;

#define INVALID_PORT 0

BOOL DoTcpEcho = TRUE;
BOOL DoUdpEcho = TRUE;
BOOL DoTcpDiscard = TRUE;
BOOL DoUdpDiscard = TRUE;
BOOL DoTcpChargen = TRUE;
BOOL DoUdpChargen = TRUE;
BOOL DoTcpDaytime = TRUE;
BOOL DoUdpDaytime = TRUE;
BOOL DoTcpQotd = TRUE;
BOOL DoUdpQotd = TRUE;

struct {
    PBOOL Boolean;
    PWSTR ValueName;
} RegistryBooleans[] = {
    &DoTcpEcho, L"EnableTcpEcho",
    &DoUdpEcho, L"EnableUdpEcho",
    &DoTcpDiscard, L"EnableTcpDiscard",
    &DoUdpDiscard, L"EnableUdpDiscard",
    &DoTcpChargen, L"EnableTcpChargen",
    &DoUdpChargen, L"EnableUdpChargen",
    &DoTcpDaytime, L"EnableTcpDaytime",
    &DoUdpDaytime, L"EnableUdpDaytime",
    &DoTcpQotd, L"EnableTcpQotd",
    &DoUdpQotd, L"EnableUdpQotd",
    NULL, NULL
};

struct {
    PDWORD Dword;
    PWSTR ValueName;
} RegistryDwords[] = {
    &MaxTcpClients, L"MaxTcpClients",
    &MaxIdleTicks, L"MaxIdleTicks",
    &SelectTimeout, L"SelectTimeout",
    &IoBufferSize, L"IoBufferSize",
    NULL, NULL
};

SERVICE_STATUS SimpServiceStatus;
SERVICE_STATUS_HANDLE SimpServiceStatusHandle;

HANDLE SimpPauseEvent;
SOCKET SimpQuitSocket;

BOOL SimpServiceExit = FALSE;

PVOID ChargenBuffer = NULL;
DWORD ChargenBufferSize;

PVOID QotdBuffer = NULL;
DWORD QotdQuoteCount;
struct {
    DWORD QuoteLength;
    PCHAR Quote;
} *QotdStrings = NULL;
PWSTR QotdFileName = NULL;
HANDLE QotdFileHandle = NULL;
HANDLE QotdFileMapping = NULL;

VOID
AnnounceServiceStatus (
    VOID
    );

VOID
ControlResponse(
    DWORD opCode
    );

VOID
AbortTcpClient (
    IN SOCKET Socket
    );

INT
AcceptTcpClient (
    IN SOCKET ListenSocket,
    IN SHORT Port
    );

VOID
DeleteTcpClient (
    IN DWORD ArraySlot,
    IN BOOLEAN Graceful
    );

VOID
DoSimpleServices (
    VOID
    );

VOID
DoSingleClient (
    IN SOCKET s,
    IN USHORT port
    );

VOID
FormatDaytimeResponse (
    IN PCHAR Buffer,
    IN PDWORD BufferLength
    );

VOID
FormatQotdResponse (
    IN PCHAR Buffer,
    IN PDWORD BufferLength
    );

SHORT
GetServicePort (
    IN PCHAR Service,
    IN PCHAR Protocol
    );

INT
InitializeChargen (
    VOID
    );

INT
InitializeQotdQuotes (
    VOID
    );

INT
ReadRegistry (
    VOID
    );

SOCKET
OpenTcpSocket (
    IN SHORT Port
    );

SOCKET
OpenUdpSocket (
    IN SHORT Port
    );

INT
SimpInitializeEventLog (
    VOID
    );

VOID
SimpTerminateEventLog(
    VOID
    );

VOID
SimpLogEvent(
    DWORD   Message,
    WORD    SubStringCount,
    CHAR    *SubStrings[],
    DWORD   ErrorCode
    );

DWORD
ThreadEntry (
    LPVOID lpThreadParameter
    );


VOID
ServiceEntry (
    IN DWORD argc,
    IN LPWSTR argv[],
    IN PTCPSVCS_GLOBAL_DATA pGlobalData
    )

/*++

Routine Description:

    This is the "main" routine for the simple TCP/IP services.  The
    containing process will call this routine when we're supposed to
    start up.

Arguments:

Return Value:

    None.

--*/

{
    SOCKET tcpEcho = INVALID_SOCKET;
    SOCKET udpEcho = INVALID_SOCKET;
    SOCKET tcpDaytime = INVALID_SOCKET;
    SOCKET udpDaytime = INVALID_SOCKET;
    SOCKET tcpDiscard = INVALID_SOCKET;
    SOCKET udpDiscard = INVALID_SOCKET;
    SOCKET tcpChargen = INVALID_SOCKET;
    SOCKET udpChargen = INVALID_SOCKET;
    SOCKET tcpQotd = INVALID_SOCKET;
    SOCKET udpQotd = INVALID_SOCKET;
    INT err;
    TIMEVAL timeout;
    INT i;
    SOCKADDR_IN remoteAddr;
    INT remoteAddrLength;
    u_long one = 1;
    DWORD maxFdSetSize;
    NTSTATUS status;

    //
    // Initialize all the status fields so that subsequent calls to
    // SetServiceStatus need to only update fields that changed.
    //

    SimpServiceStatus.dwServiceType = SERVICE_WIN32;
    SimpServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    SimpServiceStatus.dwControlsAccepted = 0;
    SimpServiceStatus.dwCheckPoint = 1;
    SimpServiceStatus.dwWaitHint = 30000;  // 30 seconds

    SimpServiceStatus.dwWin32ExitCode = NO_ERROR;
    SimpServiceStatus.dwServiceSpecificExitCode = NO_ERROR;

    //
    // Initialize server to receive service requests by registering the
    // control handler.
    //

    SimpServiceStatusHandle = RegisterServiceCtrlHandler(
                                   TEXT("SimpTcp"),
                                   ControlResponse
                                   );

    if ( SimpServiceStatusHandle == 0 ) {
        err = GetLastError();
        goto exit;
    }

    AnnounceServiceStatus( );

    //
    // Initialize our critical section.
    //

    status = RtlInitializeCriticalSection( &CriticalSection );
    if ( !NT_SUCCESS(status) ) {
        goto exit;
    }

    InitializedCriticalSection = TRUE;

    //
    // Initialize the eventlog.
    //

    err = SimpInitializeEventLog( );
    ASSERT( err == NO_ERROR );

    //
    // Read all registry information.
    //

    err = ReadRegistry( );
    if ( err != NO_ERROR ) {
        goto exit;
    }

    //
    // Allocate memory for the IO buffer.
    //

    IoBuffer = RtlAllocateHeap( RtlProcessHeap( ), 0, IoBufferSize );

    //
    // Allocate memory for the array of TCP clients.
    //

    TcpClients = RtlAllocateHeap(
                     RtlProcessHeap( ),
                     0,
                     MaxTcpClients * sizeof(TcpClients[0])
                     );
    if ( TcpClients == NULL ) {
        err = ERROR_NOT_ENOUGH_MEMORY;
        goto exit;
    }

    //
    // Initialize the chargen data buffer.
    //

    if ( DoTcpChargen || DoUdpChargen ) {
        err = InitializeChargen( );
        if ( err != NO_ERROR ) {
            DoUdpChargen = FALSE;
            DoTcpChargen = FALSE;
        }
    }

    //
    // Initialize the quote of the day quotes.
    //

    if ( DoTcpQotd || DoUdpQotd ) {
        err = InitializeQotdQuotes( );
        if ( err != NO_ERROR ) {
            DoUdpQotd = FALSE;
            DoTcpQotd = FALSE;
        }
    }

    //
    // Initialize client socket array.
    //

    for ( i = 0; (DWORD)i < MaxTcpClients; i++ ) {
        TcpClients[i].SocketHandle = INVALID_SOCKET;
        TcpClients[i].ThreadHandle = NULL;
    }

    //
    // Determine how large our FD_SET structures must be, then allocate
    // space for them.
    //

    maxFdSetSize = 4 + ( (20 + MaxTcpClients) * sizeof(SOCKET) );


    Readfds = RtlAllocateHeap( RtlProcessHeap( ), 0, maxFdSetSize );
    ReadfdsStore = RtlAllocateHeap( RtlProcessHeap( ), 0, maxFdSetSize );
    Writefds = RtlAllocateHeap( RtlProcessHeap( ), 0, maxFdSetSize );
    WritefdsStore = RtlAllocateHeap( RtlProcessHeap( ), 0, maxFdSetSize );

    if ( Readfds == NULL || ReadfdsStore == NULL ||
             Writefds == NULL || WritefdsStore == NULL ) {
        err = ERROR_NOT_ENOUGH_MEMORY;
        goto exit;
    }

    //
    // Initialize the pause event.  We use this event to stop activity
    // when the service is paused.
    //

    SimpPauseEvent = CreateEvent( NULL, TRUE, TRUE, NULL );
    if ( SimpPauseEvent == NULL ) {
        err = GetLastError( );
        goto exit;
    }

    //
    // Initialize the Windows Sockets DLL.
    //

    err = WSAStartup( 0x0101, &WsaData );
    if ( err == SOCKET_ERROR ) {
        err = GetLastError( );
        goto exit;
    }

    //
    // Initialize the FD sets we'll use.
    //

    FD_ZERO( ReadfdsStore );
    FD_ZERO( WritefdsStore );

    //
    // Open the "quit" socket.  We close this socket when we need to
    // shut down in order to wake up the main thread from it's select()
    // and begin shutdown.
    //

    SimpQuitSocket = socket( AF_INET, SOCK_DGRAM, 0 );
    if ( SimpQuitSocket != INVALID_SOCKET ) {
        FD_SET( SimpQuitSocket, ReadfdsStore );
    } else {
        err = GetLastError( );
        goto exit;
    }

    //
    // First find the port numbers for all our services.
    //

    TcpEchoPort = GetServicePort( "echo", "tcp" );
    if ( TcpEchoPort == INVALID_PORT && DoTcpEcho ) {
        SimpLogEvent(
            SIMPTCP_CANT_FIND_TCP_ECHO_PORT,
            0,
            NULL,
            WSAGetLastError( )
            );

        DoTcpEcho = FALSE;
    }

    UdpEchoPort = GetServicePort( "echo", "udp" );
    if ( UdpEchoPort == INVALID_PORT && DoUdpEcho ) {
        SimpLogEvent(
            SIMPTCP_CANT_FIND_UDP_ECHO_PORT,
            0,
            NULL,
            WSAGetLastError( )
            );

        DoUdpEcho = FALSE;
    }

    TcpDiscardPort = GetServicePort( "discard", "tcp" );
    if ( TcpDiscardPort == INVALID_PORT && DoTcpDiscard ) {
        SimpLogEvent(
            SIMPTCP_CANT_FIND_TCP_DISCARD_PORT,
            0,
            NULL,
            WSAGetLastError( )
            );

        DoTcpDiscard = FALSE;
    }

    UdpDiscardPort = GetServicePort( "discard", "udp" );
    if ( UdpDiscardPort == INVALID_PORT && DoUdpDiscard ) {
        SimpLogEvent(
            SIMPTCP_CANT_FIND_UDP_DISCARD_PORT,
            0,
            NULL,
            WSAGetLastError( )
            );

        DoUdpDiscard = FALSE;
    }

    TcpDaytimePort = GetServicePort( "daytime", "tcp" );
    if ( TcpDaytimePort == INVALID_PORT && DoTcpDaytime ) {
        SimpLogEvent(
            SIMPTCP_CANT_FIND_TCP_DAYTIME_PORT,
            0,
            NULL,
            WSAGetLastError( )
            );

        DoTcpDaytime = FALSE;
    }

    UdpDaytimePort = GetServicePort( "daytime", "udp" );
    if ( UdpDaytimePort == INVALID_PORT && DoUdpDaytime ) {
        SimpLogEvent(
            SIMPTCP_CANT_FIND_UDP_DAYTIME_PORT,
            0,
            NULL,
            WSAGetLastError( )
            );

        DoUdpDaytime = FALSE;
    }

    TcpChargenPort = GetServicePort( "chargen", "tcp" );
    if ( TcpChargenPort == INVALID_PORT && DoTcpChargen ) {
        SimpLogEvent(
            SIMPTCP_CANT_FIND_TCP_CHARGEN_PORT,
            0,
            NULL,
            WSAGetLastError( )
            );

        DoTcpChargen = FALSE;
    }

    UdpChargenPort = GetServicePort( "chargen", "udp" );
    if ( UdpChargenPort == INVALID_PORT && DoUdpChargen ) {
        SimpLogEvent(
            SIMPTCP_CANT_FIND_UDP_CHARGEN_PORT,
            0,
            NULL,
            WSAGetLastError( )
            );

        DoUdpChargen = FALSE;
    }

    TcpQotdPort = GetServicePort( "qotd", "tcp" );
    if ( TcpQotdPort == INVALID_PORT && DoTcpQotd ) {
        SimpLogEvent(
            SIMPTCP_CANT_FIND_TCP_QOTD_PORT,
            0,
            NULL,
            WSAGetLastError( )
            );

        DoTcpQotd = FALSE;
    }

    UdpQotdPort = GetServicePort( "qotd", "udp" );
    if ( UdpQotdPort == INVALID_PORT && DoUdpQotd ) {
        SimpLogEvent(
            SIMPTCP_CANT_FIND_UDP_QOTD_PORT,
            0,
            NULL,
            WSAGetLastError( )
            );

        DoUdpQotd = FALSE;
    }

    //
    // Open, bind, and listen on the necessary ports.
    //

    if ( DoTcpEcho ) {
        tcpEcho = OpenTcpSocket( TcpEchoPort );
        if ( tcpEcho != INVALID_SOCKET ) {
            FD_SET( tcpEcho, ReadfdsStore );
        } else {
            SimpLogEvent(
                SIMPTCP_CANT_OPEN_TCP_ECHO_PORT,
                0,
                NULL,
                WSAGetLastError( )
                );
        }
    }

    if ( DoUdpEcho ) {
        udpEcho = OpenUdpSocket( UdpEchoPort );
        if ( udpEcho != INVALID_SOCKET ) {
            FD_SET( udpEcho, ReadfdsStore );
        } else {
            SimpLogEvent(
                SIMPTCP_CANT_OPEN_UDP_ECHO_PORT,
                0,
                NULL,
                WSAGetLastError( )
                );
        }
    }

    if ( DoTcpDiscard ) {
        tcpDiscard = OpenTcpSocket( TcpDiscardPort );
        if ( tcpDiscard != INVALID_SOCKET ) {
            FD_SET( tcpDiscard, ReadfdsStore );
        } else {
            SimpLogEvent(
                SIMPTCP_CANT_OPEN_TCP_DISCARD_PORT,
                0,
                NULL,
                WSAGetLastError( )
                );
        }
    }

    if ( DoUdpDiscard ) {
        udpDiscard = OpenUdpSocket( UdpDiscardPort );
        if ( udpDiscard != INVALID_SOCKET ) {
            FD_SET( udpDiscard, ReadfdsStore );
        } else {
            SimpLogEvent(
                SIMPTCP_CANT_OPEN_UDP_DISCARD_PORT,
                0,
                NULL,
                WSAGetLastError( )
                );
        }
    }

    if ( DoTcpDaytime ) {
        tcpDaytime = OpenTcpSocket( TcpDaytimePort );
        if ( tcpDaytime != INVALID_SOCKET ) {
            FD_SET( tcpDaytime, ReadfdsStore );
        } else {
            SimpLogEvent(
                SIMPTCP_CANT_OPEN_TCP_DAYTIME_PORT,
                0,
                NULL,
                WSAGetLastError( )
                );
        }
    }

    if ( DoUdpDaytime ) {
        udpDaytime = OpenUdpSocket( UdpDaytimePort );
        if ( udpDaytime != INVALID_SOCKET ) {
            FD_SET( udpDaytime, ReadfdsStore );
        } else {
            SimpLogEvent(
                SIMPTCP_CANT_OPEN_UDP_DAYTIME_PORT,
                0,
                NULL,
                WSAGetLastError( )
                );
        }
    }

    if ( DoTcpChargen ) {
        tcpChargen = OpenTcpSocket( TcpChargenPort );
        if ( tcpChargen != INVALID_SOCKET ) {
            FD_SET( tcpChargen, ReadfdsStore );
        } else {
            SimpLogEvent(
                SIMPTCP_CANT_OPEN_TCP_CHARGEN_PORT,
                0,
                NULL,
                WSAGetLastError( )
                );
        }
    }

    if ( DoUdpChargen ) {
        udpChargen = OpenUdpSocket( UdpChargenPort );
        if ( udpChargen != INVALID_SOCKET ) {
            FD_SET( udpChargen, ReadfdsStore );
        } else {
            SimpLogEvent(
                SIMPTCP_CANT_OPEN_UDP_CHARGEN_PORT,
                0,
                NULL,
                WSAGetLastError( )
                );
        }
    }

    if ( DoTcpQotd ) {
        tcpQotd = OpenTcpSocket( TcpQotdPort );
        if ( tcpQotd != INVALID_SOCKET ) {
            FD_SET( tcpQotd, ReadfdsStore );
        } else {
            SimpLogEvent(
                SIMPTCP_CANT_OPEN_TCP_QOTD_PORT,
                0,
                NULL,
                WSAGetLastError( )
                );
        }
    }

    if ( DoUdpQotd ) {
        udpQotd = OpenUdpSocket( UdpQotdPort );
        if ( udpQotd != INVALID_SOCKET ) {
            FD_SET( udpQotd, ReadfdsStore );
        } else {
            SimpLogEvent(
                SIMPTCP_CANT_OPEN_UDP_QOTD_PORT,
                0,
                NULL,
                WSAGetLastError( )
                );
        }
    }

    //
    // Announce that we have successfully started.
    //

    SimpServiceStatus.dwCurrentState = SERVICE_RUNNING;
    SimpServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP |
                                         SERVICE_ACCEPT_PAUSE_CONTINUE;
    SimpServiceStatus.dwCheckPoint = 0;
    SimpServiceStatus.dwWaitHint = 0;

    AnnounceServiceStatus( );

    //
    // Loop waiting for connect attempts or datagrams, and service them
    // when they arrive.
    //

    while ( TRUE ) {

        //
        // First initialize the FD sets we'll actually use for select().
        //

        RtlCopyMemory( Readfds, ReadfdsStore, maxFdSetSize );
        RtlCopyMemory( Writefds, WritefdsStore, maxFdSetSize );

        //
        // Now wait for something to happen.  Timeout occaisonally
        // so that we can kill idle TCP clients.
        //

        timeout.tv_sec = SelectTimeout;
        timeout.tv_usec = 0;

        err = select( 0, Readfds, Writefds, NULL, &timeout );

        //
        // If the service is shutting down, stop processing requests
        // and exit.
        //

        if ( SimpServiceExit ) {
            err = NO_ERROR;
            goto exit;
        }

        if ( err == SOCKET_ERROR ) {

            //
            // This is bad.  We should do something intelligent here.
            //

            continue;
        }

        //
        // If the service is paused, wait for it to become unpaused.
        //

        err = WaitForSingleObject( SimpPauseEvent, INFINITE );
        ASSERT( err != WAIT_FAILED );

        //
        // Figure out what happened and act accordingly.
        //

        if ( tcpEcho != INVALID_SOCKET && FD_ISSET( tcpEcho, Readfds ) ) {
            i = AcceptTcpClient( tcpEcho, TcpEchoPort );
            if ( i != -1 ) {
                FD_SET( TcpClients[i].SocketHandle, ReadfdsStore );
            }
        }

        if ( tcpDiscard != INVALID_SOCKET && FD_ISSET( tcpDiscard, Readfds ) ) {
            i = AcceptTcpClient( tcpDiscard, TcpDiscardPort );
            if ( i != -1 ) {
                FD_SET( TcpClients[i].SocketHandle, ReadfdsStore );
            }
        }

        if ( tcpDaytime != INVALID_SOCKET && FD_ISSET( tcpDaytime, Readfds ) ) {

            SOCKET acceptSocket;
            DWORD length;

            //
            // A client is making a TCP daytime request.  First accept
            // the connection, then send the current time-of-day string
            // to the client, then close the socket.
            //

            acceptSocket = accept( tcpDaytime, NULL, NULL );

            if ( acceptSocket != INVALID_SOCKET ) {
                FormatDaytimeResponse( IoBuffer, &length );
                send( acceptSocket, IoBuffer, length, 0 );
                err = closesocket( acceptSocket );
                ASSERT( err != SOCKET_ERROR );
            }
        }

        if ( tcpChargen != INVALID_SOCKET && FD_ISSET( tcpChargen, Readfds ) ) {
            i = AcceptTcpClient( tcpChargen, TcpChargenPort );
            if ( i != -1 ) {
                FD_SET( TcpClients[i].SocketHandle, ReadfdsStore );
                FD_SET( TcpClients[i].SocketHandle, WritefdsStore );
                one = 1;
                err = ioctlsocket( TcpClients[i].SocketHandle, FIONBIO, &one );
                if ( err == SOCKET_ERROR ) {
                    DeleteTcpClient( i, FALSE );
                }
            }
        }

        if ( tcpQotd != INVALID_SOCKET && FD_ISSET( tcpQotd, Readfds ) ) {

            SOCKET acceptSocket;
            DWORD length;

            //
            // A client is making a TCP Qotd request.  First accept
            // the connection, then send the current time-of-day string
            // to the client, then close the socket.
            //

            acceptSocket = accept( tcpQotd, NULL, NULL );

            if ( acceptSocket != INVALID_SOCKET ) {
                FormatQotdResponse( IoBuffer, &length );
                send( acceptSocket, IoBuffer, length, 0 );
                err = closesocket( acceptSocket );
                ASSERT( err != SOCKET_ERROR );
            }
        }

        if ( udpEcho != INVALID_SOCKET && FD_ISSET( udpEcho, Readfds ) ) {

            remoteAddrLength = sizeof(remoteAddr);

            err = recvfrom(
                      udpEcho,
                      IoBuffer,
                      IoBufferSize,
                      0,
                      (PSOCKADDR)&remoteAddr,
                      &remoteAddrLength
                      );

            if ( err != SOCKET_ERROR ) {
                err = sendto(
                          udpEcho,
                          IoBuffer,
                          err,
                          0,
                          (PSOCKADDR)&remoteAddr,
                          remoteAddrLength
                          );
            }
        }

        if ( udpDiscard != INVALID_SOCKET && FD_ISSET( udpDiscard, Readfds ) ) {
            err = recvfrom(
                      udpDiscard,
                      IoBuffer,
                      IoBufferSize,
                      0,
                      NULL,
                      NULL
                      );
            ASSERT( err != SOCKET_ERROR );
        }

        if ( udpDaytime != INVALID_SOCKET && FD_ISSET( udpDaytime, Readfds ) ) {

            DWORD length;

            remoteAddrLength = sizeof(remoteAddr);

            err = recvfrom(
                      udpDaytime,
                      IoBuffer,
                      IoBufferSize,
                      0,
                      (PSOCKADDR)&remoteAddr,
                      &remoteAddrLength
                      );

            FormatDaytimeResponse( IoBuffer, &length );

            err = sendto(
                      udpDaytime,
                      IoBuffer,
                      length,
                      0,
                      (PSOCKADDR)&remoteAddr,
                      remoteAddrLength
                      );
        }

        if ( udpChargen != INVALID_SOCKET && FD_ISSET( udpChargen, Readfds ) ) {

            DWORD length;

            remoteAddrLength = sizeof(remoteAddr);

            err = recvfrom(
                      udpChargen,
                      IoBuffer,
                      IoBufferSize,
                      0,
                      (PSOCKADDR)&remoteAddr,
                      &remoteAddrLength
                      );

            srand( GetTickCount( ) );

            length = (rand( ) * MAX_UDP_CHARGEN_RESPONSE) / RAND_MAX;

            err = sendto(
                      udpChargen,
                      ChargenBuffer,
                      length,
                      0,
                      (PSOCKADDR)&remoteAddr,
                      remoteAddrLength
                      );
        }

        if ( udpQotd != INVALID_SOCKET && FD_ISSET( udpQotd, Readfds ) ) {

            DWORD length;

            remoteAddrLength = sizeof(remoteAddr);

            err = recvfrom(
                      udpQotd,
                      IoBuffer,
                      IoBufferSize,
                      0,
                      (PSOCKADDR)&remoteAddr,
                      &remoteAddrLength
                      );

            FormatQotdResponse( IoBuffer, &length );

            err = sendto(
                      udpQotd,
                      IoBuffer,
                      length,
                      0,
                      (PSOCKADDR)&remoteAddr,
                      remoteAddrLength
                      );
        }
    }

exit:

    //
    // Announce that we're going down.
    //

    SimpServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
    SimpServiceStatus.dwCheckPoint = 1;
    SimpServiceStatus.dwWaitHint = 20000;   // 20 seconds

    SimpServiceStatus.dwWin32ExitCode = err;
    SimpServiceStatus.dwServiceSpecificExitCode = err;

    AnnounceServiceStatus( );

    //
    // Delete our critical section.
    //

    if ( InitializedCriticalSection ) {
        InitializedCriticalSection = FALSE;
        RtlDeleteCriticalSection( &CriticalSection );
    }

    //
    // Close all opened listening sockets.
    //

    if ( tcpEcho != INVALID_SOCKET ) {
        closesocket( tcpEcho );
    }
    if ( udpEcho != INVALID_SOCKET ) {
        closesocket( udpEcho );
    }
    if ( tcpDiscard != INVALID_SOCKET ) {
        closesocket( tcpDiscard );
    }
    if ( udpDiscard != INVALID_SOCKET ) {
        closesocket( udpDiscard );
    }
    if ( tcpDaytime != INVALID_SOCKET ) {
        closesocket( tcpDaytime );
    }
    if ( udpDaytime != INVALID_SOCKET ) {
        closesocket( udpDaytime );
    }
    if ( tcpChargen != INVALID_SOCKET ) {
        closesocket( tcpChargen );
    }
    if ( udpChargen != INVALID_SOCKET ) {
        closesocket( udpChargen );
    }

    //
    // Close all connected TCP sockets.
    //

    for ( i = 0; TcpClients != NULL && (DWORD)i < MaxTcpClients; i++ ) {

        if ( TcpClients[i].SocketHandle != INVALID_SOCKET ) {
            AbortTcpClient( TcpClients[i].SocketHandle );
        }
    }

    //
    // Should wait here for all threads to exit!
    //

    //
    // Deinitialize the eventlog.
    //

    SimpTerminateEventLog( );

    //
    // Free allocated memory.
    //

    if ( IoBuffer != NULL ) {
        RtlFreeHeap( RtlProcessHeap( ), 0, IoBuffer );
    }

    if ( TcpClients != NULL ) {
        RtlFreeHeap( RtlProcessHeap( ), 0, TcpClients );
    }

    if ( Readfds != NULL ) {
        RtlFreeHeap( RtlProcessHeap( ), 0, Readfds );
    }

    if ( ReadfdsStore != NULL ) {
        RtlFreeHeap( RtlProcessHeap( ), 0, ReadfdsStore );
    }

    if ( Writefds != NULL ) {
        RtlFreeHeap( RtlProcessHeap( ), 0, Writefds );
    }

    if ( WritefdsStore != NULL ) {
        RtlFreeHeap( RtlProcessHeap( ), 0, WritefdsStore );
    }

    if ( ChargenBuffer != NULL ) {
        RtlFreeHeap( RtlProcessHeap( ), 0, ChargenBuffer );
    }

    if ( QotdBuffer != NULL ) {
        UnmapViewOfFile( QotdBuffer );
    }

    if ( QotdFileMapping != NULL ) {
        CloseHandle( QotdFileMapping );
    }

    if ( QotdFileHandle != NULL ) {
        CloseHandle( QotdFileHandle );
    }

    if ( QotdFileName != NULL ) {
        RtlFreeHeap( RtlProcessHeap( ), 0, QotdFileName );
    }

    if ( QotdStrings != NULL ) {
        RtlFreeHeap( RtlProcessHeap( ), 0, QotdStrings );
    }

    //
    // Announce that we're down.
    //

    SimpServiceStatus.dwCurrentState = SERVICE_STOPPED;
    SimpServiceStatus.dwControlsAccepted = 0;
    SimpServiceStatus.dwCheckPoint = 0;
    SimpServiceStatus.dwWaitHint = 0;

    SimpServiceStatus.dwWin32ExitCode = err;
    SimpServiceStatus.dwServiceSpecificExitCode = err;

    AnnounceServiceStatus( );

    return;

} // ServiceEntry


SOCKET
OpenTcpSocket (
    IN SHORT Port
    )
{
    SOCKET s;
    SOCKADDR_IN localAddr;
    INT err;
    INT one = 1;

    s = socket( AF_INET, SOCK_STREAM, 0 );
    if ( s == INVALID_SOCKET ) {
        return s;
    }

    RtlZeroMemory( &localAddr, sizeof(localAddr) );
    localAddr.sin_port = Port;
    localAddr.sin_family = AF_INET;

    err = bind( s, (PSOCKADDR)&localAddr, sizeof(localAddr) );
    if ( err ==SOCKET_ERROR ) {
        closesocket(s);
        return INVALID_SOCKET;
    }

    err = listen( s, LISTEN_BACKLOG );
    if ( err == SOCKET_ERROR ) {
        closesocket(s);
        return INVALID_SOCKET;
    }

    err = setsockopt( s, SOL_SOCKET, SO_KEEPALIVE, (char *)&one, sizeof(one) );
    if ( err == INVALID_SOCKET ) {
        closesocket(s);
        return INVALID_SOCKET;
    }

    return s;

} // OpenTcpSocket


SOCKET
OpenUdpSocket (
    IN SHORT Port
    )
{
    SOCKET s;
    SOCKADDR_IN localAddr;
    INT err;

    s = socket( AF_INET, SOCK_DGRAM, 0 );
    if ( s == INVALID_SOCKET ) {
        return s;
    }

    RtlZeroMemory( &localAddr, sizeof(localAddr) );
    localAddr.sin_port = Port;
    localAddr.sin_family = AF_INET;

    err = bind( s, (PSOCKADDR)&localAddr, sizeof(localAddr) );
    if ( err == SOCKET_ERROR ) {
        closesocket(s);
        return INVALID_SOCKET;
    }

    return s;

} // OpenUdpSocket


INT
AcceptTcpClient (
    IN SOCKET ListenSocket,
    IN SHORT Port
    )
{
    SOCKADDR_IN remoteSockaddr;
    INT remoteSockaddrLength;
    DWORD i;
    SOCKET acceptSocket;
    DWORD threadId;
    NTSTATUS status;

    //
    // Always accept the socket first.
    //

    remoteSockaddrLength = sizeof(remoteSockaddr);

    acceptSocket =
        accept( ListenSocket, (PSOCKADDR)&remoteSockaddr, &remoteSockaddrLength );
    if ( acceptSocket == INVALID_SOCKET ) {
        return -1;
    }

    //
    // Use a critical section to protect access to our database of
    // TCP clients.
    //

    status = RtlEnterCriticalSection( &CriticalSection );
    ASSERT( NT_SUCCESS(status) );

    //
    // Attempt to find a TCP client slot.
    //

    for ( i = 0; i < MaxTcpClients; i++ ) {
        if ( TcpClients[i].SocketHandle == INVALID_SOCKET ) {
            break;
        }
    }

    //
    // If we're at the max count of TCP sockets, abort this new
    // socket.
    //

    if ( i >= MaxTcpClients ) {
        AbortTcpClient( acceptSocket );
        status = RtlLeaveCriticalSection( &CriticalSection );
        ASSERT( NT_SUCCESS(status) );
        return -1;
    }

    //
    // Initialize info about this client.
    //

    TcpClients[i].SocketHandle = acceptSocket;
    RtlCopyMemory(
        &TcpClients[i].RemoteAddress,
        &remoteSockaddr,
        sizeof(remoteSockaddr)
        );
    TcpClients[i].ServicePort = Port;

    //
    // We're in multi-threaded mode, so we'll create a separate thread
    // to handle this client.
    //

    TcpClients[i].ThreadHandle = CreateThread(
                                     NULL,
                                     0,
                                     ThreadEntry,
                                     (PVOID)i,
                                     0,
                                     &threadId
                                     );
    if ( TcpClients[i].ThreadHandle == NULL ) {
        AbortTcpClient( acceptSocket );
        TcpClients[i].SocketHandle = INVALID_SOCKET;
        status = RtlLeaveCriticalSection( &CriticalSection );
        ASSERT( NT_SUCCESS(status) );
        return -1;
    }

    //
    // The created thread will handle the connected client.
    //

    status = RtlLeaveCriticalSection( &CriticalSection );
    ASSERT( NT_SUCCESS(status) );

    return -1;

} // AcceptTcpClient


VOID
AbortTcpClient (
    IN SOCKET Socket
    )
{
    LINGER lingerInfo;
    INT err;

    //
    // First set the linger timeout on the socket to 0.  This will cause
    // the connection to be reset.
    //

    lingerInfo.l_onoff = 1;
    lingerInfo.l_linger = 0;

    err = setsockopt(
              Socket,
              SOL_SOCKET,
              SO_LINGER,
              (char *)&lingerInfo,
              sizeof(lingerInfo)
              );

    if ( err == SOCKET_ERROR ) {

        //
        // There's not too much we can do.  Just close the socket.
        //

        ASSERT(FALSE);
        closesocket( Socket );
        return;
    }

    //
    // Now close the socket.
    //

    err = closesocket( Socket );
    ASSERT( err != SOCKET_ERROR );

    return;

} // AbortTcpClient


VOID
DeleteTcpClient (
    IN DWORD ArraySlot,
    IN BOOLEAN Graceful
    )
{
    INT err;
    NTSTATUS status;

    status = RtlEnterCriticalSection( &CriticalSection );
    ASSERT( NT_SUCCESS(status) );

    ASSERT( TcpClients[ArraySlot].SocketHandle != INVALID_SOCKET );

    //
    // If the socket is in any of the primary FD sets, remove it.
    //

    if ( FD_ISSET( TcpClients[ArraySlot].SocketHandle, ReadfdsStore ) ) {
        FD_CLR( TcpClients[ArraySlot].SocketHandle, ReadfdsStore );
    }

    if ( FD_ISSET( TcpClients[ArraySlot].SocketHandle, WritefdsStore ) ) {
        FD_CLR( TcpClients[ArraySlot].SocketHandle, WritefdsStore );
    }

    //
    // If this is to be an abortive disconnect, reset the connection.
    // Otherwise just close it normally.
    //

    if ( !Graceful ) {

        AbortTcpClient( TcpClients[ArraySlot].SocketHandle );

    } else {

        LINGER lingerInfo;
        INT one;

        //
        // Set the socket to blocking.
        //

        one = 0;
        ioctlsocket( TcpClients[ArraySlot].SocketHandle, FIONBIO, &one );

        //
        // Set the socket to linger no more than 60 seconds.
        //

        lingerInfo.l_onoff = 1;
        lingerInfo.l_linger = 60;

        setsockopt( TcpClients[ArraySlot].SocketHandle, SOL_SOCKET,
                        SO_LINGER, (char *)&lingerInfo, sizeof(lingerInfo) );

        err = closesocket( TcpClients[ArraySlot].SocketHandle );
        ASSERT( err != SOCKET_ERROR );
    }

    //
    // Close the thread handle, if appropriate.
    //

    if ( TcpClients[ArraySlot].ThreadHandle != NULL ) {
        CloseHandle( TcpClients[ArraySlot].ThreadHandle );
        TcpClients[ArraySlot].ThreadHandle = NULL;
    }

    //
    // Set the handle in the TCP clients array to INVALID_SOCKET so that we
    // know that it is free.
    //

    TcpClients[ArraySlot].SocketHandle = INVALID_SOCKET;

    status = RtlLeaveCriticalSection( &CriticalSection );
    ASSERT( NT_SUCCESS(status) );

    return;

} // DeleteTcpClient

PCHAR Months[] =
{
    "January",
    "Febuary",
    "March",
    "April",
    "May",
    "June",
    "July",
    "August",
    "September",
    "October",
    "November",
    "December"
};

PCHAR Days[] =
{
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday"
};


VOID
FormatDaytimeResponse (
    IN PCHAR Buffer,
    IN PDWORD BufferLength
    )
{
    SYSTEMTIME timeStruct;

    GetLocalTime( &timeStruct );

    *BufferLength = sprintf( Buffer, "%s, %s %d, %d %d:%02d:%02d\n",
                                 Days[timeStruct.wDayOfWeek],
                                 Months[timeStruct.wMonth-1],
                                 timeStruct.wDay,
                                 timeStruct.wYear,
                                 timeStruct.wHour,
                                 timeStruct.wMinute,
                                 timeStruct.wSecond ) + 1;
    return;

} // FormatDaytimeResponse


VOID
FormatQotdResponse (
    IN PCHAR Buffer,
    IN PDWORD BufferLength
    )
{
    INT index;

    //
    // Choose a random quote index.
    //

    index = (rand( ) * QotdQuoteCount) / RAND_MAX;

    //
    // Copy the quote into the output buffer.
    //

    strncpy( Buffer, QotdStrings[index].Quote, QotdStrings[index].QuoteLength );
    *BufferLength = QotdStrings[index].QuoteLength;

    return;

} // FormatDaytimeResponse


INT
InitializeQotdQuotes (
    VOID
    )
{

    BY_HANDLE_FILE_INFORMATION fileInformation;
    PCHAR buffer;
    DWORD i;

    if ( QotdFileName == NULL ) {
        return ERROR_FILE_NOT_FOUND;
    }

    //
    // Open the file containing quote information.
    //

    QotdFileHandle = CreateFileW(
                         QotdFileName,
                         GENERIC_READ,
                         FILE_SHARE_READ,
                         NULL,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL,
                         NULL
                         );
    if ( QotdFileHandle == NULL ) {
        SimpLogEvent(
            SIMPTCP_CANT_OPEN_QUOTE_FILE,
            0,
            NULL,
            GetLastError( )
            );
        return GetLastError( );
    }

    //
    // Determine the size of the QOTD file.
    //

    if ( !GetFileInformationByHandle( QotdFileHandle, &fileInformation ) ) {
        SimpLogEvent(
            SIMPTCP_CANT_OPEN_QUOTE_FILE,
            0,
            NULL,
            GetLastError( )
            );
        return GetLastError( );
    }

    //
    // Create a file mapping for the quotes file and map it into
    // the address space of this process.
    //

    QotdFileMapping = CreateFileMapping(
                          QotdFileHandle,
                          NULL,
                          PAGE_READONLY,
                          0,
                          0,
                          NULL
                          );
    if ( QotdFileMapping == NULL ) {
        SimpLogEvent(
            SIMPTCP_CANT_OPEN_QUOTE_FILE,
            0,
            NULL,
            GetLastError( )
            );
        return GetLastError( );
    }

    QotdBuffer = MapViewOfFile(
                     QotdFileMapping,
                     FILE_MAP_READ,
                     0,
                     0,
                     0
                     );
    if ( QotdBuffer == NULL ) {
        SimpLogEvent(
            SIMPTCP_CANT_OPEN_QUOTE_FILE,
            0,
            NULL,
            GetLastError( )
            );
        return GetLastError( );
    }

    //
    // Count the number of lines in the file.  The number of lines
    // corresponds to the number of quotes.
    //

    QotdQuoteCount = 0;
    buffer = (PCHAR)QotdBuffer;
    QotdQuoteCount = 1;

    for ( i = 0; i < fileInformation.nFileSizeLow; i++ ) {
        if ( *buffer++ == '%' ) {
            QotdQuoteCount++;
        }
    }

    //
    // Allocate a buffer to hold the quote array.
    //

    QotdStrings = RtlAllocateHeap(
                      RtlProcessHeap( ),
                      0,
                      sizeof(QotdStrings[0]) * QotdQuoteCount
                      );

    //
    // Initialize the quote array.
    //

    buffer = (PCHAR)QotdBuffer;

    for ( i = 0; i < QotdQuoteCount; i++ ) {

        QotdStrings[i].Quote = buffer;

        while ( (DWORD)buffer < (DWORD)QotdBuffer +
                    fileInformation.nFileSizeLow &&
                *buffer++ != '%' );

        QotdStrings[i].QuoteLength =
            (DWORD)buffer - (DWORD)QotdStrings[i].Quote - 1;
        buffer += 2;

        //
        // If this quote if longer than the IO buffer size, skip over
        // it.  We can't use it.
        //

        if ( QotdStrings[i].QuoteLength > IoBufferSize ) {
            QotdQuoteCount--;
            i--;
        }
    }

    //
    // Initialize the random-number generator.
    //

    srand( GetTickCount( ) );

    return NO_ERROR;

} // InitializeQotdQuotes


#define CHARGEN_LINE_LENGTH 72
#define CHARGEN_REAL_LINE_LENGTH (CHARGEN_LINE_LENGTH + 2)
#define CHARGEN_MIN_CHAR ' '
#define CHARGEN_MAX_CHAR '~'
#define CHARGEN_DIFFERENCE (CHARGEN_MAX_CHAR - CHARGEN_MIN_CHAR)
#define CHARGEN_LINE_COUNT (CHARGEN_DIFFERENCE)
#define CHARGEN_BUFFER_LENGTH ((CHARGEN_LINE_LENGTH + 2) * (CHARGEN_LINE_COUNT))


INT
InitializeChargen (
    VOID
    )
{
    DWORD line;
    BYTE startChar = 0;
    DWORD i;

    //
    // Allocate a buffer for the chargen data.
    //

    ChargenBufferSize = CHARGEN_BUFFER_LENGTH;

    ChargenBuffer = RtlAllocateHeap(
                        RtlProcessHeap( ),
                        0,
                        ChargenBufferSize
                        );
    if ( ChargenBuffer == NULL ) {
        return ERROR_NOT_ENOUGH_MEMORY;
    }

    //
    // Fill in the buffer with the required pattern.
    //

    for ( line = 0; line < CHARGEN_LINE_COUNT; line++ ) {

        for ( i = 0; i < CHARGEN_LINE_LENGTH; i++ ) {

            *((PCHAR)ChargenBuffer + (line * CHARGEN_REAL_LINE_LENGTH) + i) =
                (CHAR)( ((startChar + i) % CHARGEN_DIFFERENCE) + CHARGEN_MIN_CHAR);
        }

        *((PCHAR)ChargenBuffer + (line * CHARGEN_REAL_LINE_LENGTH) + i) = 0x0D;
        *((PCHAR)ChargenBuffer + (line * CHARGEN_REAL_LINE_LENGTH) + i + 1) = 0x0A;

        startChar++;
    }

    return NO_ERROR;

} // InitializeQotdQuotes


VOID
DoSingleClient (
    IN SOCKET s,
    IN USHORT port
    )
{
    INT err;

    if ( port == TcpEchoPort ) {

        while ( TRUE ) {

            err = recv( s, IoBuffer, IoBufferSize, 0 );
            if ( err == SOCKET_ERROR ) {
                return;
            }

            //
            // If the remote closed gracefully, close the socket.
            //

            if ( err == 0 ) {

                closesocket( s );
                return;

            } else {

                err = send( s, IoBuffer, err, 0 );
                if ( err == SOCKET_ERROR ) {
                    return;
                }
            }
        }

    } else if ( port == TcpDiscardPort ) {

        while ( TRUE ) {

            err = recv( s, IoBuffer, IoBufferSize, 0 );
            if ( err == SOCKET_ERROR ) {
                return;
            }

            //
            // If the remote closed gracefully, close the socket.
            //

            if ( err == 0 ) {
                closesocket( s );
                return;
            } else if ( err == SOCKET_ERROR ) {
                closesocket( s );
                return;
            }
        }

    } else {

        //
        // Something bad has happened.  Internal data
        // structures are corrupt.
        //

        ASSERT( FALSE );
    }

} // DoSingleClient


SHORT
GetServicePort (
    IN PCHAR Service,
    IN PCHAR Protocol
    )
{
    PSERVENT serviceEntry;

    //
    // Get a servent structure for the specified service.
    //

    serviceEntry = getservbyname( Service, Protocol );

    if ( serviceEntry == NULL ) {
        // log an error!
        return INVALID_PORT;
    }

    //
    // Return the port for the specified service.
    //

    return serviceEntry->s_port;

} // GetServicePort


VOID
AnnounceServiceStatus (
    VOID
    )

/*++

Routine Description:

    Announces the service's status to the service controller.

Arguments:

    None.

Return Value:

    None.

--*/

{
    //
    // Service status handle is NULL if RegisterServiceCtrlHandler failed.
    //

    if ( SimpServiceStatusHandle == 0 ) {
        return;
    }

    //
    // Call SetServiceStatus, ignoring any errors.
    //

    SetServiceStatus(SimpServiceStatusHandle, &SimpServiceStatus);

} // AnnounceServiceStatus


VOID
ControlResponse(
    DWORD opCode
    )

{
    BOOL announce = TRUE;
    BOOL err;

    //
    // Determine the type of service control message and modify the
    // service status, if necessary.
    //

    switch( opCode ) {

        case SERVICE_CONTROL_STOP:

            //
            // Announce that we are in the process of stopping.
            //

            SimpServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
            AnnounceServiceStatus( );

            //
            // Remember that we're stopping.
            //

            SimpServiceExit = TRUE;

            //
            // Close a socket that the main select()er thread is
            // waiting on.  This will cause the select to wake up
            // and shutdown processing to commence.
            //

            closesocket( SimpQuitSocket );

            //
            // Let the main thread announce when the stop is done.
            //

            announce = FALSE;

            break;

        case SERVICE_CONTROL_PAUSE:

            //
            // Announce that we are in the process of pausing.
            //

            SimpServiceStatus.dwCurrentState = SERVICE_PAUSE_PENDING;
            AnnounceServiceStatus( );

            //
            // Remember that we're paused.
            //

            err = ResetEvent( SimpPauseEvent );
            ASSERT( err );

            //
            // Announce that we're now paused.
            //

            SimpServiceStatus.dwCurrentState = SERVICE_PAUSED;

            break;

        case SERVICE_CONTROL_CONTINUE:

            //
            // Announce that continue is pending.
            //

            SimpServiceStatus.dwCurrentState = SERVICE_CONTINUE_PENDING;
            AnnounceServiceStatus( );

            //
            // Remember that we're no longer paused.
            //

            err = SetEvent( SimpPauseEvent );
            ASSERT( err );

            //
            // Announce that we're active now.
            //

            SimpServiceStatus.dwCurrentState = SERVICE_RUNNING;

            break;

        case SERVICE_CONTROL_INTERROGATE:

            break;

        default:

            break;
    }

    if ( announce ) {
        AnnounceServiceStatus( );
    }

} // ControlResponse


INT
ReadRegistry (
    VOID
    )
{
    HKEY simptcpKey = NULL;
    ULONG error;
    ULONG i;
    DWORD dwordBuffer;
    DWORD bufferLength;
    DWORD type;
    DWORD qotdFileNameLength;
    PWSTR fileName;

    //
    // First open our parameters key.
    //

    error = RegOpenKeyExW(
                HKEY_LOCAL_MACHINE,
                L"SYSTEM\\CurrentControlSet\\Services\\SimpTcp\\Parameters",
                0,
                MAXIMUM_ALLOWED,
                &simptcpKey
                );
    if ( error != NO_ERROR ) {
        return error;
    }

    //
    // Read BOOLEANs from the registry.
    //

    for ( i = 0; RegistryBooleans[i].Boolean != NULL; i++ ) {

        bufferLength = sizeof(dwordBuffer);

        error = RegQueryValueExW(
                    simptcpKey,
                    RegistryBooleans[i].ValueName,
                    NULL,
                    &type,
                    (PVOID)&dwordBuffer,
                    &bufferLength
                    );

        //
        // If we fail to read one of these for some reason, just skip it
        // and move on to the next one.
        //

        if ( error != NO_ERROR ) {
            continue;
        }

        if ( dwordBuffer == 0 ) {
            *RegistryBooleans[i].Boolean = FALSE;
        } else {
            *RegistryBooleans[i].Boolean = TRUE;
        }
    }

    //
    // Read DWORDs from the registry.
    //

    for ( i = 0; RegistryDwords[i].Dword != NULL; i++ ) {

        bufferLength = sizeof(RegistryDwords[i].Dword);

        RegQueryValueExW(
            simptcpKey,
            RegistryDwords[i].ValueName,
            NULL,
            &type,
            (PVOID)RegistryDwords[i].Dword,
            &bufferLength
            );
    }

    //
    // Read other known values from the registry.  Determine the size
    // of the QOTD file name.  We need this so that we can allocate
    // enough memory to hold it.
    //

    qotdFileNameLength = 0;

    error = RegQueryValueExW(
                simptcpKey,
                L"QotdFileName",
                NULL,
                &type,
                NULL,
                &qotdFileNameLength
                );

    if ( error == ERROR_MORE_DATA || error == NO_ERROR ) {

        fileName = RtlAllocateHeap(
                       RtlProcessHeap( ),
                       0,
                       qotdFileNameLength
                       );
        if ( fileName == NULL ) {
            return NO_ERROR;
        }

        error = RegQueryValueExW(
                    simptcpKey,
                    L"QotdFileName",
                    NULL,
                    &type,
                    (PVOID)fileName,
                    &qotdFileNameLength
                    );
        if ( error != NO_ERROR ) {
            RtlFreeHeap( RtlProcessHeap( ), 0, fileName );
            return NO_ERROR;
        }

        //
        // Expand the file name.
        //

        qotdFileNameLength = ExpandEnvironmentStringsW( fileName, NULL, 0 );

        QotdFileName = RtlAllocateHeap(
                           RtlProcessHeap( ),
                           0,
                           qotdFileNameLength * 2
                           );
        if ( QotdFileName == NULL ) {
            RtlFreeHeap( RtlProcessHeap( ), 0, fileName );
            return NO_ERROR;
        }

        ExpandEnvironmentStringsW( fileName, QotdFileName, qotdFileNameLength );
    }

    return NO_ERROR;

} // ReadRegistry


DWORD
ThreadEntry (
    LPVOID lpThreadParameter
    )
{
    DWORD i = (DWORD)lpThreadParameter;
    PVOID ioBuffer;
    INT err;
    BOOLEAN graceful = TRUE;

    //
    // First, set the send and receive timeouts for the socket.  This
    // prevents a dead client from tying up our resources for too long.
    //

    err = setsockopt( TcpClients[i].SocketHandle, SOL_SOCKET, SO_SNDTIMEO,
                          (char *)&MaxIdleTicks, sizeof(MaxIdleTicks) );
    if ( err == SOCKET_ERROR ) {
        DeleteTcpClient( i, FALSE );
        return 0;
    }

    err = setsockopt( TcpClients[i].SocketHandle, SOL_SOCKET, SO_RCVTIMEO,
                          (char *)&MaxIdleTicks, sizeof(MaxIdleTicks) );
    if ( err == SOCKET_ERROR ) {
        DeleteTcpClient( i, FALSE );
        return 0;
    }

    //
    // Get a buffer to use locally for IO on the socket.
    //

    ioBuffer = RtlAllocateHeap( RtlProcessHeap( ), 0, IoBufferSize );
    if ( ioBuffer == NULL ) {
        DeleteTcpClient( i, FALSE );
        return 0;
    }

    //
    // Now service the client as appropriate.
    //

    if ( TcpClients[i].ServicePort == TcpEchoPort ) {

        //
        // If there is data on a client's echo socket,
        // receive some data and send it back.
        //

        do {

            err = recv(
                      TcpClients[i].SocketHandle,
                      ioBuffer,
                      IoBufferSize,
                      0
                      );
            if ( err == SOCKET_ERROR ) {
                graceful = FALSE;
            }

            if ( err > 0 ) {

                err = send(
                          TcpClients[i].SocketHandle,
                          ioBuffer,
                          err,
                          0
                          );
                if ( err == SOCKET_ERROR ) {
                    graceful = FALSE;
                }

            } else if ( err < 0 ) {

                graceful = FALSE;
            }

        } while ( err > 0 );

    } else if ( TcpClients[i].ServicePort == TcpChargenPort ) {

        INT one;
        INT error;
        TIMEVAL timeout;

        //
        // Set the socket to nonblocking.
        //

        one = 1;
        err = ioctlsocket( TcpClients[i].SocketHandle, FIONBIO, &one );
        if ( err == SOCKET_ERROR ) {
            graceful = FALSE;
        }

        //
        // Calculate the select() timeout.
        //

        timeout.tv_sec = MaxIdleTicks / 1000;
        timeout.tv_usec = MaxIdleTicks % 1000;

        //
        // Loop sending data.
        //

        do {

            err = send(
                      TcpClients[i].SocketHandle,
                      ChargenBuffer,
                      ChargenBufferSize,
                      0
                      );

            if ( err == SOCKET_ERROR ) {

                error = GetLastError( );

                if ( error != WSAEWOULDBLOCK ) {

                    graceful = FALSE;

                } else {

                    struct {
                        INT Count;
                        SOCKET Handle;
                    } readfds = { 0, 0 };
                    struct {
                        INT Count;
                        SOCKET Handle;
                    } writefds = { 0, 0 };

                    //
                    // The socket's send queue is blocked.  Wait for it to
                    // become unblocked.
                    //

                    FD_SET( TcpClients[i].SocketHandle, (PFD_SET)&readfds );
                    FD_SET( TcpClients[i].SocketHandle, (PFD_SET)&writefds );

                    err = select(
                              1,
                              (PFD_SET)&readfds,
                              (PFD_SET)&writefds,
                              NULL,
                              &timeout
                              );
                    if ( err <= 0 ) {
                        graceful = FALSE;
                    }
                }
            }

            err = recv(
                      TcpClients[i].SocketHandle,
                      ioBuffer,
                      IoBufferSize,
                      0
                      );
            if ( err == SOCKET_ERROR ) {
                if ( WSAGetLastError( ) != WSAEWOULDBLOCK ) {
                    graceful = FALSE;
                } else {
                    err = 1;
                }
            }

        } while ( err > 0 );

    } else if ( TcpClients[i].ServicePort == TcpDiscardPort ) {

        //
        // If there is data on a client's socket, just
        // receive some data and discard it.
        //

        do {

            err = recv(
                      TcpClients[i].SocketHandle,
                      ioBuffer,
                      IoBufferSize,
                      0
                      );
            if ( err == SOCKET_ERROR ) {
                graceful = FALSE;
            }

        } while ( err > 0 );

    } else {

        //
        // Something bad has happened.  Internal data
        // structures are corrupt.
        //

        ASSERT( FALSE );
    }

    //
    // Free the socket and the IO buffer and return.
    //

    DeleteTcpClient( i, graceful );
    RtlFreeHeap( RtlProcessHeap( ), 0, ioBuffer );

    return 0;

} // ThreadEntry

