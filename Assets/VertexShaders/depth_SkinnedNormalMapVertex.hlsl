// This probably needs to be updated to take something besides the bind pose into account...for now, just don't bug out.
static const int MAX_MATRICES = 60;
float4x4 g_world : register( c[0] );
float4x4 g_viewProjection : register( c[4] );
float4x4 g_bones[MAX_MATRICES];

void main( in const float3 i_position : POSITION, in const float2 i_texUV : TEXCOORD0,
	in const float3 i_normal : NORMAL, in const float3 i_tangent: TANGENT, in const float3 i_biNormal: BINORMAL,
	in const float4 i_weights : BLENDWEIGHT0, in const uint4 i_boneIndices : BLENDINDICES0, out float4 o_position : POSITION, out float4 o_viewPosition : TEXCOORD0 )
{
	o_position = float4( i_position, 1.0 );
	o_position = i_weights[0] * mul(float4(i_position, 1.0f), g_bones[i_boneIndices[0]]);
	o_position += i_weights[1] * mul(float4(i_position, 1.0f), g_bones[i_boneIndices[1]]);
	o_position += i_weights[2] * mul(float4(i_position, 1.0f), g_bones[i_boneIndices[2]]);
	o_position += i_weights[3] * mul(float4(i_position, 1.0f), g_bones[i_boneIndices[3]]);
	o_position.w = 1.0;
	o_position = mul( o_position, g_world );
	o_position.w = 1.0;
	o_position = mul( o_position, g_viewProjection );
}
