#ifndef TERRAIN_H
#define TERRAIN_H

#include <stdint.h>

#include "mesh.h"

struct Terrain {
	struct Mesh *mesh;
	float *heightmap;
	uint32_t size;
	float scale;
};

float terrainGetHeightAt(struct Terrain *t, float x, float z);

struct Terrain *generateTerrain(uint32_t size, GLint positionAttribLocation, GLint vertexUVAttribLocation,
	GLint normalAttribLocation, const char *texture, unsigned int seed, const char *map, float scale);

void cleanupTerrain(struct Terrain *terrain);

#endif

