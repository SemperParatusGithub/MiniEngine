#shader vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

out vec2 v_TexCoords;

uniform mat4 u_ProjectionView;

void main()
{
	v_TexCoords = a_TexCoords;
	gl_Position = u_ProjectionView * vec4(a_Position, 1.0);
}

#shader fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

in vec2 v_TexCoords;
uniform sampler2D u_Texture;

void main()
{
	o_Color = texture(u_Texture, v_TexCoords.xy);
}