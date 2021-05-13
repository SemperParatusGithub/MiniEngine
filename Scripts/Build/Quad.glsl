#shader vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

uniform mat4 u_ProjectionView;
uniform mat4 u_Transform;

out vec2 v_TexCoords;

void main()
{
	v_TexCoords = a_TexCoords;
	gl_Position = u_ProjectionView * u_Transform * vec4(a_Position, 1.0);
}

#shader fragment
#version 450

layout(location = 0) out vec4 o_Color;

in vec2 v_TexCoords;

uniform sampler2DMS u_Texture;

vec4 textureMultisample(sampler2DMS sampler, ivec2 coord)
{
    vec4 color = vec4(0.0);

    for (int i = 0; i < 8; i++)
        color += texelFetch(sampler, coord, i);

    color /= float(8);

    return color;
}

void main()
{
    ivec2 texSize = textureSize(u_Texture);
    ivec2 texCoord = ivec2(v_TexCoords * texSize);
    o_Color = textureMultisample(u_Texture, texCoord);
}