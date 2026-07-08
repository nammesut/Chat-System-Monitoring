////////////////////////////////////////////////////////////////////////////////////////////////////                                                    
///                                                 
/// \file       SYSC_ServerApp.c                               
/// \breif      Overview: Server application entry point                                     
///                                                 
//////////////////////////////////////////////////////////////////////////////////////////////////// 

#include <stdio.h>
#include <stdlib.h>
#include "../include/APIC_ServerCore.h"

int main(int argc, char const *argv[])
{   
    APIC_ServerCommand  serverCommand;
    APIC_ServerCore     serverCore;
    if (argc != 2) {
        printf("Usage: %s <server_port>\n", argv[0]);
        return 1;
    }

    int server_port = atoi(argv[1]);

    serverCommand.displayCommandServer();
    int fd = serverCore.serverInit(server_port);
    serverCore.serverRun(fd);

    return 0;
}