/**
 * @file manageStudents.c
 * @author Noa Ben Dror <noa.bendror@mail.huji.ac.il>
 * @date 22 April 2020
 *
 * @brief System to manage the students of the university
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define SUCCESSFUL 0
#define UNSUCCESSFUL 1
#define MAX_FIELD_LEN 43
#define LEN_OF_BEST 4
#define LEN_OF_MERGE 5
#define LEN_OF_QUICK 5
#define NUM_OF_EXPECTED_ARGS 2
#define NUM_OF_EXPECTED_FIELDS 6
#define MAX_NUM_STUDENTS 5500
#define HALF_SIZE_OF_ARR 2750
#define MAX_ROW_LEN 61
#define MAX_ERR_STR_LEN 100
#define INVALID 0
#define VALID 1
#define ID_LEN 10
#define LOWEST_GRADE 0
#define HIGHEST_GRADE 100
#define YOUNGEST_AGE 18
#define OLDEST_AGE 120
#define BASE 10
#define LEN_OF_QUIT 1
#define FIRST 1
#define SECOND 2
#define NO_STUDENTS 0
#define TRUE 1
#define BEST_STR "best"
#define MERGE_STR "merge"
#define QUICK_STR "quick"
#define USAGE_MSG "USAGE: please type 'best', 'merge', or 'quick'\n"
#define GEN_MSG "Enter student info. To exit press q, then enter\n"
#define ERROR_NUM_OF_ARGS_MSG "ERROR: wrong number of fields\n"
#define ERROR_ID_MSG "ERROR: ID must contain 10 digits, first may not be zero\n"
#define ERROR_NAME_MSG "ERROR: name can only contain alphabetic characters or '-' or ' '\n"
#define ERROR_GRADE_MSG "ERROR: grade can only contain integer numbers in range 0-100\n"
#define ERROR_AGE_MSG "ERROR: age can only contain integer numbers in range 18-120\n"
#define ERROR_COUNTRY_MSG "ERROR: country can only contain alphabetic characters or '-'\n"
#define ERROR_CITY_MSG "ERROR: city can only contain alphabetic characters or '-'\n"
#define QUIT "q"
#define BEST_INFO "best student info is: %s,%s,%ld,%ld,%s,%s"
#define IN_LINE_N_MSG "in line %d\n"
#define ERROR_READ_INFO "ERROR: could not read info\n"
#define FORMAT_OF_FIELDS_PRINT "%s,%s,%ld,%ld,%s,%s"
#define SSCANF_FORMAT "%43[^,],%43[^,],%43[^,],%43[^,],%43[^,],%43[^,]"

/**
 * This struct represents a student, who has id, name, grade, age, country and city.
 */
typedef struct Student
{
    char id[MAX_FIELD_LEN];
    char name[MAX_FIELD_LEN];
    long grade;
    long age;
    char country[MAX_FIELD_LEN];
    char city[MAX_FIELD_LEN];
} Student;


/**
 * This function checks if number of given student fields is valid
 * @param numOfGivenFields - the number of student fields given by the user
 * @param outputValidityMsg - the message that should be printed in case of invalidity (given "empty")
 * @return 1 if number of args is valid, 0 otherwise
 */
int checkNumOfFieldsValidity(int numOfGivenFields, char outputValidityMsg[])
{
    if (numOfGivenFields != NUM_OF_EXPECTED_FIELDS)
    {
        strcpy(outputValidityMsg, ERROR_NUM_OF_ARGS_MSG);
        return INVALID;
    }
    return VALID;
}


/**
 * This function checks if the given ID is valid
 * @param idStr - the string of ID
 * @param outputValidityMsg - the message that should be printed in case of invalidity (given "empty")
 * @return 1 if ID is valid, 0 otherwise
 */
int checkIDValidity(char idStr[], char outputValidityMsg[])
{
    int lenOfIdStr = (int)strlen(idStr);
    if (lenOfIdStr != ID_LEN) // check if length of ID not 10
    {
        strcpy(outputValidityMsg, ERROR_ID_MSG);
        return INVALID;
    }

    for (int i = 0; i < lenOfIdStr; i++)
    {
        if (!isdigit(idStr[i])) // check if one of the characters isn't a digit
        {
            strcpy(outputValidityMsg, ERROR_ID_MSG);
            return INVALID;
        }
    }

    if (idStr[0] == '0') // check if first digit is zero
    {
        strcpy(outputValidityMsg, ERROR_ID_MSG);
        return INVALID;
    }

    return VALID;
}


/**
 * This function checks if the given name is valid
 * @param nameStr - the string of the name
 * @param outputValidityMsg - the message that should be printed in case of invalidity (given "empty")
 * @return 1 if name is valid, 0 otherwise
 */
int checkNameValidity(char nameStr[], char outputValidityMsg[])
{
    int nameLen = (int)strlen(nameStr);
    for (int i = 0; i < nameLen; i++)
    {
        if (!((nameStr[i] >= 'a' && nameStr[i] <= 'z') || (nameStr[i] >= 'A' && nameStr[i] <= 'Z') ||
            nameStr[i] == '-' || nameStr[i] == ' '))
        {
            strcpy(outputValidityMsg, ERROR_NAME_MSG);
            return INVALID;
        }
    }

    return VALID;
}


/**
 * This function checks if given grade is valid
 * @param gradeStr - the string of the grade
 * @param outputValidityMsg - the message that should be printed in case of invalidity (given "empty")
 * @return 1 if grade is valid, 0 otherwise
 */
int checkGradeValidity(char gradeStr[], char outputValidityMsg[])
{
    int gradeLen = (int)strlen(gradeStr);

    for (int i = 0; i < gradeLen; i++)
    {
        if(!isdigit(gradeStr[i])) // check if one of the characters isn't a digit
        {
            strcpy(outputValidityMsg, ERROR_GRADE_MSG);
            return INVALID;
        }
    }

    long gradeLong = strtol(gradeStr, NULL , BASE);
    if (gradeLong < LOWEST_GRADE || gradeLong > HIGHEST_GRADE) // check if grade is in the valid range
    {
        strcpy(outputValidityMsg, ERROR_GRADE_MSG);
        return INVALID;
    }

    return VALID;
}


/**
 * This function checks if given age is valid
 * @param ageStr - the string of the age
 * @param outputValidityMsg - the message that should be printed in case of invalidity (given "empty")
 * @return 1 if age is valid, 0 otherwise
 */
int checkAgeValidity(char ageStr[], char outputValidityMsg[])
{
    int ageLen = (int)strlen(ageStr);

    for (int i = 0; i < ageLen; i++)
    {
        if(!isdigit(ageStr[i])) // check if one of the characters isn't a digit
        {
            strcpy(outputValidityMsg, ERROR_AGE_MSG);
            return INVALID;
        }
    }

    long ageLong = strtol(ageStr, NULL , BASE);
    if (ageLong < YOUNGEST_AGE || ageLong > OLDEST_AGE) // check if age is in the valid range
    {
        strcpy(outputValidityMsg, ERROR_AGE_MSG);
        return INVALID;
    }

    return VALID;
}


/**
 * This function checks if given country is valid
 * @param countryStr -the string of the country
 * @param outputValidityMsg - the message that should be printed in case of invalidity (given "empty")
 * @return 1 if the country is valid, 0 otherwise
 */
int checkCountryValidity(char countryStr[], char outputValidityMsg[])
{
    int countryLen = (int)strlen(countryStr);

    for (int i = 0; i < countryLen; i++)
    {
        if (!((countryStr[i] >= 'a' && countryStr[i] <= 'z') || (countryStr[i] >= 'A' && countryStr[i] <= 'Z') ||
            countryStr[i] == '-'))
        {
            strcpy(outputValidityMsg, ERROR_COUNTRY_MSG);
            return INVALID;
        }
    }

    return VALID;
}


/**
 * This function checks if given city is valid
 * @param cityStr - the string of the city
 * @param outputValidityMsg - the message that should be printed in case of invalidity (given "empty")
 * @return 1 if city is valid, 0 otherwise
 */
int checkCityValidity(char cityStr[], char outputValidityMsg[])
{
    int cityLen = (int)strlen(cityStr);

    for (int i = 0; i < cityLen - 1; i++)
    {
        if (!((cityStr[i] >= 'a' && cityStr[i] <= 'z') || (cityStr[i] >= 'A' && cityStr[i] <= 'Z') ||
            cityStr[i] == '-'))
        {
            strcpy(outputValidityMsg, ERROR_CITY_MSG);
            return INVALID;
        }
    }

    if(cityStr[0] == '\n') // no input given for city
    {
        strcpy(outputValidityMsg, ERROR_CITY_MSG);
        return INVALID;
    }
    return VALID;
}


/**
 * This function checks if the fields of a given student are valid
 * @param numOfGivenFields
 * @param idStr - the string of the ID
 * @param nameStr - the string of name
 * @param gradeStr - the string of grade
 * @param ageStr - the string of age
 * @param countryStr - the string of country
 * @param cityStr - the string of city
 * @param outputValidityMsg - the message that should be printed in case of invalidity (given "empty")
 * @return 1 if the input fields are valid, 0 otherwise
 */
int checkValidity(int numOfGivenFields, char idStr[], char nameStr[], char gradeStr[],
                  char ageStr[], char countryStr[], char cityStr[], char outputValidityMsg[])
{
    if (!checkNumOfFieldsValidity(numOfGivenFields, outputValidityMsg) ||
        !checkIDValidity(idStr, outputValidityMsg) ||
        !checkNameValidity(nameStr, outputValidityMsg) ||
        !checkGradeValidity(gradeStr, outputValidityMsg) ||
        !checkAgeValidity(ageStr, outputValidityMsg) ||
        !checkCountryValidity(countryStr, outputValidityMsg) ||
        !checkCityValidity(cityStr, outputValidityMsg))
    {
        return INVALID;
    }

    return VALID; // if we've reached here - the input fields are valid
}


/**
 * This function puts the information of each student the user typed in an array of students
 * @param studentsArray - an array of students
 * @return the number of students whose information was given
 */
int buildStudentsArray(Student studentsArray[])
{
    char idStr[MAX_FIELD_LEN];
    char nameStr[MAX_FIELD_LEN];
    char gradeStr[MAX_FIELD_LEN];
    char ageStr[MAX_FIELD_LEN];
    char countryStr[MAX_FIELD_LEN];
    char cityStr[MAX_FIELD_LEN];
    int isValid = 0;
    int i = 0;
    int numOfInputLines = -1; // used for counting the input lines typed by the user
    char line[MAX_ROW_LEN]; // we will read the user's input into "line"
    char outputValidityMsg[MAX_ERR_STR_LEN]; // the message that should be printed in case of invalidity

    while (TRUE) // the user did not press q yet
    {
        printf(GEN_MSG);
        numOfInputLines++; // the user typed another legal/illegal input
        if (fgets(line, MAX_ROW_LEN, stdin) == NULL) // reads the input to "line". if couldn't read, exit the program
        {
            printf(ERROR_READ_INFO);
            printf(IN_LINE_N_MSG, numOfInputLines);
            exit(UNSUCCESSFUL);
        }

        if ((strlen(line) == (LEN_OF_QUIT + 1)) && (!strncmp(QUIT, line, LEN_OF_QUIT)))
        {
            break; // the user pressed q
        }

        int numOfGivenFields = sscanf(line, SSCANF_FORMAT, idStr, nameStr, gradeStr,
                                     ageStr, countryStr, cityStr); // divides the line into fields of student
        isValid = checkValidity(numOfGivenFields, idStr, nameStr, gradeStr, ageStr, countryStr,
                               cityStr, outputValidityMsg);

        if (!isValid)
        {
            printf("%s", outputValidityMsg);
            printf(IN_LINE_N_MSG, numOfInputLines);
        }

        else // the input was valid - put the  student in the array of students
        {
            strcpy(studentsArray[i].id, idStr);
            strcpy(studentsArray[i].name, nameStr);
            studentsArray[i].grade = strtol(gradeStr, NULL, BASE);
            studentsArray[i].age = strtol(ageStr, NULL, BASE);
            strcpy(studentsArray[i].country, countryStr);
            strcpy(studentsArray[i].city, cityStr);
            i++;
        }
    }
    return i; // number of students
}
/**
 * This function checks which student has the highest grade/age value
 * @param studentsArray The array of students
 * @param numOfStudents The number of actual students inserted to the array
 * @return The index in the students array of the student with the highest grade/age value
 */
int findBestStudent(Student studentsArray[], int numOfStudents)
{
    int bestStudentIndex = 0;
    double bestValue = 0.0; // highest grade/age value
    double currValue = 0.0;

    for (int j = 0; j < numOfStudents; j++)
    {
        currValue = ((double)studentsArray[j].grade) / studentsArray[j].age;
        if (currValue > bestValue)
        {
            bestStudentIndex = j;
            bestValue = currValue;
        }
    }

    return bestStudentIndex;
}

/**
 * This function merges between two parts of students array
 * @param studentsArray - array of students
 * @param left - left index
 * @param middle - middle index
 * @param right - right index
 */
void merge(Student studentsArray[], int left, int middle, int right)
{
    int i, j, k;
    int n1 = middle - left + 1;
    int n2 =  right - middle;
    Student L[HALF_SIZE_OF_ARR], R[HALF_SIZE_OF_ARR]; // temp arrays - left part- array L, right part - array R

    // copy students to temp arrays - L and R
    for (i = 0; i < n1; i++)
    {
        L[i] = studentsArray[left + i];
    }
    for (j = 0; j < n2; j++)
    {
        R[j] = studentsArray[middle + 1 + j];
    }

    // merge the temp arrays
    i = 0; // initial index of array L
    j = 0; // initial index of array R
    k = left;
    while (i < n1 && j < n2)
    {
        if (L[i].grade <= R[j].grade)
        {
            studentsArray[k] = L[i];
            i++;
        }
        else
        {
            studentsArray[k] = R[j];
            j++;
        }
        k++;
    }

    // copy the remaining elements of L
    while (i < n1)
    {
        studentsArray[k] = L[i];
        i++;
        k++;
    }

    // copy the remaining elements of R
    while (j < n2)
    {
        studentsArray[k] = R[j];
        j++;
        k++;
    }
}


/**
 * This function sorts the array of students according to their grades
 * @param studentsArray - array of students to be sorted by grade
 * @param left - left index of array
 * @param right - right index of array
 */
void mergeSort(Student studentsArray[], int left, int right)
{
    if (left < right)
    {
        int middle = left + (right - left) / 2;
        mergeSort(studentsArray, left, middle);
        mergeSort(studentsArray, middle + 1, right);
        merge(studentsArray, left, middle, right);
    }
}


/**
 * This function compares between two names, and checks which one is "higher" according to alphabetic order
 * @param name1 - first name to compare
 * @param name2 - second name to compare
 * @return 1 if the first name is "higher", 2 if the second name is "higher"
 */
int compareNames(char name1[], char name2[])
{
    int name1Len = (int)strlen(name1);
    for (int i = 0; i < name1Len; i++)
    {
        if(name1[i] > name2[i])
        {
            return FIRST;
        }
        if (name1[i] < name2[i])
        {
            return SECOND;
        }
    }

    return SECOND;
}


/**
 * This function sorts the array of students according to alphabetic order of name
 * @param studentsArray - the array of students
 * @param left - left index of array
 * @param right - right index of array
 */
void quickSort(Student studentsArray[], int left, int right)
{
    int i, j, pivot;
    Student temp;
    if (left < right) // there is still sorting to be done
    {
        pivot = left;
        i = left;
        j = right;

        while(i < j)
        {
            while((compareNames(studentsArray[i].name, studentsArray[pivot].name) == SECOND)  &&  i < right)
            {
                i++;
            }

            while(compareNames(studentsArray[j].name, studentsArray[pivot].name) == FIRST)
            {
                j--;
            }

            if(i < j)
            {
                temp = studentsArray[i];
                studentsArray[i] = studentsArray[j];
                studentsArray[j] = temp;
            }
        }

        temp = studentsArray[pivot];
        studentsArray[pivot] = studentsArray[j];
        studentsArray[j] = temp;
        quickSort(studentsArray, left, j - 1);
        quickSort(studentsArray, j + 1, right);
    }
}

/**
 * This function prints the array of students
 * @param numOfStudents - the number of students in the array
 * @param studentsArray - the array of students
 */
void printStudents(int numOfStudents, Student studentsArray[])
{
    for (int i = 0; i < numOfStudents; i++)
    {
        printf(FORMAT_OF_FIELDS_PRINT, studentsArray[i].id,
               studentsArray[i].name, studentsArray[i].grade,
               studentsArray[i].age, studentsArray[i].country,
               studentsArray[i].city);
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
    if (argc != NUM_OF_EXPECTED_ARGS) // check if num of given arguments is 1
    {
        printf(USAGE_MSG);
        return UNSUCCESSFUL;
    }

    Student studentsArray[MAX_NUM_STUDENTS];
    int numOfStudents = 0;
    int bestStudentIndex = 0;

    if ((strlen(argv[1]) == LEN_OF_BEST) && (!(strcmp(argv[1], BEST_STR)))) // the user typed "best"
    {
        numOfStudents = buildStudentsArray(studentsArray); // build an array of input students
        if (numOfStudents == NO_STUDENTS) // check if there are no students from input
        {
            return SUCCESSFUL;
        }

        bestStudentIndex = findBestStudent(studentsArray, numOfStudents); // get index of best student
        printf(BEST_INFO, studentsArray[bestStudentIndex].id,
               studentsArray[bestStudentIndex].name, studentsArray[bestStudentIndex].grade,
               studentsArray[bestStudentIndex].age, studentsArray[bestStudentIndex].country,
               studentsArray[bestStudentIndex].city);

        return SUCCESSFUL;
    }

    else if ((strlen(argv[1]) == LEN_OF_MERGE) && (!(strcmp(argv[1], MERGE_STR)))) // the user typed "merge"
    {
        numOfStudents = buildStudentsArray(studentsArray); // build an array of input students
        if (numOfStudents == NO_STUDENTS) // check if there are no students from input
        {
            return SUCCESSFUL;
        }

        mergeSort(studentsArray, 0, numOfStudents - 1); // sort the students according to grades
        printStudents(numOfStudents, studentsArray);
    }
    else if ((strlen(argv[1]) == LEN_OF_QUICK) && (!(strcmp(argv[1], QUICK_STR)))) // the user typed "quick"
    {
        numOfStudents = buildStudentsArray(studentsArray); // build an array of input students
        if (numOfStudents == NO_STUDENTS) // check if there are no students from input
        {
            return SUCCESSFUL;
        }

        quickSort(studentsArray, 0, numOfStudents - 1);
        printStudents(numOfStudents, studentsArray);
    }

    else // not "best", "merge" or "quick" - usage
    {
        printf(USAGE_MSG);
        return UNSUCCESSFUL;
    }
}