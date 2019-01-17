/*********************************************************
* Copyright (C) VERTVER & MICROSOFT 2018. 
* All rights reserved.
* OpenSoundRefenation - WINAPI open-source DAW
* MIT-License
**********************************************************
* Module Name: OSR User Interface
**********************************************************
* MBlur.hlsl
* Blur module for interface implementation
*********************************************************/

struct VSInput
{
    uint VertexID : SV_VertexID;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

// A pass-through function for the texture coordinate data.
PSInput VSSimpleBlur(VSInput input)
{
    PSInput output;
    
	uint id = input.VertexID;

	float x = -1, y = -1;
	x = (id == 2) ? 3.0 : -1.0;
	y = (id == 1) ? 3.0 : -1.0;

	output.position = float4(x, y, 1.0, 1.0);
	output.uv 		= float2(x, y)*0.5 + 0.5;
	output.uv.y 	= 1.0 - output.uv.y;

    return output;
}

static const float KernelOffsets[3] = { 0.0f, 1.3846153846f, 3.2307692308f };
static const float BlurWeights[3] = { 0.2270270270f, 0.3162162162f, 0.0702702703f };

Texture2D tex : register(t0);

SamplerState pointSampler {
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Wrap;
	AddressV = Wrap;
};

SamplerState linearSampler {
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

cbuffer GaussianBlurConstantBuffer : register(b5) 
{
    float4 textureDimensions;    // The render target width/height.
};

// Simple gaussian blur in the vertical direction.
float4 PSSimpleBlurV(PSInput input) : SV_TARGET
{
    float3 textureColor = float3(1.0f, 0.0f, 0.0f);
    float2 uv = input.uv;

    textureColor = tex.Sample(linearSampler, uv).xyz * BlurWeights[0];
    for (int i = 1; i < 3; i++)
    {
        float2 normalizedOffset = float2(0.0f, KernelOffsets[i]) / textureDimensions.y;
        textureColor += tex.Sample(linearSampler, uv + normalizedOffset).xyz * BlurWeights[i];
        textureColor += tex.Sample(linearSampler, uv - normalizedOffset).xyz * BlurWeights[i];
    }

    return float4(textureColor, 1.0);
}

// Simple gaussian blur in the horizontal direction.
float4 PSSimpleBlurU(PSInput input) : SV_TARGET
{
    float3 textureColor = float3(1.0f, 0.0f, 0.0f);
    float2 uv = input.uv;
    
    textureColor = tex.Sample(linearSampler, uv).xyz * BlurWeights[0];
    for (int i = 1; i < 3; i++)
    {
        float2 normalizedOffset = float2(KernelOffsets[i], 0.0f) / textureDimensions.x;
        textureColor += tex.Sample(linearSampler, uv + normalizedOffset).xyz * BlurWeights[i];
        textureColor += tex.Sample(linearSampler, uv - normalizedOffset).xyz * BlurWeights[i];
    }
    
    return float4(textureColor, 1.0);
}