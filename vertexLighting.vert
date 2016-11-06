#version 130

uniform mat4 modelMatrix;
uniform mat4 projection;
uniform mat4 viewMatrix;
uniform mat4 modelXRotationMatrix;
uniform mat4 modelYRotationMatrix;

uniform vec4 lightPosition;
uniform vec4 lightColour;
uniform float lightIntensity;

in vec3 position;
in vec3 normal;
in vec2 vertexUV;

out vec2 UV;
out vec4 colour;

float angleBetween(vec4 a, vec4 b)
{
	float n = dot(a, b);
	float d = length(a) * length(b);
	return degrees(acos(n / d));
}

void main()
{
	// transform from local space to world space
	vec4 worldPosition = modelMatrix * vec4(position, 1.0f);
	// no need to translate or scale normal
	vec4 worldNormal = modelYRotationMatrix * modelXRotationMatrix * vec4(normal, 0.0f);

	// not sure if normalisation is necessary
	vec4 lightToVertexRay = worldPosition - lightPosition;

	// default brightness is ambient term
	float energy = 0.5f;

	// compute diffuse
	if (dot(lightToVertexRay, worldNormal) <= 0) {
		float brightness = angleBetween(lightToVertexRay, worldNormal) / 90.0f;

		// compute attenuation (linear)
		float attenuation = 1.0f / min((0.1f * distance(worldPosition, lightPosition)), 1.0f);

		energy = max(attenuation * lightIntensity * brightness, 1.0f);
	}

	colour = energy * lightColour;
	UV = vertexUV;
	gl_Position = projection * viewMatrix * modelMatrix * vec4(position, 1.0f);
}

