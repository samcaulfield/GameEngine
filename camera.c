#include "camera.h"
#include "math.h"

void getViewMatrix(struct Camera *camera)
{
	float rotX[16] = {0}, rotY[16] = {0}, translation[16] = {0};
	loadXRotation(-radians(camera->rx), rotX);
	loadYRotation(-radians(camera->ry), rotY);
	loadTranslation(-camera->x, -camera->y, -camera->y, translation);
}

