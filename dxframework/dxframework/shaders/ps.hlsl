#pragma pack_matrix( row_major )

cbuffer lightPointBuffer : register(b0)
{
    float3 dynamicLightColor;
    float dynamicLightStrength;
    float3 dynamicLightPosition;
}

cbuffer FogBuffer : register(b1)
{
    float4 gFogColor;
    float gFogStart;
    float gFogRange;

    float3 EyePosW;

}

cbuffer LightBuffer : register(b2)
{
    float3 ambientLightColor;
    float ambientLightStrength;
    float specularPower;
}

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WORLD_POSITION;
};

Texture2D objTexture : TEXTURE: register(t0);
SamplerState objSamplerState : SAMPLER: register(s0);

float4 PS_main(PS_INPUT input) : SV_TARGET
{
   float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
   return float4(sampleColor, 1.f);
   
   float3 viewerDir = normalize(input.inPosition - input.inWorldPos);

   float3 ambientLight = ambientLightColor * ambientLightStrength;

   float3 appliedLight = ambientLight;

   float3 vectorToLight = normalize(dynamicLightPosition - input.inWorldPos);

   //float3 refVec = normalize(reflect(dynamicLightPosition, input.inNormal));

   float3 diffuseLightIntensity = max(dot(vectorToLight, input.inNormal), 0);

   float3 diffuseLight = diffuseLightIntensity * dynamicLightStrength * dynamicLightColor;

   float lightIntensity = saturate(dot(input.inNormal, dynamicLightPosition));
   float3 reflection = normalize(2 * lightIntensity * input.inNormal - dynamicLightPosition);
   float4 specular = pow(saturate(dot(reflection, viewerDir)), specularPower);

   appliedLight += diffuseLight;

   appliedLight += specular;

   float3 finalColor = sampleColor * 1.f;//appliedLight;

   float3 toEyeW = EyePosW - input.inWorldPos;
   float distToEye = length(toEyeW);
   float fogAmount = saturate((distToEye - gFogStart) / gFogRange);
   finalColor = lerp(finalColor, gFogColor, 0.f /* fogAmount */);

   return float4(finalColor, 1.0f);
}
