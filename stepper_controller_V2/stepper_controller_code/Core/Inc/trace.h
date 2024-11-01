#include <stdio.h>
#include "main.h"

#define UART_LOGING     1
#define USB_LOGING      1

#define ENABLE_INFO         true
#define LOG_BUFFER_SIZE     400
#define UART_HANDLE         huart3

extern uint8_t log_buffer[];
extern UART_HandleTypeDef huart3;

#if UART_LOGING == 1 && USB_LOGING == 1
#define LOG_INFO(x, ...) \
        { \
            int text_len  = sprintf(log_buffer,x, ##__VA_ARGS__); \
            HAL_UART_Transmit(&UART_HANDLE, log_buffer, text_len, 1000); \
            CDC_Transmit_FS(log_buffer, text_len); \
        }
#elif UART_LOGING == 1
#define LOG_INFO(x, ...) \
        { \
            int text_len  = sprintf(log_buffer,x, ##__VA_ARGS__); \
            HAL_UART_Transmit(&UART_HANDLE, log_buffer, text_len, 1000); \
        }
#elif USB_LOGING == 1
#define LOG_INFO(x, ...) \
        { \
            int text_len  = sprintf(log_buffer,x, ##__VA_ARGS__); \
            CDC_Transmit_FS(log_buffer, text_len); \
        }
#else
#define LOG_INFO(x, ...) \
        { \
        }

#endif