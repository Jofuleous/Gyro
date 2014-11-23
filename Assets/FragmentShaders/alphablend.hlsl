//I'm aware that the base diffuse shader should not contain shadow mapping, but meh...I'm way too behind with this assignment already.

uniform float g_secondsElapsed = 1.0;

//ambient light
uniform float3 g_ambientLight;

uniform float g_alpha = 1.0;

sampler2D g_diffuseMap;

sampler2D g_shadowMap;
uniform float4x4 g_view;
uniform float4x4 g_projection;

void main( in const float2 i_texUV : TEXCOORD0, in const float3 i_normal: NORMAL, in const float3 i_tangent: TANGENT, in const float3 i_biNormal: BINORMAL,
	in const float3 i_fragmentPosition : TEXCOORD1, out float4 o_color : COLOR0 )
{
	float4 texSample = tex2D( g_diffuseMap, i_texUV );
	float4 texColor = texSample;


	o_color = texColor;
	o_color = saturate( o_color );
	
	o_color.a = o_color.a * g_alpha;
}
