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
    if (!condition)                                 \
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
#endif

#endif /* DEBUG_H_INCLUDED__ */

