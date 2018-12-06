#pragma warning(disable: 4717) // removes effect deprecation warning.

#include <ThomasCG.hlsl>
#include <ThomasToonLights.hlsl>

Texture2D DiffuseTexture;
Texture2D NormalTexture : NORMALTEXTURE;
Texture2D SpecularTexture : SPECULARTEXTURE;
Texture2D rampTex;

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
    float3x3 TBN : TBN;
    float2 texcoord : TEXCOORD0;
    float3 viewDir : TEXCOORD1;
};

v2f vert(appdata_thomas v)
{
    v2f o;

    float3 posL = v.vertex;

    o.vertex = ThomasObjectToClipPos(posL);
    o.worldPos = ThomasObjectToWorldPos(posL);
     
    float3 tangent = ThomasObjectToWorldDir(v.tangent);
    float3 bitangent = ThomasObjectToWorldDir(v.bitangent);
    float3 normal = ThomasObjectToWorldDir(v.normal);

    o.TBN = float3x3(tangent, bitangent, normal);
    
    o.viewDir = normalize(ThomasWorldSpaceViewDir(mul(thomas_ObjectToWorld, float4(v.vertex, 1.0f)).xyz));

    o.texcoord = v.texcoord * uvTiling.xy + uvTiling.zw;
    return o;
}

float4 frag(v2f input) : SV_TARGET
{
    float3 normal = NormalTexture.Sample(StandardWrapSampler, input.texcoord);
    normal.xy = normal.xy * 2.0f - 1.0f;
    normal = normalize(normal);
    normal = normalize(mul(normal, input.TBN));

    float3 diffuse = DiffuseTexture.Sample(StandardWrapSampler, input.texcoord);
    diffuse *= color.xyz;
 
    float specularMapFactor = SpecularTexture.Sample(StandardWrapSampler, input.texcoord);
     
    float3 rim = pow(max(0, dot(float3(0, 1, 0), 1 - abs(dot(normal, normalize(input.viewDir))))), 2.5f) * float3(0.97f, 0.88f, 1.0f) * specularMapFactor;

    diffuse = AddToonLights(input.worldPos.xyz, normal, diffuse, specularMapFactor, smoothness + 1);        // Calculate light    
    diffuse.xyz = pow(diffuse, 0.4545454545f);                                                          // Gamma correction

    return saturate(float4(diffuse, 1.0f));
}

technique11 Standard
{
    pass P0
    {
		VERT(vert());
        SetGeometryShader(NULL);
		FRAG(frag());
        SetDepthStencilState(EnableDepth, 0);
        SetRasterizerState(TestRasterizer);
        SetBlendState(AlphaBlendingOn, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

}