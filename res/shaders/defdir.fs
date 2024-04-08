#version 420 core
out vec4 FragColor;

uniform vec4 ourColor;
//uniform sampler2D texture1;
//uniform sampler2D texture2;
uniform sampler2D shadowMap;
uniform sampler2DArray texArray;

uniform float mixer;
uniform int texLayer;
uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightDirectional;

in vec2 TexCoord;
in vec3 Normal;  
in vec3 FragPos;
in vec4 FragPosLightSpace;


float ShadowCalculation(vec4 fpLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fpLightSpace.xyz / fpLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{
	vec3 lightPos = vec3(0,0,0); 
	vec3 lightDirection = vec3(-0.2f, -1.0f, -0.3f);
	vec3 lightColor = vec3(1,1,1);

	// Directional Light
	float ambientStrength = 0.3f;
    vec4 ambient = ambientStrength * vec4(lightColor,1.0) * texture(texArray, vec3(TexCoord.x, TexCoord.y, texLayer));
  	
    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightDirectional);
    float diff = max(dot(norm, lightDir), 0.0);
	vec4 diffuse = vec4(lightColor,1.0) * diff * texture(texArray, vec3(TexCoord.x, TexCoord.y, texLayer));
    
    // specular
    float specularStrength = 0.1;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec4 specular = specularStrength * spec * vec4(lightColor, 1.0) * texture(texArray, vec3(TexCoord.x, TexCoord.y, texLayer));

	float shadow = ShadowCalculation(FragPosLightSpace);
	vec4 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * ourColor;
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
