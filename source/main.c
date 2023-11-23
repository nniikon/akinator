#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

#include "../include/akinator.h"

const char* DATABASE_PATH  = "db.akt";
const char* DUMP_FILE_PATH = "dump.html";

int main()
{
    setlocale(LC_ALL, "ru_RU.UTF8");

    Akinator akin = {};
    FILE* dumpFile = fopen(DUMP_FILE_PATH, "w");

    if (dumpFile == NULL)
    {
        fprintf(stderr, "Can't open %s\n", DUMP_FILE_PATH);
        return -1;
    }
    fprintf(dumpFile, "<pre>\n");

    akinatorCtor    (&akin, DATABASE_PATH, dumpFile);
    akinatorQuestion(&akin, akin.tree.rootBranch);
    akinatorDtor    (&akin);

    fclose(dumpFile);
}