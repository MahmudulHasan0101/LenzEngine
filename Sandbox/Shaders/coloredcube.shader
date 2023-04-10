#shader vertex
#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 position;
layout(location = 2) in vec4 color;

out vec4 v_color;

uniform mat4 u_camera;

void main()
{
    v_color = color;
    gl_Position = u_camera * vec4(vertex + position, 1.0f);
}


#shader fragment
#version 330 core

out vec4 FragColor;
in vec4 v_color;

void main()
{
    FragColor = v_color;
    //FragColor = vec4(7.0f, 0.5f, 0.8f, 1.0f);
}