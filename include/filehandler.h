//
// Created by nbigi0 on 06/06/2025.
//

#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <stddef.h>

typedef struct s_FileData
{
    size_t size;
    char* content;
} FileData;

FileData read_file(const char* filename);

#endif //FILEHANDLER_H
