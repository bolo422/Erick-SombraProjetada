#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 5) out;

struct Rect
{
	vec3 topRight;
	vec3 topLeft;
    vec3 bottomRight;
    vec3 bottomLeft;
};

uniform Rect rect;

out vec2 TexCoords;

void main() {
	gl_Position = gl_in[0].gl_Position + vec4(rect.topRight, 1.0);
	TexCoords = vec2(1.0f, 1.0f);
    EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(rect.bottomRight, 1.0);
	TexCoords = vec2(1.0f, 0.0f);
    EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(rect.bottomLeft, 1.0);
	TexCoords = vec2(0.0f, 0.0f);
    EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(rect.topLeft, 1.0);
	TexCoords = vec2(0.0f, 1.0f);
    EmitVertex();
	
	gl_Position = gl_in[0].gl_Position + vec4(rect.topRight, 1.0);
	TexCoords = vec2(1.0f, 1.0f);
    EmitVertex();

	EndPrimitive();
}