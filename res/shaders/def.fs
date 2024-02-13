#version 420 core
out vec4 FragColor;

uniform vec4 ourColor;
//uniform sampler2D texture1;
//uniform sampler2D texture2;
uniform sampler2DArray texArray;
uniform float mixer;
uniform int texLayer;

in vec2 TexCoord;

void main()
{
	//FragColor = mix(texture(texture1,TexCoord), texture(texture2, vec2(TexCoord.x, TexCoord.y)), mixer) * ourColor;
	vec4 texColor = texture(texArray, vec3(TexCoord.x, TexCoord.y, texLayer)) * ourColor;
	if (texColor.a < 0.1)
	{
		discard;
	}
	FragColor = texColor;
	//FragColor = texture(texture1, TexCoord) * ourColor;
	//FragColor = ourColor;
}
