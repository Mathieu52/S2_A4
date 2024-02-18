//
// Created by Mathieu Durand on 2024-02-16.
//

#include "string_x.h"

/**
 * @brief Calculates the length of a null-terminated string.
 *
 * This function iterates over the characters of the input string until it
 * encounters the null terminator ('\0') or reaches the maximum allowed length
 * of the string, whichever comes first. It then returns the number of characters
 * encountered before the null terminator.
 *
 * @param str Pointer to the null-terminated string whose length is to be calculated.
 *            If NULL, the function returns 0.
 * @return The length of the string, excluding the null terminator.
 */
int length(const char* str) {
    if (str == NULL) return 0;  // Handle NULL pointer gracefully

    int index = 0;
    while (index < STR_MAX_LENGTH && str[index] != '\0') {
        index++;
    }

    return index;
}

/**
 * @brief Checks if two null-terminated strings are equal.
 *
 * This function iterates over the characters of both input strings simultaneously
 * until it encounters a mismatch or reaches the maximum allowed length of the strings.
 * If a mismatch is found, it returns 0. If both strings are equal up to the null
 * terminator of either string, it returns 1. If one of the strings is NULL and the
 * other is not, it returns 0. If both strings are NULL, it returns 1.
 *
 * @param str1 Pointer to the first null-terminated string to compare.
 * @param str2 Pointer to the second null-terminated string to compare.
 * @return 1 if the strings are equal up to the null terminator of either string,
 *         0 otherwise. If one of the strings is NULL and the other is not, it returns 0.
 *         If both strings are NULL, it returns 1.
 */
int equals(const char* str1, const char* str2) {
    // Handle NULL pointer gracefully
    if (str1 == NULL && str2 == NULL) return 1;
    if (str1 == NULL || str2 == NULL) return 0;

    for (int index = 0; index < STR_MAX_LENGTH; index++) {
        if (str1[index] != str2[index]) {
            return 0; // Mismatch found
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