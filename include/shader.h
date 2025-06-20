//
// Created by nbigi0 on 19/06/2025.
//

#ifndef SHADER_H
#define SHADER_H

#include <filehandler.h>

struct ShaderInfo
{
    unsigned int ID;
};

void initShader(struct ShaderInfo* shader, FileData vertexShaderSource, FileData fragmentShaderSource);
void use(unsigned int shaderID);
void setInt(struct ShaderInfo shader, const char* name, int value);
void setFloat(struct ShaderInfo shader, const char* name, float value);

#endif //SHADER_H
