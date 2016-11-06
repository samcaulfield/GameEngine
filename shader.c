#include <stdio.h>
#include <stdlib.h>

#include "file.h"
#include "shader.h"
#include "utils.h"

GLuint getProgram(const char *f_fragmentShader, const char *f_vertexShader)
{
	char *log = NULL;

	GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, f_fragmentShader, &log);
	if (log) {
		fprintf(stderr, "%s log:\n%s", f_fragmentShader, log);
		freeAndNull((void **) &log); // discard
	}
	if (!fragmentShader) {
		return 0;
	}

	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, f_vertexShader, &log);
	if (log) {
		fprintf(stderr, "%s shader log:\n%s", f_vertexShader, log);
		freeAndNull((void **) &log); // discard
	}
	if (!vertexShader) {
		glDeleteShader(fragmentShader);
		return 0;
	}

	GLuint program = createProgram(fragmentShader, vertexShader, &log);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
	if (log) {
		fprintf(stderr, "Program log:\n%s", log);
		free(log);
	}
	return program;
}

GLuint createProgram(GLuint fragmentShader, GLuint vertexShader, char **log)
{
	GLuint program = glCreateProgram();
	if (!program) {
		return 0;
	}
	glAttachShader(program, fragmentShader);
	glAttachShader(program, vertexShader);

	glLinkProgram(program);
	GLint len;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
	if (len > 1) {
		*log = malloc(len * sizeof **log);
		if (!log) {
			glDeleteProgram(program);
			return 0;
		}
		glGetProgramInfoLog(program, len, NULL, *log);
	}

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (!status) {
		glDeleteProgram(program);
		return 0;
	}
	return program;
}

GLuint loadShader(GLenum type, const char *name, char **log)
{
	GLuint shader = glCreateShader(type);
	if (!shader) {
		return 0;
	}

	char *source = loadFile(name);
	if (!source) {
		glDeleteShader(shader);
		return 0;
	}
	glShaderSource(shader, 1, (const GLchar **) &source, NULL);
	free(source);

	glCompileShader(shader);
	GLint len;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len > 1) {
		*log = malloc(len * sizeof **log);
		if (!*log) {
			glDeleteShader(shader);
			return 0;
		}
		glGetShaderInfoLog(shader, len, NULL, *log);
	}

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (!status) {
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

