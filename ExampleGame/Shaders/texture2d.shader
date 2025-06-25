#shader vertex
#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 position;
layout(location = 2) in float slot;

out float v_slot;
out vec2 v_texcoord;

uniform mat4 u_camera;

void main()
{
    v_slot = slot;
    v_texcoord = vertex.xy;
    gl_Position = u_camera * vec4(vertex + position, 1.0f);
}


#shader fragment
#version 330 core

in float v_slot;
in vec2 v_texcoord;
out vec4 FragColor;

uniform sampler u_textures[16];

void main()
{
    FragColor = texture(u_textures[int(v_slot)], v_texcoord);
    //FragColor = vec4(0.5, 0.8, 0.7, 1.0);
}