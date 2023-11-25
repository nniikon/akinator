#include "../include/akinator_saveLoad.h"
#include "../include/akinator_cfg.h"
#include "../binaryTree/tree_include/tree.h"
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <ctype.h>
#include <wctype.h>

static void akinatorSave_recursive(TreeNode* node, FILE* file);


static TreeNode* akinatorLoad_recursive(Akinator* akin, Tree* tree,
                                            wchar_t** text, AkinatorError* err);

AkinatorError akinatorSaveToFile(Akinator* akin)
{
	assert(akin);
	assert(akin->databasePath);
	if (akin == NULL || akin->databasePath == NULL)
		AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_NULLPTR_PASSED);

	FILE* dbFile = fopen(akin->databasePath, "w");
	if (dbFile == NULL)
		AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_OPEN_FILE);

	akinatorSave_recursive(akin->tree.rootBranch, dbFile);

	fclose(dbFile);

	return AKINATOR_ERR_NO;
}


// FIXME: add error pls...... (ssize_t* maybe). 95% sure it returns in bytes
static int getFileSize(const char* fileName, size_t* size)
{
    struct stat bf = {};
    int error = stat(fileName, &bf);
    if (error == -1)
    {
        return -1;
    }

    *size = (size_t) bf.st_size;
    return 0;
}


// TODO: maybe add recursion depth to avoid infinite loops?
static void akinatorSave_recursive(TreeNode* node, FILE* file)
{
	if (node == NULL)
	{
		fwprintf(file, L"%ls ", AKINATOR_FILE_NIL_NAME);
		return;
	}
	fwprintf(file, L"( ");

	fwprintf(file, L"<%ls> ", node->data->str);
	akinatorSave_recursive(node-> leftBranch, file);
	akinatorSave_recursive(node->rightBranch, file);
	
	fwprintf(file, L") ");
}

AkinatorError akinatorLoad(Akinator* akin)
{
    // its temporary
    size_t size = 0;
    if (getFileSize(akin->databasePath, &size) == -1)
        AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_STAT);

    wchar_t* buffer = (wchar_t*) calloc(size + 1, sizeof(wchar_t));
    if (buffer == NULL)
        AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_BAD_MEM_ALLOC);
    akin->loadBuffer = buffer;

	FILE* dbFile = fopen(akin->databasePath, "r");
	if (dbFile == NULL)
    {
        free(buffer);
        AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_OPEN_FILE);
    }

    if (fgetws(buffer, (int) size, dbFile) == NULL)
    {
        free(buffer);
        fclose(dbFile);
        AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_BAD_FGETS);
    }
    fwprintf(stderr, buffer);

    AkinatorError err = AKINATOR_ERR_NO;
    akin->tree.rootBranch = akinatorLoad_recursive(akin, &akin->tree, &buffer, &err);
    fclose(dbFile);
    return AKINATOR_ERR_NO;
}


static wchar_t* skipWhiteSpaces(wchar_t* input)
{
    assert(input);

    size_t i = 0;
    for (i = 0; input[i] != L'\0'; i++)
    {
        if (!iswspace(input[i]))
            return (input + i);
    }
    return input + i;
}

static size_t getWordSize(const wchar_t* input, const wchar_t* delims)
{
    assert(input);

    return wcscspn(input, delims);
}


#define setBranch(dir)                                                        \
	if ((*text)[0] == L'(')                                                   \
	{                                                                         \
		node-> dir ## Branch = akinatorLoad_recursive(akin,                   \
                                                tree, text, err);             \
        if (node-> dir ## Branch == NULL)                                     \
            return NULL;                                                      \
	}                                                                         \
	else                                                                      \
	{                                                                         \
		size_t wordSize = getWordSize(*text, L" ");                           \
        if (wcsncmp(*text, AKINATOR_FILE_NIL_NAME, wordSize) == 0)            \
        {                                                                     \
            node-> dir ## Branch = NULL;                                      \
            node->data->type = AKINATOR_NODE_OBJ;                             \
            *text += wordSize;                                                \
            *text = skipWhiteSpaces(*text);                                   \
        }                                                                     \
        else                                                                  \
        {                                                                     \
            return NULL;                                                      \
            *err = AKINATOR_ERR_SYNTAX;                                       \
        }                                                                     \
	} (void) 0                                                                \


// FIXME: not that much cringe define, i'd say it's fine
static TreeNode* akinatorLoad_recursive(Akinator* akin, Tree* tree,
                                            wchar_t** text, AkinatorError* err)
{
    assert(text);
    assert(*text);

    if (*err != AKINATOR_ERR_NO)
    {
        return NULL;
    }

	if ((*text)[0] != L'(')
    {
		*err = AKINATOR_ERR_SYNTAX;
        return NULL;
    }

	(*text)++;
    *text = skipWhiteSpaces(*text);

	TreeNode* node = treeCreateEmptyNode(tree);
    node->data = akintorNodeCalloc(akin);

	node->data->str = *text + 1;
	*text = wcschr(*text, L'>');
    if ((*text) == NULL)
    {
        *err = AKINATOR_ERR_SYNTAX;
        return NULL;
    }
    *text[0] = L'\0';

	(*text)++;
    *text = skipWhiteSpaces(*text);
	setBranch(left);
    setBranch(right);

    if (node->leftBranch == NULL && node->rightBranch == NULL)
    {
        *text = skipWhiteSpaces(*text + 1); 
        return node;
    }

    if ((*text)[0] == L')')
    {
	    node->data->type = AKINATOR_NODE_QUESTION;
        (*text)++;
        *text = skipWhiteSpaces(*text);
        return node;
    }
    else
    {
        *err = AKINATOR_ERR_SYNTAX;
        return NULL;
    }
}























