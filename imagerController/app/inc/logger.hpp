#include <stdio.h>

#define ENABLE_INFO         true
#define ENABLE_WARNING      true
#define ENABLE_ERROR        true

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define KDEF  "\033[0m"

#if ENABLE_INFO == true
#define LOG_INFO(x, ...) \
        { \
            printf(KBLU); \
            printf("INFO: ");\
            printf(x, ##__VA_ARGS__); \
            printf(KDEF); \
        }
#else
#define LOG_INFO(x, ...)
#endif

#if ENABLE_WARNING == true
#define LOG_WARNING(x, ...) \
        { \
            printf(KYEL); \
            printf("WARNING: ");\
            printf(x, ##__VA_ARGS__); \
            printf(KDEF); \
        }
#else
#define LOG_WARNING(x, ...)
#endif

#if ENABLE_ERROR == true
#define LOG_ERROR(x, ...) \
        { \
            printf(KRED); \
            printf("ERROR: ");\
            printf(x, ##__VA_ARGS__); \
            printf(KDEF); \
        }
#else
#define LOG_ERROR(x, ...)
#endif