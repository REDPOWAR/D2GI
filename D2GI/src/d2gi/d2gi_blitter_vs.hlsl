
float4 g_vRect: register(c0);
float4 g_vTexRect: register(c1);


struct VS_OUT
{
	float4 vPos: POSITION;
	float2 vTexCoord: TEXCOORD0;
};


VS_OUT main(float4 vPos: POSITION, float2 vTC: TEXCOORD0)
{
	VS_OUT sOut;

	sOut.vPos = float4(vPos.xy * g_vRect.xy + g_vRect.zw, 0.0, 1.0f);
	sOut.vTexCoord = (vTC * g_vTexRect.xy) + g_vTexRect.zw;

	return sOut;
}
