/////////////////////////////////////////////////////////////////////////////////////////////
// Copyright 2017 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");// you may not use this file except in compliance with the License.// You may obtain a copy of the License at//// http://www.apache.org/licenses/LICENSE-2.0//// Unless required by applicable law or agreed to in writing, software// distributed under the License is distributed on an "AS IS" BASIS,// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.// See the License for the specific language governing permissions and// limitations under the License.
/////////////////////////////////////////////////////////////////////////////////////////////

#define _GAMMA_CORRECT
#define _DECODE_NORMAL


static const int MAX_MATRICES = 26;
static const int LIGHT_NUM = 3;


static const float3 LIGHT_COLOR[LIGHT_NUM] = 
{ 
	{ 0.7f, 0.7f, 0.7f },
	{ 1.0f, 1.0f, 1.0f },
	{ 0.3f, 0.3f, 0.3f },
};


static const float3 LIGHT_DIR[LIGHT_NUM] = 
{
	normalize(float3(0.0f, 2.0f, -10.0f)),
	normalize(float3(1.0f, -0.2f, -1.0f)),
	normalize(float3(-10.0f, -10.0f, -10.0f)),
};

int BoneNumber = 2;

float4x3 BoneMatrices[MAX_MATRICES] : WORLDMATRIXARRAY;
float4x4 ViewProjMatix : VIEWPROJECTION;

float4 EyePos = { 1.0f, 0.0f, 0.0f, 0.0f };

texture DiffuseTexture;           
texture NormalTexture;
texture SpecularTexture;
texture EnvironTexture;
texture MaskTexture;


sampler DiffuseTextureSampler =
sampler_state
{
	Texture = <DiffuseTexture>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};


sampler NormalTextureSampler =
sampler_state
{
	Texture = <NormalTexture>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};


sampler SpecularTextureSampler =
sampler_state
{
	Texture = <SpecularTexture>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};


sampler EnvironTextureSampler = 
sampler_state
{
	Texture = <EnvironTexture>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};


sampler MaskTextureSampler =
sampler_state
{
	Texture = <MaskTexture>;
	MipFilter = LINEAR;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
};


struct VS_INPUT
{
    float4 Pos				: POSITION;
    float4 BlendWeights		: BLENDWEIGHT;
    float4 BlendIndices		: BLENDINDICES;
	float2 UV0				: TEXCOORD0;
    float3 Normal			: NORMAL;
	float3 Tangent	        : TANGENT;
	float3 Binormal			: BINORMAL;
};


struct VS_OUTPUT
{
	float4 Pos			: POSITION;
    float2 UV0			: TEXCOORD0;
	float3 Normal		: TEXCOORD1;
	float3 Tangent		: TEXCOORD2;
	float3 Binormal		: TEXCOORD3;
	float3 EyeDir		: TEXCOORD4;
};


float GetFresnelFactor(float3 v, float3 h)
{
	return 1.0f + pow(1.0f - saturate(dot(v, h)), 5);
}


float3 GetLambert(float3 l, float3 n)
{
	return max(dot(n, l), 0.0f);
}


float3 GetBlinnPhong(float3 n, float3 h, float shininess)
{
	return pow(max(dot(n, h), 0.0f), shininess);
}


float4x3 GetSkinMatrix(float blendWeights[4], int blendIndices[4], uniform int BoneNum)
{
	float4x3 mat = 0;
	float lastWeight = 0.0f;

	[unroll]
	for (int i = 0; i < BoneNum - 1; i++)
	{
		lastWeight += blendWeights[i];
		mat += BoneMatrices[blendIndices[i]] * blendWeights[i];
	}

	lastWeight = 1.0f - lastWeight;
	mat += BoneMatrices[blendIndices[BoneNum - 1]] * lastWeight;

	return mat;
}


VS_OUTPUT VShade(VS_INPUT In, uniform int BoneNum)
{
	VS_OUTPUT Out;

	float3 Pos = 0.0f;
	float weights[4] = (float[4])(In.BlendWeights);
	int indices[4] = (int[4])(In.BlendIndices);

	float4x3 skinMatrix = GetSkinMatrix(weights, indices, BoneNum);

	Pos = mul(In.Pos, skinMatrix);
	Out.Pos = mul(float4(Pos.xyz, 1.0f), ViewProjMatix);

	Out.Normal = mul(In.Normal, skinMatrix);
	Out.Tangent = mul(In.Tangent, skinMatrix);
	Out.Binormal = mul(In.Binormal, skinMatrix);

	Out.EyeDir = float3(EyePos.xyz - Pos.xyz);
	Out.UV0 = In.UV0;

	return Out;
}


float3 InvertCorrectGamma(float3 input)
{
#ifdef _GAMMA_CORRECT
	return pow(abs(input), 2.2f);
#else
	return input;
#endif
}


float3 CorrectGamma(float3 input)
{
#ifdef _GAMMA_CORRECT
	return pow(abs(input), 1.0f / 2.2f);
#else
	return input;
#endif
}


float3 DecodeNormal(float4 normalInput)
{
#ifdef _DECODE_NORMAL
	float3 normal;
	normal.xy = normalInput.xy * 2.0f - 1.0f;
	normal.z = sqrt(1.0f - dot(normalInput.xy, normalInput.xy));
	return normal;
#else
	return normalInput.xyz * 2.0f - 1.0f;
#endif
}


float4 PShader(VS_OUTPUT In) : COLOR0
{
	float4 normalColor = tex2D(NormalTextureSampler, In.UV0);
	float3x3 tbn = float3x3(normalize(In.Tangent), normalize(In.Binormal), normalize(In.Normal));

	float3 N = normalize(mul(DecodeNormal(normalColor), tbn));
	float3 V = normalize(In.EyeDir);

	float3 specularColor = InvertCorrectGamma(tex2D(SpecularTextureSampler, In.UV0).xyz);
	float4 diffuseColor = tex2D(DiffuseTextureSampler, In.UV0);
	diffuseColor.xyz = InvertCorrectGamma(diffuseColor.xyz);

	float3 refl = reflect(-V, N);
	float3 envColor = texCUBE(EnvironTextureSampler, refl).xyz;
	envColor = pow(envColor, 2);

	float3 diffuse = 0.0f;
	float3 specular = 0.0f;

	[unroll]
	for (int i = 0; i < LIGHT_NUM; ++i)
	{
		float3 L = LIGHT_DIR[i];
		float3 lightColor = LIGHT_COLOR[i];
		diffuse += GetLambert(L, N) * lightColor;

		float3 H = normalize(L + V);
		specular += GetBlinnPhong(N, H, 200) * 0.001f;
	}

	specular *= envColor;

	float4 color;
	color.xyz = (diffuse + 0.3f)* diffuseColor.xyz + specular;
	color.xyz = CorrectGamma(color.xyz);
	color.w = diffuseColor.w;
	return color;

}


float4 PShaderEye(VS_OUTPUT In) : COLOR0
{
	float4 normalColor = tex2D(NormalTextureSampler, In.UV0);
	float3x3 tbn = float3x3(normalize(In.Tangent), normalize(In.Binormal), normalize(In.Normal));

	float3 N1 = normalize(mul(DecodeNormal(normalColor), tbn));
	float3 N2 = normalize(In.Normal);
	float3 V = normalize(In.EyeDir);

	float3 specularColor = InvertCorrectGamma(tex2D(SpecularTextureSampler, In.UV0).xyz);
	float4 diffuseColor = tex2D(DiffuseTextureSampler, In.UV0);
	diffuseColor.xyz = InvertCorrectGamma(diffuseColor.xyz);

	float4 maskColor = tex2D(MaskTextureSampler, In.UV0);
	float3 mask = pow(maskColor, 2);

	float3 refl = reflect(-V, N2);
	float3 envColor = texCUBE(EnvironTextureSampler, refl).xyz * mask;
	envColor = pow(envColor, 2) * 0.2;

	float3 diffuse1 = 0.0f;
	float3 specular1 = 0.0f;
	float3 specular2 = 0.0f;

	[unroll]
	for (int i = 0; i < LIGHT_NUM; ++i)
	{
		float3 L = LIGHT_DIR[i];
		float3 lightColor = LIGHT_COLOR[i];
		diffuse1 += GetLambert(L, N1) * lightColor;
		
		float3 H = normalize(L + V);
		specular1 += GetBlinnPhong(N1, H, 3000);
		specular2 += GetBlinnPhong(N2, H, 6000) * 50;
	}

	
	specular2 *= mask;

	float lerpF = min(max(0, pow(abs(1.0 - dot(N2, V)), 4.0f)), 0.02);

	float4 color;
	color.xyz = lerp(diffuse1 * diffuseColor.xyz + specular1, envColor + specular2, lerpF);
	color.xyz = CorrectGamma(color.xyz);
	color.w = diffuseColor.w;
	return color;
}


VertexShader VSArray[4] = { compile vs_2_0 VShade(1),
compile vs_3_0 VShade(2),
compile vs_3_0 VShade(3),
compile vs_3_0 VShade(4)
};

technique solid
{
	pass p0
	{
		CullMode = CCW;
		ZEnable = TRUE;
		ZWriteEnable = TRUE;
		ZFunc = LessEqual;
		AlphaBlendEnable = FALSE;

		VertexShader = (VSArray[BoneNumber]);
		PixelShader = compile ps_3_0 PShader();
	}
}

technique translucent
{
	pass p0
	{
		CullMode = NONE;
		ZEnable = TRUE;
		ZWriteEnable = FALSE;
		ZFunc = LessEqual;
		AlphaBlendEnable = TRUE;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = (VSArray[BoneNumber]);
		PixelShader = compile ps_3_0 PShader();
	}
}

technique eyeball
{
	pass p0
	{
		CullMode = NONE;
		ZEnable = TRUE;
		ZWriteEnable = TRUE;
		ZFunc = LessEqual;
		AlphaBlendEnable = TRUE;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = (VSArray[BoneNumber]);
		PixelShader = compile ps_3_0 PShaderEye();
	}
}
