#include "../include/akinator_definition.h"

#include <assert.h>
#include "../include/akinator_cfg.h"

static int akinatorPushDefinition_recursive(Akinator* akin, const wchar_t* name,
                                      const wchar_t* notPrefix, TreeNode* node);

AkinatorError akinatorGetDefinition(Akinator* akin)
{
    assert(akin);

    wchar_t objName[AKINATOR_MAX_WORD_SIZE] = {};
    akinatorPrintAndSay(L"Введите название вашего объекта: ");

    if (fgetws(objName, AKINATOR_MAX_WORD_SIZE, stdin) == NULL)
        AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_BAD_FGETS);
    *wcschr(objName, L'\n') = L'\0';

    const wchar_t* notPrefix = L"не";

    if (akinatorPushDefinition_recursive(akin, objName, notPrefix, akin->tree.rootBranch))
    {
        const wchar_t* def = NULL;
        while (akin->defStack.size > 1)
        {
            if (stackPop(&akin->defStack, &def) != NO_ERROR)
                AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_STACK);
            if (def == notPrefix)
            {
                akinatorSay(def);
                wprintf(L"%ls ", def);
                if (stackPop(&akin->defStack, &def) != NO_ERROR)
                    AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_STACK);
                akinatorSay(def);
                wprintf(L"%ls, ", def);
            }
            else
            {
                akinatorSay(def);
                wprintf(L"%ls, ", def);
            }
        }
        if (stackPop(&akin->defStack, &def) != NO_ERROR)
            AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_STACK);

        wprintf(L"или просто %ls\n", def);
        return AKINATOR_ERR_NO;
    }
    else
    {
        akinatorPrintAndSay(L"Я не знаю ничего подобного!\n");
        return AKINATOR_ERR_NO;
    }
}


static int akinatorPushDefinition_recursive(Akinator* akin, const wchar_t* name,
                                      const wchar_t* notPrefix, TreeNode* node)
{
    if (node == NULL)
        return 0;

    if (node->data->type == AKINATOR_NODE_OBJ)
    {
        if (wcsncmp(node->data->str, name, AKINATOR_MAX_WORD_SIZE) == 0)
        {
            stackPush(&akin->defStack, node->data->str);
            return 1;
        }
    }

    int isLeft  = akinatorPushDefinition_recursive(akin, name, notPrefix, node-> leftBranch);
    if (isLeft)
    {
        stackPush(&akin->defStack, node->data->str);
        return 1;
    }

    int isRight = akinatorPushDefinition_recursive(akin, name, notPrefix, node->rightBranch);
    if (isRight)
    {
        stackPush(&akin->defStack, node->data->str);
        stackPush(&akin->defStack, notPrefix);
        return 1;
    }

    return 0;
}