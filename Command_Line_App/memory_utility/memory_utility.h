#ifndef MEMORY_UTILITY_H
#define MEMORY_UTILITY_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

// Configuration
#define MEMORY_POOL_SIZE    (uint32_t) 1024  // Total memory pool size in bytes
#define BLOCK_SIZE          (uint32_t) 32    // Size of each block in bytes
#define BLOCK_COUNT         (uint32_t) (MEMORY_POOL_SIZE / BLOCK_SIZE) // Total number of blocks in the memory pool

// memory pool structure to manage the pool and track block usage
typedef struct 
{
    uint8_t pool[MEMORY_POOL_SIZE]; // Array to represent the memory pool
    bool block_usage[BLOCK_COUNT]; // Boolean array to track which blocks are allocated
} MemoryPool;

//buffers for receiving and processing incoming XML data via UART
extern char* g_uart_xml_raw_buffer;  //temporary buffer for receiving raw UART data
extern char* g_uart_xml_main_buffer; // main buffer for processed XML data
extern struct XMLDataExtractionResult *g_extracted_data;

/*************function prototypes**********************/
void MemoryPool_Init(void);
void* MemoryPool_Allocate(void);
void MemoryPool_Free(void* block_pointer);
void* MemoryPool_AllocatePages(uint32_t page_count);
void MemoryPool_FreePages(void* block_pointer, uint32_t page_count);
uint32_t MemoryPool_GetFreeBlocks(void);

#endif // MEMORY_UTILITY_H
