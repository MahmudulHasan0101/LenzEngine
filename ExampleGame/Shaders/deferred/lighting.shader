#shader vertex
#version 330 core

layout(location = 0) in vec2 vertex;
out vec2 TexCoords;

void main() {

    TexCoords = vec2((vertex.x+1)/2, (vertex.y+1)/2);
    gl_Position = vec4(vertex, 0, 1.0);
}


#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
  
uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
    vec3 position;
    vec3 color;
};

const int NR_LIGHTS =  5;
uniform Light lights[NR_LIGHTS];
uniform vec3 u_cameraPos;

void main()
{             
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    
    
    vec3 lighting = Albedo * 0.1;
    vec3 viewDir = normalize(u_cameraPos - FragPos);

    for(int i = 0; i < NR_LIGHTS; ++i)
    {
        vec3 lightDir = normalize(lights[i].position - FragPos);
        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Albedo * lights[i].color;
        lighting += diffuse;
    }
    
    FragColor = vec4(lighting, 1.0);
}  