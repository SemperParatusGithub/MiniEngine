#shader vertex
#version 450 core

layout(location = 0) in vec3 a_Position;

out vec3 v_TexCoords;

uniform mat4 u_Projection;
uniform mat4 u_View;

void main()
{
    v_TexCoords = a_Position;

    vec4 position = u_Projection * u_View * vec4(a_Position, 1.0);
    gl_Position = position.xyww;
}


#shader fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
in vec3 v_TexCoords;
uniform float u_TextureLod;

uniform samplerCube u_ImageCube;

void main()
{    
    o_Color = textureLod(u_ImageCube, v_TexCoords, u_TextureLod);
}