////////////////////////////////////////////////////////////////////////////////////////////////////                                                    
///                                                 
/// \file       APIC_ServerCommand.c                               
/// \brief      Overview: Command server functionalities                                     
///                                                 
////////////////////////////////////////////////////////////////////////////////////////////////////  

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <mqueue.h>

#include "../include/APIC_ServerCommand.h"
#include "system/ipc/APIC_MessageQueue.h"
#include "system/ipc/APIC_SharedMemory.h"

static volatile sig_atomic_t check_notifications_daemon = 0;

static void APIC_ServerCommand::serverRequestDaemon(mqd_t *mq, const char *msg){
    APIC_MessageQueue messageQueue;
    messageQueue.messageQueueSend(*mq, msg);
}

static void APIC_ServerCommand::serverReceiveDataShm(double *data_cpu_usage, double *data_mem_usage) {
    APIC_SharedMemory sharedMemory;
    sharedMemory.sharedMemoryRead(data_cpu_usage, data_mem_usage);
}

char *APIC_ServerCommand::convertPIDtoStr(pid_t *pid_server) {
    char *buffer = malloc(16);
    if (buffer) {
        sprintf(buffer, "%d", *pid_server);
    }
    return buffer; // caller nh�? free
}


static void APIC_ServerCommand::signalHandler(int signo) {
    if (signo == SIGUSR1) {
        // printf("Daemon reported data write conplete\n");
        check_notifications_daemon = 1;
    }
}

static void APIC_ServerCommand::setupSignal() {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));        // Khởi tạo to�?n b�? struct v�? 0
    sa.sa_handler = signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;           // tự restart syscall b�? gián đoạn

    if (sigaction(SIGUSR1, &sa, NULL) < 0) {
        perror("sigaction");
    }
}

/*----------------------------------------------------------------------------------------*/

void APIC_ServerCommand::displayCommandServer(void) {
    int i = 0;
    int SIZE = sizeof(commands) / sizeof(commands[0]);
    printf("\n======================== SERVER COMMANDS ======================\n");
    for (; i < SIZE ;) {
        printf("%d. %-23s : %s\n", i + 1, commands[i].mc_Command, commands[i].mc_Description);
        i++;
    }   
    printf("===============================================================\n\n");
}

void APIC_ServerCommand::commandList(void) {
    APIC_ServerUtilities serverUtilities;
    serverUtilities.displayClients();
}

void APIC_ServerCommand::commandTerminate(int id_client) {
    APIC_ServerUtilities serverUtilities;
    serverUtilities.killClient(id_client);
}

void APIC_ServerCommand::commandMessage(void) {
    APIC_ServerUtilities serverUtilities;
    serverUtilities.readLog();
}

void APIC_ServerCommand::commandSysInfo(pid_t *pid_server, mqd_t *mq) {
    char *pid_str = convertPIDtoStr(pid_server);
    setupSignal();
    double data_cpu_usage;
    double data_mem_usage;
    serverRequestDaemon(mq, pid_str);
    
    printf("Getting data...\n");
 
    // Ch�? signal từ daemon
    while (!check_notifications_daemon);

    // Sau khi có signal thì đọc dữ liệu
    serverReceiveDataShm(&data_cpu_usage, &data_mem_usage);      // Đọc từ shared memory

    // In ra m�?n hình
    printf("CPU usage = %.2f%%\n", data_cpu_usage);
    printf("RAM usage = %.2f%%\n", data_mem_usage); 

    check_notifications_daemon = 0;
}


void APIC_ServerCommand::commandAddClient(int i_SocketFD, const char* c_IP, int i_Port, TYPE_ConnectionStatus e_Status) {
    APIC_ServerUtilities serverUtilities;
    serverUtilities.addClient(i_SocketFD, c_IP, i_Port, e_Status);
}