#version 330 core

layout (location = 0) in mat3 transform;
layout (location = 3) in vec4 color;

// in VS_IN
// {
	// mat3 transform;
	// vec4 color;
// } vs_in;

out VS_OUT
{
	mat3 transform;
	vec4 color;
} vs_out;

void main()
{
	vs_out.transform = transform;
	vs_out.color = color;
	gl_Position = vec4(transform[0][2], transform[1][2], 1, 1);
}