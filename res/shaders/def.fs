#version 420 core
out vec4 FragColor;

uniform vec4 ourColor;
//uniform sampler2D texture1;
//uniform sampler2D texture2;
uniform sampler2DArray texArray;
uniform float mixer;
uniform int texLayer;
uniform vec3 viewPos;

in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos; 

void main()
{
	vec3 lightPos = vec3(0,0,0); 
	//vec3 viewPos = vec3(0,0,0);
	vec3 lightColor = vec3(1,1,1);
	//vec3 objectColor = vec3(ourColor);

	// Point light stuff
	float distance    = length(lightPos - FragPos);
	float attenuation = 1.0 / (1.0f + 0.022f * distance + 
    		    		0.0019f * (distance * distance));

	float ambientStrength = 0.3f;
    vec4 ambient = ambientStrength * vec4(lightColor,1.0);
	ambient = attenuation * ambient * texture(texArray, vec3(TexCoord.x, TexCoord.y, texLayer));
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    //vec3 diffuse = diff * lightColor;
	vec4 diffuse = vec4(lightColor,1.0) * diff * texture(texArray, vec3(TexCoord.x, TexCoord.y, texLayer)) * attenuation;
    
    // specular
    float specularStrength = 0.1;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor * attenuation;
	vec4 result = (ambient + diffuse + vec4(specular,0.0)) * ourColor;
	//FragColor = mix(texture(texture1,TexCoord), texture(texture2, vec2(TexCoord.x, TexCoord.y)), mixer) * ourColor;
	vec4 texColor = result;//texture(texArray, vec3(TexCoord.x, TexCoord.y, texLayer)) * vec4(result, 1.0);
	if (texColor.a < 0.1)
	{
		discard;
	}
	FragColor = texColor;
	//FragColor = texture(texture1, TexCoord) * ourColor;
	//FragColor = ourColor;
}
