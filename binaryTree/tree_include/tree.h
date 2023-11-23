#ifndef TREE_H_
#define TREE_H_

#include <stdlib.h>
#include <string.h>
#include "../../include/akinator_tree_cfg.h"
#include "../../common/include/dump.h"

typedef struct TreeNode
{
    treeElem_t data;
    struct TreeNode* leftBranch;
    struct TreeNode* rightBranch;
    struct TreeNode* parentBranch;
} TreeNode;

typedef struct
{
	int isDebug;
	FILE* file;
	unsigned int dumpIndex;
} TreeDebugInfo;

typedef struct
{
    TreeNode* rootBranch;
    unsigned int size;
    unsigned int capacity;
    unsigned int freeIndex;
    TreeNode*    memBuffer;
	TreeDebugInfo debugInfo;
} Tree;

typedef enum
{
    #define DEF_TREE_ERR(err, msg) TREE_ERROR_ ## err,
    #include "tree_err_codegen.inc"
    #undef DEF_TREE_ERR
} TreeError;

TreeError treeCtor(Tree* tree, FILE* file);

TreeError treeDtor(Tree* tree);

TreeError treeInsertLeft (Tree* tree, TreeNode* node, treeElem_t data);
TreeError treeInsertRight(Tree* tree, TreeNode* node, treeElem_t data);

void treePrintPostorder(TreeNode* node, FILE* file);
void treePrintPreorder (TreeNode* node, FILE* file);
void treePrintInorder  (TreeNode* node, FILE* file);

TreeError treeVerify(Tree* tree);

const char* treeGetErrorMsg(TreeError err);

TreeNode* treeCreateNode(Tree* tree, TreeNode* left, TreeNode* right,
                                     TreeNode* parent, treeElem_t data);
TreeNode* treeCreateEmptyNode(Tree* tree);

#endif // TREE_H_
