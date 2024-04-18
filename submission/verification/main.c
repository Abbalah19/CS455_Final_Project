/*
 * File: main_verification_version.c
 * Authors: MH, MQ
 * Description: This file contains the implementation of a program that replaces a word in a file with another word.
 * Each function in this file has been annotated with the author(s) who implemented it.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/**
 * @brief This function converts a string to lowercase.
 * @param str The string to convert.
 * @side_effects Modifies the input string in-place.
 * @author MQ
 */
void toLowerCase(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        if (str[i] >= 'A' && str[i] <= 'Z')
        {
            str[i] = str[i] + 'a' - 'A';
        }
    }
}

/**
 * @brief This function checks if a file exists.
 * @param filename The name of the file to check.
 * @return 1 if the file exists, 0 otherwise.
 * @side_effects No side effects.
 * @author MQ
 */
int fileExists(const char *filename)
{
    struct _stat buffer;
    return (_stat(filename, &buffer) == 0);
}

/**
 * @brief This function checks if a filename ends with ".txt".
 * @param filename The name of the file to check.
 * @return 1 if the file is a .txt file, 0 otherwise.
 * @side_effects No side effects.
 * @author MQ
 */
int isTxtFile(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return 0;
    return strcmp(dot, ".txt") == 0;
}

/**
 * @brief This function replaces a word in a string with another word.
 * @param str The original string.
 * @param searchWord The word to be replaced.
 * @param replaceWord The word to replace the search word.
 * @return A new string with the word replaced, or NULL if an error occurs.
 * @side_effects Allocates memory for a new string. The caller is responsible for freeing this memory.
 * @author MH, MQ
 */
char *replaceWordInString(const char *str, const char *searchWord, const char *replaceWord)
{
    int searchWordLen = strlen(searchWord);
    int replaceWordLen = strlen(replaceWord);
    int diff = replaceWordLen - searchWordLen;

    int count = 0;
    const char *tmp = str;
    while ((tmp = strstr(tmp, searchWord)) != NULL)
    {
        count++;
        tmp += searchWordLen;
    }

    int multiplication = count * diff;
    // Author: MQ
    // 3-1: Integer Overflows
    // The code checks for integer overflow when calculating the new length of the string after replacement.
    if ((diff > 0 && (multiplication < 0 || multiplication / diff != count)) ||
        (diff < 0 && multiplication > 0))
    {
        fprintf(stderr, "Error: Integer overflow\n");
        return NULL;
    }

    int newLength = strlen(str) + multiplication + 1;
    if (newLength < 0)
    {
        fprintf(stderr, "Error: Integer overflow\n");
        return NULL;
    }

    // Author: MH
    // 6-1: Failure to Handle Errors Correctly
    // The code checks if memory allocation was successful when creating a new string.
    char *newStr = malloc(newLength);
    if (newStr == NULL)
    {
        printf("Error allocating memory");
        return NULL;
    }

    char *current = newStr;
    int remainingSpace = newLength;
    while (*str)
    {
        if (strstr(str, searchWord) == str)
        {
            // Author: MH
            // 1-1: Buffer Overruns
            // The code checks if there is enough space in the buffer before copying the replacement word
            if (replaceWordLen > remainingSpace)
            {
                printf("Not enough space in the buffer");
                free(newStr);
                return NULL;
            }
            strncpy(current, replaceWord, replaceWordLen);
            current += replaceWordLen;
            remainingSpace -= replaceWordLen;
            str += searchWordLen;
        }
        else
        {
            *current++ = *str++;
            remainingSpace--;
        }
    }

    *current = '\0';

    return newStr;
}

/**
 * @brief This function processes a file, replacing a word with another word.
 * @param input The input file pointer.
 * @param output The output file pointer.
 * @param searchWord The word to search for in the file.
 * @param replaceWord The word to replace the search word with.
 * @side_effects Reads from the input file and writes to the output file. Allocates and frees buffer memory.
 * @author MH
 */
void processFile(FILE *input, FILE *output, const char *searchWord, const char *replaceWord)
{
    size_t bufferSize = 1024;
    char *buffer = malloc(bufferSize * sizeof(char));
    if (buffer == NULL)
    {
        printf("Error: memory allocation failed\n");
        exit(1);
    }
    while (fgets(buffer, bufferSize, input) != NULL)
    {
        // Author: MH
        // 1-2: Buffer Overruns
        // 6-2: Failure to Handle Errors Correctly
        // The code checks if the buffer size is exceeded when reading from the file.
        if (strlen(buffer) >= bufferSize)
        {
            printf("Error: Buffer overflow\n");
            free(buffer);
            return;
        }
        char *newBuffer = replaceWordInString(buffer, searchWord, replaceWord);
        if (newBuffer != NULL)
        {
            fputs(newBuffer, output);
            free(newBuffer);
        }
        else
        {
            free(buffer);
            return;
        }
    }
    free(buffer);
}

/**
 * @brief This is the main function that processes the command line arguments and calls the processFile function.
 * @param argc The number of command line arguments.
 * @param argv The command line arguments.
 * @return 0 if successful, 1 otherwise.
 * @side_effects Opens and closes files, processes command line arguments, and calls processFile function which has its own side effects.
 * @author MH, MQ
 */
int main(int argc, char *argv[])
{
    // Author: MH, MQ
    // 9-1: Poor Usability
    // The code checks if the correct number of arguments were provided.
    if (argc != 5)
    {
        printf("command takes 4 arguments. [file to search] [word to search for] [word to replace with] [output file]\n");
        return 1;
    }

    char *file = argv[1];
    char *searchWord = argv[2];
    char *replaceWord = argv[3];
    char *outputFile = argv[4];

    // Author: MQ
    // 9-2: Poor Usability
    // 12-1: Failure to Protect Stored Data
    // The code checks if the output file already exists
    if (fileExists(outputFile))
    {
        char response[4];
        printf("The file %s already exists. Do you want to overwrite it? (yes/no or y/n): ", outputFile);
        fgets(response, sizeof(response), stdin);
        response[strcspn(response, "\n")] = 0;
        toLowerCase(response);
        if (strcmp(response, "yes") != 0 && strcmp(response, "y") != 0)
        {
            printf("Aborted by user.\n");
            return 1;
        }
    }

    // Author: MQ
    // 9-3: Poor Usability
    // The code checks if the input and output files are .txt files
    if (!isTxtFile(file) || !isTxtFile(outputFile))
    {
        printf("Error: Only .txt files are supported\n");
        return 1;
    }

    // Author: MH
    // 9-4: Poor Usability
    // The code checks if the search word and replace word are not empty
    if (strlen(searchWord) == 0 || strlen(replaceWord) == 0)
    {
        printf("Error: search word and replace word must not be empty\n");
        return 1;
    }

    // Author: MQ
    // 9-5: Poor Usability
    // The code checks if the search word and replace word are the same
    if (strcmp(searchWord, replaceWord) == 0)
    {
        printf("Error: search word and replace word must not be the same\n");
        return 1;
    }

    // Author: MH
    // 2-1: Format String Problems
    // The code uses a controlled format string to print the search and replace words.
    printf("Searching for '%s' and replacing with '%s' in file '%s'\n", searchWord, replaceWord, file);

    // Author: MH
    // 6-3: Failure to Handle Errors Correctly
    // The code checks if the file was opened successfully.
    FILE *fp = fopen(file, "r");
    if (fp == NULL)
    {
        printf("Error: Unable to open the input file\n");
        return 1;
    }

    // Author: MH
    // 6-4: Failure to Handle Errors Correctly
    // The code checks if the replace file was created successfully.
    FILE *temp = fopen(outputFile, "w");
    if (temp == NULL)
    {
        printf("Error: Unable to create the output file\n");
        fclose(fp);
        return 1;
    }
    processFile(fp, temp, searchWord, replaceWord);

    fclose(fp);
    fclose(temp);

    return 0;
}