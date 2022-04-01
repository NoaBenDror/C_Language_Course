#include "RBTree.h"
#include <stdlib.h>

#define EMPTY_TREE 0
#define SUCCESS 1
#define FAILURE 0
#define NO_PARENT 0
#define RIGHT_CHILD 1
#define LEFT_CHILD -1
#define RIGHT_UNCLE 1
#define LEFT_UNCLE -1
#define YES 1
#define NO 0
#define INIT 0
#define EQUAL 0
#define ONLY_NODE 1

void delete3(RBTree *tree, Node *node);
int gFuncSucceeded = YES; // flag for exiting forEach process if the function fails

/**
 * constructs a new RBTree
 * @param compFunc - a function to compare two variables.
 * @param freeFunc - a function to free the tree's data
 * @return the new RBTree, NULL if couldn't be allocated
 */
RBTree* newRBTree(CompareFunc compFunc, FreeFunc freeFunc)
{
    RBTree *newRBTree = (RBTree *)malloc(sizeof(RBTree)); // the caller is responsible for freeing
    if (newRBTree == NULL) // newRBTree couldn't be allocated
    {
        return NULL;
    }
    newRBTree->root = NULL;
    newRBTree->compFunc = compFunc;
    newRBTree->freeFunc = freeFunc;
    newRBTree->size = EMPTY_TREE; // new tree's size is 0
    return newRBTree;
}

/**
 * constructs a new Node
 * @param data - the data of the node
 * @return the new node, NULL if couldn't be allocated
 */
Node* createNewNode(void* const data)
{
    Node *newNode = (Node*)malloc(sizeof(Node)); // the caller is responsible for freeing this node
    if (newNode == NULL) // newNode couldn't be allocated
    {
        return NULL;
    }
    newNode->right = NULL;
    newNode->left = NULL;
    newNode->parent = NULL;
    newNode->data = data;
    newNode->color = RED; // new node is red
    return newNode;
}

/**
 * This function inserts the given data (as a Node) to the tree in it's correct place
 * @param root - a pointer to a Node which represents the root of the tree
 * @param data - the data to be inserted
 * @param compFunc - a function that compares tree items
 * @return a pointer to the new inserted Node
 */
Node* basicInsert(Node* const root, void* const data, const CompareFunc compFunc)
{
    Node * node = root;
    Node *parent = NULL;
    while (node != NULL)
    {
        parent = node;
        if (compFunc(data, node->data) > 0)
        {
            node = node->right;
        }
        else if (compFunc(data, node->data) < 0)
        {
            node = node->left;
        }
        else // the item is already in the tree - failure
        {
            return NULL;
        }
    }

    Node * newNode = createNewNode(data); // memory for node is allocated here, will be freed when the node is
    // deleted from the tree
    if (parent != NULL) // the tree was not empty before we inserted the node
    {
        if (compFunc(data, parent->data) > 0)
        {
            parent->right = newNode; // set the newNode as the right child of its parent
        }
        else
        {
            parent->left = newNode; // set the newNode as the left child of its parent
        }
        newNode->parent = parent; // set parent as the newNode's parent
    }
    return newNode;
}

/**
 * This function checks if a (non NULL) node is a right child or a left child in a tree
 * @param node - a pointer to Node
 * @return 1 if node is a right child, -1 if node is a left child, 0 if node has no parent
 */
int whichChildNodeIs(const Node * const node)
{
    if (node->parent == NULL)
    {
        return NO_PARENT;
    }

    if (node->parent->right == node)
    {
        return RIGHT_CHILD;
    }
    else
    {
        return LEFT_CHILD;
    }
}

/**
 * This function rotates the sub tree of node to the left
 * @param tree - a pointer to an RBTree which we are making a rotation in
 * @param node - a pointer to a node in the tree which we are making a rotation on its sub tree
 */
void leftRotation(RBTree * const tree, Node * const node)
{
    Node *node2 = node->right; // we are assuming node->right isn't null
    Node *node3 = node->parent; // might be NULL if node is the root
    node->right = node2->left;
    if (node2->left != NULL)
    {
        node2->left->parent = node;
    }
    node2->parent = node3;
    if (whichChildNodeIs(node) == NO_PARENT) // node is a root
    {
        tree->root = node2;
    }
    else
    {
        if (whichChildNodeIs(node) == RIGHT_CHILD && node3 != NULL)
        {
            node3->right = node2;
        }
        else if (whichChildNodeIs(node) == LEFT_CHILD && node3 != NULL)
        {
            node3->left = node2;
        }
    }
    node2->left = node;
    node->parent = node2;
}

/**
 * This function rotates the sub tree of node to the right
 * @param tree - a pointer to an RBTree which we are making a rotation in
 * @param node - a pointer to a node in the tree which we are making a rotation on its sub tree
 */
void rightRotation(RBTree * const tree, Node * const node)
{
    Node *node2 = node->left; // we are assuming node->left isn't null
    Node *node3 = node->parent; // might be NULL if node is the root
    node->left = node2->right;
    if (node2->right != NULL)
    {
        node2->right->parent = node;
    }
    node2->parent = node3;
    if (whichChildNodeIs(node) == NO_PARENT) // node is a root
    {
        tree->root = node2;
    }
    else
    {
        if (whichChildNodeIs(node) == RIGHT_CHILD && node3 != NULL) // node is a right child
        {
            node3->right = node2;
        }
        else if (whichChildNodeIs(node) == LEFT_CHILD && node3 != NULL) // node is a left child
        {
            node3->left = node2;
        }
    }
    node2->right = node;
    node->parent = node2;
}

/**
 * This function fixes the tree according to red uncle, red parent situation
 * @param tree - the tree we are working on
 * @param currentNode - a pointer to a pointer to the node we are working on
 * @param uncle - the uncle of the currentNode
 */
void redUncleAfterInsertion(Node ** currentNode,  Node * const uncle)
{
    uncle->color = BLACK;
    (*currentNode)->parent->color = BLACK;
    (*currentNode)->parent->parent->color = RED;
    *currentNode = (*currentNode)->parent->parent;
}

/**
 * This function fixes the tree according to black uncle, red parent situation
 * @param tree - the tree we are working on
 * @param currentNode - a pointer to a pointer to the node we are working on
 * @param whichUncle - represents which side the currentNode's uncle is
 */
void blackUncleAfterInsertion(RBTree * tree, Node ** currentNode, const int whichUncle)
{
    if (whichUncle == RIGHT_UNCLE)
    {
        if (whichChildNodeIs(*currentNode) == RIGHT_CHILD) // currentNode is right child, of left child
        {
            leftRotation(tree, (*currentNode)->parent);
            *currentNode = (*currentNode)->left;
        }
        (*currentNode)->parent->color = BLACK;
        (*currentNode)->parent->parent->color = RED;
        rightRotation(tree, (*currentNode)->parent->parent);
    }

    if (whichUncle == LEFT_UNCLE)
    {
        if (whichChildNodeIs(*currentNode) == LEFT_CHILD) // currentNode is left child, of right child
        {
            rightRotation(tree, (*currentNode)->parent);
            *currentNode = (*currentNode)->right;
        }
        (*currentNode)->parent->color = BLACK;
        (*currentNode)->parent->parent->color = RED;
        leftRotation(tree, (*currentNode)->parent->parent);
    }
}

/**
 * This function fixes the tree after insertion of a new node, according to red black tree rules
 * @param tree - a pointer to the tree to be fixed
 * @param node - a pointer to the node we inserted
 */
void fixTreeAfterInsertion(RBTree * const tree, Node * const node)
{
    Node *currentNode = node;
    Node *uncle = NULL;
    int whichUncle = INIT;
    while (currentNode != tree->root && currentNode->parent->color == RED)
    {
        whichUncle = INIT;
        //(currentNode has a grandparent, even while fixing, because its parent is RED and the root is always BLACK)
        if (whichChildNodeIs(currentNode->parent) == LEFT_CHILD) // currentNode's parent is a left child
        {
            uncle = currentNode->parent->parent->right;
            whichUncle = RIGHT_UNCLE;
        }
        else // currentNode's parent is a right child
        {
            uncle = currentNode->parent->parent->left;
            whichUncle = LEFT_UNCLE;
        }

        if (uncle != NULL && uncle->color == RED) // parent is red, uncle is red
        {
            redUncleAfterInsertion(&currentNode, uncle);
        }
        else // parent is red, uncle is black
        {
            blackUncleAfterInsertion(tree, &currentNode, whichUncle);
        }
    }
    tree->root->color = BLACK;
}

/**
 * add an item to the tree
 * @param tree: the tree to add an item to.
 * @param data: item to add to the tree.
 * @return: 0 on failure, other on success. (if the item is already in the tree - failure).
 */
int insertToRBTree(RBTree *tree, void *data)
{
    if (tree == NULL || data == NULL)
    {
        return FAILURE;
    }
    Node *newNode = basicInsert(tree->root, data, tree->compFunc);
    if (newNode == NULL) // the item is already in the tree or node couldn't be allocated
    {
        return FAILURE;
    }

    tree->size++; // insertion succeeded
    if (newNode->parent == NULL) // means the tree was empty, the node we inserted is now the root
    {
        tree->root = newNode;
        tree->root->color = BLACK; // change the root to black
        return SUCCESS;
    }
    if (newNode->parent->color == RED) // the newNode's parent is red - fix the tree
    {
        fixTreeAfterInsertion(tree, newNode);
    }
    return SUCCESS;
}

/**
 * This function finds a node in the tree, according to given data
 * @param node - the root of the tree (or sub-tree)
 * @param compFunc - function to compare two items in the tree
 * @param data - we are looking for a node which contains this data
 * @return a pointer to the node which contains the given data, NULL if not found
 */
Node* findNodeInTree(Node * const node, const CompareFunc compFunc, const void *data)
{
    if (node == NULL)
    {
        return NULL;
    }
    if (compFunc(data, node->data) == EQUAL) // we found the item in the tree
    {
        return node;
    }
    else if (compFunc(data, node->data) > 0)
    {
        return findNodeInTree(node->right, compFunc, data);
    }
    else
    {
        return findNodeInTree(node->left, compFunc, data);
    }
}

/**
 * This function finds the successor of the given node (assuming it has a right child)
 * @param node - the node to find the successor to
 * @return the successor
 */
Node* getSuccFromRightChild(Node * const node)
{
    Node * successor = node->right;
    while(successor->left != NULL)
    {
        successor = successor->left;
    }
    return successor;
}

/**
 * This function finds and returns the sibling of a given node
 * @param node - the node to find the sibling to
 * @param whichChild - an integer that represents which child is the given node - we will fill this in this function
 * @return the sibling
 */
Node* getSibling(Node * const node, int * const whichChild)
{
    Node *sibling = NULL;
    if (whichChildNodeIs(node) == LEFT_CHILD)
    {
        *whichChild = LEFT_CHILD;
        sibling = node->parent->right;
    }
    else
    {
        *whichChild = RIGHT_CHILD;
        sibling = node->parent->left;
    }
    return sibling;
}

/**
 *  * If we've reached this function it means node is black, it's children are NULL, it's sibling is black,
 *  and it's sibling's children are black
 * @param tree - the tree we are working on
 * @param sibling - the sibling of the node we are going to delete
 * @param node - the node we are going to delete
 */
void delete6(RBTree * const tree, Node * const sibling, Node * const node)
{
    if (node->parent->color == RED) // case 3Bi
    {
        node->parent->color = BLACK;
        sibling->color = RED;
    }
    else // case 3Bii
    {
        sibling->color = RED;
        delete3(tree, node->parent);
    }
}

/**
 * If we've reached this function it means node is black, its children are NULL, it's sibling is red
 * @param tree - the tree we are working on
 * @param sibling - the sibling of the node we are going to delete
 * @param node - the node we are going to delete
 * @param whichChild - an integer that represents which child is the node we are going to delete
 */
void delete5(RBTree *const tree, Node * const sibling, Node * const node, const int whichChild)
{
    sibling->color = BLACK;
    node->parent->color = RED;
    if (whichChild == LEFT_CHILD)
    {
        leftRotation(tree, node->parent);
    }
    else
    {
        rightRotation(tree, node->parent);
    }
    delete3(tree, node);
}

/**
 * If we've reached this function it means node is black, it's children are NULL, and it's sibling is black
 * @param tree - the tree we are working on
 * @param sibling - the sibling of the node we are going to delete
 * @param node - the node we are going to delete
 * @param whichChild - an integer that represents which child is the node we are going to delete
 */
void delete4(RBTree * const tree, Node * sibling, Node * const node, const int whichChild)
{
    if ((sibling->left == NULL || sibling->left->color == BLACK) &&
        (sibling->right == NULL || sibling->right->color == BLACK)) // case 3B
    {
        delete6(tree, sibling, node);
    }

    else if ((whichChild == LEFT_CHILD) &&
             (sibling->right == NULL || sibling->right->color == BLACK) &&
             (sibling->left != NULL && sibling->left->color == RED)) // case 3D LEFT
    {
        sibling->left->color = BLACK;
        sibling->color = RED;
        rightRotation(tree, sibling);
        sibling = sibling->parent;
    }

    else if ((whichChild == RIGHT_CHILD) &&
             (sibling->left == NULL || sibling->left->color == BLACK) &&
             (sibling->right != NULL && sibling->right->color == RED)) // case 3D RIGHT
    {
        sibling->right->color = BLACK;
        sibling->color = RED;
        leftRotation(tree, sibling);
        sibling = sibling->parent;
    }

    if ((whichChild == LEFT_CHILD) && (sibling->right != NULL) && (sibling->right->color == RED)) // case 3E LEFT
    {
        sibling->color = node->parent->color;
        node->parent->color = BLACK;
        sibling->right->color = BLACK;
        leftRotation(tree, node->parent);
    }

    else if ((whichChild == RIGHT_CHILD) && (sibling->left != NULL) && (sibling->left->color == RED)) // case 3E RIGHT
    {
        sibling->color = node->parent->color;
        node->parent->color = BLACK;
        sibling->left->color = BLACK;
        rightRotation(tree, node->parent);
    }
}

/**
 * If we've reached this function it means node is black, and it's children are NULL
 * @param tree - the tree we are working on
 * @param node - the node we are going to delete
 */
void delete3(RBTree * const tree, Node * const node)
{
    if (node->parent == NULL) // case 3A - the node is the root
    {
        return;
    }

    int whichChild = INIT;
    Node *sibling = getSibling(node, &whichChild);
    if (sibling->color == BLACK) // sibling is black - cases 3B, 3D, 3E
    {
        delete4(tree, sibling, node, whichChild);
    }
    else // sibling is red - case 3C
    {
        delete5(tree, sibling, node, whichChild);
    }
}

/**
 * This function swaps data between two given nodes
 * @param node1 - first node
 * @param node2 - second node
 */
void swapData(Node * const node1, Node * const node2)
{
    void *temp = node1->data;
    node1->data = node2->data;
    node2->data = temp;
}

/**
 * This function changes the given node to it's child
 * @param tree - the tree we are working on
 * @param node - the node we are about to change to it's child
 * @param child - the node which is about to replace it's parent
 */
void changeNode(RBTree * const tree, Node * const node, Node * const child)
{
    child->parent = node->parent;
    if (whichChildNodeIs(node) == LEFT_CHILD)
    {
        node->parent->left = child;
    }
    else if (whichChildNodeIs(node) == RIGHT_CHILD)
    {
        node->parent->right = child;
    }
    else // child is root
    {
        tree->root = child;
    }
}

/**
 * This function checks if we need to change the tree - and changes it if we do
 * @param tree - the tree we are working on
 * @param node - the node we are going to delete
 */
void delete2(RBTree * const tree, Node * const node)
{
    int whichChild = INIT;
    if (node->color == BLACK)
    {
        Node *child = (node->right == NULL) ? node->left : node->right; // node has at most 1 child
        if (child == NULL)
        {
            delete3(tree, node); // both children are NULL - case 3
        }
        else // child's color is red for sure - case 2 - change the node to it's child
        {
            changeNode(tree, node, child);
            child->color = BLACK;
            return;
        }
    }
    // disconnect parent of node from node
    whichChild = whichChildNodeIs(node);
    if (whichChild == RIGHT_CHILD)
    {
        node->parent->right = NULL;
    }
    else if (whichChild == LEFT_CHILD)
    {
        node->parent->left = NULL;
    }
}

/**
 * remove an item from the tree
 * @param tree: the tree to remove an item from.
 * @param data: item to remove from the tree.
 * @return: 0 on failure, other on success. (if data is not in the tree - failure).
 */
int deleteFromRBTree(RBTree *tree, void *data)
{
    if (tree == NULL)
    {
        return FAILURE;
    }

    Node *node = findNodeInTree(tree->root, tree->compFunc, data);
    if (node == NULL)
    {
        return FAILURE; // the data is not in the tree
    }

    if (tree->size == ONLY_NODE) // the node we are deleting is the only node in the tree
    {
        tree->freeFunc(tree->root->data);
        free(tree->root);
        tree->root = NULL;
        tree->size--;
        return SUCCESS;
    }

    if (node->left != NULL && node->right != NULL) // node has 2 children - swap node's val and successor's val and
        // and change node to successor
    {
        Node *successor = getSuccFromRightChild(node);
        swapData(node, successor);
        node = successor; // now node will have at most 1 child
    }

    delete2(tree, node); // check if we need to change the tree - and change if we do
    tree->freeFunc(node->data);
    free(node);
    node = NULL;
    tree->size--;
    return SUCCESS;
}

/**
 * check whether the tree RBTreeContains this item.
 * @param tree: the tree to check an item in.
 * @param data: item to check.
 * @return: 0 if the item is not in the tree, other if it is.
 */
int RBTreeContains(const RBTree *tree, const void *data)
{
    if (tree == NULL)
    {
        return FAILURE;
    }
    if (findNodeInTree(tree->root, tree->compFunc, data) == NULL)
    {
        return FAILURE;
    }
    return SUCCESS;
}

/**
 * This function activates a function on each item of the tree, recursively, in an ascending order. if one of the
 * activations of the function returns 0, the process stops.
 * @param node - the node to activate the function on
 * @param func - the function which will be activated
 * @param args - more optional arguments to the function (may be null if the given function support it).
 * @param funcSucceeded - an integer which represents if one of the activations of the function returned 0
 * @return 0 on failure, other on success.
 */
void forEachHelper(const Node *node, const forEachFunc func, void **args)
{
    if (node == NULL)
    {
        return;
    }

    if (gFuncSucceeded)
    {
        forEachHelper(node->left, func, args);
    }

    if (gFuncSucceeded && func(node->data, *args) == FAILURE)
    {
        gFuncSucceeded = NO;
        return;
    }

    if (gFuncSucceeded)
    {
        forEachHelper(node->right, func, args);
    }
}

/**
 * Activate a function on each item of the tree. the order is an ascending order. if one of the activations of the
 * function returns 0, the process stops.
 * @param tree: the tree with all the items.
 * @param func: the function to activate on all items.
 * @param args: more optional arguments to the function (may be null if the given function support it).
 * @return: 0 on failure, other on success.
 */
int forEachRBTree(const RBTree *tree, forEachFunc func, void *args)
{
    if (tree == NULL)
    {
        return FAILURE;
    }
    forEachHelper(tree->root, func, &args);
    return gFuncSucceeded;
}

/**
 * This function frees the memory of the tree
 * @param node - the node to free
 * @param freeFunc - the function that frees the data in the node
 */
void freeHelper(Node *node, FreeFunc freeFunc)
{
    if (node == NULL)
    {
        return;
    }
    freeHelper(node->left, freeFunc);
    freeHelper(node->right, freeFunc);
    freeFunc(node->data); // run the freeFunc of this tree, with the node's data as input
    free(node);
    node = NULL;
}

/**
 * free all memory of the data structure.
 * @param tree: pointer to the tree to free.
 */
void freeRBTree(RBTree **tree)
{
    freeHelper((*tree)->root, (*tree)->freeFunc);
    free(*tree);
    *tree = NULL;
}