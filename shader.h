#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

GLuint getProgram(const char *fragmentShader, const char *vertexShader);

GLuint createProgram(GLuint fragmentShader, GLuint vertexShader, char **log);

GLuint loadShader(GLenum type, const char *name, char **log);

#endif

