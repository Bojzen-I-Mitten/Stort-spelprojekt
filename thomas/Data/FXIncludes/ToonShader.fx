#pragma warning(disable: 4717) // removes effect deprecation warning.

#include <ThomasCG.hlsl>
#include <ThomasLights.hlsl>

Texture2D DiffuseTexture;
Texture2D NormalTexture : NORMALTEXTURE;
Texture2D SpecularTexture : SPECULARTEXTURE;

cbuffer MATERIAL_PROPERTIES
{
    float4 color : COLOR;
    float smoothness : MATERIALSMOOTHNESSFACTOR;
    float4 uvTiling : UVTILING; // (uv tiling x, uv tiling y, uv offset x, uv offset y)
};


SamplerState StandardWrapSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};

RasterizerState TestRasterizer
{
    FillMode = SOLID;
    CullMode = BACK;
    FrontCounterClockWise = TRUE;
    DepthClipEnable = FALSE;
};

BlendState AlphaBlendingOn
{
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = INV_SRC_ALPHA;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;

};

struct v2f
{
    float4 vertex : SV_POSITION;
    float4 worldPos : POSITIONWS;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
};

// Test for toon shading
static float3 DiffuseLightDirection = float3(1.f, 0.f, 0.f);
static float4 DiffuseColor = float4(1.f, 1.f, 1.f, 1.f);
static float DiffuseIntensity = 1.f;
static float4 LineColor = float4(0.f, 0.f, 0.f, 1.f);
static float LineThickness = 0.03f;

v2f CelVertexShader(appdata_thomas v)
{
    v2f o;

    o.vertex = ThomasObjectToClipPos(v.vertex);
    o.worldPos = ThomasObjectToWorldPos(v.vertex);
    o.normal = normalize(v.normal);
    o.texcoord = v.texcoord * uvTiling.xy + uvTiling.zw;

    return o;
}

float4 CelPixelShader(v2f input) : SV_TARGET
{
	// Calculate diffuse light amount
    float intensity = dot(normalize(DiffuseLightDirection), input.normal);

    if (intensity < 0)
        intensity = 0;
 
    // Calculate what would normally be the final color, including texturing and diffuse lighting
    float4 color = DiffuseTexture.Sample(StandardWrapSampler, input.texcoord) * DiffuseColor * DiffuseIntensity;
    color.a = 1;
 
    // Discretize the intensity, based on a few cutoff points
    if (intensity > 0.95)
        color = float4(1.0, 1, 1, 1.0) * color;
    else if (intensity > 0.5)
        color = float4(0.7, 0.7, 0.7, 1.0) * color;
    else if (intensity > 0.05)
        color = float4(0.35, 0.35, 0.35, 1.0) * color;
    else
        color = float4(0.1, 0.1, 0.1, 1.0) * color;
 
    return color;
}

// The vertex shader that does the outlines
v2f OutlineVertexShader(appdata_thomas input)
{
    v2f output = (v2f)0;
 
    float4 original = ThomasObjectToWorldPos(input.vertex);
    float4 normal = ThomasObjectToWorldPos(input.normal);
 
    // Take the correct "original" location and translate the vertex a little
    // bit in the direction of the normal to draw a slightly expanded object.
    // Later, we will draw over most of this with the right color, except the expanded
    // part, which will leave the outline that we want.
    output.vertex = original + (mul(LineThickness, normal));
 
    return output;
}

//// The pixel shader for the outline.  It is pretty simple: draw everything with the
// correct line color.
float4 OutlinePixelShader(v2f input) : SV_TARGET
{
    return LineColor;
}

technique11 Toon
{
	// The first pass will go through and draw the back-facing triangles with the outline shader,
    // which will draw a slightly larger version of the model with the outline color.  Later, the
    // model will get drawn normally, and draw over the top most of this, leaving only an outline.
  //  pass Pass1
  //  {
		//VERT(OutlineVertexShader());
  //      SetGeometryShader(NULL);
		//FRAG(OutlinePixelShader());
  //      SetDepthStencilState(EnableDepth, 0);
  //      SetRasterizerState(TestRasterizer);
  //      SetBlendState(AlphaBlendingOn, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
  //  }

	// The second pass will draw the model like normal, but with the cel pixel shader, which will
    // color the model with certain colors, giving us the cel/toon effect that we are looking for.
    pass Pass1
    {
		VERT(CelVertexShader());
        SetGeometryShader(NULL);
		FRAG(CelPixelShader());
        SetDepthStencilState(EnableDepth, 0);
        SetRasterizerState(TestRasterizer);
        SetBlendState(AlphaBlendingOn, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
}