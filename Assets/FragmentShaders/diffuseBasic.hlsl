//directional
uniform float4 g_dirLightColor;
uniform float4 g_dirLightDir;
//point light 1
uniform float3 g_lightPosition;
uniform float4 g_lightParms; // color + radius
uniform float  g_lightIntensity;
//ambient light
uniform float3 g_ambientLight;

uniform float g_alpha = 1.0;

sampler2D g_diffuseMap;

void main( in const float2 i_texUV : TEXCOORD0, in const float3 i_normal: NORMAL, in const float3 i_tangent: TANGENT, in const float3 i_biNormal: BINORMAL,
	in const float3 i_fragmentPosition : TEXCOORD1, out float4 o_color : COLOR0 )
{
	float4 texSample = tex2D( g_diffuseMap, i_texUV );
	float4 texColor = texSample;

	// POINT LIGHT ONE
	float3 normal = normalize( i_normal );
	float3 lightDirection = normalize(g_lightPosition - i_fragmentPosition);
	float cosine = dot( lightDirection, normal );
	cosine = saturate( cosine );
	float3 lightColor = g_lightParms.xyz * cosine;

	//accounting for attenuation of the point light + intensity.
	float d = distance( g_lightPosition, i_fragmentPosition );
	float radius = g_lightParms.w;
	float attenuation = (1/g_lightIntensity) / (pow(((d/radius) + 1), 2));
	lightColor *= attenuation;

	//DIRECTIONAL LIGHT
	//g_dirLightDir = float4(0, 1, 0, 1);
	cosine = dot( -g_dirLightDir.xyz, normal );
	float3 directionalLightColor = g_dirLightColor.xyz * cosine * g_dirLightColor.w;
	lightColor += directionalLightColor;
	
	//AMBIENT
	lightColor += (g_ambientLight );
	
	// technically, all the lights should be multiplied by texColor.  we just do it at the end to save operations.
	o_color = saturate( float4(lightColor, 0.0 ) * texColor );
	
	o_color.a = g_alpha;
}