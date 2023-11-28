#ifndef AKINATOR_H_
#define AKINATOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "akinator_stack.h"
#include "../binaryTree/tree_include/tree.h"
#include "../common/include/memAllocations.h"

typedef struct
{
    Tree tree;

    FILE* dumpFile;
    unsigned int dumpIndex;

    const char* databasePath;

	AkinatorNode* nodeBuffer;
    unsigned int nodeCapacity;
    unsigned int nodeFreeIndex;

	wchar_t* wordBuffer;
	unsigned int wordCapacity;
	unsigned int wordFreeIndex;

    wchar_t* loadBuffer;
} Akinator;

typedef enum
{
    #define DEF_AKINATOR_ERR(err, msg) AKINATOR_ERR_ ## err,
    #include "akinator_err_codegen.inc"
    #undef  DEF_AKINATOR_ERR
} AkinatorError;

AkinatorError akinatorCtor       (Akinator* akin, const char* database, FILE* dumpFile);
AkinatorError akinatorDtor       (Akinator* akin);
AkinatorNode* akintorNodeCalloc  (Akinator* akin);
wchar_t*      akinatorWordCalloc (Akinator* akin);
const char*   akinatorGetErrorMsg(AkinatorError err);

AkinatorError akinatorStartModeGuess(Akinator* akin);

#define AKINATOR_DUMP_RETURN_ERROR(err)                                       \
    do                                                                        \
    {                                                                         \
        DUMP(akin->dumpFile, "%s\n", akinatorGetErrorMsg(err));               \
        return err;                                                           \
    } while (0)

#endif // AKINATOR_H_
