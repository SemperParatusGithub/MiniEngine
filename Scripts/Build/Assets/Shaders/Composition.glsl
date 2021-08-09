#shader vertex
#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = vec4(a_Position.xy, 0.0, 1.0);;
}

#shader fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

in vec2 v_TexCoord;

uniform sampler2DMS u_Texture;
uniform int u_TextureSamples;

uniform bool u_EnableTonemapping;
uniform float u_Exposure;

vec4 SampleTexture(sampler2D tex, vec2 texCoord)
{
    return texture(tex, texCoord);
}

vec4 MultiSampleTexture(sampler2DMS tex, vec2 tc)
{
	ivec2 texSize = textureSize(tex);
	ivec2 texCoord = ivec2(tc * texSize);
	vec4 result = vec4(0.0);
    for (int i = 0; i < u_TextureSamples; i++)
        result += texelFetch(tex, texCoord, i);

    result /= float(u_TextureSamples);
    return result;
}

void main()
{
	const float gamma     = 2.2;
	const float pureWhite = 1.0;

	// Tonemapping
	vec4 msColor = MultiSampleTexture(u_Texture, v_TexCoord);

	vec3 color = msColor.rgb;

	vec3 mapped;

	if(u_EnableTonemapping)
		mapped = vec3(1.0) - exp(-color * u_Exposure);
	else
		mapped = color * u_Exposure;

	// Gamma correction.
	o_Color = vec4(pow(mapped, vec3(1.0 / gamma)), 1.0);
}