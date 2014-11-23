uniform float g_secondsElapsed = 1.0;

//directional
uniform float4 g_dirLightColor;
uniform float4 g_dirLightDir;
//point light 1
uniform float3 g_lightPosition;
uniform float4 g_lightParms;
uniform float  g_lightIntensity;
//ambient light
uniform float3 g_ambientLight;

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
	float3 lightDirection = normalize(g_lightPosition - i_fragmentPosition);
	float cosine = dot( lightDirection, normal );
	cosine = saturate( cosine );
	float3 lightColor = g_lightParms.xyz * g_lightParms.w * cosine;

	//accounting for attenuation of the point light + intensity.
	float d = distance( g_lightPosition, i_fragmentPosition );
	float radius = g_lightParms.w;
	float attenuation = (1/g_lightIntensity) / (pow(((d/radius) + 1), 2));
	lightColor *= attenuation;

	lightColor += g_ambientLight;

	//directional light
	//g_dirLightDir = float4(0, 1, 0, 1);
	cosine = dot( -g_dirLightDir.xyz, normal );
	float3 directionalLightColor = g_dirLightColor.xyz * cosine * g_dirLightColor.w;
	lightColor += directionalLightColor;

	//shadow map lookup
	float4 view = mul( float4(i_fragmentPosition, 1), g_view );
	float4 proj = mul( view, g_projection );
	float2 coord = proj.xy;
	float shadowModifier = 0;
	coord = coord / proj.w;

		coord = ( coord * 0.5 ) + 0.5;
		coord.y = 1 - coord.y;
		float z = tex2D( g_shadowMap, coord );
		float difference = proj.z - z; //if z > view.z, then this fragment is in shadow
		shadowModifier = ceil( difference ) * 0.08;


	o_color = texColor * float4(lightColor, 1) - shadowModifier;
	o_color = saturate( o_color );
	
	o_color.a = o_color.a * g_alpha;
}
