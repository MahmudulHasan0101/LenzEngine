#shader vertex
#version 330 core

layout(location = 0) in vec4 vertex;

out vec3 v_texcoord;

uniform mat4 u_camera;

void main()
{
    v_texcoord = vec3(vertex.x, vertex.y, -vertex.z);;
    vec4 pos = u_camera * vertex;
    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
}


#shader fragment
#version 330 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;
in vec3 v_texcoord;

struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

const int MAX_LIGHTS = 10;
uniform Light lights[MAX_LIGHTS];
uniform int numLights;

uniform samplerCube u_texture;

void main()
{
    gPosition = vec3(2147483647.0,2147483647.0,2147483647.0);
    gNormal = vec3(0.0,0.0,0.0);
    gAlbedoSpec =  vec4(texture(u_texture, v_texcoord).rgb, 0);
}