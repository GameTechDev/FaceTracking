/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

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




