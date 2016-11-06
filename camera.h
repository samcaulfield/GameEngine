#ifndef CAMERA_H
#define CAMERA_H

struct Camera {
	float x, y, z;
	float rx, ry; /* x and y axis rotation in degrees */
	float height;

	float movementSpeed, rotationSpeed; /* speed in m/s and rotation speed in degress / sec */
};

void getViewMatrix(struct Camera *camera);

#endif

