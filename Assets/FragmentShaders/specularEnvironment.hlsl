uniform float g_secondsElapsed = 1.0;

uniform float3 g_lightPosition;
uniform float4 g_lightParms;
uniform float g_lightIntensity;
uniform float3 g_ambientLight;
uniform float3 g_cameraPosition;
uniform float g_specularPower;
uniform float g_alpha = 1.0;

sampler2D		g_diffuseMap;
sampler2D		g_normalMap;
samplerCUBE		g_environmentMap;


void main( in const float2 i_texUV : TEXCOORD0, in const float3 i_normal: NORMAL, in const float3 i_tangent: TANGENT, in const float3 i_biNormal: BINORMAL,
	in const float3 i_fragmentPosition : TEXCOORD1, out float4 o_color : COLOR0 )
{
	float4 texSample = tex2D( g_diffuseMap, i_texUV );
	float4 texColor = texSample;

	float3 normSample = tex2D( g_normalMap, i_texUV ).xyz;
	normSample = (2*normSample) - 1;

	float3x3 mapConversion;
	mapConversion[0] = normalize( i_tangent );
	mapConversion[1] = normalize( i_biNormal );
	mapConversion[2] = normalize( i_normal );

	float3 normal = normalize( mul( normSample, mapConversion ) );
	float3 lightDirection = normalize(g_lightPosition - i_fragmentPosition);
	float cosine = dot( lightDirection, normal );
	cosine = saturate( cosine );
	float3 lightColor = (g_lightParms.xyz * g_lightParms.w) * cosine;

	//accounting for attenuation of the point light + intensity.
	float d = distance( g_lightPosition, i_fragmentPosition );
	float radius = g_lightParms.w;
	float attenuation = g_lightIntensity / (pow(((d/radius) + 1), 2));
	lightColor *= attenuation;

	lightColor += g_ambientLight;

	//specular
	float3 R = reflect( -lightDirection, normal );
	float3 V = normalize(g_cameraPosition - i_fragmentPosition);
	float cos = saturate(dot( R, V ));
	float shine = pow( cos, g_specularPower );
	float3 specLight = shine * lightColor * saturate( dot( normal, lightDirection ) );

	//environment mapping
	float3 camToFrag = normalize( i_fragmentPosition - g_cameraPosition );
	R = reflect( camToFrag, normal );
	float4 envTexColor = texCUBE( g_environmentMap, R );

	//schlick's!
	float fM = pow((1 - saturate( dot( normal, V ) ) ), 5 );
	float reflectedAmount = 0.05f + (( 1 - 0.05f ) * fM );
	float4 diffuse = lerp( texColor, envTexColor, float4(reflectedAmount,reflectedAmount,reflectedAmount, 1) );
	
	//combine them all
	o_color = diffuse * float4(lightColor, 1) + float4(specLight, 1);
	//o_color.a = g_alpha;
}
