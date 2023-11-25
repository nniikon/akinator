#include "../include/akinator_dump.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../include/akinator_dump_cfg.h"

#define BUFFER_SIZE 50

// Should somehow return error...
void akinatorMakeLogdir()
{
    char cmdBuffer[BUFFER_SIZE] = {};
    snprintf(cmdBuffer, BUFFER_SIZE, "mkdir %s -p", AKINATOR_LOGS_FOLDER_NAME);

    system(cmdBuffer);
}

static void akinsatorCompiteDot(Akinator* akin)
{
    char cmdBuffer[BUFFER_SIZE] = {};
    snprintf(cmdBuffer, BUFFER_SIZE, "dot " AKINATOR_DOT_FILE_MASK\
                 " -Tpng -o " AKINATOR_PNG_OUTPUT_MASK, akin->dumpIndex, akin->dumpIndex);
    system(cmdBuffer);
}


#define log(...) fwprintf(dotFile, __VA_ARGS__)


static void akinatorPrintNodes(TreeNode* node, FILE* dotFile)
{
    const char* color = NULL;
    if (node->data->type == AKINATOR_NODE_OBJ)
        color = AKINATOR_OBJ_COLOR;
    else
        color = AKINATOR_QST_COLOR;

    log(L"\"%p\" [shape = Mrecord, style = filled, fillcolor = \"%s\", color = \"%s\", ", 
                    node, color, AKINATOR_BLK_COLOR);
    log(L"label = \"{adress: %p | DATA: ", node);
    fwprintf(dotFile, node->data->str);
    log(L" | {<leftPrt> LEFT: %p | <rightPrt> RIGHT: %p}}\"];\n", node->leftBranch, node->rightBranch);

    if (node->leftBranch != NULL)
    {
        akinatorPrintNodes(node->leftBranch, dotFile);
        if (node->leftBranch->data->type == AKINATOR_NODE_OBJ)
            color = AKINATOR_OBJ_COLOR;
        else
            color = AKINATOR_QST_COLOR;
        log(L"\"%p\":<leftPrt> -> \"%p\"[color = \"%s\", weight = 1]\n", node, node->leftBranch, color);
    }
    if (node->rightBranch != NULL)
    {
        akinatorPrintNodes(node->rightBranch, dotFile);
        if (node->rightBranch->data->type == AKINATOR_NODE_OBJ)
            color = AKINATOR_OBJ_COLOR;
        else
            color = AKINATOR_QST_COLOR;
        log(L"\"%p\":<rightPrt> -> \"%p\"[color = \"%s\", weight = 1]\n", node, node->rightBranch, color);
    }
    return;
}


AkinatorError akinatorGenPng(Akinator* akin)
{
    akinatorMakeLogdir();
    if (akin == NULL)
        AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_NULLPTR_PASSED);
    if (akin->dumpFile == NULL)
        AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_NULLPTR_PASSED);

    char fileBuffer[BUFFER_SIZE] = {};
    snprintf(fileBuffer, BUFFER_SIZE, AKINATOR_DOT_FILE_MASK, akin->dumpIndex);

    FILE* dotFile = fopen(fileBuffer, "w");
    if (dotFile == NULL)
    {
        fclose(dotFile);
        AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_OPEN_FILE);
    }
    log(L"digraph G{\n"
        L"rankdir = TB;\n"
        L"bgcolor = \"%s\";\n", AKINATOR_BGR_COLOR);
    akinatorPrintNodes(akin->tree.rootBranch, dotFile);
    log(L"}\n");

    fclose(dotFile);
    akinsatorCompiteDot(akin);

    akin->dumpIndex++;

    return AKINATOR_ERR_NO;
}