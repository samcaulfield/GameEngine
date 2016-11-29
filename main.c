#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "camera.h"
#include "file.h"
#include "light.h"
#include "maths.h"
#include "mesh.h"
#include "shader.h"
#include "terrain.h"
#include "time.h"

/* Globals needed by processEvents */
bool running = true;
struct Camera camera = {.x = 0.0f, .y = 0.0f, .z = 0.0f, .rx = 0.0f, .ry = 0.0f, .height = 1.5f,
	.movementSpeed = 1.0f, .rotationSpeed = 90.0f};
double time; /* time since last frame */
bool cameraMoved = true;
struct Terrain *g_terrain;
struct Mesh **g_meshes;
struct Mesh **g_skyboxMeshes;

void processEvents(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		running = false;
	}

	/* Camera translation */
	float xMove = 0, yMove = 0, zMove = 0;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		float moveVector[] = {0.0f, 0.0f, -camera.movementSpeed * time, 1.0f};
		vectorYRotate(camera.ry, moveVector);
		xMove += moveVector[0];
		yMove += moveVector[1];
		zMove += moveVector[2];
		cameraMoved = true;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		float moveVector[] = {-camera.movementSpeed * time, 0.0f, 0.0f, 1.0f};
		vectorYRotate(camera.ry, moveVector);
		xMove += moveVector[0];
		yMove += moveVector[1];
		zMove += moveVector[2];
		cameraMoved = true;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		float moveVector[] = {0.0f, 0.0f, camera.movementSpeed * time, 1.0f};
		vectorYRotate(camera.ry, moveVector);
		xMove += moveVector[0];
		yMove += moveVector[1];
		zMove += moveVector[2];
		cameraMoved = true;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		float moveVector[] = {camera.movementSpeed * time, 0.0f, 0.0f, 1.0f};
		vectorYRotate(camera.ry, moveVector);
		xMove += moveVector[0];
		yMove += moveVector[1];
		zMove += moveVector[2];
		cameraMoved = true;
	}

	if (cameraMoved) {
		camera.x += xMove; camera.y += yMove; camera.z += zMove;
		camera.y = terrainGetHeightAt(g_terrain, camera.x, camera.z) + camera.height;
		// move skybox with camera
		for (int i = 0; i < 6; i++) {
			g_skyboxMeshes[i]->x += xMove;
			g_skyboxMeshes[i]->y += yMove;
			g_skyboxMeshes[i]->z += zMove;
		}
	}

	/* Camera rotation */
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
		camera.ry -= camera.rotationSpeed * time;
		cameraMoved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		camera.rx += camera.rotationSpeed * time;
		cameraMoved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		camera.rx -= camera.rotationSpeed * time;
		cameraMoved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		camera.ry += camera.rotationSpeed * time;
		cameraMoved = true;
	}
}

int main(int argc, char **argv)
{
	// init opengl context
	int windowWidth = 900, windowHeight = 900;
	if (!glfwInit()) {
		fprintf(stderr, "Error initialising GLFW. Exiting.\n");
		return EXIT_FAILURE;
	}
	GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL", NULL, NULL);
	if (!window) {
		fprintf(stderr, "Error creating a window. Exiting.\n");
		glfwTerminate();
		return EXIT_FAILURE;
	}
	glfwMakeContextCurrent(window);

	GLenum GLEWResult;
	if ((GLEWResult = glewInit()) != GLEW_OK) {
		fprintf(stderr, "Error: %s. Exiting.\n", glewGetErrorString(GLEWResult));
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// init shaders
	GLuint basicProgram = getProgram("basic.frag", "basic.vert");
	if (!basicProgram) {
		fprintf(stderr, "Error creating basicProgram. Exiting.\n");
		glfwTerminate();
		return EXIT_FAILURE;
	}
	GLuint vertexLightingProgram = getProgram("basic.frag", "vertexLighting.vert");
	if (!vertexLightingProgram) {
		fprintf(stderr, "Error creating vertexLightingProgram. Exiting.\n");
		glDeleteProgram(basicProgram); glfwTerminate();
		return EXIT_FAILURE;
	}

	float viewMatrix[16];
	float projection[16] = {0};
	loadPerspective(projection, 0.1f, 1000.0f, 45, (float) windowWidth / windowHeight);

	glUseProgram(vertexLightingProgram);
	/* Load attribs */
	GLint positionAttribLocation = glGetAttribLocation(vertexLightingProgram, "position");
	GLint normalAttribLocation = glGetAttribLocation(vertexLightingProgram, "normal");
	GLint vertexUVAttribLocation = glGetAttribLocation(vertexLightingProgram, "vertexUV");
	/* Load uniforms */
	GLint projectionUniformLocation = glGetUniformLocation(vertexLightingProgram, "projection");
	GLint viewMatrixUniformLocation = glGetUniformLocation(vertexLightingProgram, "viewMatrix");
	GLint modelMatrixUniformLocation = glGetUniformLocation(vertexLightingProgram, "modelMatrix");
	GLint modelXRotationMatrixUniformLocation = glGetUniformLocation(vertexLightingProgram, "modelXRotationMatrix");
	GLint modelYRotationMatrixUniformLocation = glGetUniformLocation(vertexLightingProgram, "modelYRotationMatrix");
	GLint lightPositionUniformLocation = glGetUniformLocation(vertexLightingProgram, "lightPosition");
	GLint lightColourUniformLocation = glGetUniformLocation(vertexLightingProgram, "lightColour");
	GLint lightIntensityUniformLocation = glGetUniformLocation(vertexLightingProgram, "lightIntensity");
	glUniformMatrix4fv(projectionUniformLocation, 1, GL_FALSE, projection);

	const uint32_t terrainSize = 512;
	g_terrain = generateTerrain(terrainSize, positionAttribLocation, vertexUVAttribLocation,
		normalAttribLocation, "textures/slate128.png", 123, "heightmaps/pit.heightmap512.png", 1);
	if (!g_terrain) {
		fprintf(stderr, "Error creating terrain. Exiting.\n");
		glDeleteProgram(basicProgram); glDeleteProgram(vertexLightingProgram); glfwTerminate();
		return EXIT_FAILURE;
	}
	g_terrain->mesh->x = (g_terrain->scale * (float) terrainSize) / -2.0f;
	g_terrain->mesh->z = (g_terrain->scale * (float) terrainSize) / -2.0f;

	camera.y = terrainGetHeightAt(g_terrain, camera.x, camera.z) + camera.height;

	// set up light
	struct Light light = {
		.x = 0.0f, .y = 5.0f/*terrainGetHeightAt(g_terrain, 0.0f, 0.0f) + 10.0f*/, .z = 0.0f, .w = 1.0f,
		.r = 1.0f, .g = 1.0f, .b = 1.0f, .a = 1.0f, .intensity = 1.0f
	};
	glUniform4f(lightPositionUniformLocation, light.x, light.y, light.z, light.w);
	glUniform4f(lightColourUniformLocation, light.r, light.g, light.b, light.a);
	glUniform1f(lightIntensityUniformLocation, light.intensity);

	struct Mesh *meshes[] = {
		// ground
		square(0.0f, 0.0f, 0.0f, 0.0f, positionAttribLocation, vertexUVAttribLocation, normalAttribLocation,
			"textures/slate512.png"),
		// origin marker - 7
		pyramid(0.0f, terrainGetHeightAt(g_terrain, 0.0f, 0.0f), 0.0f, 1.0f, positionAttribLocation,
			vertexUVAttribLocation, normalAttribLocation, "textures/walnut512.png"),

		// objects
		cube(0.0f, 0.5f + terrainGetHeightAt(g_terrain, 0.0f, -7.0f), -7.0f, 1.0f, positionAttribLocation,
			vertexUVAttribLocation, normalAttribLocation, "textures/brick512.png"),
		cube(-5.0f, 0.5f + terrainGetHeightAt(g_terrain, -5.0f, -3.0f), -3.0f, 1.0f, positionAttribLocation,
			vertexUVAttribLocation, normalAttribLocation, "textures/brick512.png"),
		cube(-4.0f, 0.5f + terrainGetHeightAt(g_terrain, -4.0f, -5.0f), -5.0f, 1.0f, positionAttribLocation,
			vertexUVAttribLocation, normalAttribLocation, "textures/brick512.png"),
		pyramid(-2.0f, terrainGetHeightAt(g_terrain, -2.0f, 3.0f), 3.0f, 1.0f, positionAttribLocation,
			vertexUVAttribLocation, normalAttribLocation, "textures/stone512.png"),
		pyramid(-3.0f, terrainGetHeightAt(g_terrain, -3.0f, 3.0f), 3.0f, 1.0f, positionAttribLocation,
			vertexUVAttribLocation, normalAttribLocation, "textures/stone512.png"),
		pyramid(-4.0f, terrainGetHeightAt(g_terrain, -4.0f, 3.0f), 3.0f, 1.0f, positionAttribLocation,
			vertexUVAttribLocation, normalAttribLocation, "textures/stone512.png"),
		// light
//		cube(light.x, light.y, light.z, 1.0f, positionAttribLocation,
//			vertexUVAttribLocation, normalAttribLocation, "textures/lightning128.png")
	};
	// rotate ground so it's flat
	meshes[0]->rx = 90.0f;

	glUseProgram(basicProgram);
	// load attribs
	GLint basicPositionAttribLocation = glGetAttribLocation(basicProgram, "position");
	GLint basicNormalAttribLocation = glGetAttribLocation(basicProgram, "normal");
	GLint basicVertexUVAttribLocation = glGetAttribLocation(basicProgram, "vertexUV");
	// load uniforms
	GLint basicProjectionUniformLocation = glGetUniformLocation(basicProgram, "projection");
	GLint basicViewMatrixUniformLocation = glGetUniformLocation(basicProgram, "viewMatrix");
	GLint basicModelMatrixUniformLocation = glGetUniformLocation(basicProgram, "modelMatrix");
	GLint basicModelXRotationMatrixUniformLocation = glGetUniformLocation(basicProgram, "modelXRotationMatrix");
	GLint basicModelYRotationMatrixUniformLocation = glGetUniformLocation(basicProgram, "modelYRotationMatrix");
	glUniformMatrix4fv(basicProjectionUniformLocation, 1, GL_FALSE, projection);

	struct Mesh *skyboxMeshes[] = {
		// skybox
		// front 1
		square(0.0f, 0.0f, -500.0f, 1000.0f, basicPositionAttribLocation, basicVertexUVAttribLocation, basicNormalAttribLocation,
			"skyboxes/bluecloud/bluecloud_bk.jpg"),
		// back 2
		square(0.0f, 0.0f, 500.0f, 1000.0f, basicPositionAttribLocation, basicVertexUVAttribLocation, basicNormalAttribLocation,
			"skyboxes/bluecloud/bluecloud_ft.jpg"),
		// left 3
		square(-500.0f, 0.0f, 0.0f, 1000.0f, basicPositionAttribLocation, basicVertexUVAttribLocation, basicNormalAttribLocation,
			"skyboxes/bluecloud/bluecloud_lf.jpg"),
		// right 4
		square(500.0f, 0.0f, 0.0f, 1000.0f, basicPositionAttribLocation, basicVertexUVAttribLocation, basicNormalAttribLocation,
			"skyboxes/bluecloud/bluecloud_rt.jpg"),
		// top 5
		square(0.0f, 500.0f, 0.0f, 1000.0f, basicPositionAttribLocation, basicVertexUVAttribLocation, basicNormalAttribLocation,
			"skyboxes/bluecloud/bluecloud_up.jpg"),
		//bottom 6
		square(0.0f, -500.0f, 0.0f, 1000.0f, basicPositionAttribLocation, basicVertexUVAttribLocation, basicNormalAttribLocation,
			"skyboxes/bluecloud/bluecloud_dn.jpg")
	};
	// rotate skybox meshes so they face inwards
	skyboxMeshes[1]->ry = 180.0f;
	skyboxMeshes[2]->ry = 270.0f;
	skyboxMeshes[3]->ry = 90.0f;
	skyboxMeshes[4]->rx = -90.0f; skyboxMeshes[4]->ry = 90.0f;
	skyboxMeshes[5]->rx = 90.0f; skyboxMeshes[5]->ry = 90.0f;

	g_skyboxMeshes = skyboxMeshes;
	g_meshes = meshes;


	glClearColor(0.0f, 0.6f, 0.8f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	uint32_t frame = 0;
	float totalTime = 0;
	const uint32_t frameRateUpdateInterval = 100;
	const char *titleFormat = "OpenGL - FPS = %.2f";
	uint32_t titleFormatLength = 1 + snprintf(NULL, 0, titleFormat, 111.11f);

	while (running && !glfwWindowShouldClose(window)) {
		/* Setup */
		time = getTimeSinceLastFrame();

		// rotate origin marker
//		meshes[7]->ry += 30.0f * time;

		frame++;
		totalTime += time;
		if (frame == frameRateUpdateInterval) {
			float FPS = frameRateUpdateInterval / totalTime;
			char title[titleFormatLength];
			snprintf(title, titleFormatLength, titleFormat, FPS);
			glfwSetWindowTitle(window, title);
			totalTime = 0;
			frame = 0;
		}

		if (cameraMoved) {
			loadXRotation(-camera.rx, viewMatrix);
			float temp[16];
			loadYRotation(-camera.ry, temp);
			MatrixMatrixMul(viewMatrix, temp);
			loadTranslation(-camera.x, -camera.y, -camera.z, temp);
			MatrixMatrixMul(viewMatrix, temp);
			cameraMoved = false;
		}

		/* Render */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(vertexLightingProgram);
		glUniformMatrix4fv(viewMatrixUniformLocation, 1, GL_TRUE, viewMatrix);

		drawMesh(g_terrain->mesh, modelMatrixUniformLocation, modelXRotationMatrixUniformLocation,
			modelYRotationMatrixUniformLocation);

//		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		for (int i = 0; i < sizeof(meshes) / sizeof(struct Mesh *); i++) {
			drawMesh(meshes[i], modelMatrixUniformLocation, modelXRotationMatrixUniformLocation,
				modelYRotationMatrixUniformLocation);
		}
//		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glUseProgram(basicProgram);
		glUniformMatrix4fv(basicViewMatrixUniformLocation, 1, GL_TRUE, viewMatrix);
		for (int i = 0; i < sizeof(skyboxMeshes) / sizeof(struct Mesh *); i++) {
			drawMesh(skyboxMeshes[i], basicModelMatrixUniformLocation, basicModelXRotationMatrixUniformLocation,
				basicModelYRotationMatrixUniformLocation);
		}

		glfwSwapBuffers(window);

		/* Process events */
		glfwPollEvents();
		processEvents(window);
	}

	// free resources

	// meshes and data structures
	for (int i = 0; i < sizeof(meshes) / sizeof(struct Mesh *); i++) {
		CleanupMesh(meshes[i]);
	}
	for (int i = 0; i < sizeof(skyboxMeshes) / sizeof(struct Mesh *); i++) {
		CleanupMesh(skyboxMeshes[i]);
	}
	cleanupTerrain(g_terrain);

	// shaders
	glDeleteProgram(basicProgram);
	glDeleteProgram(vertexLightingProgram);

	glfwTerminate();
	return EXIT_SUCCESS;
}

