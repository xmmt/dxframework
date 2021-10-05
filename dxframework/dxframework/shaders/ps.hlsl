#pragma pack_matrix( row_major )

cbuffer point_light_buf : register(b0)
{
    float3 light_color;
    float light_strength;
    float3 light_pos;
}

cbuffer fog_buf : register(b1)
{
    float4 fog_color;
    float fog_near;
    float fog_far;

    float3 eye_pos_w;

}

cbuffer light_buf : register(b2)
{
    float3 ambient_light_color;
    float ambient_light_strength;
    float specular_power;
}

struct PS_INPUT
{
    float4 in_pos : SV_POSITION;
    float2 inTexCoord : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inWorldPos : WORLD_POSITION;
};

Texture2D objTexture : TEXTURE: register(t0);
SamplerState objSamplerState : SAMPLER: register(s0);

float4 PS_main(PS_INPUT input) : SV_TARGET
{
   float3 sampleColor = objTexture.Sample(objSamplerState, input.inTexCoord);
   //return float4(sampleColor, 1.f);
   
   float3 viewerDir = normalize(input.in_pos - input.inWorldPos);

   float3 ambientLight = ambient_light_color * ambient_light_strength;

   float3 appliedLight = ambientLight;

   float3 vectorToLight = normalize(light_pos - input.inWorldPos);

   float3 diffuseLightIntensity = max(dot(vectorToLight, input.inNormal), 0);

   float3 diffuseLight = diffuseLightIntensity * light_strength * light_color;

   float lightIntensity = saturate(dot(input.inNormal, light_pos));
   float3 reflection = normalize(2 * lightIntensity * input.inNormal - light_pos);
   float4 specular = pow(saturate(dot(reflection, viewerDir)), specular_power);

   appliedLight += diffuseLight;

   appliedLight += specular;

   float3 finalColor = sampleColor * appliedLight;

   float3 toEyeW = eye_pos_w - input.inWorldPos;
   float distToEye = length(toEyeW);
   float fogAmount = saturate((distToEye - fog_near) / fog_far);
   finalColor = lerp(finalColor, fog_color, fogAmount);

   return float4(finalColor, 1.0f);
}
