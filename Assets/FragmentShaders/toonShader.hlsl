uniform float g_secondsElapsed = 1.0;

//directional
//uniform float4 g_dirLightColor;
uniform float4 g_dirLightDir;
//point light 1
uniform float3 g_lightPosition;
uniform float4 g_lightParms;

uniform float g_alpha = 1.0;

sampler2D g_diffuseMap;

sampler2D g_shadowMap;
uniform float4x4 g_view;
uniform float4x4 g_projection;

void main( in const float2 i_texUV : TEXCOORD0, in const float3 i_normal: NORMAL, in const float3 i_tangent: TANGENT, in const float3 i_biNormal: BINORMAL,
	in const float3 i_fragmentPosition : TEXCOORD1, out float4 o_color : COLOR0 )
{
	float4 texSample = tex2D( g_diffuseMap, i_texUV );
	float4 texColor = texSample;

	float3 normal = normalize( i_normal );
	float3 lightDirection = normalize(g_dirLightDir);
	float cosine = dot( -lightDirection, normal );
	cosine = saturate( cosine );
	float l_one = step( cosine, 0.1 );
	cosine = 1 - (0.3 * l_one);
	float3 lightColor = g_lightParms.xyz * cosine;

	//shadow map lookup
	float shadowModifier = 1;
	if( cosine >= 0.99 )
	{
		float4 view = mul( float4(i_fragmentPosition, 1), g_view );
		float4 proj = mul( view, g_projection );
		float2 coord = proj.xy;
		coord = coord / proj.w;

		coord = ( coord * 0.5 ) + 0.5;
		coord.y = 1 - coord.y;
		float z = tex2D( g_shadowMap, coord );
		float difference = proj.z - z; //if z > view.z, then this fragment is in shadow
		shadowModifier = 1 - ceil( difference ) * 0.3;
	}
	
	o_color = texColor * float4(lightColor, 1) * shadowModifier; //- shadowModifier;
	o_color = saturate( o_color );
	
	o_color.a = o_color.a * g_alpha;
}
