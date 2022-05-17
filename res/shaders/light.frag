#version 430 core

#define NR_POINT_LIGHTS 1
#define NR_SPOT_LIGHTS 2

out vec4 FragColor;

struct BaseLight
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Attenuation
{
    float constant;
    float linear;
    float quadratic;

};

float calcAttenuation(Attenuation att, float distance)
{
    return ( 1.0f / ( att.constant + att.linear * distance + att.quadratic * (distance * distance) ) );
}

struct DirLight
{
    bool isActive;
	
    vec3 direction;
	
    BaseLight colors;
};

struct PointLight
{    
    bool isActive;

	vec3 position;

    Attenuation att;
    BaseLight colors;
};

struct SpotLight
{
    bool isActive;

    vec3 position;
    vec3 direction;

    float cutOff;
    float outerCutOff;

    Attenuation att;
    BaseLight colors;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 viewPos;

uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotLights[NR_SPOT_LIGHTS];

//MATERIAL
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform float shininess;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);  
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{	
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

	vec3 result = vec3(0.0);

	for(int i = 0; i < NR_POINT_LIGHTS; i++)
	{
        if(pointLights[i].isActive)
		    result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
	}

	for(int i = 0; i < NR_SPOT_LIGHTS; i++)
	{
        if(spotLights[i].isActive)
		   result += CalcSpotLight(spotLights[i], norm, FragPos, viewDir);
	}

    if(dirLight.isActive)
        result += CalcDirLight(dirLight, norm, viewDir);

 
    FragColor = vec4(result, 1.0);
} 

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    
    // combine results
    vec3 ambient = light.colors.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.colors.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.colors.specular * spec * vec3(texture(texture_specular1, TexCoords));

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = calcAttenuation(light.att, distance);//1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.colors.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.colors.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.colors.specular * spec * vec3(texture(texture_specular1, TexCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);

}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = calcAttenuation(light.att, distance);//1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.colors.ambient * vec3(texture(texture_diffuse1, TexCoords));
    vec3 diffuse = light.colors.diffuse * diff * vec3(texture(texture_diffuse1, TexCoords));
    vec3 specular = light.colors.specular * spec * vec3(texture(texture_specular1, TexCoords));
    
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}