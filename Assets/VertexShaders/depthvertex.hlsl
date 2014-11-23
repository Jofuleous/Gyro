float4x4 g_world : register( c[0] );
float4x4 g_viewProjection : register( c[4] );

void main( in const float3 i_position : POSITION, in const float2 i_texUV : TEXCOORD0,
	in const float3 i_normal : NORMAL, in const float3 i_tangent: TANGENT, in const float3 i_biNormal: BINORMAL,
	out float4 o_position : POSITION, out float4 o_viewPosition : TEXCOORD0 )
{
	o_position = mul( float4( i_position, 1.0 ), g_world );
	o_position.w = 1.0;
	o_position = mul( o_position, g_viewProjection );
}
