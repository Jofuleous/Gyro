void main( in const float3 i_position : POSITION, in const float2 i_texUV : TEXCOORD0,
	in const float3 i_normal : NORMAL, in const float3 i_tangent: TANGENT, in const float3 i_biNormal: BINORMAL,
	out float4 o_position : POSITION, out float2 o_texUV : TEXCOORD0 )
{
	o_position = float4( i_position.x, i_position.y, 0.0, 1.0 );
	o_texUV = i_texUV;
}
