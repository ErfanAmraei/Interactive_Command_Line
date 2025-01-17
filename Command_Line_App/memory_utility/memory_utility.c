
/**
 * @file memory_utility.c
 * 
 * @brief Custom memory pool implementation for efficient memory management.
 * 
 * This memory pool was developed as an alternative to using dynamic memory 
 * allocation functions like `malloc` and `free` for several reasons:
 * 
 * 1. **Avoiding Fragmentation**:
 *    - `malloc` and `free` can lead to memory fragmentation, especially in 
 *      embedded systems with constrained memory.
 *    - Over time, fragmentation can cause allocation failures even when 
 *      sufficient total memory is available, due to scattered free blocks.
 *    - The memory pool ensures contiguous blocks of memory and avoids fragmentation.
 * 
 * 2. **Deterministic Behavior**:
 *    - `malloc` and `free` have non-deterministic execution times, making them
 *      unsuitable for real-time systems where predictability is critical.
 *    - The memory pool uses fixed-size blocks and simple bookkeeping, ensuring 
 *      constant-time allocation and deallocation.
 * 
 * 3. **Efficient Resource Utilization**:
 *    - Memory pools are designed to allocate and deallocate fixed-size blocks 
 *      efficiently, minimizing overhead and maximizing available memory.
 *    - Since the block size is known in advance, memory waste due to alignment 
 *      issues is minimized.
 * 
 * 4. **Improved Reliability**:
 *    - Embedded systems often operate in environments where memory leaks or 
 *      excessive allocations could lead to system failure.
 *    - A memory pool provides better control over memory usage, reducing the 
 *      likelihood of such issues.
 * 
 * 5. **Simplified Debugging**:
 *    - Issues such as double freeing, dangling pointers, and heap corruption 
 *      can be challenging to debug in systems using `malloc` and `free`.
 *    - The memory pool design is straightforward and less prone to such errors.
 * 
 * 6. **Suitability for Constrained Systems**:
 *    - Many embedded systems have limited heap space, and managing it dynamically 
 *      can be risky.
 *    - By allocating a dedicated memory pool at compile-time, we ensure that the 
 *      memory usage is predictable and within bounds.
 * 
 * In summary, this memory pool is tailored to the specific needs of embedded 
 * systems, offering predictable, efficient, and reliable memory management, 
 * which is crucial for maintaining system stability and performance.
 */

#include "memory_utility.h"
#include <stdlib.h>

// Global memory pool instance
static MemoryPool memPool;

//buffers for receiving and processing incoming XML data via UART
char* g_uart_xml_raw_buffer = NULL;  //temporary buffer for receiving raw UART data
char* g_uart_xml_main_buffer = NULL; // main buffer for processed XML data

struct XMLDataExtractionResult g_extracted_data;
/**
 * @brief Initializes the memory pool by clearing the memory and marking all blocks as free.
 */
void MemoryPool_Init(void) 
{
    // Clear all bytes in the memory pool to zero
    memset(memPool.pool, 0, MEMORY_POOL_SIZE);

    // Set all blocks in the block usage array to false (free)
    memset(memPool.block_usage, 0, BLOCK_COUNT);
}

/**
 * @brief Allocates a single block of memory from the pool.
 * @return Pointer to the allocated block, or NULL if no free block is available.
 */
void* MemoryPool_Allocate(void) 
{
    void* allocated_block = NULL; // Pointer to the block to return

    // Iterate through the block usage array to find a free block
    for (uint32_t block_index = 0; block_index < BLOCK_COUNT; block_index++) 
    {
        if (!memPool.block_usage[block_index]) 
        { // Check if the block is free
            memPool.block_usage[block_index] = true; // Mark the block as allocated
            allocated_block = &memPool.pool[block_index * BLOCK_SIZE]; // Get the address of the block
            break; // Stop searching after finding a free block
        }
    }

    return allocated_block; // Return the allocated block pointer or NULL
}

/**
 * @brief Frees a single block of memory back to the pool.
 * @param block_pointer Pointer to the block to free.
 */
void MemoryPool_Free(void* block_pointer) 
{
    if (block_pointer != NULL) 
    { // Ensure the pointer is valid
        // Calculate the block index from the pointer
        uint32_t block_index = ((uint8_t*)block_pointer - memPool.pool) / BLOCK_SIZE;

        // If the index is valid, mark the block as free
        if (block_index < BLOCK_COUNT) 
        {
            memPool.block_usage[block_index] = false;

            block_pointer = NULL;
        }
    }
}

/**
 * @brief Allocates multiple contiguous blocks (pages) of memory from the pool.
 * @param page_count Number of contiguous blocks to allocate.
 * @return Pointer to the first block of the allocated pages, or NULL if allocation fails.
 */
void* MemoryPool_AllocatePages(uint32_t page_count) 
{
    void* allocated_pages = NULL; // Pointer to the first block of allocated pages
    
    uint32_t start_index = 0;
    uint32_t offset = 0;

    // Ensure the requested page count is valid
    if (page_count > 0 && page_count <= BLOCK_COUNT) 
    {
        // Search the block usage array for a range of free blocks
        for (start_index = 0; start_index <= BLOCK_COUNT - page_count; start_index++) 
        {
            bool can_allocate_pages = true; // Assume the blocks can be allocated

            // Check if the required number of contiguous blocks are free
            for (offset = 0; offset < page_count; ++offset) 
            {
                if (memPool.block_usage[start_index + offset]) 
                { // If a block is already allocated
                    can_allocate_pages = false; // Allocation is not possible
                    break; // Stop checking further
                }
            }

            if (can_allocate_pages) 
            { // If the range is free, allocate the blocks
                for (offset = 0; offset < page_count; ++offset) 
                {
                    memPool.block_usage[start_index + offset] = true; // Mark the blocks as allocated
                }
                allocated_pages = &memPool.pool[start_index * BLOCK_SIZE]; // Get the address of the first block
                break; // Allocation complete, exit loop
            }
        }
    }

    return allocated_pages; // Return the pointer to the allocated pages or NULL
}

/**
 * @brief Frees multiple contiguous blocks (pages) of memory back to the pool.
 * @param block_pointer Pointer to the first block of the pages to free.
 * @param page_count Number of contiguous blocks to free.
 */
void MemoryPool_FreePages(void* block_pointer, uint32_t page_count) 
{
    // Ensure valid inputs
    if (block_pointer != NULL && page_count > 0) 
    { 
        //calculate the starting block index
        uint32_t start_block_index = ((uint8_t*)block_pointer - memPool.pool) / BLOCK_SIZE;

        //ensure the range is valid
        if (start_block_index + page_count <= BLOCK_COUNT) 
        {
            //mark all blocks in the range as free
            for (uint32_t offset = 0; offset < page_count; offset++) 
            {
                memPool.block_usage[start_block_index + offset] = false;
            }

            block_pointer = NULL; //avoid dangling pointer after freeing memory
        }
    }
}

/**
 * @brief Calculates and returns the number of free blocks in the memory pool.
 * @return Number of free blocks available in the pool.
 */
uint32_t MemoryPool_GetFreeBlocks(void) 
{
    uint32_t free_block_count = 0; // Counter for free blocks

    // Iterate through the block usage array
    for (uint32_t block_index = 0; block_index < BLOCK_COUNT; block_index++) 
    {
        if (!memPool.block_usage[block_index]) 
        { // If the block is free
            free_block_count++; // Increment the counter
        }
    }

    return free_block_count; // Return the total number of free blocks
}