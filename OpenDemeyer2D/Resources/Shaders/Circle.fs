#version 330 core

in GS_OUT
{
	vec4 color;
	vec4 centerPos; //w is radius
} fs_in;

out vec4 color;

float SquareDistance(vec2 v0, vec2 v1)
{
	return (v0.x - v1.x) * (v0.x - v1.x) + (v0.y - v1.y) * (v0.y - v1.y);
}

void main()
{
	color = fs_in.color;

	//vec2 pixelCoord = gl_FragCoord.xy;
	//float radius = fs_in.centerPos.w;
	//
	//color = clamp(SquareDistance(pixelCoord, fs_in.centerPos.xy) - radius*radius,0,1) * fs_in.color;
}