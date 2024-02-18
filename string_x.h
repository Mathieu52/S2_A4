//
// Created by Mathieu Durand on 2024-02-16.
//

#ifndef DISTRIBUTION_STRING_H
#define DISTRIBUTION_STRING_H

#include "string.h"
#include <stdio.h>

#define STR_MAX_LENGTH 4096

int length(char* str);
int equals(char* str1, char* str2);

int split(char* source, char* delim, int size, char dest[size][STR_MAX_LENGTH]);

#endif //DISTRIBUTION_STRING_H
