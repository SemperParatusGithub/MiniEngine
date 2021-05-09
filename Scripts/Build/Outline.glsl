#shader vertex
#version 450

layout(location = 0) in vec3 a_Position;

uniform mat4 u_ProjectionView;
uniform mat4 u_Transform;

void main()
{
	gl_Position = u_ProjectionView * u_Transform * vec4(a_Position, 1.0);
}

#shader fragment
#version 450

layout(location = 0) out vec4 o_Color;

void main()
{
	o_Color = vec4(0.26, 0.59, 0.98, 1.0);
}