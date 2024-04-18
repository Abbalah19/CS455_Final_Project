#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int fileExists(const char *filename)
{
    struct _stat buffer;
    return (_stat(filename, &buffer) == 0);
}

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
    // 3. Integer Overflows
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

    // 6. Failure to Handle Errors Correctly
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
            // 1. Buffer Overruns
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
        // 1. Buffer Overruns
        // 6. Failure to Handle Errors Correctly
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

int main(int argc, char *argv[])
{
    // 9. Poor Usability
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

    // 9. Poor Usability
    // 12. Failure to Protect Stored Data
    // The code checks if the output file already exists
    if (fileExists(outputFile))
    {
        char response[4];
        printf("The file %s already exists. Do you want to overwrite it? (yes/no): ", outputFile);
        fgets(response, sizeof(response), stdin);
        response[strcspn(response, "\n")] = 0;
        if (strcmp(response, "yes") != 0)
        {
            printf("Aborted by user.\n");
            return 1;
        }
    }

    // 9. Poor Usability
    // The code checks if the search word and replace word are not empty
    if (strlen(searchWord) == 0 || strlen(replaceWord) == 0)
    {
        printf("Error: search word and replace word must not be empty\n");
        return 1;
    }

    // 1. Buffer Overruns
    // 9. Poor Usability
    // The code checks if the search word and replace word are not too long
    if (strlen(searchWord) >= 50 || strlen(replaceWord) >= 50)
    {
        printf("Error: search word and replace word can not be longer than 99 characters\n");
        return 1;
    }

    // 2. Format String Problems
    // The code uses a controlled format string to print the search and replace words.
    printf("Searching for '%s' and replacing with '%s' in file '%s'\n", searchWord, replaceWord, file);

    // 6. Failure to Handle Errors Correctly
    // The code checks if the file was opened successfully.
    FILE *fp = fopen(file, "r");
    if (fp == NULL)
    {
        printf("Error opening file");
        return 1;
    }

    // 6. Failure to Handle Errors Correctly
    // The code checks if the replace file was created successfully.
    FILE *temp = fopen(outputFile, "w");
    if (temp == NULL)
    {
        printf("Error: replace file could not be created\n");
        fclose(fp);
        return 1;
    }
    processFile(fp, temp, searchWord, replaceWord);

    fclose(fp);
    fclose(temp);

    return 0;
}