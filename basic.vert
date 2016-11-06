#version 130

uniform mat4 modelMatrix;
uniform mat4 projection;
uniform mat4 viewMatrix;

uniform mat4 modelXRotationMatrix;
uniform mat4 modelYRotationMatrix;

in vec3 position;
in vec3 normal;
in vec2 vertexUV;

out vec2 UV;
out vec4 colour;

void main()
{
	UV = vertexUV;
	colour = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	gl_Position = projection * viewMatrix * modelMatrix * vec4(position, 1.0f);
}

