#version 330 core
out vec4 FragColor;

//in vec3 fColor;
in vec2 TexCoords;

// texture samplers
uniform sampler2D tex;

void main()
{
	FragColor = vec4(texture(tex, 1-TexCoords));
	if (FragColor.a < 1.0)
	{
		discard;
	}
	//FragColor = vec4(0.0, 1.0, 1.0, 1.0);
}