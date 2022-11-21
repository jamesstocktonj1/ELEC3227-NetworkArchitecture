#ifndef GETLINE_H
#define GETLINE_H

#ifdef _WIN32
#include <stdio.h>
#include <stdlib.h>
#define GETLINE_BUFLEN 128
size_t getline(char** lineptr, size_t* n, FILE* stream);
#endif

#endif // GETLINE_H