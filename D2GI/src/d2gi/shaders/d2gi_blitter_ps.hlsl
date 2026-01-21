
sampler g_txSourceTexture : register(s0);


float4 g_vBorder:register(c0);


float4 main(float2 vTC : TEXCOORD0) : COLOR0
{
	vTC = clamp(vTC, g_vBorder.xy, g_vBorder.zw);
	return tex2D(g_txSourceTexture, vTC);
}
