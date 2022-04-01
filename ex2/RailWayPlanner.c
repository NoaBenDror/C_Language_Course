/**
 * @file RailWayPlanner.c
 * @author Noa Ben Dror <noa.bendror@mail.huji.ac.il>
 * @date 06 May 2020
 *
 * @brief System to find the the minimal price of the railway which can be built from the given parts
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

#define RESIZE_NUM_OF_PARTS 50
#define NUM_OF_ALL_CHARS 256
#define NOT_USED -1
#define NUM_OF_EXPECTED_ARGS 2
#define BASE 10
#define MAX_CH_IN_ROW 1024
#define SUCCESSFUL 1
#define LAST_CH 1
#define UNSUCCESSFUL 0
#define DUMMY_LINE 0
#define INITIALIZE 0
#define POSITIVE 1
#define START_OF_FILE 0
#define LINE_OF_PART 4
#define LOWER_BOUND 0
#define VALID_CONN_LEN 1
#define ROW_NUM_1 0
#define FIRST_ROW 1
#define SECOND_ROW 2
#define THIRD_ROW 3
#define NO_SOLUTION -1
#define OUTPUT_FILE "railway_planner_output.txt"
#define WRITE "w"
#define READ "r"
#define ERR_NUM_ARGS_INVALID "Usage: RailwayPlanner <InputFile>"
#define ERR_DOESNT_EXIST "File doesn't exists."
#define ERR_EMPTY_FILE "File is empty."
#define ERR_INVALID_INPUT "Invalid input in line: %d."
#define DELIMITER ','
#define SSCANF_FORMAT "%1024[^,],%1024[^,],%1024[^,],%1024[^,]"
#define MINIMAL_PRICE_MSG "The minimal price is: %d"
#define END_OF_LINE '\n'
#define END_OF_STR '\0'

/**
 * This struct represents a part, which is used to build the rail
 */
typedef struct Part
{
    char start; // left connection
    char end; // right connection
    int pLen; // length
    int price;
} Part;


/**
 * This function fills the cell with the minimal price for railway in length "row",
 * which ends with right connection "col"
 * @param row - the row we are filling
 * @param col - the column we are filling
 * @param table - the table we are filling
 * @param parts - an array of the parts which can be used to build the railway
 * @param numOfParts - the number of the parts which we can use to build the railway
 * @param indexOfConnectionArray - an array which represents which chars(connections) are being used,
 * and their index in the table
 */
void fillCell(const int row, const int col, int ** table, const Part* const parts, const int numOfParts,
              const int indexOfConnectionArray[])
{
    int minPrice = INT_MAX;
    int indexOfStart = 0;
    for (int i = 0; i < numOfParts; i++) // iterate all the parts
    {
        if (indexOfConnectionArray[(int)(parts[i].end)] == col) // found potential matching part (by edges)
        {
            indexOfStart = indexOfConnectionArray[(int)(parts[i].start)]; // index (column) of the left connection
            if (row - parts[i].pLen >= 0 &&
                table[row-parts[i].pLen][indexOfStart] != INT_MAX &&
                table[row-parts[i].pLen][indexOfStart] + parts[i].price >= 0 && // making sure there isn't int overflow
                table[row-parts[i].pLen][indexOfStart] + parts[i].price < minPrice)
            {
                minPrice = table[row-parts[i].pLen][indexOfStart] + parts[i].price;
            }
        }
    }
    table[row][col] = minPrice; // fill the cell
}

/**
 * This function fills the row with the minimal price for railway in length "row"
 * @param numOfCols - the number of
 * @param row - the row we are filling
 * @param table - the table we are filling
 * @param parts - an array of the parts which can be used to build the railway
 * @param numOfParts - the number of the parts which we can use to build the railway
 * @param indexOfConnectionArray - an array which represents which chars(connections) are being used,
 * and their index in the table
 */
void fillRow(const long numOfCols, const int row, int ** table, const Part* const parts, const int numOfParts,
             const int indexOfConnectionArray[])
{
    for (int col = 0; col < numOfCols; col++) // iterate every cell in this row
    {
        fillCell(row, col, table, parts, numOfParts, indexOfConnectionArray);
    }
}

/**
 * This function handles errors - opens an output file to print an informative message to
 * @param message - the message that should be printed to file
 * @param lineNum - the number of the line in which there was an error in the file, 0 if the error isn't in the
 * lines of file
 */
void handleError(const char message[], const int lineNum)
{
    FILE* outputFile = fopen(OUTPUT_FILE, WRITE);
    if (outputFile == NULL) // there was a problem opening the output file
    {
        return;
    }
    if (lineNum == DUMMY_LINE) // means the error was not in the lines of the file
    {
        fprintf(outputFile, "%s", message);
    }
    else // means the error was in one of the lines
    {
        fprintf(outputFile, message, lineNum);
    }
    fclose(outputFile); // in this case, no need to check if fclose worked, because we will EXIT_FAILURE anyway
}

/**
 * This function checks if a string is an integer number
 * @param str - the string
 * @return 1 if the string is an integer, 0 otherwise
 */
int checkIfInteger(char str[])
{
    int lenOfStr = (int)strlen(str);
    if (str[lenOfStr - 1] == END_OF_LINE)
    {
        str[lenOfStr - 1] = END_OF_STR;
        lenOfStr--;
    }
    for (int i = 0; i < lenOfStr ; i++)
    {
        if(!isdigit(str[i])) // check if one of the characters isn't a digit
        {
            return UNSUCCESSFUL;
        }
    }
    return SUCCESSFUL;
}

/**
 * This function checks if the given connection is in the list of connections
 * @param indexOfConnectionArray- an array which represents which chars (connections) are being used,
 * and their index in the table
 * @param str - the string which represents a connection
 * @return 1 if the connection is valid, 0 otherwise
 */
int checkConnValidity(const int indexOfConnectionArray[], const char str[])
{
    if (strlen(str) != VALID_CONN_LEN)
    {
        return UNSUCCESSFUL;
    }
    if (indexOfConnectionArray[(int)(str[0])] == NOT_USED)
    {
        return UNSUCCESSFUL; // the given connection of the part is not in the list of connections
    }
    return SUCCESSFUL;
}

/**
 * This function checks if a string is a positive integer
 * @param str - the string to check
 * @return 1 if the string is a positive integer, 0 otherwise
 */
int checkPositiveInteger(char str[])
{
    const int isInteger = checkIfInteger(str);
    if (!isInteger)
    {
        return UNSUCCESSFUL;
    }
    const long num = strtol(str, NULL, BASE);
    if (num < POSITIVE)
    {
        return UNSUCCESSFUL;
    }
    return SUCCESSFUL;
}

/**
 * This function checks if a string is a non-negative integer
 * @param str - the string to check
 * @return 1 if the string is a non-negative integer, 0 otherwise
 */
int checkNonNegativeInteger(char str[])
{
    const int isInteger = checkIfInteger(str);
    if (!isInteger)
    {
        return UNSUCCESSFUL;
    }

    const long num = strtol(str, NULL, BASE);
    if (num < LOWER_BOUND)
    {
        return UNSUCCESSFUL;
    }
    return SUCCESSFUL;
}

/**
 * This function checks if a part is valid part
 * @param indexOfConnectionArray - an array which represents which chars(connections) are being used,
 * and their index in the table
 * @param startStr - the string of the start connection of the part
 * @param endStr - the string of the end connection of the part
 * @param pLenStr - the string of the length of the part
 * @param priceStr - the string of the price of the part
 * @return 1 if the part is valid, 0 otherwise
 */
int checkPartValidity(const int indexOfConnectionArray[], const char startStr[], const char endStr[], char pLenStr[],
                      char priceStr[])
{
    if (!checkConnValidity(indexOfConnectionArray, startStr) ||
        !checkConnValidity(indexOfConnectionArray, endStr) ||
        !checkPositiveInteger(pLenStr) ||
        !checkPositiveInteger(priceStr))
    {
        return UNSUCCESSFUL;
    }
    return SUCCESSFUL;
}

/**
 * This function checks validity of the line of the connections, and if found valid -
 * updates the indexOfConnectionArray
 * @param connectionsStr - the string of the connections
 * @param lenOfConnections - the length of the line of connections
 * @param indexOfConnectionArray - an array which represents which chars(connections) are being used,
 * and their index in the table
 * @return 1 if the line of connections is valid, 0 otherwise
 */
int lineOfConnProcessAndValidate(const char connectionsStr[], const int lenOfConnections, int indexOfConnectionArray[])
{
    int connectionsCounter = 0;
    for (int i = 0; i < lenOfConnections - 1; i = i + 2)
    {
        if (connectionsStr[i + 1] != DELIMITER) // more than one char (which is not a comma) one after the other
        {
            return UNSUCCESSFUL;
        }
        if (indexOfConnectionArray[(int)(connectionsStr[i])] == NOT_USED) // in case we get more than one of the same
            // char
        {
            indexOfConnectionArray[(int)(connectionsStr[i])] = connectionsCounter;
            connectionsCounter++;
        }
    }
    //put the last char in the array
    indexOfConnectionArray[(int)(connectionsStr[lenOfConnections - 1])] = connectionsCounter;
    return SUCCESSFUL;
}


/**
 * This function is responsible for handling the input - check file's validity, checks lines' validity,
 * and updates the information about the length of the rail, the connections, and the parts.
 * @param arg - the argument given by the user
 * @param pParts - pointer to array of parts
 * @param pPartCounter - pointer to a counter of the parts
 * @param pNumOfConnections - pointer to the number of connections
 * @param pLenOfRail - pointer to the length of the rail
 * @param indexOfConnectionArray - an array which represents which chars(connections) are being used,
 * and their index in the table
 */
void getInput(const char arg[], Part** pParts, int* pPartCounter, long* pNumOfConnections, long* pLenOfRail,
              int indexOfConnectionArray[])
{
    int lenOfConnections = 0; // the length of line 3 (the connections)
    int capacity = 0; // the size of the allocation
    char lenOfRailStr[MAX_CH_IN_ROW];
    char numOfConnectionsStr[MAX_CH_IN_ROW];
    char connectionsStr[MAX_CH_IN_ROW];
    char partStr[MAX_CH_IN_ROW];
    char startStr[MAX_CH_IN_ROW];
    char endStr[MAX_CH_IN_ROW];
    char pLenStr[MAX_CH_IN_ROW];
    char priceStr[MAX_CH_IN_ROW];

    FILE* inputFile  = fopen(arg, READ);
    // check if there was a problem opening the input file
    if (inputFile == NULL)
    {
        handleError(ERR_DOESNT_EXIST, DUMMY_LINE);
        exit(EXIT_FAILURE);
    }

    // check if file is empty
    fseek(inputFile, START_OF_FILE, SEEK_END); // go to end of file
    if (ftell(inputFile) == START_OF_FILE)
    {
        handleError(ERR_EMPTY_FILE, DUMMY_LINE);
        fclose(inputFile); // in this case, no need to check if fclose not worked, because we will EXIT_FAILURE anyway
        exit(EXIT_FAILURE);
    }
    fseek(inputFile, START_OF_FILE, SEEK_SET); // go back to start of file

    // if we got here - it means we have a file with at least 4 rows. go over the rows in the file
    // check and read first row -  len of rail
    fgets(lenOfRailStr, MAX_CH_IN_ROW, inputFile);
    if (!checkNonNegativeInteger(lenOfRailStr))
    {
        handleError(ERR_INVALID_INPUT, FIRST_ROW);
        fclose(inputFile); // in this case, no need to check if fclose not worked, because we will EXIT_FAILURE anyway
        exit(EXIT_FAILURE);
    }
    *pLenOfRail = strtol(lenOfRailStr, NULL , BASE);

    // check and read the second row - number of connections
    fgets(numOfConnectionsStr, MAX_CH_IN_ROW, inputFile);
    if (!checkPositiveInteger(numOfConnectionsStr))
    {
        handleError(ERR_INVALID_INPUT, SECOND_ROW);
        fclose(inputFile); // in this case, no need to check if fclose not worked, because we will EXIT_FAILURE anyway
        exit(EXIT_FAILURE);
    }
    *pNumOfConnections = strtol(numOfConnectionsStr, NULL , BASE);

    // check and read the third row - the connections
    fgets(connectionsStr, MAX_CH_IN_ROW, inputFile);
    lenOfConnections = (int)strlen(connectionsStr);
    if (!lineOfConnProcessAndValidate(connectionsStr, lenOfConnections - LAST_CH,
        indexOfConnectionArray))
    {
        handleError(ERR_INVALID_INPUT, THIRD_ROW);
        fclose(inputFile); // in this case, no need to check if fclose not worked, because we will EXIT_FAILURE anyway
        exit(EXIT_FAILURE);
    }

    // check and read the parts
    while (fgets(partStr, MAX_CH_IN_ROW, inputFile) != NULL)
    {
        sscanf(partStr, SSCANF_FORMAT, startStr, endStr, pLenStr, priceStr);
        int isValid = checkPartValidity(indexOfConnectionArray, startStr, endStr, pLenStr, priceStr);
        if (!isValid)
        {
            handleError(ERR_INVALID_INPUT, (*pPartCounter) + LINE_OF_PART);
            free(*pParts);
            *pParts = NULL;
            fclose(inputFile); // in this case, no need to check if fclose not worked, we will EXIT_FAILURE anyway
            exit(EXIT_FAILURE);
        }

        // check if realloc is needed for more parts
        if (*pPartCounter % RESIZE_NUM_OF_PARTS == 0)
        {
            capacity += RESIZE_NUM_OF_PARTS;
            *pParts = (Part *)realloc(*pParts, sizeof(Part) * capacity);
            if (*pParts == NULL) // check if the allocation worked
            {
                fclose(inputFile); // in this case, no need to check if fclose not worked, we will EXIT_FAILURE anyway
                exit(EXIT_FAILURE);
            }
        }

        long pLen = strtol(pLenStr, NULL, BASE);
        long price = strtol(priceStr, NULL, BASE);
        (*pParts)[*pPartCounter].start = startStr[0];
        (*pParts)[*pPartCounter].end = endStr[0];
        (*pParts)[*pPartCounter].pLen = (int)pLen;
        (*pParts)[*pPartCounter].price = (int)price;
        (*pPartCounter)++;
    }

    if(fclose(inputFile) == EOF) // done working on input file - close it
    {
        exit(EXIT_FAILURE);
    }
}

/**
 * This function is responsible for calculating and returning the minimal price of the railway that can be built
 * from the given parts
 * @param lenOfRail - The length of the railway
 * @param numOfConnections - The number of connections
 * @param partCounter - The number of the parts
 * @param parts - the parts that buld the railway
 * @param indexOfConnectionArray - an array which represents which chars(connections) are being used,
 * and their index in the table
 * @return The minimal price of the railway
 */
int calculateMinPrice(const long lenOfRail, const long numOfConnections, const int partCounter,
                      const Part* const parts, const int indexOfConnectionArray[])
{
    int minPriceForLenL = INT_MAX;

    // build the table -
    int ** table  = (int**)malloc((lenOfRail + 1) * sizeof(int*));
    if (table == NULL) // couldn't allocate memory (according to instructions - in this case no need to free memory)
    {
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < lenOfRail + 1; i++)
    {
        table[i] = (int*)malloc(numOfConnections * sizeof(int));
        if (table[i] == NULL) // couldn't allocate memory (according to instructions - in this case no need
            // to free memory)
        {
            exit(EXIT_FAILURE);
        }
    }

    // set the first row of table to be zeros
    for(int i = 0; i < numOfConnections ; i++)
    {
        table[ROW_NUM_1][i] = INITIALIZE;
    }

    // fill the table to find the minimal price, row by row
    for (int row = 1; row <= lenOfRail; row++)
    {
        fillRow(numOfConnections, row, table, parts, partCounter, indexOfConnectionArray);
    }

    // traverse "lenOfRail"-th row, to find minimum price
    for (int i = 0; i < numOfConnections; i++)
    {
        if (table[lenOfRail][i] < minPriceForLenL)
        {
            minPriceForLenL = table[lenOfRail][i];
        }
    }

    if (minPriceForLenL == INT_MAX)
    {
        minPriceForLenL = NO_SOLUTION;
    }

    // free memory of table
    for (int i = 0; i < lenOfRail + 1; i++)
    {
        free(table[i]);
        table[i] = NULL;
    }
    free(table);
    table = NULL;
    return minPriceForLenL;
}

/**
 * This function initializes the array of the chars' indexing to be -1 (represents "not used")
 * @param indexOfConnectionArray
 */
void initializeArrayOfChars(int indexOfConnectionArray[])
{
    for (int i = 0; i < NUM_OF_ALL_CHARS; i++) // initialize indices as -1 ("not used")
    {
        indexOfConnectionArray[i] = NOT_USED;
    }
}

/**
 * This function prints the minimal price to an output file
 * @param minPrice
 */
void handleOutputFile(const int minPrice)
{
    FILE* outputFile = fopen(OUTPUT_FILE, WRITE);
    if (outputFile == NULL) // there was a problem opening the output file
    {
        exit(EXIT_FAILURE);
    }

    fprintf(outputFile, MINIMAL_PRICE_MSG, minPrice);
    if (fclose(outputFile) == EOF) // there was a problem closing the output file
    {
        exit(EXIT_FAILURE);
    }
}

/**
 * The main function - runs the program
 * @param argc - the number of parameters
 * @param argv - the parameters
 * @return 0 if the program has been successfully executed, 1 if not
 */
int main(int argc, char *argv[])
{
    int minPrice = 0;
    long lenOfRail = 0;
    long numOfConnections = 0;
    int partCounter = 0;
    Part *parts = NULL;
    int indexOfConnectionArray[NUM_OF_ALL_CHARS]; // an array which represents which chars(connections) are being used,
    // and their index in the table
    initializeArrayOfChars(indexOfConnectionArray);

    // check if num of given arguments is 1
    if (argc != NUM_OF_EXPECTED_ARGS)
    {
        handleError(ERR_NUM_ARGS_INVALID, DUMMY_LINE);
        exit(EXIT_FAILURE);
    }

    getInput(argv[1], &parts, &partCounter, &numOfConnections, &lenOfRail, indexOfConnectionArray);
    // if we got here it means the input was completely valid - calculate the minimal price
    minPrice = calculateMinPrice(lenOfRail, numOfConnections, partCounter, parts, indexOfConnectionArray);
    free(parts);
    parts = NULL;
    handleOutputFile(minPrice);

    return EXIT_SUCCESS;
}