#version 460 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float alpha;
void main()
{
	//FragColor = vec4(1.0);
	FragColor = texture(texture2, TexCoord);
	//FragColor = mix(texture(texture1, TexCoord),
	//				texture(texture2, TexCoord),alpha);
};
