//
// Created by nbigi0 on 06/06/2025.
//

#include <filehandler.h>
#ifndef FILEHANDLER_H_NO_INCLUDE
#include <stddef.h>
#endif
#include <malloc.h>
#include <memory.h>
#include <stdio.h>

FileData read_file(const char *filename)
{
    FileData data = {0, NULL};
    FILE *file;

    // read for size
    file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Error opening file %s\n", filename);
        return data;
    }
    fseek(file, 0L, SEEK_END);
    data.size = ftell(file)+1;
    fclose(file);

    // read for data

    fopen(filename, "r");
    data.content = (char*)malloc(data.size);
    memset(data.content, '\0', data.size);
    fread(data.content, data.size, 1, file);
    fclose(file);

    return data;
}
