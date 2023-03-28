#shader vertex
#version 330 core

layout(location = 0) in vec4 vertex;

out vec3 v_texcoord;

uniform mat4 u_camera;

void main()
{
    v_texcoord = vertex.xyz;
    vec4 pos = u_camera * vertex;
    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);
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