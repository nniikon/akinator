#ifndef AKINATOR_PROMTS_H_
#define AKINATOR_PROMTS_H_

#include <wchar.h>

typedef enum
{
    AKIN_OPT_ERROR       = 0,
    AKIN_OPT_GUESS       = 1,
    AKIN_OPT_DEFINITION  = 2,
    AKIN_OPT_COMPARE     = 3,
    AKIN_OPT_DUMP        = 4,
    AKIN_OPT_SAVE_QUIT   = 5,
    AKIN_OPT_FORCE_QUIT  = 6,
} AkinatorOptions;  

AkinatorOptions akinatorGetMode();

int akinatorGetOption(const wchar_t cAllowed[]);

void akinatorSay        (const wchar_t input[]);

void akinatorPrintAndSay(const wchar_t input[]);

#endif // AKINATOR_PROMTS_H_
