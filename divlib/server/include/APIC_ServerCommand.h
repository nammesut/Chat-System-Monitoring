////////////////////////////////////////////////////////////////////////////////////////////////////
///
///	\file		APIC_ServerCommand.h
///	\brief		Overview: Interface of the APIC_ServerCommand class
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __APIC_SERVER_COMMAND_H__
#define __APIC_SERVER_COMMAND_H__

#include <mqueue.h>
#include "APIC_ServerUtilities.h"

struct TYPS_Command {
    /// \brief The command string that the user can input to execute a specific action.
    const char *mc_Command;

    /// \brief Description of the command.
    const char *mc_Description;
};

static const TYPS_Command commands[] = {
    {"list",                           "Display list of connected clients"      },
    {"message",                        "Show all messages from clients "        },
    {"terminate <ID Client>",          "Terminate a specific connection"        },
    {"sysinfo",                        "Display system information (CPU, RAM)"  },
    {"exit",                           "Exit server command"                    },
};

class APIC_ServerCommand {
public:
    /// \brief Add client in the list
    /// \param sock_fd The socket file descriptor of the client
    /// \param ip The IP address of the client
    /// \param port The port number of the client
    /// \param status The connection status of the client
    void commandAddClient(int i_SocketFD, const char* c_IP, int i_Port, TYPE_ConnectionStatus e_Status);

    /// \brief Displays instructions for use and purpose of commands.
    void displayCommandServer(void);

    /// \brief Execute command 1. list
    void commandList(void);

    /// \brief Execute command 2. message   
    void commandMessage(void);

    /// \brief Execute command 3. terminate <ID Client>
    /// \param id_client The ID of the client to terminate
    void commandTerminate(int id_client);

    /// \brief Execute command 4. sysinfo  
    /// \param pid_server The PID of the server process
    /// \param mq The message queue descriptor
    void commandSysInfo(pid_t *pid_server, mqd_t *mq);

private:
    /// \brief Send a request to the daemon process via message queue.
    /// \param mq The message queue descriptor.
    /// \param msg The message to send.
    void serverRequestDaemon(mqd_t *mq, const char *msg);

    /// \brief Receive data from shared memory.
    /// \param cpu_usage Pointer to store the CPU usage data.
    /// \param mem_usage Pointer to store the memory usage data.
    void serverReceiveDataShm(double *cpu_usage, double *mem_usage);

    /// \brief Convert the PID of the server process to a string.
    /// \param pid_server Pointer to the PID of the server process.
    /// \return A dynamically allocated string containing the PID.
    char* convertPIDtoStr(pid_t *pid);

    /// \brief Set up signal handling for SIGUSR1.
    void setupSignal(void);

    /// \brief Signal handler for SIGUSR1.
    /// \param signo The signal number.
    void signalHandler(int signo);
};

#endif 
