/*******************************************************************************

INTEL CORPORATION PROPRIETARY INFORMATION
This software is supplied under the terms of a license agreement or non-disclosure
agreement with Intel Corporation and may not be copied or disclosed except in
accordance with the terms of that agreement
Copyright(c) 2011-2015 Intel Corporation. All Rights Reserved.

*******************************************************************************/

float4x4 InvertMVP;
texture EnvironTexture;


sampler EnvironTextureSampler = sampler_state
{ 
    Texture = (EnvironTexture);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};


struct VSInput
{
    float4 Pos : POSITION;
};


struct VSOutput
{
    float4 Pos : POSITION;
    float3 Tex : TEXCOORD0;
};


VSOutput VS(VSInput In)
{
    VSOutput Out;
    
	Out.Pos = In.Pos;
	Out.Tex = normalize(mul(In.Pos, InvertMVP));
    
    return Out;
}


float4 PS(VSOutput In) : COLOR
{
	float3 uv = In.Tex;
    float4 color = texCUBE(EnvironTextureSampler, uv);
	color.xyz *= 0.5;
    color.a = 1.0f;
	return color;
}


technique SkyBox
{
    pass p0
    {
		CullMode = NONE;
		ZEnable = FALSE;
		ZWriteEnable = TRUE;
		AlphaBlendEnable = FALSE;

        VertexShader = compile vs_2_0 VS();
        PixelShader = compile ps_2_0 PS();
    }
}




