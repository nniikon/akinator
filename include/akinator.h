#ifndef AKINATOR_H_
#define AKINATOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "../binaryTree/tree_include/tree.h"
#include "akinator_promts.h"
#include "../common/include/memAllocations.h"

typedef struct
{
    Tree tree;

    FILE* dumpFile;
    unsigned int dumpIndex;

    const char* databasePath;

	AkinatorNode* nodeBuffer;
    unsigned int capacity;
    unsigned int freeIndex;

	wchar_t* wordBuffer;
	unsigned int wordCapacity;
	unsigned int wordFreeIndex;
} Akinator;

typedef enum
{
    #define DEF_AKINATOR_ERR(err, msg) AKINATOR_ERR_ ## err,
    #include "akinator_err_codegen.inc"
    #undef  DEF_AKINATOR_ERR
} AkinatorError;

AkinatorError akinatorCtor(Akinator* akin, const char* database, FILE* dumpFile);

AkinatorError akinatorDtor(Akinator* akin);

const char* akinatorGetErrorMsg(AkinatorError err);

TreeNode* akinatorQuestion(Akinator* akin, TreeNode* node); 

#endif // AKINATOR_H_
