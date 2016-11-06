#version 130

uniform sampler2D textureSampler;

in vec2 UV;
in vec4 colour;

void main()
{
	gl_FragColor = colour * texture(textureSampler, UV);
}

