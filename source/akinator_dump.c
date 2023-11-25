#include "../include/akinator_dump.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "../include/akinator_dump_cfg.h"

#define BUFFER_SIZE 20

void akinatorMakeLogdir()
{
    char cmdBuffer[BUFFER_SIZE] = {};
    snprintf(cmdBuffer, BUFFER_SIZE, "mkdir %s -p", AKINATOR_LOGS_FOLDER_NAME);

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


void akinatorGenPng(Akinator* akin)
{
    akinatorMakeLogdir();
    if (akin == NULL)
        return;
    if (akin->dumpFile == NULL)
        return;

    char fileBuffer[BUFFER_SIZE] = {};
    snprintf(fileBuffer, BUFFER_SIZE, AKINATOR_DOT_FILE_MASK, akin->dumpIndex);

    FILE* dotFile = fopen(fileBuffer, "w");
    if (dotFile == NULL)
    {
        DUMP(akin->dumpFile, "Unable to open %s", fileBuffer);
        fclose(dotFile);
        return;
    }
    log(L"digraph G{\n"
        L"rankdir = TB;\n"
        L"bgcolor = \"%s\";\n", AKINATOR_BGR_COLOR);
    akinatorPrintNodes(akin->tree.rootBranch, dotFile);
    log(L"}\n");

    fclose(dotFile);
}