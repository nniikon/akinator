#ifndef AKINATOR_TREE_CFG_H_
#define AKINATOR_TREE_CFG_H_

#define TREE_ELEM_FORMAT "%p"

typedef enum 
{
    AKINATOR_NODE_QUESTION,
    AKINATOR_NODE_OBJ,
} AkinatorNodeType;

typedef struct
{
    AkinatorNodeType type;
    const wchar_t* str;
} AkinatorNode;

typedef AkinatorNode* treeElem_t;

#endif // AKINATOR_TREE_CFG_H_