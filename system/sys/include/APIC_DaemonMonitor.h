////////////////////////////////////////////////////////////////////////////////////////////////////
///
///	\file		APIC_DaemonMonitor.h
///	\brief		Overview: Interface of the APIC_DaemonMonitor class
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __APIC_DAEMON_MONITOR_H__
#define __APIC_DAEMON_MONITOR_H__

#include "system/ipc/APIC_MessageQueue.h"
#include "system/ipc/APIC_SharedMemory.h"

// Thông tin cần thiết khi đọc từ /proc/stat 
struct TYPS_CPUStatistic {
    /// \brief Time spent in user mode. 
    unsigned long long mull_User;

    /// \brief Time spent in user mode with low priority (nice).
    unsigned long long mull_Nice;

    /// \brief Time spent in system mode.
    unsigned long long mull_System;

    /// \brief Time spent in the idle task. This value should be USER_HZ times the second entry in the /proc/uptime pseudo-file.
    unsigned long long mull_Idle;

    /// \brief Time spent waiting for I/O to complete.
    unsigned long long mull_IOWait;

    /// \brief Time spent servicing interrupts.
    unsigned long long mull_IRQ;

    /// \brief Time spent servicing softirqs.
    unsigned long long mull_SoftIRQ;
};

// Thông tin cần thiết khi đọc từ /proc/meminfo
struct TYPS_MemoryInfo {
    /// \brief Total memory.
    unsigned long long mull_MemoryTotal;

    /// \brief Available memory.
    unsigned long long mull_MemoryAvailable;
};

class APIC_DaemonMonitor {
public:
    /// \brief Get CPU usage percentage by reading /proc/stat and calculating the difference.
    /// \param stat1 Pointer to the first TYPS_CPUStatistic structure to store the first reading.
    /// \param stat2 Pointer to the second TYPS_CPUStatistic structure to store the second reading.
    /// \return CPU usage percentage.
    double getCpuUsage(TYPS_CPUStatistic *stat1, TYPS_CPUStatistic *stat2);

    /// \brief Get memory usage percentage by reading /proc/meminfo and calculating the difference.
    /// \param info Pointer to the TYPS_MemoryInfo structure to store memory information.
    /// \return Memory usage percentage.
    double getMemoryUsage(TYPS_MemoryInfo *info);

    /// \brief Receive data from the server via Message Queue.
    /// \param buffer Pointer to the buffer to store the received data.
    void daemonReceiveDataFromMq(char *buffer);

    /// \brief Write CPU and memory usage data to shared memory.
    void daemonWriteDataFromShm(void);

    /// \brief Send a signal to the server to notify that data has been written.
    /// \param pid The process ID of the server to send the signal to.
    void sendSignal(pid_t pid);

private:
    /// \brief Convert a string representation of a PID to a pid_t type.
    /// \param pid_str The string representation of the PID.
    /// \return The converted pid_t value.
    static pid_t convertStringToPid(const char *pid_str);

    /// \brief Read CPU statistics from /proc/stat.
    /// \param stat Pointer to the TYPS_CPUStatistic structure to store the read statistics.
    /// \return True if successful, False otherwise.
    bool readCPUStatistic(TYPS_CPUStatistic *stat);

    /// \brief Read memory information from /proc/meminfo.
    /// \param info Pointer to the TYPS_MemoryInfo structure to store the read information.
    /// \return True if successful, False otherwise.
    bool readMemoryInfo(TYPS_MemoryInfo *info);

    /// \brief Compute CPU usage percentage based on two readings of CPU statistics.
    /// \param stat1 Pointer to the first TYPS_CPUStatistic structure.
    /// \param stat2 Pointer to the second TYPS_CPUStatistic structure.
    /// \return CPU usage percentage.
    double computeCPUUsage(TYPS_CPUStatistic *stat1, TYPS_CPUStatistic *stat2);
};

#endif