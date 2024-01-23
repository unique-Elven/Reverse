/*****************************************************************************/
/* LPC.cpp                                Copyright (c) Ladislav Zezula 2005 */
/*---------------------------------------------------------------------------*/
/* Demonstration program of using LPC facility                               */
/*---------------------------------------------------------------------------*/
/*   Date    Ver   Who  Comment                                              */
/* --------  ----  ---  -------                                              */
/* 22.02.05  1.00  Lad  The first version of LPC.cpp                         */
/*****************************************************************************/

#define UNICODE
#define _UNICODE
#define _CRT_SECURE_NO_WARNINGS
#include <tchar.h>
#include <conio.h>
#include <stdio.h>
#include <windows.h>

#include "ntdll.h"

//-----------------------------------------------------------------------------
// Local structures

#define LPC_COMMAND_REQUEST_NOREPLY  0x00000000
#define LPC_COMMAND_REQUEST_REPLY    0x00000001
#define LPC_COMMAND_STOP             0x00000002

// This is the data structure transferred through LPC.
// Every structure must begin with PORT_MESSAGE, and must NOT be
// greater that MAX_LPC_DATA

typedef struct _TRANSFERRED_MESSAGE
{
    PORT_MESSAGE Header;

    ULONG   Command;
    WCHAR   MessageText[48];

} TRANSFERRED_MESSAGE, *PTRANSFERRED_MESSAGE;

typedef BOOL (WINAPI *ISWOW64PROCESS) (HANDLE, PBOOL);

//-----------------------------------------------------------------------------
// Local variables

LPWSTR LpcPortName = L"\\TestLpcPortName";      // Name of the LPC port
                                                // Must be in the form of "\\name"
HANDLE g_hHeap = NULL;                          // Application heap

//-----------------------------------------------------------------------------
// Server and client thread for testing small LPC messages

DWORD WINAPI ServerThread1(LPVOID)
{
    SECURITY_DESCRIPTOR sd;
    OBJECT_ATTRIBUTES ObjAttr;              // Object attributes for the name
    UNICODE_STRING PortName;
    NTSTATUS Status;
    HANDLE LpcPortHandle = NULL;
    BYTE RequestBuffer[sizeof(PORT_MESSAGE) + MAX_LPC_DATA];
    BOOL WeHaveToStop = FALSE;
    int nError;

    __try     // try-finally
    {
        //
        // Initialize security descriptor that will be set to
        // "Everyone has the full access"
        //

        if(!InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION))
        {
            nError = GetLastError();
            __leave;
        }

        //
        // Set the empty DACL to the security descriptor
        //

        if(!SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE))
        {
            nError = GetLastError();
            __leave;
        }

        //
        // Initialize attributes for the port and create it
        //

        RtlInitUnicodeString(&PortName, LpcPortName);
        InitializeObjectAttributes(&ObjAttr, &PortName, 0, NULL, &sd);

        _tprintf(_T("Server: Creating LPC port \"%s\" (NtCreatePort) ...\n"), LpcPortName);
        Status = NtCreatePort(&LpcPortHandle,
                              &ObjAttr,
                               NULL,
                               sizeof(PORT_MESSAGE) + MAX_LPC_DATA,
                               0);
        _tprintf(_T("Server: NtCreatePort result 0x%08lX\n"), Status);

        if(!NT_SUCCESS(Status))
            __leave;

        //
        // Process all incoming LPC messages
        //

        while(WeHaveToStop == FALSE)
        {
            PTRANSFERRED_MESSAGE LpcMessage = NULL;
            HANDLE ServerHandle = NULL;

            //
            // Create the data buffer for the request
            //

            LpcMessage = (PTRANSFERRED_MESSAGE)RequestBuffer;
            _tprintf(_T("Server: ------------- Begin loop ----------------------\n"), Status);

            //
            // Listen to the port. This call is blocking, and cannot be interrupted,
            // even if the handle is closed. The only way how to stop the block is to send
            // an LPC request which will be recognized by server thread as "Stop" command
            //

            if(NT_SUCCESS(Status))
            {
                _tprintf(_T("Server: Listening to LPC port (NtListenPort) ...\n"), LpcPortName);
                Status = NtListenPort(LpcPortHandle,
                                     &LpcMessage->Header);
                _tprintf(_T("Server: NtListenPort result 0x%08lX\n"), Status);
            }

            //
            // Accept the port connection
            //

            if(NT_SUCCESS(Status))
            {
                _tprintf(_T("Server: Accepting LPC connection (NtAcceptConnectPort) ...\n"), LpcPortName);
                Status = NtAcceptConnectPort(&ServerHandle,
                                              NULL,
                                             &LpcMessage->Header,
                                              TRUE,
                                              NULL,
                                              NULL);
                _tprintf(_T("Server: NtAcceptConnectPort result 0x%08lX\n"), Status);
            }

            //
            // Complete the connection
            //

            if(NT_SUCCESS(Status))
            {
                _tprintf(_T("Server: Completing LPC connection (NtCompleteConnectPort) ...\n"), LpcPortName);
                Status = NtCompleteConnectPort(ServerHandle);
                _tprintf(_T("Server: NtCompleteConnectPort result 0x%08lX\n"), Status);
            }

            //
            // Now accept the data request coming from the port.
            //

            if(NT_SUCCESS(Status))
            {
                _tprintf(_T("Server: Receiving LPC data (NtReplyWaitReceivePort) ...\n"), LpcPortName);
                Status = NtReplyWaitReceivePort(ServerHandle,
                                                NULL,
                                                NULL,
                                               &LpcMessage->Header);
                _tprintf(_T("Server: NtReplyWaitReceivePort result 0x%08lX\n"), Status);
            }

            //
            // Get the data sent by the client 
            //

            if(NT_SUCCESS(Status))
            {
                // If a request has been received, answer to it.
                switch(LpcMessage->Command)
                {
                    case LPC_COMMAND_REQUEST_NOREPLY:
                        _tprintf(_T("Server: Received request \"%s\"\n"), LpcMessage->MessageText);
                        break;      // Nothing more to do

                    case LPC_COMMAND_REQUEST_REPLY:
                        _tprintf(_T("Server: Received request \"%s\"\n"), LpcMessage->MessageText);
                        _tprintf(_T("Server: Sending reply (NtReplyPort) ...\n"), LpcPortName);
                        Status = NtReplyPort(LpcPortHandle, &LpcMessage->Header);
                        _tprintf(_T("Server: NtReplyPort result 0x%08lX\n"), Status);
                        break;

                    case LPC_COMMAND_STOP:      // Stop the work
                        _tprintf(_T("Server: Stopping ...\n"));
                        WeHaveToStop = TRUE;
                        break;
                }
            }

            //
            // Close the server connection handle
            //

            if(ServerHandle != NULL)
            {
                _tprintf(_T("Server: Closing the request handle (NtClose) ...\n"), LpcPortName);
                Status = NtClose(ServerHandle);
                _tprintf(_T("Server: NtClose result 0x%08lX\n"), Status);
            }

            _tprintf(_T("Server: ------------- End loop ----------------------\n"), Status);
        }
    }

    __finally
    {
        if(LpcPortHandle != NULL)
            NtClose(LpcPortHandle);
    }
    
    return 0;
}


DWORD WINAPI ClientThread1(LPVOID)
{
    SECURITY_QUALITY_OF_SERVICE SecurityQos;
    PTRANSFERRED_MESSAGE LpcMessage = NULL;
    PTRANSFERRED_MESSAGE LpcReply = NULL;
    UNICODE_STRING PortName;
    NTSTATUS Status = STATUS_SUCCESS;
    HANDLE PortHandle = NULL;
    ULONG MaxMessageLength = 0;
    ULONG MessageLength = sizeof(TRANSFERRED_MESSAGE);
    ULONG ReplyLength = sizeof(TRANSFERRED_MESSAGE);
    ULONG PassCount;

    __try
    {
        //
        // Allocate space for message to be transferred through LPC
        //

        LpcMessage = (PTRANSFERRED_MESSAGE)HeapAlloc(g_hHeap, HEAP_ZERO_MEMORY, MessageLength);
        LpcReply   = (PTRANSFERRED_MESSAGE)HeapAlloc(g_hHeap, HEAP_ZERO_MEMORY, ReplyLength);
        if(LpcMessage == NULL || LpcReply == NULL)
        {
            Status = ERROR_NOT_ENOUGH_MEMORY;
            __leave;
        }

        for(PassCount = 0; PassCount < 3; PassCount++)
        {
            //
            // Initialize the parameters of LPC port and connect to it
            //

            RtlInitUnicodeString(&PortName, LpcPortName);
            SecurityQos.Length = sizeof(SECURITY_QUALITY_OF_SERVICE);
            SecurityQos.ImpersonationLevel = SecurityImpersonation;
            SecurityQos.EffectiveOnly = FALSE;
            SecurityQos.ContextTrackingMode = SECURITY_DYNAMIC_TRACKING;

            _tprintf(_T("Client: Test sending LPC data of size less than 0x%lX bytes ...\n"), MAX_LPC_DATA);
            _tprintf(_T("Client: Connecting to port \"%s\" (NtConnectPort) ...\n"), LpcPortName);
            Status = NtConnectPort(&PortHandle,
                                   &PortName,
                                   &SecurityQos,
                                    NULL,
                                    NULL,
                                   &MaxMessageLength,
                                    NULL,
                                    NULL);
            _tprintf(_T("Client: NtConnectPort result 0x%08lX\n"), Status);

            //
            // Initialize the request header, reply header and fill request text
            //

            InitializeMessageHeader(&LpcMessage->Header, MessageLength, 0);
            InitializeMessageHeader(&LpcReply->Header, ReplyLength, 0);
            wcscpy(LpcMessage->MessageText, L"Message text through LPC");

            //
            // FIRST PASS: Send the request, don't wait for reply
            //

            if(PassCount == 0)
            {
                _tprintf(_T("Client: Sending request, reply not required (NtRequestPort)\n"));
                LpcMessage->Command = LPC_COMMAND_REQUEST_NOREPLY;
                Status = NtRequestPort(PortHandle, &LpcMessage->Header);
                _tprintf(_T("Client: NtRequestPort result 0x%08lX\n"), Status);
                Sleep(500);
            }

            //
            // SECOND PASS: Send the request and wait for reply
            //

            if(PassCount == 1)
            {
                _tprintf(_T("Client: Sending request, waiting for reply (NtRequestWaitReplyPort)\n"));
                LpcMessage->Command = LPC_COMMAND_REQUEST_REPLY;
                Status = NtRequestWaitReplyPort(PortHandle, &LpcMessage->Header, &LpcReply->Header);
                _tprintf(_T("Client: NtRequestWaitReplyPort result 0x%08lX\n"), Status);
                Sleep(500);
            }

            //
            // THIRD PASS: Send the Stop command
            //

            if(PassCount == 2)
            {
                _tprintf(_T("Client: Sending STOP request, reply not required (NtRequestPort)\n"));
                LpcMessage->Command = LPC_COMMAND_STOP;
                Status = NtRequestPort(PortHandle, &LpcMessage->Header);
                _tprintf(_T("Client: NtRequestPort result 0x%08lX\n"), Status);
                Sleep(500);
            }

            //
            // Close the connected port
            //

            if(PortHandle != NULL)
            {
                _tprintf(_T("Client: Closing the port (NtClose) \n"));
                Status = NtClose(PortHandle);
                _tprintf(_T("Client: NtClose result 0x%08lX\n"), Status);
            }
        }
    }

    __finally
    {
        if(LpcReply != NULL)
            HeapFree(g_hHeap, 0, LpcReply);

        if(LpcMessage != NULL)
            HeapFree(g_hHeap, 0, LpcMessage);
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Server and client thread for testing large LPC messages

#define LARGE_MESSAGE_SIZE 0x9000

DWORD WINAPI ServerThread2(LPVOID)
{
    OBJECT_ATTRIBUTES ObjAttr;
    REMOTE_PORT_VIEW ClientView;
    UNICODE_STRING PortName;
    LARGE_INTEGER SectionSize = {LARGE_MESSAGE_SIZE};
    PORT_MESSAGE MessageHeader;
    PORT_VIEW ServerView;
    NTSTATUS Status;
    HANDLE LpcPortHandle = NULL;
    HANDLE SectionHandle = NULL;
    HANDLE ServerHandle = NULL;

    __try   // try-finally
    {
        //
        // Create a memory section in the pagefile used for transfer the data
        // to the client
        //

        _tprintf(_T("Server: Creating section for transferring data (NtCreateSection) ...\n"));
        Status = NtCreateSection(&SectionHandle,
                                  SECTION_MAP_READ | SECTION_MAP_WRITE,
                                  NULL,     // Backed by the pagefile
                                 &SectionSize,
                                  PAGE_READWRITE,
                                  SEC_COMMIT,
                                  NULL);
        _tprintf(_T("Server: NtCreateSection result 0x%08lX\n"), Status);
        if(!NT_SUCCESS(Status))
            __leave;

        //
        // Create the named port
        //

        RtlInitUnicodeString(&PortName, LpcPortName);
        InitializeObjectAttributes(&ObjAttr, &PortName, 0, NULL, NULL);
        _tprintf(_T("Server: Creating port \"%s\" (NtCreatePort) ...\n"), LpcPortName);
        Status = NtCreatePort(&LpcPortHandle,
                              &ObjAttr,
                               NULL,
                               sizeof(PORT_MESSAGE),
                               0);
        _tprintf(_T("Server: NtCreatePort result 0x%08lX\n"), Status);
        if(!NT_SUCCESS(Status))
            __leave;

        //
        // Listen the port and receive request
        //

        _tprintf(_T("Server: Listening to port (NtListenPort) ...\n"), LpcPortName);
        Status = NtListenPort(LpcPortHandle, &MessageHeader);
        _tprintf(_T("Server: NtCreatePort result 0x%08lX\n"), Status);
        if(!NT_SUCCESS(Status))
            __leave;


        //
        // Fill local and remote memory views. When the LPC
        // message comes to the client, the section will be remapped
        // to be accessible to the listener, even if the client is in another
        // process or different processor mode (UserMode/KernelMode)
        //

        ServerView.Length        = sizeof(PORT_VIEW);
        ServerView.SectionHandle = SectionHandle;
        ServerView.SectionOffset = 0;
        ServerView.ViewSize      = LARGE_MESSAGE_SIZE;
        ClientView.Length        = sizeof(REMOTE_PORT_VIEW);

        //
        // Accept the port connection and receive the client's view
        //

        _tprintf(_T("Server: Accepting connection (NtAcceptConnectPort) ...\n"));
        Status = NtAcceptConnectPort(&ServerHandle,
                                      NULL,
                                     &MessageHeader,
                                      TRUE,
                                     &ServerView,
                                     &ClientView);
        _tprintf(_T("Server: NtAcceptConnectPort result 0x%08lX\n"), Status);
        if(!NT_SUCCESS(Status))
            __leave;

        //
        // Complete the connection
        //

        _tprintf(_T("Server: Completing connection (NtCompleteConnectPort) ...\n"));
        Status = NtCompleteConnectPort(ServerHandle);
        _tprintf(_T("Server: NtCompleteConnectPort result 0x%08lX\n"), Status);
        if(!NT_SUCCESS(Status))
            __leave;

        //
        // Now (wait for and) accept the data request coming from the port.
        //

        _tprintf(_T("Server: Replying, waiting for receive (NtReplyWaitReceivePort) ...\n"));
        Status = NtReplyWaitReceivePort(ServerHandle,
                                        NULL,
                                        NULL,
                                       &MessageHeader);
        _tprintf(_T("Server: NtReplyWaitReceivePort result 0x%08lX\n"), Status);
        if(!NT_SUCCESS(Status))
            __leave;

        //
        // Now, when the connection is done, is time for giving data to the client.
        // Just fill the entire client's data with 'S'
        //

        wcscpy((PWSTR)ClientView.ViewBase, L"This is a long reply from the server\n"
                                           L"This is a long reply from the server\n"
                                           L"This is a long reply from the server\n"
                                           L"This is a long reply from the server\n"
                                           L"This is a long reply from the server\n"
                                           L"This is a long reply from the server\n"
                                           L"This is a long reply from the server\n"
                                           L"This is a long reply from the server\n"
                                           L"This is a long reply from the server\n"
                                           L"This is a long reply from the server\n");

        //
        // Reply to the message
        //

        _tprintf(_T("Server: Replying (NtReplyPort) ...\n"));
        Status = NtReplyPort(LpcPortHandle, &MessageHeader);
        _tprintf(_T("Server: NtReplyPort result 0x%08lX\n"), Status);
    }
    __finally
    {
        if(ServerHandle != NULL)
            NtClose(ServerHandle);
        if(LpcPortHandle != NULL)
            NtClose(LpcPortHandle);
        if(SectionHandle != NULL)
            NtClose(SectionHandle);
    }

    return 0;
}


DWORD WINAPI ClientThread2(LPVOID)
{
    SECURITY_QUALITY_OF_SERVICE SecurityQos;
    REMOTE_PORT_VIEW ServerView;
    UNICODE_STRING PortName;
    LARGE_INTEGER SectionSize = {LARGE_MESSAGE_SIZE};
    PORT_MESSAGE MessageHeader;
    PORT_VIEW ClientView;
    NTSTATUS Status = STATUS_SUCCESS;
    HANDLE SectionHandle = NULL;
    HANDLE PortHandle = NULL;

    __try
    {
        //
        // Create a memory section in the pagefile used for transfer the data
        // to the client
        //

        _tprintf(_T("Client: Test sending LPC data with size of %lX ...\n"), LARGE_MESSAGE_SIZE);
        _tprintf(_T("Client: Creating section for transferring data (NtCreateSection) ...\n"));
        Status = NtCreateSection(&SectionHandle,
                                  SECTION_MAP_READ | SECTION_MAP_WRITE,
                                  NULL,         // Backed by the pagefile
                                 &SectionSize,
                                  PAGE_READWRITE,
                                  SEC_COMMIT,
                                  NULL);
        _tprintf(_T("Client: NtCreateSection result 0x%08lX\n"), Status);
        if(!NT_SUCCESS(Status))
            __leave;

        //
        // Initialize the parameters of LPC port
        //

        RtlInitUnicodeString(&PortName, LpcPortName);
        SecurityQos.Length = sizeof(SECURITY_QUALITY_OF_SERVICE);
        SecurityQos.ImpersonationLevel = SecurityImpersonation;
        SecurityQos.EffectiveOnly = FALSE;
        SecurityQos.ContextTrackingMode = SECURITY_DYNAMIC_TRACKING;

        //
        // Fill local and remote memory view. When the LPC
        // message comes to the listener, the section will be remapped
        // to be accessible to the listener, even if the listener is in another
        // process or different processor mode (UserMode/KernelMode)
        //

        ClientView.Length        = sizeof(PORT_VIEW);
        ClientView.SectionHandle = SectionHandle;
        ClientView.SectionOffset = 0;
        ClientView.ViewSize      = LARGE_MESSAGE_SIZE;
        ServerView.Length        = sizeof(REMOTE_PORT_VIEW);

        //
        // Connect to the port
        //

        _tprintf(_T("Client: Connecting to port \"%s\" (NtConnectPort) ...\n"), LpcPortName);
        Status = NtConnectPort(&PortHandle,
                               &PortName,
                               &SecurityQos,
                               &ClientView,
                               &ServerView,
                                0,
                                NULL,
                                NULL);
        _tprintf(_T("Client: NtConnectPort result 0x%08lX\n"), Status);
        if(!NT_SUCCESS(Status))
            __leave;

        //
        // Initialize the request header. Give data to the server
        //

        InitializeMessageHeader(&MessageHeader, sizeof(PORT_MESSAGE), 0);
        wcscpy((PWSTR)ServerView.ViewBase, L"This is a long message data from the client\n"
                                           L"This is a long message data from the client\n"
                                           L"This is a long message data from the client\n"
                                           L"This is a long message data from the client\n"
                                           L"This is a long message data from the client\n"
                                           L"This is a long message data from the client\n"
                                           L"This is a long message data from the client\n"
                                           L"This is a long message data from the client\n"
                                           L"This is a long message data from the client\n");

        //
        // Send the data request, and wait for reply
        //

        _tprintf(_T("Client: Sending request, waiting for reply (NtRequestWaitReplyPort)\n"));
        Status = NtRequestWaitReplyPort(PortHandle, &MessageHeader, &MessageHeader);
        _tprintf(_T("Client: NtRequestWaitReplyPort result 0x%08lX\n"), Status);
    }

    __finally
    {
        if(PortHandle != NULL)
            NtClose(PortHandle);

        if(SectionHandle != NULL)
            NtClose(SectionHandle);
    }

    return 0;
}

static BOOL Is32BitProcessUnderWOW64()
{
    ISWOW64PROCESS pfnIsWoW64Process = NULL;
    HMODULE hKernel32 = GetModuleHandle(_T("Kernel32.dll"));
    BOOL bIsWow64Process = FALSE;

    if(hKernel32 != NULL)
    {
        pfnIsWoW64Process = (ISWOW64PROCESS)GetProcAddress(hKernel32, "IsWow64Process");

        if(pfnIsWoW64Process != NULL)
            pfnIsWoW64Process(GetCurrentProcess(), &bIsWow64Process);
    }

    return bIsWow64Process;
}

//-----------------------------------------------------------------------------
// main

int _tmain(void)
{
    HANDLE hThread[2];
    DWORD dwThreadId;

    //
    // Save the handle of the main heap
    //
    
    g_hHeap = GetProcessHeap();

    //
    // Note for 64-bit Windows: Functions that use PORT_MESSAGE structure
    // as one of the parameters DO NOT WORK under WoW64. The reason is that
    // the layer between 32-bit NTDLL and 64-bit NTDLL does not translate
    // the structure to its 64-bit layout and most of the functions
    // fail with STATUS_INVALID_PARAMETER (0xC000000D)
    //

    if(Is32BitProcessUnderWOW64())
    {
        _tprintf(_T("WARNING: You are running 32-bit version of the example under 64-bit Windows.\n")
                 _T("This is not supported and will not work.\n"));
        _getch();
    }

    //
    // Run both threads testing small LPC messages
    //
  
    hThread[0] = CreateThread(NULL, 0, ServerThread1, NULL, 0, &dwThreadId);
    hThread[1] = CreateThread(NULL, 0, ClientThread1, NULL, 0, &dwThreadId);
    WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
    CloseHandle(hThread[0]);
    CloseHandle(hThread[1]);
  
    //
    // Run both threads testing large LPC messages
    //
/*
    hThread[0] = CreateThread(NULL, 0, ServerThread2, NULL, 0, &dwThreadId);
    hThread[1] = CreateThread(NULL, 0, ClientThread2, NULL, 0, &dwThreadId);
    WaitForMultipleObjects(2, hThread, TRUE, INFINITE);
    CloseHandle(hThread[0]);
    CloseHandle(hThread[1]);
*/
    _getch();
    return 0;
}

