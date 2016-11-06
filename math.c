#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "math.h"

float dotProduct(float ax, float ay, float az, float bx, float by, float bz)
{
	return ax * bx + ay * by + az * bz;
}

float dotProduct2D(float ax, float ay, float bx, float by)
{
	return ax * bx + ay * by;
}

float distance2D(float ax, float ay, float bx, float by)
{
	return sqrtf(powf((bx - ax), 2) + powf((by - ay), 2));
}

float distance3D(float ax, float ay, float az, float bx, float by, float bz)
{
	return sqrtf(powf((bx - ax), 2) + powf((by - ay), 2) + powf((bz - az), 2));
}

float triangleArea2D(float ax, float ay, float bx, float by, float cx, float cy)
{
	// heron's formula
	float a = distance2D(bx, by, cx, cy), b = distance2D(ax, ay, cx, cy), c = distance2D(ax, ay, bx, by);
	float s = (a + b + c) / 2.0f;
	return sqrtf(s * (s - a) * (s - b) * (s - c));
}

float barycentric(float ax, float ay, float az, float bx, float by, float bz, float cx, float cy, float cz,
	float x, float z)
{
        float det = (bz - cz) * (ax - cx) + (cx - bx) * (az - cz);

        float l1 = ((bz - cz) * (x - cx) + (cx - bx) * (z - cz)) / det;
        float l2 = ((cz - az) * (x - cx) + (ax - cx) * (z - cz)) / det;
        float l3 = 1.0f - l1 - l2;

        return l1 * ay + l2 * by + l3 * cy;
/*
	// *iy = (A1*ay + A2*by + A3*cy)/A
	float A1 = triangleArea2D(bx, bz, cx, cz, ix, iz);
	float A2 = triangleArea2D(ax, az, cx, cz, ix, iz);
	float A3 = triangleArea2D(ax, az, bx, bz, ix, iz);
	float A = A1 + A2 + A3;
	return (A1 * ay + A2 * by + A3 * cy) / A;
*/
}

float l(float a, float b, float x)
{
	return a * (1.0f - x) + b * x;
}

float lerpx(float x, float y)
{
//	printf("lerpx(%f, %f)\n", x, y);
	float x0, y0, x1, y1, fx, fy;
	x0 = floorf(x); x1 = ceilf(x);
	y0 = floorf(y); y1 = ceilf(y);
	fx = x - x0; fy = y - y0;

//	float i0 = lerp(x0, y0, x1, y0, x);
	float i0 = l(x0, x1, fx);
	printf("i0 %f\n", i0);
	float i1 = lerp(x0, y1, x1, y1, x);
	float r = lerp(i0, y0, i1, y1, y);
//	printf("r %f\n", r);
}

float noise2D(float x, float z)
{
//	printf("noise2D(%f, %f)\n", x, z);
	float y = 0;

	const int n = 1;

	float freq, amp;

	for (int i = 0; i < n; i++) {
		freq = 1.0f;//powf(2, i);
		amp = 1.0f;// / (powf(freq, 2));
		y += lerpx(x * freq, z * freq) * amp;
	}

	return y;
}

float smallRand()
{
	return (((float) rand() / RAND_MAX) * 2.0f) - 1.0f;
}

float lerp(float x0, float y0, float x1, float y1, float x)
{
	// ratio of similar triangles
	return (x - x0) / ((x1 - x0) / (y1 - y0)) + y0;
}

float magnitude(float x, float y, float z)
{
	return sqrtf(x * x + y * y + z * z);
}

void normalise(float *v3)
{
	float m = magnitude(v3[0], v3[1], v3[2]);
	v3[0] /= m;
	v3[1] /= m;
	v3[2] /= m;
}

void crossProduct(float ax, float ay, float az, float bx, float by, float bz, float *cx, float *cy, float *cz)
{
	*cx = ay * bz - az * by;
	*cy = az * bx - ax * bz;
	*cz = ax * by - ay * bx;
}

// from the web
bool invertMatrix(const float *m, float *invOut)
{
    float inv[16], det;
    int i;

    inv[0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return true;
}

void MatrixMatrixMul(float *a, float *b)
{
	float c[16] = {0};
	int i, j, row, col;
	for (i = 0; i < 16; i++) {
		row = i / 4;
		col = i % 4;
		for (j = 0; j < 4; j++) {
			c[i] += a[row * 4 + j] * b[j * 4 + col];
		}
	}
	memcpy(a, c, 16 * sizeof(float));
}

void printMatrix(float *mat)
{
	int i;
	for (i = 0; i < 16; i++) {
		printf("%.3f", mat[i]);
		if (i == 3 || i == 7 || i == 11 || i == 15) {
			printf("\n");
		} else {
			printf(", ");
		}
	}
}

void loadXRotation(float xRotation, float *mat)
{
	identity(mat);
	mat[5] = cos(radians(xRotation));
	mat[6] = sin(radians(xRotation));
	mat[9] = -sin(radians(xRotation));
	mat[10] = cos(radians(xRotation));
}

void loadYRotation(float yRotation, float *mat)
{
	identity(mat);
	mat[0] = cos(radians(yRotation));
	mat[2] = -sin(radians(yRotation));
	mat[8] = sin(radians(yRotation));
	mat[10] = cos(radians(yRotation));
}

void loadZRotation(float zRotation, float *mat)
{
	identity(mat);
	mat[0] = cos(radians(zRotation));
	mat[1] = sin(radians(zRotation));
	mat[4] = -sin(radians(zRotation));
	mat[5] = cos(radians(zRotation));
}

void loadTranslation(float x, float y, float z, float *mat)
{
	identity(mat);
	mat[3] = x;
	mat[7] = y;
	mat[11] = z;
}


void identity(float *mat)
{
	memset(mat, 0, 16 * sizeof(float));
	mat[0] = 1.0f;
	mat[5] = 1.0f;
	mat[10] = 1.0f;
	mat[15] = 1.0f;
}

void vectorMatrixMul(float *vec, float *mat)
{
	float result[4] = {0};
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			result[i] += mat[i * 4 + j] * vec[j];
		}
	}
	vec[0] = result[0];
	vec[1] = result[1];
	vec[2] = result[2];
	vec[3] = result[3];
}

void vectorXRotate(float xRotation, float *vec)
{
	float mat[16];
	loadXRotation(xRotation, mat);
	vectorMatrixMul(vec, mat);
}

void vectorYRotate(float yRotation, float *vec)
{
	float mat[16];
	loadYRotation(yRotation, mat);
	vectorMatrixMul(vec, mat);
}

void vectorZRotate(float zRotation, float *vec)
{
	float mat[16];
	loadZRotation(zRotation, mat);
	vectorMatrixMul(vec, mat);
}

void loadPerspective(float *mat, float near, float far, float FOV, float aspectRatio)
{
//	float depth = far - near;
//	float yScale = (1.0f / tanf(radians(FOV / 2.0f))) * aspectRatio;
//	float xScale = yScale / aspectRatio;
/*
	identity(mat);
	mat[0] = 1.0f / (aspectRatio * tanf(radians(FOV / 2.0f)));
	mat[5] = 1.0f / (tanf(radians(FOV / 2.0f)));
	mat[10] = -(far + near) / (far - near); //(-near - far) / (near - far);
	mat[11] = -(2.0f * near * far) / (far - near);
	mat[14] = -1.0f;
	mat[15] = 0.0f;
*/

	const float a = 1.0f / tan(radians(FOV / 2.0f));

	identity(mat);

	mat[0] = a / aspectRatio;
	mat[5] = a;
	mat[10] = -((far + near) / (far - near));
	mat[11] = -1.0f; //
	mat[14] = -((2.0f * far * near) / (far - near));//
	mat[15] = 0.0f;
}

float radians(float x)
{
	return x * 0.0174533f;
}

