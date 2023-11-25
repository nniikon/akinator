#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

#include "../include/akinator.h"

#include "../include/akinator_dump.h" // FIXME: delete
#include "../include/akinator_saveLoad.h" // FIXME: delete


const char* DATABASE_PATH  = "db.akt";
const char* DUMP_FILE_PATH = "dump.html";

int main()
{
    setlocale(LC_ALL, "ru_RU.UTF8");

    Akinator akin = {};
    FILE* dumpFile = fopen(DUMP_FILE_PATH, "w");

    if (dumpFile == NULL)
    {
        fwprintf(stderr, L"Can't open %s\n", DUMP_FILE_PATH);
        return -1;
    }
    fprintf(dumpFile, "<pre>\n");

    akinatorCtor(&akin, DATABASE_PATH, dumpFile);
    akinatorLoad(&akin);
    AkinatorOptions opt = AKIN_OPT_ERROR;
    while (1)
    {
        opt = akinatorGetMode();
        switch (opt)
        {
            case AKIN_OPT_GUESS:
                akinatorQuestion(&akin, akin.tree.rootBranch);
                break;
            case AKIN_OPT_DEFINITION:
                break;
            case AKIN_OPT_DUMP:
                akinatorGenPng(&akin);
                break;
            case AKIN_OPT_COMPARE:
                break;
            case AKIN_OPT_FORCE_QUIT:
                akinatorDtor(&akin);
                akinatorGoodbye();
                return 0;
            case AKIN_OPT_SAVE_QUIT:
                akinatorSaveToFile(&akin);
                akinatorDtor(&akin);
                akinatorGoodbye();
                return 0;

            case AKIN_OPT_ERROR:
            default:
                akinatorError();
                break;
        }
    }

    fclose(dumpFile);
}
