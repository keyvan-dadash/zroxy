


#ifndef LOGS_H
#define LOGS_H

#include <stdio.h>

#define MINIMUM_LOG_LEVEL 4


#define FATAL_ERROR_LOG_LEVEL 1
#define ERROR_LOG_LEVEL       2
#define WARNING_LOG_LEVEL     3
#define INFO_LOG_LEVEL        4
#define VERBOSE_LOG_LEVEL     5


//FATAL
#if FATAL_ERROR_LOG_LEVEL < MINIMUM_LOG_LEVEL
#define LOG_FATAL(error_code, fmt, ...) \
        do { \
            fprintf(stderr, fmt, ##__VA_ARGS__); \
            exit(error_code); \
        } while(0);
#else
#define LOG_FATAL(fmt, ...) {}
#endif

//ERROR
#if ERROR_LOG_LEVEL < MINIMUM_LOG_LEVEL
#define LOG_ERROR(fmt, ...) \
        do { \
            fprintf(stderr, fmt, ##__VA_ARGS__); \
        } while(0);
#else
#define LOG_ERROR(fmt, ...) {}
#endif

//WARNING
#if WARNING_LOG_LEVEL < MINIMUM_LOG_LEVEL
#define LOG_WARNING(fmt, ...) \
        do { \
            fprintf(stdout, fmt, ##__VA_ARGS__); \
        } while(0);
#else
#define LOG_WARNING(fmt, ...) {}
#endif

//INFO
#if INFO_LOG_LEVEL < MINIMUM_LOG_LEVEL
#define LOG_INFO(fmt, ...) \
        do { \
            fprintf(stdout, fmt, ##__VA_ARGS__); \
        } while(0);
#else
#define LOG_INFO(fmt, ...) {}
#endif

//VERBOSE
#if VERBOSE_LOG_LEVEL < MINIMUM_LOG_LEVEL
#define LOG_VERBOSE(fmt, ...) \
        do { \
            fprintf(stdout, fmt, ##__VA_ARGS__); \
        } while(0);
#else
#define LOG_VERBOSE(fmt, ...) {}
#endif



#endif /* LOGS_H */