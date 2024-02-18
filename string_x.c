//
// Created by Mathieu Durand on 2024-02-16.
//

#include "string_x.h"

int length(char* str) {
    int index = 0;
    while (index < STR_MAX_LENGTH && str[index] != '\0') {
        index++;
    }

    return index;
}

int equals(char* str1, char* str2) {
    for (int index = 0; index < STR_MAX_LENGTH; index++) {
        if (str1[index] != str2[index]) {
            return 0;
        }

        if (str1[index] == '\0' || str2[index] == '\0') {
            return 1;
        }
    }

    return 0;
}

/**
 * @brief Splits a string into tokens based on a delimiter.
 *
 * This function takes a source string and a delimiter string and splits the source
 * string into tokens based on the delimiter. The tokens are stored in an array of
 * strings, provided as the 'dest' parameter. The maximum number of tokens to be
 * stored in the 'dest' array is specified by the 'size' parameter.
 *
 * @param source Pointer to the source string to be split.
 * @param delim Pointer to the delimiter string used to split the source string.
 * @param size Maximum number of tokens to be stored in the 'dest' array.
 * @param dest Array of strings where the tokens will be stored.
 *
 * @return The number of tokens found and stored in the 'dest' array.
 */
int split(char* source, char* delim, int size, char dest[size][STR_MAX_LENGTH]) {
    char temp[strlen(source) + 1];
    strcpy(temp, source);

    char* ptr = strtok(temp, delim);
    int index = 0;

    while (index < size - 1) {
        strcpy(dest[index++], ptr);
        ptr = strtok(NULL, delim);
        if (ptr == NULL) return index;
    }

    strcpy(dest[index++], ptr);

    return index;
}