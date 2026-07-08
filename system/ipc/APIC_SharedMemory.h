////////////////////////////////////////////////////////////////////////////////////////////////////
///
///	\file		APIC_SharedMemory.h
///	\brief		Overview: Interface of the APIC_SharedMemory class
///
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __APIC_SHARED_MEMORY_H__
#define __APIC_SHARED_MEMORY_H__

#include <stdio.h>
#include <semaphore.h>

class APIC_SharedMemory {
public:
    /// \brief Write CPU and memory usage data to shared memory.
    /// \param cpu_usage Pointer to the CPU usage data.
    /// \param mem_usage Pointer to the memory usage data.
    void sharedMemoryWrite(double *cpu_usage, double *mem_usage);

    /// \brief Read CPU and memory usage data from shared memory.
    /// \param get_data_cpu Pointer to store the CPU usage data.
    /// \param get_data_mem Pointer to store the memory usage data.
    void sharedMemoryRead(double *get_data_cpu, double *get_data_mem);

    /// \brief Unlink shared memory and semaphore (cleanup when server exits).
    void sharedMemoryCleanup(void);
};

#endif