uniform float g_secondsElapsed = 1.0;

uniform float3 g_lightPosition;
uniform float4 g_lightParms;
uniform float g_lightIntensity;
uniform float3 g_ambientLight;
uniform float3 g_cameraPosition;
uniform float g_specularPower;
uniform float g_alpha = 1.0;

//directional
uniform float4 g_dirLightColor;
uniform float4 g_dirLightDir;

sampler2D g_diffuseMap;

void main( in const float2 i_texUV : TEXCOORD0, in const float3 i_normal: NORMAL, in const float3 i_tangent: TANGENT, in const float3 i_biNormal: BINORMAL,
	in const float3 i_fragmentPosition : TEXCOORD1, out float4 o_color : COLOR0 )
{
	float4 texSample = tex2D( g_diffuseMap, i_texUV );
	float4 texColor = texSample;

	float3 normal = normalize( i_normal );
	float3 lightDirection = normalize(g_lightPosition - i_fragmentPosition);
	float cosine = dot( lightDirection, normal );
	cosine = saturate( cosine );
	float3 lightColor = (g_lightParms.xyz * g_lightParms.w) * cosine;

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

	//specular
	float3 R = reflect( -lightDirection, normal );
	float3 V = normalize(g_cameraPosition - i_fragmentPosition);
	float cos = saturate(dot( R, V ));
	float shine = pow( cos, g_specularPower );
	float3 specLight = shine * lightColor * saturate( dot( normal, lightDirection ) );

	
	//combine them all
	o_color = texColor * float4(lightColor, 1) + float4(specLight, 1);
	//o_color.a = g_alpha;
}
