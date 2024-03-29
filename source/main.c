#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

#include "../include/akinator.h"

#include "../include/akinator_definition.h"
#include "../include/akinator_dump.h"
#include "../include/akinator_prompts.h"
#include "../include/akinator_saveLoad.h"

// FIXME: add cmd args.
const char* AKINATOR_DATABASE_PATH  = "db.akt";
const char* AKINATOR_DUMP_FILE_PATH = "dump.html";

#define AKINATOR_CHECK_MAIN_ERR(err)                                          \
    if (err != AKINATOR_ERR_NO)                                               \
    {                                                                         \
        akinatorError();                                                      \
        gameOver = 1;                                                         \
    }


FILE* akinatorLogFileInit(const char fileName[]);

int main()
{
    setlocale(LC_ALL, "ru_RU.UTF8");

    Akinator akin = {};

    AkinatorError err = AKINATOR_ERR_NO;

	FILE* dumpFile = akinatorLogFileInit(AKINATOR_DUMP_FILE_PATH);
	if (dumpFile == NULL)
	{
		fwprintf(stderr, L"Не удалось открыть файл %s\n", AKINATOR_DUMP_FILE_PATH);
		return AKINATOR_ERR_OPEN_FILE;
	}

    err = akinatorCtor(&akin, AKINATOR_DATABASE_PATH, dumpFile);
	if (err != AKINATOR_ERR_NO)
	{
        fclose(dumpFile);
		fprintf(dumpFile, "%s\n", akinatorGetErrorMsg(err));
        return err;
	}

    err = akinatorLoad(&akin);
    if (err != AKINATOR_ERR_NO)
    {
        fclose(dumpFile);
        akinatorDtor(&akin);
        fprintf(dumpFile, "%s\n", akinatorGetErrorMsg(err));
        return err;
    }

    AkinatorOptions opt = AKIN_OPT_ERROR;
    // bool?
    int gameOver = 0;
    while (!gameOver)
    {
        opt = akinatorGetMode(&err);
        if (err != AKINATOR_ERR_NO)
        {
            fprintf(dumpFile, "%s\n", akinatorGetErrorMsg(err));
            break;
        }
        switch (opt)
        {
case AKIN_OPT_GUESS     : err = akinatorStartModeGuess     (&akin);    break;
case AKIN_OPT_DEFINITION: err = akinatorStartModeDefinition(&akin);    break;
case AKIN_OPT_DUMP      : err = akinatorStartModeDump      (&akin, 1); break;
case AKIN_OPT_COMPARE   : err = akinatorStartModeCompare   (&akin);    break;
case AKIN_OPT_SAVE_QUIT : err = akinatorSaveToFile         (&akin);
case AKIN_OPT_FORCE_QUIT:       akinatorGoodbye();
                gameOver = 1;
                break;

            case AKIN_OPT_ERROR:
            default:
                akinatorError();
                gameOver = 1;
                break;
        }
        AKINATOR_CHECK_MAIN_ERR(err);
    }

	akinatorDtor(&akin);
    fclose(dumpFile);
    return AKINATOR_ERR_NO;
}


FILE* akinatorLogFileInit(const char fileName[])
{
	FILE* dumpFile = fopen(AKINATOR_DUMP_FILE_PATH, "w");
    if (dumpFile != NULL)
    {
   		fprintf(dumpFile, "<pre>\n");
	}
	return dumpFile;

}
