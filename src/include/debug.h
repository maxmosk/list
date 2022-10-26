#ifndef DEBUG_H_INCLUDED__
#define DEBUG_H_INCLUDED__


#include <stdio.h>
#include <stdlib.h>


#define CHECK(condition, retval)  \
do                                 \
{                                   \
    int condition__ = (condition);   \
    if (!condition__)                 \
    {                                  \
        ASSERT(condition__, #condition);\
        return retval;                   \
    }                                     \
}                                          \
while (0)

#ifndef NDEBUG
#define ASSERT(condition, message)               \
do                                                \
{                                                  \
    if (!(condition))                               \
    {                                                \
        fprintf(stderr, "*** ASSERTION ERROR ***\n"); \
        fprintf(stderr, "    Failed %s\n", message);   \
        fprintf(stderr, "    Location:\n");             \
        fprintf(stderr, "    File ----- %s\n", __FILE__);\
        fprintf(stderr, "    Function - %s\n", __func__); \
        fprintf(stderr, "    Line ----- %d\n", __LINE__);  \
        abort();                                            \
    }                                                        \
}                                                             \
while (0)
#else
#define ASSERT(condition, message) do {} while (0)
#endif /* NDEBUG */


#ifndef NDEBUG_LOG
#define LOGOPEN(logfile_)                                       \
FILE *log_opened = fopen(logfile_, "a");                         \
do                                                                \
{                                                                  \
    ASSERT(NULL != log_opened, "Log file opening");                 \
    LOGPRINTF("\nLog start  in file %32s, function %32s, line %6d\n",\
                                    __FILE__, __func__, __LINE__);    \
}                                                                      \
while (0)

#define LOGPRINTF(...) fprintf(log_opened, __VA_ARGS__)

#define LOGCLOSE()                                              \
do                                                               \
{                                                                 \
    LOGPRINTF("Log finish in file %32s, function %32s, line %6d\n",\
                                    __FILE__, __func__, __LINE__);  \
    fclose(log_opened);                                              \
}                                                                     \
while (0)
#else
#endif


#define PRINTLINE                                                         \
do                                                                         \
{                                                                           \
    printf("On file %s function %s line %d\n", __FILE__, __func__, __LINE__);\
    fflush(stdout);                                                           \
}                                                                              \
while (0)


#endif /* DEBUG_H_INCLUDED__ */

