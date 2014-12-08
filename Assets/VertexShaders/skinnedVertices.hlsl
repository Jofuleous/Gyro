static const int MAX_MATRICES = 60;

float4x4 g_world : register( c[0] );
float4x4 g_viewProjection : register( c[4] );
float4x4 g_bones[MAX_MATRICES];

void main( in const float3 i_position : POSITION, in const float2 i_texUV : TEXCOORD0,
	in const float3 i_normal : NORMAL, in const float3 i_tangent: TANGENT, in const float3 i_biNormal: BINORMAL,
	in const float4 i_weights : BLENDWEIGHT0, in const uint4 i_boneIndices : BLENDINDICES0,
	out float4 o_position : POSITION, out float2 o_texUV : TEXCOORD0,
	out float3 o_normalWorld : NORMAL, out float3 o_tangent: TANGENT, out float3 o_biNormal: BINORMAL,
	out float3 o_fragmentPosition : TEXCOORD1, out float4 o_fragScreenPosition : TEXCOORD2, out float4 o_viewPosition : TEXCOORD3)
{
	uint indexZero = i_boneIndices[0];
	uint indexOne = i_boneIndices[1];
	uint indexTwo = i_boneIndices[2];
	uint indexThree = i_boneIndices[3];
	
	float4x4 matZero = g_bones[indexZero];
	float4x4 matOne = g_bones[indexOne];
	float4x4 matTwo = g_bones[indexTwo];
	float4x4 matThree = g_bones[indexThree];//
	
	float a_0 = g_bones[indexZero][0][0];
	float a_1 = g_bones[indexZero][0][1];
	float a_2 = g_bones[indexZero][0][2];
	float b_0 = g_bones[indexZero][1][0];
	float b_1 = g_bones[indexZero][1][1];
	float b_2 = g_bones[indexZero][1][2];
	float c_0 = g_bones[indexZero][2][0];
	float c_1 = g_bones[indexZero][2][1];
	float c_2 = g_bones[indexZero][2][2];
	float t_0 = g_bones[indexZero][3][0];
	float t_1 = g_bones[indexZero][3][1];
	float t_2 = g_bones[indexZero][3][2];
	float s_0 = g_bones[indexZero][0][3];
	float s_1 = g_bones[indexZero][1][3];
	float s_2 = g_bones[indexZero][2][3];
	float end = (a_0 + b_0 + c_0 + a_1 + b_1 + c_1 + a_2 + b_2 + c_2 + t_0 + t_1 + t_2 + s_0 + s_1 + s_2) * 0.001;
	
	o_position = float4( i_position, 1.0 );
	o_position = i_weights[0] * mul(float4(i_position, 1.0f), matZero);
	o_position += i_weights[1] * mul(float4(i_position, 1.0f), matOne);
	o_position += i_weights[2] * mul(float4(i_position, 1.0f), matTwo);
	o_position += i_weights[3] * mul(float4(i_position, 1.0f), matThree);
	
	o_position.w = 1.0;
	o_position = mul( float4(o_position.xyz, 1.0f), g_world );
	o_position.w = 1.0;
	float3 worldPosition = float3( o_position.xyz );
	o_position = mul( o_position, g_viewProjection );

	float3x3 worldRotation;
	worldRotation[0] = g_world[0].xyz;
	worldRotation[1] = g_world[1].xyz;
	worldRotation[2] = g_world[2].xyz;

	o_normalWorld = mul( i_normal, worldRotation ) + end;
	o_tangent = mul( i_tangent, worldRotation );
	o_biNormal = mul( i_biNormal, worldRotation );
	o_texUV = i_texUV;
	o_fragmentPosition = worldPosition;
	o_fragScreenPosition = o_position;
}
