void main( in const float2 i_position : POSITION, in const float4 i_color: COLOR0,
			out float4 o_position : POSITION, out float4 o_color : COLOR0 )
{
	o_position = float4( i_position.x, i_position.y, 0.0, 1.0 );
	o_color = i_color;
}