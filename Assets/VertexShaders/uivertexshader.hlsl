float4x4 g_world : register( c[0] );

void main( in const float3 i_position : POSITION, in const float2 i_texUV : TEXCOORD0,
	in const float3 i_normal : NORMAL, in const float3 i_tangent: TANGENT, in const float3 i_biNormal: BINORMAL,
	out float4 o_position : POSITION, out float2 o_texUV : TEXCOORD0 )
{
	o_position = mul( g_world, float4(i_position, 1.0f) );
	o_texUV = i_texUV;
}
