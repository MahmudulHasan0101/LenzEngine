#shader vertex
#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 position;

out vec3 v_texcoord;

uniform mat4 u_camera;

void main()
{
    v_texcoord = vertex;
    gl_Position = u_camera * vec4(vertex + position, 1.0f);
}


#shader fragment
#version 330 core

out vec4 FragColor;
in vec3 v_texcoord;

uniform samplerCube u_texture;

void main()
{
    FragColor = textureCube(u_texture, v_texcoord);
}