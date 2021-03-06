#pragma warning(disable: 4717) // removes effect deprecation warning.


#include <ThomasCG.hlsl>
#include <ThomasPBRLights.hlsl>

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
    MipLODBias = -2;    // Do not sample to low!!!
};

DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};

RasterizerState RasterizerSolid
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
    
    o.texcoord = v.texcoord * uvTiling.xy + uvTiling.zw;
    return o;
}

float4 frag(v2f input) : SV_TARGET
{
    float3 diffuse = DiffuseTexture.Sample(StandardWrapSampler, input.texcoord);
    diffuse *= color.xyz;
    float3 specular = SpecularTexture.Sample(StandardWrapSampler, input.texcoord);

    float3 normal = NormalTexture.Sample(StandardWrapSampler, input.texcoord);
    normal.xy = normal.xy * 2.0f - 1.0f;
    normal = normalize(normal);
    normal = normalize(mul(normal, input.TBN));

    
    diffuse = AddLights(input.worldPos.xyz, normal, diffuse, specular, smoothness); // Calculate light
    
    
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
        SetRasterizerState(RasterizerSolid);
        SetBlendState(AlphaBlendingOn, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }

}