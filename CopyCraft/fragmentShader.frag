#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

// If this ever bugs out, it could be because texture is a reserved keyword
uniform sampler2D texture;

void main()
{
	FragColor = texture(texture, TexCoord);
}
