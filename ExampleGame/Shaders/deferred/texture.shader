#shader vertex
#version 330 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texcoord;
layout(location = 4) in mat4 transformation;

out vec3 v_position;
out vec2 v_texcoord;
out vec3 v_normal;

uniform mat4 u_camera;

void main()
{
    v_texcoord = texcoord;
    v_normal = normal;
    v_position = v_position = vec3(transformation * vec4(vertex, 1.0));
    gl_Position = u_camera * (vec4(v_position, 1.0f) / 400);
}



#shader fragment
#version 330 core

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

in vec3 v_position;
in vec2 v_texcoord;
in vec3 v_normal;

uniform sampler2D u_texture;
uniform sampler2D u_specular;
uniform sampler2D u_depthbuffer;

void main()
{
    gPosition = v_position;
    gNormal = normalize(v_normal);
    gAlbedoSpec.rgb = texture(u_texture, v_texcoord).rbg;
    gAlbedoSpec.a = texture(u_specular, v_texcoord).r;
}