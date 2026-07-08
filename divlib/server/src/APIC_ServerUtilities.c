////////////////////////////////////////////////////////////////////////////////////////////////////                                                    
///                                                 
/// \file       APIC_ServerUtilities.h                               
/// \breif      Overview: Server utility functions                                     
///                                                 
//////////////////////////////////////////////////////////////////////////////////////////////////// 

#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#include "../include/APIC_ServerUtilities.h"

TYPS_ClientInfo clients[MAX_CLIENTS];
int client_count = 0;    

pthread_mutex_t client_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

const char *log_path = "/home/haidoan2098/Workspace/Mini-Project_SysMonChat/log/server_messages.log";  


void APIC_ServerUtilities::addClient(int sock_fd, const char *ip, int port, TYPE_ConnectionStatus status) {
    pthread_mutex_lock(&client_mutex);

    // Thêm mới nếu chưa đầy
    if (client_count < MAX_CLIENTS) {
        clients[client_count].mi_ID = client_count;
        clients[client_count].mi_SocketFD = sock_fd;
        strncpy(clients[client_count].mc_IP, ip, INET_ADDRSTRLEN - 1);
        clients[client_count].mc_IP[INET_ADDRSTRLEN - 1] = '\0';
        clients[client_count].mi_Port = port;   
        clients[client_count].me_Status = status;
        client_count++;
    } 
    else {
        // Tái sử dụng slot đã ngắt kết nối
        bool replaced = false;
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].me_Status == TYPE_ConnectionStatus_Disconnected) {
                clients[i].mi_ID = client_count;
                clients[i].mi_SocketFD = sock_fd;
                strncpy(clients[i].mc_IP, ip, INET_ADDRSTRLEN - 1);
                clients[i].mc_IP[INET_ADDRSTRLEN - 1] = '\0';
                clients[i].mi_Port = port;   
                clients[i].me_Status = status;
                replaced = true;
                break;
            }
        }
        if (!replaced) {
            printf("Client list is full!\n");
        }
    }

    pthread_mutex_unlock(&client_mutex);
}

void APIC_ServerUtilities::displayClients() {
    pthread_mutex_lock(&client_mutex);

    printf("\n============= Current Clients List =============\n");
    for (int i = 0; i < client_count; i++) {
        if (clients[i].me_Status == TYPE_ConnectionStatus_Connected) {
            printf("ID: %d  |  IP: %s  | PORT: %d\n", i, clients[i].mc_IP, clients[i].mi_Port);
        }
    }
    printf("================================================\n\n");

    pthread_mutex_unlock(&client_mutex);
}

void APIC_ServerUtilities::killClient(int id_client) {
    pthread_mutex_lock(&client_mutex);

    if (id_client >= 0 && id_client < MAX_CLIENTS && clients[id_client].me_Status == TYPE_ConnectionStatus_Connected) {
        send(clients[id_client].mi_SocketFD, "__KILL__", strlen("__KILL__"), 0);     // Clinet nhận được cái n�?y thì cút    
        close(clients[id_client].mi_SocketFD);
        clients[id_client].me_Status = TYPE_ConnectionStatus_Disconnected;

        printf("\nTerminated Client ID %d\n", id_client);
    } else {
        printf("Invalid client ID or already disconnected\n");
    }

    pthread_mutex_unlock(&client_mutex);
}

void APIC_ServerUtilities::writeLog(const char *msg) {
    char buffer[BUF_SIZE];      
    int len = snprintf(buffer, sizeof(buffer), "%s", msg);

    if (len < 0) {
        return;
    }

    pthread_mutex_lock(&log_mutex);

    int fd = open(log_path,  O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        perror("open log");
        pthread_mutex_unlock(&log_mutex);
        return;
    }

    ssize_t ret = write(fd, buffer, len);
    fsync(fd);      
    if (ret < 0) {
        perror("write log");
    }

    close(fd);
    pthread_mutex_unlock(&log_mutex);
}

void APIC_ServerUtilities::readLog() {
    char buffer[BUF_SIZE];
    ssize_t n;

    pthread_mutex_lock(&log_mutex);

    int fd = open(log_path, O_RDONLY);
    if (fd < 0) {
        perror("open log");
        pthread_mutex_unlock(&log_mutex);
        return; 
    }

    while ((n = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[n] = '\0';       
        printf("%s", buffer);
    }

    if (n < 0) {
        perror("read log file");
    }

    close(fd);
    pthread_mutex_unlock(&log_mutex);
}