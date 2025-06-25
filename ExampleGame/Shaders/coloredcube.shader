#shader vertex
#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in mat4 transformation;
layout(location = 5) in vec4 color;


out vec4 v_color;

uniform mat4 u_camera;

void main()
{
    v_color = color;
    vec4 vector = vec4(vertex, 1.0);

    gl_Position = u_camera * transformation * vector;
}


#shader fragment
#version 330 core

out vec4 FragColor;
in vec4 v_color;

void main()
{
    FragColor = v_color;
    //FragColor = vec4(7.0f, 0.5f, 0.8f, 1.0f);
};