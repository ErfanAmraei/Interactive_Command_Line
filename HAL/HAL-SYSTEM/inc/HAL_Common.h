#ifndef __HAL_COMM_H
#define __HAL_COMM_H


typedef enum {
    HAL_OK = 0,         // Operation completed successfully
    HAL_ERROR = 1,      // General error occurred
    HAL_BUSY = 2,       // Peripheral is busy
    HAL_TIMEOUT = 3,    // Operation timed out
    HAL_INVALID = 4,    // Invalid parameter or operation
    HAL_UNSUPPORTED = 5 // Operation not supported
} HAL_StatusTypeDef;

#endif /* __HAL_COMM_H */
