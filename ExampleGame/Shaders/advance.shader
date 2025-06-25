#shader vertex
#version 330 core

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_texCoord;
layout(location = 3) in vec3 a_tangent;
layout(location = 4) in vec3 a_bitangent;
layout(location = 5) in mat4 a_model;

uniform mat4 u_camera; // View * Projection matrix

out vec2 v_texCoord;
out vec3 v_fragPos;
out mat3 v_TBN;

void main() {
    vec4 worldPosition = a_model * vec4(a_position, 1.0);
    gl_Position = u_camera * worldPosition;

    v_fragPos = worldPosition.xyz;
    v_texCoord = a_texCoord;

    // Transform TBN vectors to world space
    vec3 T = normalize(mat3(a_model) * a_tangent);
    vec3 B = normalize(mat3(a_model) * a_bitangent);
    vec3 N = normalize(mat3(a_model) * a_normal);
    v_TBN = mat3(T, B, N);
}




#shader fragment
#version 330 core

in vec2 v_texCoord;
in vec3 v_fragPos;
in mat3 v_TBN;

out vec4 FragColor;

uniform sampler2D u_diffuse;
uniform sampler2D u_normalMap;
uniform vec3 u_lightPos = vec3(100,100,100);
uniform vec3 u_cameraPos;

void main() {
    // Sample normal map and transform to world space
    vec3 normalMap = texture(u_normalMap, v_texCoord).rgb;
    vec3 normal = normalize(v_TBN * (normalMap * 2.0 - 1.0));

    // Lighting
    vec3 lightDir = normalize(u_lightPos - v_fragPos);
    vec3 viewDir = normalize(u_cameraPos - v_fragPos);
    vec3 reflectDir = reflect(-lightDir, normal);

    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * vec3(1.0);

    vec3 baseColor = texture(u_diffuse, v_texCoord).rgb;
    FragColor = vec4(baseColor * diffuse, 1.0);
}
