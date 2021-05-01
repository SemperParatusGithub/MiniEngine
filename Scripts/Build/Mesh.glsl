#shader vertex
#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormal;
layout(location = 4) in vec2 a_TexCoord;

uniform mat4 u_ProjectionView;
uniform mat4 u_Transform;


out vec2 v_TexCoords;


void main()
{
	v_TexCoords = vec2(a_TexCoord.x, a_TexCoord.y);

	gl_Position = u_ProjectionView * u_Transform * vec4(a_Position, 1.0);
}

#shader fragment
#version 430 core

layout(location = 0) out vec4 o_Color;

uniform bool u_UseAlbedoMap;
uniform sampler2D u_AlbedoMap;


in vec2 v_TexCoords;


void main()
{
	if(u_UseAlbedoMap)
		o_Color = texture(u_AlbedoMap, v_TexCoords.xy);
	else
		o_Color = vec4(0.2, 0.4, 0.8, 1.0);
	//o_Color = vec4(v_TexCoords.xy, 0.0, 1.0);
}