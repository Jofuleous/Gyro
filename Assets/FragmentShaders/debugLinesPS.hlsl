struct VS_OUT
{
	float4 position			: POSITION;
	float4 color			: COLOR0;
};


float4 main( in VS_OUT In ) : COLOR0
{
	return In.color;
}