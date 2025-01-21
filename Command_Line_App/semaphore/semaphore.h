#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <stdbool.h>

// Semaphore states for a binary semaphore
typedef enum 
{
    SEMAPHORE_UNLOCKED = 0,  // Semaphore is available for use
    SEMAPHORE_LOCKED = 1     // Semaphore is currently in use (locked)
} BinarySemaphoreState;

//interrupt handler flag
extern BinarySemaphoreState g_semaphore;

// Function to acquire the semaphore
void acquire_semaphore(BinarySemaphoreState *semaphore);

// Function to release the semaphore
void release_semaphore(BinarySemaphoreState *semaphore);

// Function to check semaphore
bool obtain_semaphore(BinarySemaphoreState *semaphore);

#endif // SEMAPHORE_H
