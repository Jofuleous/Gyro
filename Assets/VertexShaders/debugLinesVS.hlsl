float4x4 gWVP : register( vs_3_0, c[8] );

struct VS_LINE_IN
{
	float3 pos0 : POSITION0;
	float3 pos1 : POSITION1;
	float4 weights : TEXCOORD0;
	float startRadius : TEXCOORD1;
	float endRadius : TEXCOORD2;
	float aspect : TEXCOORD3;
	float4 color : COLOR0;
};

struct VS_OUT
{
	float4 position			: POSITION;
	float4 color			: COLOR0;
};

VS_OUT main( in VS_LINE_IN In )
{
	VS_OUT Output;

	// Transform the input points.
	float4 p0 = mul( float4( In.pos0, 1.0f ), gWVP );
	float4 p1 = mul( float4( In.pos1, 1.0f ), gWVP );
	
	// Warp transformed points by aspect ratio.
	float4 w0 = p0;
	float4 w1 = p1;
	w0.y /= In.aspect;
	w1.y /= In.aspect;

	// Calc vectors between points in screen space.
	float2  delta2 = w1.xy / w1.z - w0.xy / w0.z;
	float3  delta_p;

	delta_p.xy = delta2;
	delta_p.z = w1.z - w0.z;


	// Calc U
	float   len = length( delta2 );
	float3  U = delta_p / len;

	// Create V orthogonal to U.
	float3  V;
	V.x = U.y;
	V.y = -U.x;
	V.z = 0;

	// Calculate output position based on this
	// vertex's weights.
	Output.position = p0 * In.weights.x + p1 * In.weights.y;

	// Calc offset part of postion.
	float3 offset = U * In.weights.z + V * In.weights.w;

	// Apply line thickness.

	//offset.xy *= In.startRadius;
	offset.xy *= In.startRadius * In.weights.x + In.endRadius * In.weights.y;
	//offset.xy *= In.radius;

	// Unwarp by inverse of aspect ratio.
	offset.y *= In.aspect;

	// Undo perspective divide since the hardware will do it.
	Output.position.xy += offset.xy * Output.position.z;

	Output.color = In.color;

	return Output;
}