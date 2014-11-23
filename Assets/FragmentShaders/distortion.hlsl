float			g_Distortion = 10.0f;
sampler2D		g_postTexture;
float			g_secondsElapsed = 1.0;

void main( in const float2 i_texUV : TEXCOORD0, out float4 o_color : COLOR0 )
{

	float2 offset;
	offset.x =  sin( g_secondsElapsed + i_texUV.x * 20 ) / 200   ;
	offset.y =  sin( g_secondsElapsed * 5 + i_texUV.y * 50 ) / 200  ;
	o_color = tex2D( g_postTexture, float2( i_texUV.x + offset.x, i_texUV.y + offset.y ) );
}