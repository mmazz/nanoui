#include "renderer.h"
#include <stdlib.h>
#include <stdio.h>

GLuint init_shader_program(const char *vertexPath, const char *fragmentPath) {
    GLuint vertexShader, fragmentShader;
    compile_shader(&vertexShader, GL_VERTEX_SHADER, vertexPath);
    compile_shader(&fragmentShader, GL_FRAGMENT_SHADER, fragmentPath);
    GLuint program = link_shader(vertexShader, fragmentShader);
    if (program== 0) {
        fprintf(stderr, "Error: Failed to initialize program shader\n");
        return -1;
    }
    return program;
}

const char* get_shader_content(const char* fileName)
{
    FILE *fp = fopen(fileName, "rb");
    if (fp == NULL) {
        return NULL;
    }

    // Ir al final para obtener tamaño
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp); // volver al principio

    if (size <= 0) {
        fclose(fp);
        return NULL;
    }

    // Reservar memoria (+1 para '\0')
    char* content = malloc(size + 1);
    if (content == NULL) {
        fclose(fp);
        return NULL;
    }

    // Leer contenido
    size_t read_size = fread(content, 1, size, fp);
    if (read_size != size) {
        fprintf(stderr, "Warning: Shader file %s read %zu bytes, expected %ld\n", fileName, read_size, size);
    }
    fclose(fp);
    content[read_size] = '\0'; // aseguramos null-terminación

    return (const char*)content;
}

void compile_shader(GLuint* shaderId, GLenum shaderType, const char* shaderFilePath)
{
    GLint isCompiled = 0;
    const char* shaderSource = get_shader_content(shaderFilePath);

    if (shaderSource == NULL) {
        fprintf(stderr, "Error: could not read shader file: %s\n", shaderFilePath);
        exit(1); // 💥 abortar: sin shader no tiene sentido seguir
    }

    *shaderId = glCreateShader(shaderType);
    if (*shaderId == 0) {
        fprintf(stderr, "Error: glCreateShader failed for %s\n", shaderFilePath);
        free((void*)shaderSource);
        exit(1);
    }

    glShaderSource(*shaderId, 1, &shaderSource, NULL);
    glCompileShader(*shaderId);

    glGetShaderiv(*shaderId, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE) {
        GLchar infoLog[1024];
        glGetShaderInfoLog(*shaderId, sizeof(infoLog), NULL, infoLog);
        fprintf(stderr, "Shader compilation failed (%s):\n%s\n", shaderFilePath, infoLog);
        glDeleteShader(*shaderId);
        free((void*)shaderSource);
        exit(1); // 💥 abortar directamente
    }

    free((void*)shaderSource);
}


GLuint link_shader(GLuint vertexShaderID, GLuint fragmentShaderID)
{
    GLuint programID = glCreateProgram();
    if (programID == 0) {
        fprintf(stderr, "Error: glCreateProgram() failed\n");
        return 0;
    }

    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    GLint isLinked = 0;
    glGetProgramiv(programID, GL_LINK_STATUS, &isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &maxLength);

        char* infoLog = malloc(maxLength);
        if (infoLog) {
            glGetProgramInfoLog(programID, maxLength, NULL, infoLog);
            fprintf(stderr, "Shader Program Link Error:\n%s\n", infoLog);
            free(infoLog);
            exit(1);
        }

        glDeleteProgram(programID);
        glDeleteShader(vertexShaderID);
        glDeleteShader(fragmentShaderID);
        return 0;
    }

    glDetachShader(programID, vertexShaderID);
    glDetachShader(programID, fragmentShaderID);
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    return programID;
}

