#include "../include/akinator_definition.h"

#include <assert.h>
#include "../include/akinator_stack.h"
#include "../include/akinator_promts.h"
#include "../include/akinator_cfg.h"


const wchar_t* NOT_PREFIX = L"не ";

static int akinatorPushDefinition_recursive(Stack* stk , const wchar_t* name,
                                            TreeNode* node);

static AkinatorError akinatorGetDefinitionStack(Akinator* akin, const wchar_t msg[],
                                                                Stack* stk);

#define OUTPUT_BUFFER_SIZE 165

AkinatorError akinatorCompareDefinitions(Akinator* akin)
{
    assert(akin);

    const int N_STK = 2;

    AkinatorError err = AKINATOR_ERR_NO;
    Stack stk[N_STK] = {};
	err = akinatorGetDefinitionStack(akin, L"Введите название первого объекта: ", &stk[0]);
    if (err != AKINATOR_ERR_NO)
    {
        AKINATOR_DUMP_RETURN_ERROR(err);
    }

	err = akinatorGetDefinitionStack(akin, L"Введите название второго объекта: ", &stk[1]);
    if (err != AKINATOR_ERR_NO)
    {
        stackDtor(&stk[0]);
        AKINATOR_DUMP_RETURN_ERROR(err);
    }

    int index[N_STK] = {};
    for (int i = 0; i < N_STK; i++)
    {
        index[i] = stk[i].size;
    }

    while(index[0] >= 0 && index[1] >= 0)
    {
        int hasNot[2] = {};

        for (int i = 0; i < N_STK; i++)
        {
            if (stk[i].data[index[i]] == NOT_PREFIX)
            {
                index[i]--;
                assert(index[i] >= 0);
                hasNot[i] = 1;
            }
        }

        if (stk[0].data[index[0]] == stk[1].data[index[1]] && 
                       hasNot[0]  ==            hasNot[1]     )
        {
            for (int i = 0; i < 2; i++)
                index[i]--;
            continue;
        }

        akinatorPrintAndSay(stk[0].data[0]);
        wprintf(L" - ");
        if (hasNot[0]) akinatorPrintAndSay(NOT_PREFIX);
        akinatorPrintAndSay(stk[0].data[index[0]]);
        wprintf(L", а ");
        akinatorPrintAndSay(stk[1].data[0]);
        wprintf(L" ");
        if (hasNot[1]) akinatorPrintAndSay(NOT_PREFIX);
        akinatorPrintAndSay(stk[1].data[index[1]]);
        putwchar(L'\n');
        break;
    }
    for (int i = 0; i < 2; i++)
    {
        if (stackDtor(&stk[i]) != NO_ERROR)
            return AKINATOR_ERR_STACK;
    }
    return AKINATOR_ERR_NO;
}


AkinatorError akinatorGetDefinition(Akinator* akin)
{
    assert(akin);

    AkinatorError err = AKINATOR_ERR_NO;

	Stack defStack = {};
	err = akinatorGetDefinitionStack(akin, L"Введите название вашего объекта: ", &defStack);
    if (err != AKINATOR_ERR_NO)
        AKINATOR_DUMP_RETURN_ERROR(err);

    const wchar_t* def = NULL;
    while (defStack.size > 1)
    {
        if (stackPop(&defStack, &def) != NO_ERROR)
        {
            stackDtor(&defStack);
            AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_STACK);
        }
        if (def == NOT_PREFIX)
        {
            akinatorSay(def);
            wprintf(L"%ls ", def);
            if (stackPop(&defStack, &def) != NO_ERROR)
            {
                stackDtor(&defStack);
                AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_STACK);
            }
            akinatorSay(def);
            wprintf(L"%ls, ", def);
        }
        else
        {
            akinatorSay(def);
            wprintf(L"%ls, ", def);
        }
    }
    if (stackPop(&defStack, &def) != NO_ERROR)
    {
        stackDtor(&defStack);
        AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_STACK);
    }
    wprintf(L"и просто %ls\n", def);
    return AKINATOR_ERR_NO;

    stackDtor(&defStack);
}


static int akinatorPushDefinition_recursive(Stack* stk , const wchar_t* name,
                                            TreeNode* node)
{
    if (node == NULL)
        return 0;

    if (node->data->type == AKINATOR_NODE_OBJ)
    {
        if (wcsncmp(node->data->str, name, AKINATOR_MAX_WORD_SIZE) == 0)
        {
            stackPush(stk, node->data->str);
            return 1;
        }
    }

    int isLeft  = akinatorPushDefinition_recursive(stk, name, node->leftBranch);
    if (isLeft)
    {
        stackPush(stk, node->data->str);
        return 1;
    }

    int isRight = akinatorPushDefinition_recursive(stk, name, node->rightBranch);
    if (isRight)
    {
        stackPush(stk, node->data->str);
        stackPush(stk, NOT_PREFIX);
        return 1;
    }

    return 0;
}


static AkinatorError akinatorGetDefinitionStack(Akinator* akin, const wchar_t msg[],
                                                                Stack* stk)
{
    assert(akin);
    assert(stk);

    akinatorPrintAndSay(msg);

	// While the user is thinking.
	StackError stkErr = stackInit(stk);
	if (stkErr != NO_ERROR)
		AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_STACK);
 
    int isCorrectInput = 0;
    while (!isCorrectInput)
    {
        wchar_t objName[AKINATOR_MAX_WORD_SIZE] = {};
        if (fgetws(objName, AKINATOR_MAX_WORD_SIZE, stdin) == NULL)
        {
            stackDtor(stk);
            AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_STACK);
        }
        *wcschr(objName, L'\n') = L'\0';

        isCorrectInput = akinatorPushDefinition_recursive(stk, objName, akin->tree.rootBranch);
        if (!isCorrectInput)
        {
            wprintf(L"Я не знаю ничего подобного! Попробуйте еще раз:\n");
            memset(stk->data, stk->size, sizeof(wchar_t*));
            stk->size = 0;
        }
    }
    return AKINATOR_ERR_NO;
}
