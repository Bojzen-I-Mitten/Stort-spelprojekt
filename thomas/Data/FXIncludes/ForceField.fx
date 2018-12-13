#pragma warning(disable: 4717) // removes effect deprecation warning.

#include <ThomasCG.hlsl>
#include <ThomasLights.hlsl>

Texture2D ForcefieldTexture;
cbuffer MATERIAL_PROPERTIES
{
    float4 color : COLOR;
    float4 uvTiling : UVTILING; // (uv tiling x, uv tiling y, uv offset x, uv offset y)
    float4 ballPosition : POSITION;
    float hole;
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
    DepthWriteMask = ZERO;
    DepthFunc = LESS_EQUAL;
};

RasterizerState RasterizerSolid
{
    FillMode = SOLID;
    CullMode = NONE;
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
    float3 objectPos : POSITION;
    float3 viewDir : TEXCOORD1;
};

v2f vert(appdata_thomas v)
{
    v2f o;

    float3 scale = float3(thomas_ObjectToWorld[0][0], thomas_ObjectToWorld[1][1], thomas_ObjectToWorld[2][2]);

    float3 posL = v.vertex;

    o.vertex = ThomasObjectToClipPos(posL);
    o.worldPos = ThomasObjectToWorldPos(posL);
	
    o.objectPos = v.vertex;
    
    float3 tangent = ThomasObjectToWorldDir(v.tangent);
    float3 bitangent = ThomasObjectToWorldDir(v.bitangent);
    float3 normal = ThomasObjectToWorldDir(v.normal);

    o.TBN = float3x3(tangent, bitangent, normal);
    
    o.viewDir = normalize(ThomasWorldSpaceViewDir(mul(thomas_ObjectToWorld, float4(v.vertex, 1.0f)).xyz));

    o.texcoord = v.texcoord * uvTiling.xy * scale.yx + uvTiling.zw;
    return o;
}


float triWave(float t, float offset, float yOffset)
{
    return saturate(abs(frac(offset + t) * 2 - 1) + yOffset);
}

float4 frag(v2f input) : SV_TARGET
{
      
    float maxBallDistance = ballPosition.w;

    float rim = 1 - abs(dot(input.TBN[2], normalize(input.viewDir)));
    float ballDist = distance(input.worldPos.xyz, ballPosition.xyz);

    float4 forceTex = ForcefieldTexture.Sample(StandardWrapSampler, input.texcoord);

    float ballImpact = smoothstep(maxBallDistance, 0.0f, ballDist) * 8.0f;

    ballImpact -= smoothstep(maxBallDistance * hole, 0.0f, ballDist) * 8.0f;

    forceTex.r *= ballImpact + triWave(thomas_Time.x * 5, abs((uvTiling.z * input.texcoord.y / uvTiling.y) + 1), -0.7) * 6;
				// I ended up saturaing the rim calculation because negative values caused weird artifacts
    forceTex.g *= ballImpact + saturate(saturate(rim) + 0.05f) * (sin(thomas_Time.z + forceTex.b * 5) + 1);

    float glow = rim * 0.2f;
    float4 glowColor = float4(lerp(color.rgb, float3(1, 1, 1), pow(glow, 4)), 1);
    //return ballImpact * color;
    

   // float strength = 0.5f + forcefield + impact * maxBallDistance / ballDist;
    float4 hexes = forceTex.r * color + forceTex.g * color;
    return color * color.a + glowColor * glow + hexes;
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