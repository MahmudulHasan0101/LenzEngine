#shader vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in mat4 a_model; 

uniform mat4 u_camera; // View * Projection matrix

out vec3 v_normal;
out vec3 v_fragPos;
out vec2 v_texCoord;

void main() {
    vec4 worldPosition =  vec4(a_position, 1.0);
    gl_Position = u_camera * worldPosition;

    v_fragPos = worldPosition.xyz;
    v_normal = normalize(mat3(1.0) * a_normal);
    v_texCoord = a_texCoord;
}




#shader fragment
#version 330 core

in vec3 v_normal;
in vec3 v_fragPos;
in vec2 v_texCoord;

out vec4 FragColor;

uniform sampler2D u_diffuse;
uniform vec3 u_lightPos = vec3(100,100,100);

void main() {
    vec3 normal = normalize(v_normal);
    vec3 lightDir = normalize(u_lightPos - v_fragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * texture(u_diffuse, v_texCoord).rgb;

    FragColor = vec4(diffuse, 1.0);
}
