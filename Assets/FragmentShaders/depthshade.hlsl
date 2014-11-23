float g_farPlaneDistance;

void main( in const float4 i_viewPosition : TEXCOORD0, out float4 o_color : COLOR0 )
{
	//float depth = o_viewPosition.z / g_farPlaneDistance;
	o_color = float4( i_viewPosition.z/g_farPlaneDistance, 0.0, 0.0,1.0 );
}