#ifndef MATH_H
#define MATH_H

#include <stdbool.h>

typedef struct {
	float x, y, z;
} vec3f;

float triangleArea2D(float ax, float ay, float bx, float by, float cx, float cy);

float dotProduct(float ax, float ay, float az, float bx, float by, float bz);
float dotProduct2D(float ax, float ay, float bx, float by);

float triangleArea2D(float ax, float ay, float bx, float by, float cx, float cy);

float distance2D(float ax, float ay, float bx, float by);
float distance3D(float ax, float ay, float az, float bx, float by, float bz);

// returns interpolated iy inside the triangle ABC
float barycentric(float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz,
	float ix, float iz);

float noise2D(float x, float z);

// return a random number [-1, 1]. Seed before calling
float smallRand();

float lerp(float x0, float y0, float x1, float y1, float x);

float magnitude(float x, float y, float z);

void normalise(float *v3);

void crossProduct(float ax, float ay, float az, float bx, float by, float bz, float *cx, float *cy, float *cz);

bool invertMatrix(const float *m, float *invOut);

void printMatrix(float *mat);

void loadXRotation(float xRotation, float *mat);

void loadYRotation(float yRotation, float *mat);

void loadTranslation(float x, float y, float z, float *mat);

void loadPerspective(float *mat, float near, float far, float FOV, float aspectRatio);

float radians(float x);

void identity(float *mat);

void MatrixMatrixMul(float *a, float *b);

void vectorMatrixMul(float *vec, float *mat);

void vectorXRotate(float xRotation, float *vec);

void vectorYRotate(float yRotation, float *vec);

void vectorZRotate(float yRotation, float *vec);

#endif

