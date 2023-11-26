#include "../include/akinator.h"
#include <assert.h>
#include <wchar.h>

#include "../include/akinator_tree_cfg.h"
#include "../include/akinator_cfg.h"
#include "../include/akinator_promts.h"


#define AKINATOR_DUMP_RETURN_TREE_ERROR(err)                                  \
    do                                                                        \
    {                                                                         \
        DUMP(akin->dumpFile, "%s\n", treeGetErrorMsg(err));                   \
        AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_TREE);                        \
    } while (0)


const char* akinatorGetErrorMsg(AkinatorError err)
{
    #define DEF_AKINATOR_ERR(err, msg)                                        \
        case AKINATOR_ERR_ ## err:                                            \
            return msg;                                                       \

    switch (err)
    {
        #include "../include/akinator_err_codegen.inc"
        default:
            return "No such error was found";
    }
    #undef DEF_AKINATOR_ERR
}


AkinatorNode* akintorNodeCalloc(Akinator* akin)
{
    return (AkinatorNode*) dynArrCalloc(akin->nodeBuffer, &akin->nodeCapacity,
                                   &akin->nodeFreeIndex, sizeof(AkinatorNode));
}


wchar_t* akinatorWordCalloc(Akinator* akin)
{
	return (wchar_t*) dynArrCalloc(akin->wordBuffer, &akin->nodeCapacity,
                                   &akin->nodeFreeIndex, sizeof(wchar_t) * AKINATOR_MAX_WORD_SIZE);
}


AkinatorError akinatorCtor(Akinator* akin, const char* database, FILE* dumpFile)
{
    DUMP_FUNC_START(akin->dumpFile);
    if (akin == NULL || database == NULL)
        AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_NULLPTR_PASSED);

    memset(akin, 0, sizeof(Akinator));

    TreeError treeErr = treeCtor(&akin->tree, dumpFile);
    if (treeErr != TREE_ERROR_NO)
        AKINATOR_DUMP_RETURN_TREE_ERROR(treeErr);

    akin->databasePath = database;
    akin->dumpFile     = dumpFile;

    AkinatorError akinErr = AKINATOR_ERR_NO;
    StackError    stkErr  = NO_ERROR;
    akin->nodeBuffer = (AkinatorNode*) dynArrCtor(&akin->nodeCapacity, 
                                    sizeof(AkinatorNode));
    if (akin->nodeBuffer == NULL)
    {
        akinErr = AKINATOR_ERR_BAD_MEM_ALLOC;
        goto NodeBufferFailure;
    }
    akin->wordBuffer = (wchar_t*) dynArrCtor(&akin->wordCapacity,
                                    sizeof(wchar_t) * AKINATOR_MAX_WORD_SIZE);
    if (akin->wordBuffer == NULL)
    {
        akinErr = AKINATOR_ERR_BAD_MEM_ALLOC;
        goto WordBufferFailure;
    }
    akin->tree.rootBranch->data = akintorNodeCalloc(akin);
    if (akin->tree.rootBranch->data == NULL)
    {
        akinErr = AKINATOR_ERR_BAD_MEM_ALLOC;
        goto NodeCallocFailure;
    }
    akin->tree.rootBranch->data->type = AKINATOR_NODE_OBJ;
    akin->tree.rootBranch->data->str  = AKINATOR_UNKNOWN_OBJ_NAME;

    DUMP_FUNC_SUCCESS(akin->dumpFile);
    return AKINATOR_ERR_NO;

    free(akin->tree.rootBranch->data);
    NodeCallocFailure:
    free(akin->wordBuffer);
    WordBufferFailure:
    free(akin->nodeBuffer);
    NodeBufferFailure:
    treeDtor(&akin->tree);
    AKINATOR_DUMP_RETURN_ERROR(akinErr);
}


AkinatorError akinatorDtor(Akinator* akin)
{
    DUMP_FUNC_START(akin->dumpFile);
    if (akin == NULL)
        AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_NULLPTR_PASSED);

    TreeError treeErr = treeDtor(&akin->tree);
    if (treeErr != TREE_ERROR_NO)
        AKINATOR_DUMP_RETURN_TREE_ERROR(treeErr); // May cause UB so return asap.

    akin->databasePath = NULL;
    akin->dumpFile     = NULL;

    free(akin->nodeBuffer);
    free(akin->wordBuffer);
    free(akin->loadBuffer);

    DUMP_FUNC_SUCCESS(akin->dumpFile);
    return AKINATOR_ERR_NO;
}


static int akinatorAskQuestion(Akinator* akin, TreeNode* node, 
                                const wchar_t question[], AkinatorError* err)
{
    akinatorPrintAndSay(question);
    akinatorPrintAndSay(node->data->str);
    wprintf(L"?\n");
    char input = -1;
    while (input == -1)
    {
        wprintf(L"[1] Да \n"
                L"[2] Нет\n");

        input = (char) akinatorGetOption(L"12", err);
        if (*err != AKINATOR_ERR_NO)
            AKINATOR_DUMP_RETURN_ERROR(*err);

        if (input == '1') return 1;
        if (input == '2') return 0;

        akinatorPrintAndSay(L"Неверный ввод");
        wprintf            (L", попробуйте еще раз.\n");
    }
    AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_UNEXPECTED);
}


static void akinatorLose()
{
    akinatorPrintAndSay(L"Ты проиграл! Ха-ха.\n");
}


// FIXME: split into two seperate function.
// function1: take the two wchar strings.
// function2: change the tree.
static AkinatorError akinatorVictory(Akinator* akin, TreeNode* node)
{
    DUMP_FUNC_START(akin->dumpFile);
	assert(node);
	if (node == NULL)
		AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_NULLPTR_PASSED);
	akinatorPrintAndSay(L"Тебе повезло. Ты меня обыграл\n"
						L"Как зовут твоего персонажа?  \n");

	wchar_t* newObj = akinatorWordCalloc(akin);
    if (newObj == NULL)
        AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_BAD_MEM_ALLOC);

    if (fgetws(newObj, (int)AKINATOR_MAX_WORD_SIZE, stdin) == NULL)
    {
        akin->wordFreeIndex--;
        AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_BAD_FGETS);
    }

    node->leftBranch  = treeCreateEmptyNode(&akin->tree);
    node->rightBranch = treeCreateEmptyNode(&akin->tree);

    node->leftBranch ->data = akintorNodeCalloc(akin);
    node->rightBranch->data = akintorNodeCalloc(akin);

    akinatorPrintAndSay(L"И чем он отличается от ");
    akinatorPrintAndSay(node->data->str);
    wprintf(L"?\n");

    wprintf(L"Ваш персонаж ... ");
    wchar_t* newQuestion = akinatorWordCalloc(akin);
    if (newQuestion == NULL)
    {
        akin->wordFreeIndex--;
        AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_BAD_MEM_ALLOC);
    }
    if (fgetws(newQuestion, (int)AKINATOR_MAX_WORD_SIZE, stdin) == NULL)
    {
        akin->wordFreeIndex -= 2;
        AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_BAD_FGETS);
    }

    newObj[AKINATOR_MAX_WORD_SIZE - 1] = L'\n';
    newQuestion[AKINATOR_MAX_WORD_SIZE - 1] = L'\n';

    assert(wcsrchr(newObj,      L'\n'));
    assert(wcsrchr(newQuestion, L'\n'));

    *wcsrchr(newObj,      L'\n') = L'\0';
    *wcsrchr(newQuestion, L'\n') = L'\0';

    node->leftBranch->data->str  = newObj;
    node->leftBranch->data->type = AKINATOR_NODE_OBJ;

    node->rightBranch->data->str  = node->data->str;
    node->rightBranch->data->type = AKINATOR_NODE_OBJ;

    node->data->str  = newQuestion;
    node->data->type = AKINATOR_NODE_QUESTION;

    return AKINATOR_ERR_NO;
}


TreeNode* akinatorQuestion(Akinator* akin, TreeNode* node, AkinatorError* err)
{
    if (*err != AKINATOR_ERR_NO)
        return NULL;
    if (node == NULL)
		return NULL;

	switch (node->data->type)
	{
		case AKINATOR_NODE_OBJ:
			if (akinatorAskQuestion(akin, node, L"Вы загадали ", err) == 0)
            {
                if (*err == AKINATOR_ERR_NO)
				    *err = akinatorVictory(akin, node);
                return NULL;
            }
			else
            {
                if (*err == AKINATOR_ERR_NO)
				    akinatorLose();
                return NULL;
            }
			break;
		case AKINATOR_NODE_QUESTION:
			if (akinatorAskQuestion(akin, node, L"Ваш персонаж ", err) == 0)
				return akinatorQuestion(akin, node->rightBranch, err);
			else
				return akinatorQuestion(akin, node->leftBranch, err);
		default:
			assert(0);
			return NULL;
	}
}






























