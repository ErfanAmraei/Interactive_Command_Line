#include "semaphore.h"
#include <stdio.h>

//interrupt handler flag
BinarySemaphoreState g_semaphore = SEMAPHORE_UNLOCKED;

/**
 * @brief Acquires the binary semaphore.
 * 
 * This function attempts to acquire the semaphore. If the semaphore is unlocked, it will
 * be locked, and the function will print a message indicating the semaphore has been acquired.
 * If the semaphore is already locked, the function will print a message indicating it's waiting.
 * This is typically used in the main application to check if the interrupt signal is ready to be processed.
 * 
 * @param semaphore Pointer to the semaphore state.
 */
void acquire_semaphore(BinarySemaphoreState *semaphore) 
{
    // Check if the semaphore is unlocked (available for use)
    if (*semaphore == SEMAPHORE_UNLOCKED) 
    {
        *semaphore = SEMAPHORE_LOCKED; // Lock the semaphore
    } 
}

/**
 * @brief Releases the binary semaphore.
 * 
 * This function releases the semaphore. If the semaphore is locked, it will be unlocked, 
 * and the function will print a message indicating the semaphore has been released.
 * 
 * @param semaphore Pointer to the semaphore state.
 */
void release_semaphore(BinarySemaphoreState *semaphore) 
{
    // Check if the semaphore is locked before releasing
    if (*semaphore == SEMAPHORE_LOCKED) 
    {
        *semaphore = SEMAPHORE_UNLOCKED; // Unlock the semaphore
    } 
}

/**
 * @brief Checks the state of a binary semaphore.
 *
 *
 * @param semaphore Pointer to the BinarySemaphoreState variable representing 
 *                  the state of the semaphore. The pointer must not be NULL.
 *
 * @return `true` if the semaphore is locked (`SEMAPHORE_LOCKED`), `false` otherwise.
 *
 */
bool obtain_semaphore(BinarySemaphoreState *semaphore)
{
    bool outcome = false;

    if(semaphore)
    {
        outcome = (*semaphore == SEMAPHORE_LOCKED);
    }
    else
    {
        outcome = false;
    }

    return outcome;
}

