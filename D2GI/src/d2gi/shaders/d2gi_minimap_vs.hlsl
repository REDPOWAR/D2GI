
float4x4 g_Transform : register(c0);

struct VS_OUT
{
	float4 vPos : POSITION;
	float4 vColor : COLOR0;
};

VS_OUT main(float4 vPos : POSITION, float4 vColor : COLOR0)
{
	VS_OUT sOut;

	sOut.vPos = mul(g_Transform, vPos);
	sOut.vColor = vColor;

	return sOut;
}
