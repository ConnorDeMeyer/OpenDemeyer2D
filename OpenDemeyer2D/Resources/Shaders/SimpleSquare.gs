#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

uniform mat3 cameraTransform = mat3(1.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,1.0);

in VS_OUT
{
	mat3 transform;
	vec4 color;
} gs_in[];

out GS_OUT
{
	vec4 color;
	vec4 centerPos; //w is radius
} gs_out;

void main()
{
	// Set the color for all output vertices
	gs_out.color = gs_in[0].color;
	
	// Keep the center position of the square (original position)
	gs_out.centerPos = gl_in[0].gl_Position;
	
	gl_Position = vec4(cameraTransform * (gs_in[0].transform * vec3(gs_out.centerPos.xy + vec2(-1,-1), 1)), 1);
	EmitVertex();
	
	gl_Position = vec4(cameraTransform * (gs_in[0].transform * vec3(gs_out.centerPos.xy + vec2( 1,-1), 1)), 1);
	EmitVertex();
	
	gl_Position = vec4(cameraTransform * (gs_in[0].transform * vec3(gs_out.centerPos.xy + vec2(-1, 1), 1)), 1);
	EmitVertex();
	
	gl_Position = vec4(cameraTransform * (gs_in[0].transform * vec3(gs_out.centerPos.xy + vec2( 1, 1), 1)), 1);
	EmitVertex();
	
	EndPrimitive();
}