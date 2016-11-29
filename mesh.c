#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "file.h"
#include "maths.h"
#include "mesh.h"
#include "utils.h"

void CleanupMesh(struct Mesh *mesh)
{
	glDeleteVertexArrays(1, &mesh->VAO);
	glDeleteBuffers(1, &mesh->positionsBuffer);
	glDeleteBuffers(1, &mesh->normals);
	glDeleteBuffers(1, &mesh->textureCoordinatesBuffer);
	glDeleteTextures(1, &mesh->texture);
	free(mesh);
}

void drawMesh(struct Mesh *mesh, GLint modelMatrixUniformLocation, GLint modelXRotationMatrixUniformLocation, GLint modelYRotationMatrixUniformLocation)
{
	float xRotation[16];
	loadXRotation(mesh->rx, xRotation);
	glUniformMatrix4fv(modelXRotationMatrixUniformLocation, 1, GL_TRUE, xRotation);

	float yRotation[16];
	loadYRotation(mesh->ry, yRotation);
	glUniformMatrix4fv(modelYRotationMatrixUniformLocation, 1, GL_TRUE, yRotation);

	float translation[16]; // model
	loadTranslation(mesh->x, mesh->y, mesh->z, translation);
	MatrixMatrixMul(yRotation, xRotation);
	MatrixMatrixMul(translation, yRotation);

	glUniformMatrix4fv(modelMatrixUniformLocation, 1, GL_TRUE, translation);

	glBindVertexArray(mesh->VAO);
	glBindTexture(GL_TEXTURE_2D, mesh->texture);
	glDrawArrays(GL_TRIANGLES, 0, mesh->numVertices);

	glBindVertexArray(0);
}

struct Mesh *square(float x, float y, float z, float size, GLint positionAttribLocation,
	GLint vertexUVAttribLocation, GLint normalAttribLocation, const char *texture)
{
	struct Mesh *mesh = calloc(1, sizeof(struct Mesh));
	if (!mesh) {
		return NULL;
	}

	const float a = size / 2.0f;
	const float positions[] = {
		 a,  a, 0.0f,
		-a,  a, 0.0f,
		-a, -a, 0.0f,
		 a,  a, 0.0f,
		-a, -a, 0.0f,
		 a, -a, 0.0f
	};

	const float normals[] = {
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f
	};

	const float textureCoordinates[] = {
		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f
	};

	mesh->numVertices = sizeof(positions) / (3 * sizeof(float));
	mesh->x = x; mesh->y = y; mesh->z = z;

	glGenVertexArrays(1, &mesh->VAO);
	glBindVertexArray(mesh->VAO);

	glGenBuffers(1, &mesh->positionsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->positionsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(positionAttribLocation);

	glGenBuffers(1, &mesh->normals);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(normalAttribLocation);

	glGenBuffers(1, &mesh->textureCoordinatesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->textureCoordinatesBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexUVAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(vertexUVAttribLocation);

	// load a texture
	int width, height, n;
	unsigned char *imageData = stbi_load(texture, &width, &height, &n, 0);
	if (!imageData) {
		fprintf(stderr, "Error loading texture %s.\n", texture);
		glBindVertexArray(0);
		return mesh;
	}

	glGenTextures(1, &mesh->texture);
	glBindTexture(GL_TEXTURE_2D, mesh->texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);
	free(imageData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glBindVertexArray(0);
	return mesh;
}

struct Mesh *pyramid(float x, float y, float z, float size, GLint positionAttribLocation,
	GLint vertexUVAttribLocation, GLint normalAttribLocation, const char *texture)
{
	struct Mesh *mesh = calloc(1, sizeof(struct Mesh));
	if (!mesh) {
		return NULL;
	}

	const float a = size / 2.0f;
	const float positions[] = {
		// base
		 a, 0.0f,  a, 
		-a, 0.0f,  a, 
		-a, 0.0f, -a, 
		 a, 0.0f,  a, 
		-a, 0.0f, -a, 
		 a, 0.0f, -a, 
		// front
		0.0f, size, 0.0f, 
		-a, 0.0f, a, 
		 a, 0.0f, a, 
		// right
		0.0f, size, 0.0f, 
		a, 0.0f, a, 
		a, 0.0f, -a, 
		// back
		0.0f, size, 0.0f, 
		 a, 0.0f, -a, 
		-a, 0.0f, -a, 
		// left
		0.0f, size, 0.0f, 
		-a, 0.0f, -a, 
		-a, 0.0f,  a
	};

	const float normals[] = {
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,

		0.0f, 0.707107f, 0.707107f,
		0.0f, 0.707107f, 0.707107f,
		0.0f, 0.707107f, 0.707107f,

		0.707107f, 0.707107f, 0.0f,
		0.707107f, 0.707107f, 0.0f,
		0.707107f, 0.707107f, 0.0f,

		0.0f, 0.707107f, -0.707107f,
		0.0f, 0.707107f, -0.707107f,
		0.0f, 0.707107f, -0.707107f,

		-0.707107f, 0.707107f, 0.0f,
		-0.707107f, 0.707107f, 0.0f,
		-0.707107f, 0.707107f, 0.0f
	};

	mesh->numVertices = sizeof(positions) / (3 * sizeof(float));
	mesh->x = x; mesh->y = y; mesh->z = z;

	const float textureCoordinates[] = {
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

		0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 
		0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f
	};

	glGenVertexArrays(1, &mesh->VAO);
	glBindVertexArray(mesh->VAO);

	glGenBuffers(1, &mesh->positionsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->positionsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(positionAttribLocation);

	glGenBuffers(1, &mesh->normals);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(normalAttribLocation);

	glGenBuffers(1, &mesh->textureCoordinatesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->textureCoordinatesBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexUVAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(vertexUVAttribLocation);

	// load a texture
	int width, height, n;
	unsigned char *imageData = stbi_load(texture, &width, &height, &n, 0);
	if (!imageData) {
		fprintf(stderr, "Error loading texture.\n");
		glBindVertexArray(0);
		return mesh;
	}

	glGenTextures(1, &mesh->texture);
	glBindTexture(GL_TEXTURE_2D, mesh->texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	free(imageData);
	glBindVertexArray(0);
	return mesh;
}

struct Mesh *cube(float x, float y, float z, float size, GLint positionAttribLocation,
	GLint vertexUVAttribLocation, GLint normalAttribLocation, const char *texture)
{
	struct Mesh *mesh = calloc(1, sizeof(struct Mesh));
	if (!mesh) {
		return NULL;
	}
	// 6 faces per cube, 2 tris per face
	// 3 vertices per tri, 4 floats per vertex
	const float a = size / 2.0f;
	const float BB = -a; // called it BB so it's more readable below
	const float positions[] = {
		// right face
		a,  a, BB, 
		a,  a,  a, 
		a, BB, BB, 
		a,  a,  a, 
		a, BB,  a, 
		a, BB, BB, 
		// left face
		BB,  a, BB, 
		BB,  a,  a, 
		BB, BB, BB, 
		BB,  a,  a, 
		BB, BB,  a, 
		BB, BB, BB, 
		// top 
		 a, a, BB, 
		BB, a, BB, 
		BB, a,  a, 
		 a, a, BB, 
		BB, a,  a, 
		 a, a,  a, 
		// bottom 
		 a, BB, BB, 
		BB, BB, BB, 
		BB, BB,  a, 
		 a, BB, BB, 
		BB, BB,  a, 
		 a, BB,  a, 
		// front 
		 a,  a, a, 
		BB,  a, a, 
		BB, BB, a, 
		 a,  a, a, 
		BB, BB, a, 
		 a, BB, a, 
		//back 
		 a,  a, BB, 
		BB,  a, BB, 
		BB, BB, BB, 
		 a,  a, BB, 
		BB, BB, BB, 
		 a, BB, BB
	};

	const float normals[] = {
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f
	};

	mesh->numVertices = sizeof(positions) / (3 * sizeof(float));
	mesh->x = x; mesh->y = y; mesh->z = z;

	const float textureCoordinates[] = {
		1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

		1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,

		1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	};

	glGenVertexArrays(1, &mesh->VAO);
	glBindVertexArray(mesh->VAO);

	glGenBuffers(1, &mesh->positionsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->positionsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(positionAttribLocation);

	glGenBuffers(1, &mesh->normals);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->normals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(normalAttribLocation);

	glGenBuffers(1, &mesh->textureCoordinatesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->textureCoordinatesBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexUVAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(vertexUVAttribLocation);

	// load a texture
	int width, height, n;
	unsigned char *imageData = stbi_load(texture, &width, &height, &n, 0);
	if (!imageData) {
		fprintf(stderr, "Error loading texture.\n");
		glBindVertexArray(0);
		return mesh;
	}

	glGenTextures(1, &mesh->texture);
	glBindTexture(GL_TEXTURE_2D, mesh->texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	free(imageData);
	glBindVertexArray(0);
	return mesh;
}

