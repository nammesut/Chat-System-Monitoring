////////////////////////////////////////////////////////////////////////////////////////////////////
///
///	\file		APIC_ServerCore.h
///	\brief		Overview: Interface of the APIC_ServerCore class
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __APIC_SERVER_CORE_H__
#define __APIC_SERVER_CORE_H__

#include "APIC_ServerCommand.h"

class APIC_ServerCore {
public:
    /// \brief Initialize, bind and listen a TCP Server socket on the specified port.
    /// \param port The TCP port number to bind the socket to.
    /// \return The listening socket file descriptor on success, or -1 on failure.
    int serverInit(int port);

    /// \brief Run the TCP server loop to accept and handle client connections.
    /// \param server_fd The listening socket file descriptor returned by serverInit().
    void serverRun(int server_fd);

    /// \brief Handle communication with a connected client in a separate thread.
    /// \param arg Pointer to a TYPS_ClientInfo structure containing client information.
    static void *clientHandler(void *arg);

    /// \brief Handle server-side console commands in a separate thread.
    /// \param arg Pointer to a message queue descriptor for inter-process communication.
    static void *commandHandler(void *arg);
};


#endif 