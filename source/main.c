#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

#include "../include/akinator.h"

#include "../include/akinator_dump.h" // FIXME: delete
#include "../include/akinator_saveLoad.h" // FIXME: delete
#include "../include/akinator_definition.h"

const char* DATABASE_PATH  = "db.akt";
const char* DUMP_FILE_PATH = "dump.html";

#define AKINATOR_CHECK_MAIN_ERR(err)                                          \
    if (err != AKINATOR_ERR_NO)                                               \
    {                                                                         \
        akinatorError();                                                      \
        akinatorDtor(&akin);                                                  \
        gameOver = 1;                                                         \
    }

int main()
{
    setlocale(LC_ALL, "ru_RU.UTF8");

    Akinator akin = {};

    FILE* dumpFile = fopen(DUMP_FILE_PATH, "w");
    if (dumpFile == NULL)
    {
        fwprintf(stderr, L"Can't open %s\n", DUMP_FILE_PATH);
        return AKINATOR_ERR_OPEN_FILE;
    }
    fprintf(dumpFile, "<pre>\n");

    akinatorCtor(&akin, DATABASE_PATH, dumpFile);
    akinatorLoad(&akin);

    AkinatorOptions opt = AKIN_OPT_ERROR;
    AkinatorError err = AKINATOR_ERR_NO;
    int gameOver = 0;
    while (!gameOver)
    {
        opt = akinatorGetMode();
        switch (opt)
        {
            case AKIN_OPT_GUESS:
                akinatorQuestion(&akin, akin.tree.rootBranch, &err);
                AKINATOR_CHECK_MAIN_ERR(err);
                break;

            case AKIN_OPT_DEFINITION:
                err = akinatorGetDefinition(&akin);
                AKINATOR_CHECK_MAIN_ERR(err);
                break;

            case AKIN_OPT_DUMP:
                akinatorGenPng(&akin);
                break;

            case AKIN_OPT_COMPARE:
                break;

            case AKIN_OPT_SAVE_QUIT:
                akinatorSaveToFile(&akin);
                [[fallthrough]];
            case AKIN_OPT_FORCE_QUIT:
                akinatorDtor(&akin);
                akinatorGoodbye();
                gameOver = 1;
                break;

            case AKIN_OPT_ERROR:
            default:
                akinatorError();
                akinatorDtor(&akin);
                gameOver = 1;
                break;
        }
    }

    fclose(dumpFile);
}
