#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>

struct Mesh {
	GLuint VAO, normals, texture, positionsBuffer, textureCoordinatesBuffer;
	uint32_t numVertices;
	float x, y, z;
	float rx, ry;
};

void drawMesh(struct Mesh *mesh, GLint modelMatrixUniformLocation, GLint modelXRotationMatrixUniformLocation, GLint modelYRotationMatrixUniformLocation);

struct Mesh *cube(float x, float y, float z, float size, GLint positionsAttribLocation,
	GLint textureCoordinatesAttribLocation, GLint normalAttribLocation, const char *texture);

struct Mesh *pyramid(float x, float y, float z, float size, GLint positionsAttribLocation,
	GLint textureCoordinatesAttribLocation, GLint normalAttribLocation, const char *texture);

struct Mesh *square(float x, float y, float z, float size, GLint positionsAttribLocation,
	GLint textureCoordinatesAttribLocation, GLint normalAttribLocation, const char *texture);

void CleanupMesh(struct Mesh *mesh);

#endif

