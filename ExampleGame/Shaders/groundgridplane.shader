#shader vertex
#version 330 core
layout(location = 0) in vec2 aPos; // -1 to 1 range

out vec2 v_UV;

void main() {
    v_UV = aPos * 0.5 + 0.5;
    gl_Position = vec4(aPos, 0.0, 1.0);
}




#shader fragment
// raymarch_grid.frag
#version 330 core
in vec2 v_UV;
out vec4 FragColor;

uniform mat4 u_invcamera;
uniform vec3 u_camPos;

uniform float u_GroundY = -15.0;
uniform float u_GridSpacing = 2.0;
uniform float u_LineThickness = 0.05;
uniform vec3 u_GridColor = vec3(0.95);
uniform vec3 u_BackgroundColor = vec3(0.3);
uniform float u_GridFadeDist = 200.0;


float cbrt_fast(float x) {
    return sign(x) * exp(log(abs(x)) / 5.0);
}


void main() {
    // Convert screen UV to NDC
    vec2 ndc = v_UV * 2.0 - 1.0;
    vec4 nearPoint = u_invcamera * vec4(ndc, 0.0, 1.0);
    vec4 farPoint = u_invcamera * vec4(ndc, 1.0, 1.0);

    // Perspective divide
    nearPoint /= nearPoint.w;
    farPoint /= farPoint.w;

    // Ray direction
    vec3 rayDir = normalize(farPoint.xyz - nearPoint.xyz);

    // Intersect with ground plane at y = 0
    float t = (u_GroundY - u_camPos.y) / rayDir.y;
    if (t < 0.0) {
    discard;
    }
    vec3 hit = u_camPos + rayDir * t;
    


    float gridFade = clamp(1.0 - cbrt_fast(distance(u_camPos.xz, hit.xz) / u_GridFadeDist), 0.1, 1.0);

    vec2 gridUV = hit.xz / u_GridSpacing;
    vec2 grid = abs(fract(gridUV - 0.5) - 0.5) / fwidth(gridUV);
    float line = min(grid.x, grid.y);
    float gridLine = 1.0 - min(line, 1.0);

    FragColor = vec4(vec3(gridLine)*gridFade, 0.5);
}
