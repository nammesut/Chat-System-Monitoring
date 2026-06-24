#ifndef __DAEMON_H__
#define __DAEMON_H__

#include "message_queue.h"
#include "shared_memory.h"

// Information needed when reading from /proc/stat.
typedef struct {
    unsigned long long user, nice, system, idle, iowait, irq, softirq;
} cpu_stat_t;

// Information needed when reading from /proc/meminfo.
typedef struct {
    unsigned long long memtotal, memavailable;
} mem_info_t;

/**
 * Retrieve data from /pro/stat and calculate CPU usage (%).
 */
double get_cpu_usage(cpu_stat_t *stat1, cpu_stat_t *stat2);

/**
 * Retrieve data from /proc/meminfo and calculate CPU usage (%).
 */
double get_mem_usage(mem_info_t *info);

/**
 * Receive data from the server via Message Queue.
 */
void daemon_receive_data_mq(char *buffer);

/**
 * Retrieving data to write to Shared Memory.
 */
void daemon_write_data_shm(void);

/**
 * Send a signal to the server to indicate that writing is complete.
 */
void send_signal(pid_t pid);

#endif