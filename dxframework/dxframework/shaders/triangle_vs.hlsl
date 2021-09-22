#pragma pack_matrix(row_major)

cbuffer perObjectBuffer : register(b0) {
    float4x4 wvpMatrix;
    float4x4 worldMatrix;
};

struct VS_IN
{
	float3 pos : POSITION;
	float4 col : COLOR0;
};

struct PS_IN
{
	float4 pos: SV_POSITION;
 	float4 col : COLOR;
};

PS_IN VS_main( VS_IN input )
{
	PS_IN output = (PS_IN)0;
	
	//output.pos = input.pos;
    output.pos = mul(float4(input.pos, 1.0f), wvpMatrix);
	output.col = input.col;
	
	return output;
}
