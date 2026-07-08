////////////////////////////////////////////////////////////////////////////////////////////////////
///
///	\file		APIC_ServerUtilities.h
///	\brief		Overview: Interface of the APIC_ServerUtilities class
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __APIC_SERVER_UTILITIES_H__
#define __APIC_SERVER_UTILITIES_H__

#include <netinet/in.h>

#define MAX_CLIENTS 10
#define BUF_SIZE 2048

enum TYPE_ConnectionStatus {
    TYPE_ConnectionStatus_Disconnected = 0,
    TYPE_ConnectionStatus_Connected,
};

// Thông tin của 1 client 
struct TYPS_ClientInfo {
    /// \brief The unique identifier for the client.
    int mi_ID;

    /// \brief The socket file descriptor for the client.
    int mi_SocketFD;

    /// \brief The IP address of the client in string format.
    char mc_IP[INET_ADDRSTRLEN];   // IPv4 string

    /// \brief The port number of the client.
    int mi_Port;

    /// \brief The connection status of the client (connected or disconnected).
    TYPE_ConnectionStatus me_Status;  
};

// Mảng chứa thông tin của tất cả client 
extern TYPS_ClientInfo clients[MAX_CLIENTS];
// Đếm s�? lượng client 
extern int client_count;    

class APIC_ServerUtilities {
public:
    /// \brief Add a new client to the client list.
    /// \param sock_fd The socket file descriptor of the client.
    /// \param ip The IP address of the client.
    /// \param port The port number of the client.
    /// \param status The connection status of the client.
    void addClient(int sock_fd, const char *ip, int port, TYPE_ConnectionStatus status);

    /// \brief Print the list of currently connected clients.
    void displayClients(void);

    /// \brief Terminate a client connection.
    /// \param id_client The ID of the client to terminate.
    void killClient(int id_client);

    /// \brief Write the status and messages of all clients sent to the server to the log.
    /// \param msg The message to write to the log.
    void writeLog(const char *msg);

    /// \brief Read the status and messages of all clients sent to the server in log to console.
    void readLog(void);
};

#endif