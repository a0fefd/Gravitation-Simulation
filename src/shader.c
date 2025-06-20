//
// Created by nbigi0 on 19/06/2025.
//

#include <shader.h>
#include <stdio.h>
#include <glad/glad.h>

void initShader(struct ShaderInfo* shader, FileData vertexShaderSource, FileData fragmentShaderSource)
{
    int success;
    char infoLog[512];

    const GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, (const GLchar* const*)&(vertexShaderSource.content), NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        fprintf(stderr, "%s\n", infoLog);
    }

    const GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, (const GLchar* const*)&(fragmentShaderSource.content), NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        fprintf(stderr, "%s\n", infoLog);
    }

    shader->ID = glCreateProgram();
    glAttachShader(shader->ID, vertex);
    glAttachShader(shader->ID, fragment);
    glLinkProgram(shader->ID);
    glGetProgramiv(shader->ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shader->ID, 512, NULL, infoLog);
        fprintf(stderr, "%s\n", infoLog);
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void use(unsigned int shaderID)
{
    glUseProgram(shaderID);
}

void setInt(struct ShaderInfo shader, const char* name, int value)
{
    glUniform1i(glGetUniformLocation(shader.ID, name), value);
}

void setFloat(struct ShaderInfo shader, const char* name, float value)
{
    glUniform1f(glGetUniformLocation(shader.ID, name), value);
}