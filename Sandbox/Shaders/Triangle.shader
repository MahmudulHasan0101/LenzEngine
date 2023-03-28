#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec4 aColor;
layout(location = 2) in vec3 aPosition;
layout(location = 5) in int aTexSlot;

uniform mat4 uProjectionMatrix;

out vec3 v_crntPos;
out vec2 v_texPos;
out vec4 v_color;
flat out int v_texSlot;

void main()
{
	v_crntPos = vec3(vec4(aPos, 1.0f));
	v_texPos = aPos.xy;
	v_color = aColor;
	v_texSlot = aTexSlot;
	gl_Position = uProjectionMatrix * (vec4(aPos + aPosition, 1.0));
}



#shader fragment
#version 330 core

out vec4 FragColor;
in vec3 v_crntPos;
in vec4 v_color;
in vec2 v_texPos;
flat in int v_texSlot;


vec3 lightPos = vec3(0.3f, 0.7f, 0.5f);

uniform sampler2D uTexture[16];

void main()
{
	float ambient = 0.10f;
	float range = -ambient + 1.0f;

	float intencity = ambient + max(dot(v_crntPos, lightPos), 0.0f) * range;

	FragColor = texture(uTexture[v_texSlot], v_texPos);
	//FragColor = v_color * intencity;
}

#shader end