struct VS_IN
{
	float4 pos : POSITION0;
	float4 col : COLOR0;
};

struct PS_IN
{
	float4 pos: SV_POSITION;
 	float4 col : COLOR;
};

float4 main( PS_IN input ) : SV_Target
{
	float4 col = input.col;

	return col;
}
