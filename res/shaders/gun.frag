#version 430 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;

uniform samplerCube skybox;
uniform vec3 cameraPos;

uniform bool isReflective;
uniform float refractiveRatio;

void main()
{
    vec3 I = normalize(FragPos - cameraPos);
    vec3 R = vec3(0.0f);
    
    if(isReflective)
    {
        R = reflect(I, normalize(Normal));
    }
    else
    {
        R = refract(I, normalize(Normal), refractiveRatio);
    }

    FragColor = vec4( texture(skybox, R).rgb, 1.0f);

}