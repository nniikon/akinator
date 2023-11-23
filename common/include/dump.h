#ifndef DUMP_H_
#define DUMP_H_

#include <stdio.h>

#ifndef DUMP_RELEASE
    #define DUMP(file ,...)                                                 \
    do                                                                      \
    {                                                                       \
        if (file != NULL)                                                   \
        {                                                                   \
            fprintf(file, "%s[%d]\t\t", __FILE__, __LINE__);                \
            fprintf(file, __VA_ARGS__);                                     \
        }                                                                   \
    } while (0)
    #define DUMP_FUNC_START(file)   DUMP(file, "%s started\n", __PRETTY_FUNCTION__)
    #define DUMP_FUNC_SUCCESS(file) DUMP(file, "%s success\n", __PRETTY_FUNCTION__)
#else
    #define DUMP              do {} while (0)
    #define DUMP_FUNC_START   do {} while (0)
    #define DUMP_FUNC_SUCCESS do {} while (0)
#endif

#endif // DUMP_H_