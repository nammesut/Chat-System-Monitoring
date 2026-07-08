////////////////////////////////////////////////////////////////////////////////////////////////////                                                    
///                                                 
/// \file       APIC_DaemonMonitor.c                               
/// \breif      Overview: Daemon monitor functionalities                                     
///                                                 
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>

#include "../include/APIC_DaemonMonitor.h"

typedef struct {
    float cpu_usage;        
    float memory_usage;
} sysinfo_t;

const char *path_cpu_stat       = "/proc/stat";
const char *path_memory_info    = "/proc/meminfo";


// H�?m đọc cpu từ /proc/stat
static bool APIC_DaemonMonitor::readCPUStatistic(TYPS_CPUStatistic *stat) {
    int fd = open(path_cpu_stat, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return false;  
    }

    char buffer[2048];  
    ssize_t len;
    
    while ((len = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[len] = '\0'; 
    }

    if (len < 0) {
        perror("read");
        close(fd);
        return false; 
    }

    sscanf(buffer, "cpu %llu %llu %llu %llu %llu %llu %llu",
        &stat->mull_User, &stat->mull_Nice, &stat->mull_System, &stat->mull_Idle,
        &stat->mull_IOWait, &stat->mull_IRQ, &stat->mull_SoftIRQ);

    close(fd);
    return true;
}

// H�?m tính CPU usage % giữa 2 lần lấy 
static double APIC_DaemonMonitor::computeCPUUsage(TYPS_CPUStatistic *prev, TYPS_CPUStatistic *curr) {
    unsigned long long prev_busy = prev->mull_User + prev->mull_Nice + prev->mull_System + prev->mull_IRQ + prev->mull_SoftIRQ;
    unsigned long long curr_busy = curr->mull_User + curr->mull_Nice + curr->mull_System + curr->mull_IRQ + curr->mull_SoftIRQ;

    unsigned long long prev_total = prev_busy + prev->mull_Idle + prev->mull_IOWait;
    unsigned long long curr_total = curr_busy + curr->mull_Idle + curr->mull_IOWait;

    unsigned long long delta_busy = curr_busy - prev_busy;
    unsigned long long delta_total = curr_total - prev_total;

    if (delta_total == 0) return 0.0;

    return (double)delta_busy / delta_total * 100.0;
}

// H�?m đọc mem từ /proc/meminfo v�? lấy data
static bool APIC_DaemonMonitor::readMemoryInfo(TYPS_MemoryInfo *info) {
    char buffer[2048];
    ssize_t len;

    int fd = open(path_memory_info, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return false;
    }

    len = read(fd, buffer, sizeof(buffer) - 1);

    if (len < 0) {
        perror("read");
        close(fd);
        return false;
    }

    buffer[len] = '\0';         

    close(fd);

    char *line = strtok(buffer, "\n");      // Lấy đoạn đầu tiên \n trong buffer

    while (line != NULL) {
        sscanf(line, "MemTotal:        %llu kB", &info->mull_MemoryTotal);
        sscanf(line, "MemAvailable:    %llu kB", &info->mull_MemoryAvailable);
        line = strtok(NULL, "\n");
    }

    return true;
}

// H�?m chuyển pid �? dạng str th�?nh pid_t
static pid_t APIC_DaemonMonitor::convertStringToPid(const char *pid_str) {
    return (pid_t)atoi(pid_str);
}

/*----------------------------------------------------------------------*/
// Lấy data CPU
double APIC_DaemonMonitor::getCpuUsage(TYPS_CPUStatistic *stat1, TYPS_CPUStatistic *stat2) {
    // Lấy lần 1
    if (APIC_DaemonMonitor::readCPUStatistic(stat1) == false) return -1;

    sleep(1);   // Ch�? 1s
    
    // Lấy lần 2
    if (APIC_DaemonMonitor::readCPUStatistic(stat2) == false) return -1;

    return APIC_DaemonMonitor::computeCPUUsage(stat1, stat2); 
}

// Tính toán data mem chuyển v�? %
double APIC_DaemonMonitor::getMemoryUsage(TYPS_MemoryInfo *info) {
    if (APIC_DaemonMonitor::readMemoryInfo(info) == false) return -1;

    return ((double)(info->mull_MemoryTotal - info->mull_MemoryAvailable) / info->mull_MemoryTotal) * 100.0;
}

// Nhận data từ server l�? pid của server (Thông báo thực hiện lấy data đi)
void APIC_DaemonMonitor::daemonReceiveDataFromMq(char *buffer) {
    APIC_MessageQueue messageQueue;
    messageQueue.messageQueueReceive(buffer);
}

// Lấy data v�? ghi v�?o Shared Memory
void APIC_DaemonMonitor::daemonWriteDataFromShm(void) {
    APIC_SharedMemory sharedMemory;
    TYPS_CPUStatistic stat1, stat2;    
    TYPS_MemoryInfo info;

    double cpu = APIC_DaemonMonitor::getCpuUsage(&stat1, &stat2);
    sleep(1);
    double mem = APIC_DaemonMonitor::getMemoryUsage(&info);

    sharedMemory.sharedMemoryWrite(&cpu, &mem);
}

// Từ PID nhận được gửi signal cho Server biết
void APIC_DaemonMonitor::sendSignal(pid_t pid) {
    if (kill(pid, SIGUSR1) < 0) {
        perror("kill");
    }   
    printf("Send data for server\n");
}


int main(int argc, char const *argv[]) {
    APIC_DaemonMonitor daemonMonitor;
    char buffer[64];  // Lấy PID �? dạng str

    while (1)
    {
        daemonMonitor.daemonReceiveDataFromMq(buffer);     // Do bật blocking nên sẽ ch�? tới khi nhận được message

        pid_t pid = APIC_DaemonMonitor::convertStringToPid(buffer);    // convert th�?nh pid_t

        daemonMonitor.daemonWriteDataFromShm();        // Ghi v�?o vùng shared memory
        daemonMonitor.sendSignal(pid);   // Gửi thông báo cho Server

        printf("Data wirtten\n");
    }
    
    return 0;   
}
