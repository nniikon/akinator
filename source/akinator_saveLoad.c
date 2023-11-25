#include "../include/akinator_saveLoad.h"
#include "../include/akinator_cfg.h"
#include "../binaryTree/tree_include/tree.h"
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include <ctype.h>

static void akinatorSave_recursive(TreeNode* node, FILE* file);


static TreeNode* akinatorLoad_recursive(Akinator* akin, Tree* tree,
                                            wchar_t** text, AkinatorError* err);

// FIXME: add better error handling.
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
    return -1;
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
    getFileSize(akin->databasePath, &size);

    wchar_t* buffer = calloc(size + 1, sizeof(wchar_t));
    if (buffer == NULL)
        return -1;
    akin->loadBuffer = buffer;

	FILE* dbFile = fopen(akin->databasePath, "r");
	if (dbFile == NULL)
        AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_OPEN_FILE);

    if (fgetws(buffer, (int) size, dbFile) == NULL)
    {
        fclose(dbFile);
        AKINATOR_DUMP_RETURN_ERROR(AKINATOR_ERR_BAD_FGETS);
    }
    fwprintf(stderr, buffer);

    AkinatorError err = AKINATOR_ERR_NO;
    akin->tree.rootBranch = akinatorLoad_recursive(akin, &akin->tree, &buffer, &err);
    fclose(dbFile);
    return AKINATOR_ERR_NO;
}


#define setBranch(dir)                                                        \
	if ((*text)[0] == L'(')                                                   \
	{                                                                         \
		node-> dir ## Branch = akinatorLoad_recursive(akin,                   \
                                                tree, text, err);             \
        if (node-> dir ## Branch == NULL)                                     \
            return NULL;                                                      \
	}                                                                         \
	else /* ADD CHECK FOR NIL */                                              \
	{                                                                         \
		*text = wcschr(*text, L')');                                          \
		*text += 2;                                                           \
		node->leftBranch = NULL;                                              \
		node->rightBranch = NULL;                                             \
		node->data->type = AKINATOR_NODE_OBJ;                                 \
		return node;                                                          \
	} (void) 0

// FIXME: cringe define.
static TreeNode* akinatorLoad_recursive(Akinator* akin, Tree* tree,
                                            wchar_t** text, AkinatorError* err)
{
    assert(text);
    assert(*text);
    fwprintf(stderr, L"AKINATOR_LOAD started\n");

    if (*err != AKINATOR_ERR_NO)
    {
        return NULL;
    }

	if ((*text)[0] != L'(')
    {
		*err = AKINATOR_ERR_SYNTAX;
        return NULL;
    }

	*text += 2;

	TreeNode* node = treeCreateEmptyNode(tree);
    node->data = nodeCalloc(akin);

	node->data->str = *text + 1;
	*text = wcschr(*text, L'>');
    if ((*text) == NULL)
    {
        *err = AKINATOR_ERR_SYNTAX;
        return NULL;
    }
    *text[0] = L'\0';
	*text += 2;

	setBranch(left);
    setBranch(right);

    if ((*text)[0] == L')')
    {
	    node->data->type = AKINATOR_NODE_QUESTION;
        *text += 2;
        return node;
    }
    else
    {
        *err = AKINATOR_ERR_SYNTAX;
        return NULL;
    }
}























