////////////////////////////////////////////////////////////////////////////////////////////////////                                                    
///                                                 
/// \file       APIC_SharedMemory.c                               
/// \breif      Overview: Shared memory functionalities                                     
///                                                 
//////////////////////////////////////////////////////////////////////////////////////////////////// 

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>

#include "APIC_SharedMemory.h"

#define SHM_SIZE 4096

const char *shm_name = "/my_shared_memory";
const char *sem_name  = "/my_shm_semaphore";


void APIC_SharedMemory::sharedMemoryWrite(double *cpu_usage, double *mem_usage) {
    int shm_fd;
    void *shm_ptr;

    // M? ho?c t?o semaphore (initial value = 1)
    sem_t *sem = sem_open(sem_name, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open (write)");
        return;
    }

    // T?o v?ng nh? c?n chia s?
    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);    
    if (shm_fd < 0) {
        perror("shm_open (write)");
        sem_close(sem);
        return;
    }

    // Xin kernel d? tr? frame
    if (ftruncate(shm_fd, SHM_SIZE) < 0) {
        perror("ftruncate");
        close(shm_fd);
        sem_close(sem);
        return;
    }

    // Xin c?p ph?t Virtual Page mapping v?o Physical Page
    shm_ptr = mmap(0, SHM_SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap (write)");
        close(shm_fd);
        sem_close(sem);
        return;
    }

    // --- V?o critical section ---
    sem_wait(sem);

    double *data = (double *)shm_ptr;   
    *(data + 0) = *cpu_usage;
    *(data + 1) = *mem_usage;

    // --- Ra critical section ---
    sem_post(sem);

    munmap(shm_ptr, SHM_SIZE);
    close(shm_fd);
    sem_close(sem);
}

void APIC_SharedMemory::sharedMemoryRead(double *get_data_cpu, double *get_data_mem) {
    int shm_fd; 
    void *shm_ptr;

    // M? semaphore ?? t?n t?i (daemon ph?i t?o tr??c)
    sem_t *sem = sem_open(sem_name, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open (read)");
        return;
    }

    shm_fd = shm_open(shm_name, O_RDONLY, 0666);
    if (shm_fd < 0) {
        perror("shm_open (read)");
        sem_close(sem);
        return;
    }

    shm_ptr = mmap(0, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap (read)");
        close(shm_fd);
        sem_close(sem);
        return;
    }

    // --- V?o critical section ---
    sem_wait(sem);

    double *data = (double *)shm_ptr;
    *get_data_cpu = *(data + 0);
    *get_data_mem = *(data + 1);

    // --- Ra critical section ---
    sem_post(sem);

    munmap(shm_ptr, SHM_SIZE);
    close(shm_fd);
    sem_close(sem);
}

void APIC_SharedMemory::sharedMemoryCleanup(void) {
    shm_unlink(shm_name);
    sem_unlink(sem_name);
}