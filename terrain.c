#include <stdio.h>
#include <stdlib.h>

#include "stb_image.h"

#include "file.h"
#include "math.h"
#include "terrain.h"
#include "utils.h"

float heightmapGet(float *heightmap, uint32_t size, float x, float z)
{
	int i = x + z * size;
	if (i < 0 || i > (size * size - 1))
		return 0;
	return heightmap[i];
}

float terrainGetHeightAt(struct Terrain *t, float x, float z)
{
	float cameraXTerrainSpace = x - t->mesh->x, cameraZTerrainSpace = z - t->mesh->z;
	cameraXTerrainSpace /= t->scale;
	cameraZTerrainSpace /= t->scale;

	/*
		h0 x----x h1
		   |    |
		   |    |
		h2 x----x h3
	*/

	int gx = (int) cameraXTerrainSpace, gz = (int) cameraZTerrainSpace;

	float h0 = heightmapGet(t->heightmap, t->size, gx, gz);
	float h1 = heightmapGet(t->heightmap, t->size, gx + 1, gz);
	float h2 = heightmapGet(t->heightmap, t->size, gx, gz + 1);
	float h3 = heightmapGet(t->heightmap, t->size, gx + 1, gz + 1);

	if (cameraXTerrainSpace <= 1 - cameraZTerrainSpace) {
		// upper tri
		return barycentric(gx + 1, h1, gz, gx, h0, gz, gx, h2, gz + 1, cameraXTerrainSpace, cameraZTerrainSpace);
	} else {
		// lower tri
		return barycentric(gx + 1, h1, gz, gx, h2, gz + 1, gx + 1, h3, gz + 1, cameraXTerrainSpace, cameraZTerrainSpace);
	}
}

void cleanupTerrain(struct Terrain *terrain)
{
	CleanupMesh(terrain->mesh);
	free(terrain->heightmap);
	free(terrain);
}

/* for a size * size grid, returns the number of triangles */
uint32_t getTerrainMeshNumTris(uint32_t size)
{
	uint32_t numTris = 2;
	for (uint32_t i = 3; i <= size; i++) {
		numTris += 2 * ((i - 1) + (i - 2));
	}
	return numTris;
}

struct Terrain *generateTerrain(uint32_t size, GLint positionAttribLocation, GLint vertexUVAttribLocation,
	GLint normalAttribLocation, const char *texture, unsigned int seed, const char *map, float scale)
{
	float *heightmap = calloc(size * size, sizeof(float));
	if (!heightmap) {
		return NULL;
	}

	struct Mesh *mesh = calloc(1, sizeof(struct Mesh));
	if (!mesh) {
		free(heightmap);
		return NULL;
	}

	struct Terrain *terrain = calloc(1, sizeof(struct Terrain));
	if (!terrain) {
		free(heightmap);
		free(mesh);
		return NULL;
	}

	terrain->heightmap = heightmap;
	terrain->mesh = mesh;
	terrain->size = size;
	terrain->scale = scale;

	int mapWidth, mapHeight, nn;
	int x, y;

	unsigned char *hmap = stbi_load(map, &mapWidth, &mapHeight, &nn, 0);
	if (!hmap) {
		fprintf(stderr, "Could not load height map.\n");
		free(heightmap); free(mesh); free(terrain);
		return NULL;
	}
	// generate
	for (y = 0; y < mapHeight; y++) {
		for (x = 0; x < mapWidth; x++) {
			heightmap[x + y * mapWidth] = hmap[x + y * mapWidth] / 15.0f;
		}
	}
	free(hmap);

	// produce mesh
	uint32_t numTris = getTerrainMeshNumTris(size);

	float *positions = malloc(numTris * 9 * sizeof(float)); // 3 floats per vertex
	if (!positions) {
		free(heightmap); free(mesh); free(terrain);
		return NULL;
	}
	float *textureCoordinates = malloc(numTris * 6 * sizeof(float)); // 2 floats per vertex
	if (!textureCoordinates) {
		free(heightmap); free(mesh); free(terrain); free(positions);
		return NULL;
	}
	float *normals = malloc(numTris * 9 * sizeof(float));
	if (!normals) {
		free(heightmap); free(mesh); free(terrain); free(positions); free(textureCoordinates);
		return NULL;
	}

	mesh->numVertices = numTris * 3;

	uint32_t positionsIndex = 0, textureCoordinatesIndex = 0, normalsIndex = 0;
	for (y = 0; y < size; y++) {
		for (x = 0; x < size; x++) {
			if (x < size - 1 && y < size - 1) {
				// 0
				positions[positionsIndex++] = (float) x + 1.0f;
				positions[positionsIndex++] = heightmap[x + 1 + y * size];
				positions[positionsIndex++] = (float) y; // a plane of heightmap is z plane
				textureCoordinates[textureCoordinatesIndex++] = 1.0f;
				textureCoordinates[textureCoordinatesIndex++] = 0.0f;

				// 1
				positions[positionsIndex++] = (float) x;
				positions[positionsIndex++] = heightmap[x + y * size];
				positions[positionsIndex++] = (float) y; // a plane of heightmap is z plane
				textureCoordinates[textureCoordinatesIndex++] = 0.0f;
				textureCoordinates[textureCoordinatesIndex++] = 0.0f;

				// 2
				positions[positionsIndex++] = (float) x;
				positions[positionsIndex++] = heightmap[x + (y + 1) * size];
				positions[positionsIndex++] = (float) y + 1.0f; // a plane of heightmap is z plane
				textureCoordinates[textureCoordinatesIndex++] = 0.0f;
				textureCoordinates[textureCoordinatesIndex++] = 1.0f;

				float ux = (float) x - (float) (x + 1);
				float uy = heightmap[x + y * size] - heightmap[x + 1 + y * size];
				float uz = (float) y - (float) y;

				float vx = (float) x - (x + 1.0f);
				float vy = heightmap[x + (y + 1) * size] - heightmap[x + 1 + y * size];
				float vz = (float) (y + 1) - (float) y;

				float nx, ny, nz;
				crossProduct(ux, uy, uz, vx, vy, vz, &nx, &ny, &nz);

				normals[normalsIndex++] = nx; normals[normalsIndex++] = ny; normals[normalsIndex++] = nz;
				normals[normalsIndex++] = nx; normals[normalsIndex++] = ny; normals[normalsIndex++] = nz;
				normals[normalsIndex++] = nx; normals[normalsIndex++] = ny; normals[normalsIndex++] = nz;
			}
			if (x > 0 && y < size - 1) {
				positions[positionsIndex++] = (float) x;
				positions[positionsIndex++] = heightmap[x + y * size];
				positions[positionsIndex++] = (float) y; // plane of heightmap is z plane
				textureCoordinates[textureCoordinatesIndex++] = 1.0f;
				textureCoordinates[textureCoordinatesIndex++] = 0.0f;

				positions[positionsIndex++] = (float) x - 1.0f;
				positions[positionsIndex++] = heightmap[x - 1 + (y + 1) * size];
				positions[positionsIndex++] = (float) y + 1.0f; // plane of heightmap is z plane
				textureCoordinates[textureCoordinatesIndex++] = 0.0f;
				textureCoordinates[textureCoordinatesIndex++] = 1.0f;

				positions[positionsIndex++] = (float) x;
				positions[positionsIndex++] = heightmap[x + (y + 1) * size];
				positions[positionsIndex++] = (float) y + 1.0f; // plane of heightmap is z plane
				textureCoordinates[textureCoordinatesIndex++] = 1.0f;
				textureCoordinates[textureCoordinatesIndex++] = 1.0f;

				float ux = (float) (x - 1.0f) - (float) x;
				float uy = heightmap[x - 1 + (y + 1) * size] - heightmap[x + y * size];
				float uz = (float) (y + 1.0f) - (float) y;

				float vx = (float) x - (float) x;
				float vy = heightmap[x + (y + 1) * size] - heightmap[x + y * size];
				float vz = (float) (y + 1.0f) - (float) y;

				float nx, ny, nz;
				crossProduct(ux, uy, uz, vx, vy, vz, &nx, &ny, &nz);

				normals[normalsIndex++] = nx; normals[normalsIndex++] = ny; normals[normalsIndex++] = nz;
				normals[normalsIndex++] = nx; normals[normalsIndex++] = ny; normals[normalsIndex++] = nz;
				normals[normalsIndex++] = nx; normals[normalsIndex++] = ny; normals[normalsIndex++] = nz;

			}
		}
	}

	glGenVertexArrays(1, &mesh->VAO);
	glBindVertexArray(mesh->VAO);

	glGenBuffers(1, &mesh->positionsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->positionsBuffer);
	glBufferData(GL_ARRAY_BUFFER, mesh->numVertices * 3 * sizeof(float), positions, GL_STATIC_DRAW);
	free(positions);
	glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(positionAttribLocation);

	glGenBuffers(1, &mesh->normals);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->normals);
	glBufferData(GL_ARRAY_BUFFER, mesh->numVertices * 3 * sizeof(float), normals, GL_STATIC_DRAW);
	free(normals);
	glVertexAttribPointer(normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(normalAttribLocation);

	glGenBuffers(1, &mesh->textureCoordinatesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mesh->textureCoordinatesBuffer);
	glBufferData(GL_ARRAY_BUFFER, mesh->numVertices * 2 * sizeof(float), textureCoordinates, GL_STATIC_DRAW);
	free(textureCoordinates);
	glVertexAttribPointer(vertexUVAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(vertexUVAttribLocation);

	// load a texture
	int width, height, n;
	unsigned char *imageData = stbi_load(texture, &width, &height, &n, 0);
	if (!imageData) {
		fprintf(stderr, "Error loading texture %s.\n", texture);
		glBindVertexArray(0);
		return terrain;
	}

	glGenTextures(1, &mesh->texture);
	glBindTexture(GL_TEXTURE_2D, mesh->texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
	glGenerateMipmap(GL_TEXTURE_2D);
	free(imageData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	glBindVertexArray(0);
	return terrain;
}

