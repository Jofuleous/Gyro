float4x4 g_world : register( c[0] );
float4x4 g_viewProjection : register( c[4] );

void main( in const float3 i_position : POSITION, in const float2 i_texUV : TEXCOORD0,
	in const float3 i_normal : NORMAL, in const float3 i_tangent: TANGENT, in const float3 i_biNormal: BINORMAL,
	out float4 o_position : POSITION, out float2 o_texUV : TEXCOORD0,
	out float3 o_normalWorld : NORMAL, out float3 o_tangent: TANGENT, out float3 o_biNormal: BINORMAL,
	out float3 o_fragmentPosition : TEXCOORD1, out float4 o_fragScreenPosition : TEXCOORD2, out float4 o_viewPosition : TEXCOORD3 )
{
	o_position = mul( float4( i_position, 1.0 ), g_world );
	o_position.w = 1.0;
	float3 worldPosition = float3( o_position.xyz );
	o_position = mul( o_position, g_viewProjection );

	float3x3 worldRotation;
	worldRotation[0] = g_world[0].xyz;
	worldRotation[1] = g_world[1].xyz;
	worldRotation[2] = g_world[2].xyz;

	o_normalWorld = mul( i_normal, worldRotation );
	o_tangent = mul( i_tangent, worldRotation );
	o_biNormal = mul( i_biNormal, worldRotation );
	o_texUV = i_texUV;
	o_fragmentPosition = worldPosition;
	o_fragScreenPosition = o_position;
}
