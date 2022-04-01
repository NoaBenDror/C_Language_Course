#include "Structs.h"
#include <string.h>
#include <stdlib.h>
#include "RBTree.h"

#define FAILURE 0
#define SUCCESS 1
#define EQUAL 0
#define NEG -1
#define POS 1
#define INIT 0
#define END_OF_LINE "\n"

/**
 * This function compares length of two strings and finds the minimal length
 * @param a - first string
 * @param b - second string
 * @return the minimal length of the two strings
 */
int getMinLenOf2Strings(const char *a, const char *b)
{
    int minLen = INIT;
    if (strlen(a) > strlen(b))
    {
        minLen = (int)strlen(b);
    }
    else
    {
        minLen = (int)strlen(a);
    }
    return minLen;
}

/**
 * CompFunc for strings (assumes strings end with "\0")
 * @param a - char* pointer
 * @param b - char* pointer
 * @return equal to 0 iff a == b. lower than 0 if a < b. Greater than 0 iff b < a. (lexicographic
 * order)
 */
int stringCompare(const void *a, const void *b)
{
    char *aCh = (char*) a;
    char *bCh = (char*) b;
    int minLen = getMinLenOf2Strings(aCh, bCh);
    int res = strncmp(aCh, bCh, minLen);
    if (res == EQUAL) // the strings are equal in the first minLen characters
    {
        if (strlen(aCh) == strlen(bCh))
        {
            return EQUAL;
        }
        else if (strlen(aCh) > strlen(bCh))
        {
            return POS;
        }
        else
        {
            return NEG;
        }
    }
    else
    {
        return res;
    }
}

/**
 * ForEach function that concatenates the given word and \n to pConcatenated. pConcatenated is
 * already allocated with enough space.
 * @param word - char* to add to pConcatenated
 * @param pConcatenated - char*
 * @return 0 on failure, other on success
 */
int concatenate(const void *word, void *pConcatenated)
{
    if (word == NULL || pConcatenated == NULL)
    {
        return FAILURE;
    }
    char *wordCh = (char*) word;
    char *pConCh = (char*) pConcatenated;
    strcat(pConCh, wordCh);
    strcat(pConCh, END_OF_LINE);
    return SUCCESS;
}

/**
 * FreeFunc for strings
 */
void freeString(void *s)
{
    free((char *)s);
    s = NULL;
}

/**
 * This function compares lengths of two vectors and finds the minimal length
 * @param a - Vector* pointer
 * @param b - Vector* pointer
 * @return the minimal length of the two vectors
 */
int getMinLenOf2Vectors(const Vector *a, const Vector *b)
{
    int minLen = INIT;
    if (a->len > b->len)
    {
        minLen = b->len;
    }
    else
    {
        minLen = a->len;
    }
    return minLen;
}

/**
 * CompFunc for Vectors, compares element by element, the vector that has the first larger
 * element is considered larger. If vectors are of different lengths and identify for the length
 * of the shorter vector, the shorter vector is considered smaller.
 * @param a - first vector
 * @param b - second vector
 * @return equal to 0 iff a == b. lower than 0 if a < b. Greater than 0 iff b < a.
 */
int vectorCompare1By1(const void *a, const void *b)
{
    Vector *aVec = (Vector *)a;
    Vector *bVec = (Vector *)b;
    int minLen = getMinLenOf2Vectors(aVec, bVec);

    for (int i = 0; i < minLen; i++)
    {
        if (aVec->vector[i] > bVec->vector[i])
        {
            return POS;
        }
        else if (aVec->vector[i] < bVec->vector[i])
        {
            return NEG;
        }
    }

    if (aVec->len == bVec->len)
    {
        return EQUAL;
    }
    else if (aVec->len > bVec->len)
    {
        return POS;
    }
    else
    {
        return NEG;
    }
}

/**
 * FreeFunc for vectors
 */
void freeVector(void *pVector)
{
    Vector *pVec = (Vector *)pVector;
    if (pVec != NULL)
    {
        free(pVec->vector);
        free(pVec);
        pVec = NULL;
    }
}

/**
 * This function calculates the square norm of a vector (it assumes Vector's vector not null)
 * @param pVector - the vector to calculate it's square norm
 * @return the square norm
 */
double calculateSquareVecNorm(const Vector *pVector)
{
    double sum = INIT;
    double cor = INIT;
    for (int i = 0; i < pVector->len; i++)
    {
        cor = pVector->vector[i];
        sum = sum + (cor*cor);
    }
    return sum;
}

/**
 * copy pVector to pMaxVector if : 1. The norm of pVector is greater then the norm of pMaxVector.
 * 								   2. pMaxVector->vector == NULL.
 * @param pVector pointer to Vector
 * @param pMaxVector pointer to Vector that will hold a copy of the data of pVector.
 * @return 1 on success, 0 on failure (if pVector == NULL || pMaxVector==NULL: failure).
 */
int copyIfNormIsLarger(const void *pVector, void *pMaxVector)
{
    if (pVector == NULL || pMaxVector == NULL)
    {
        return FAILURE;
    }

    Vector *pVec = (Vector *)pVector;
    Vector *pMaxVec = (Vector *)pMaxVector;
    if (pVec->vector == NULL)
    {
        return FAILURE;
    }

    if (pMaxVec->vector == NULL || calculateSquareVecNorm(pVec) > calculateSquareVecNorm(pMaxVec))
    {
        pMaxVec->len = pVec->len; // copy the len
        pMaxVec->vector = (double *)realloc(pMaxVec->vector, sizeof(double)*pVec->len);
        if (pMaxVec->vector == NULL) // vector couldn't be allocated
        {
            return FAILURE;
        }

        for (int i = 0; i < pMaxVec->len; i++) // copy the data
        {
            pMaxVec->vector[i] = pVec->vector[i];
        }
    }
    return SUCCESS;
}

/**
 * This function allocates memory it does not free.
 * @param tree - a pointer to a tree of Vectors
 * @return pointer to a *copy* of the vector that has the largest norm (L2 Norm), NULL on failure.
 */
Vector *findMaxNormVectorInTree(RBTree *tree)
{
    if (tree == NULL)
    {
        return NULL;
    }

    Vector *maxVec = (Vector *)malloc(sizeof(Vector)); // the caller is responsible for freeing this Vector
    if (maxVec == NULL) // vector couldn't be allocated
    {
        return NULL;
    }
    maxVec->vector = NULL;
    maxVec->len = INIT;
    if (forEachRBTree(tree, copyIfNormIsLarger, (void *)maxVec) == FAILURE)
    {
        freeVector(maxVec);
        maxVec = NULL;
        return NULL;
    }
    return maxVec;
}