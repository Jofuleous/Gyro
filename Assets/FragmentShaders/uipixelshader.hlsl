sampler2D		g_diffuseMap;
uniform float g_alpha = 1.0;

void main( in const float2 i_texUV : TEXCOORD0, out float4 o_color : COLOR0 )
{
	o_color = tex2D( g_diffuseMap, i_texUV );
}