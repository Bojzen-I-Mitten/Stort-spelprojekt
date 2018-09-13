#pragma warning(disable: 4717) // removes effect deprecation warning.

#include <ThomasCG.hlsl>
Texture2D ambientTex;


SamplerState StandardWrapSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

cbuffer MATERIAL_PROPERTIES
{
    
	float4 wow : COLOR;
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

float3 GetHalfwayVec(float3 lightDir, float3 viewDir)
{
    return normalize(viewDir + lightDir);
}

struct v2f {
	float4 vertex : SV_POSITION;
	float4 worldPos : POSITIONWS;
	float3 normal : NORMAL;
    float2 texcoord : TEXCOORD0;
};

v2f vert(appdata_thomas v)
{
	v2f o;

    //float3x3 boneTransform = v.boneWeight.boneIndex0 * v.boneWeight.weight0;
    //boneTransform += v.boneWeight.boneIndex1 * v.boneWeight.weight1;
    //boneTransform += v.boneWeight.boneIndex2 * v.boneWeight.weight2;
    //boneTransform += v.boneWeight.boneIndex3 * v.boneWeight.weight3;
    //float3 posL = mul(boneTransform, v.vertex);
    //float3 normalL = mul(boneTransform, v.normal);

    float3 posL = v.vertex;
    float3 normalL = v.normal;

    o.vertex = ThomasObjectToClipPos(posL);
    o.worldPos = ThomasObjectToWorldPos(posL);
    o.normal = ThomasWorldToObjectDir(normalL);
    o.texcoord = v.texcoord;
	return o;
}



cbuffer LightCountsStruct
{
	uint nrOfDirectionalLights;
    uint nrOfPointLights;
    uint nrOfSpotLights;
};

struct LightStruct
{
	float3  color;
	float   intensity;
	float3  position;
	float   spotOuterAngle;
	float3  direction;
	float   spotInnerAngle;
	float3  attenuation;
    float   pad;
};


StructuredBuffer<LightStruct> lights;

float CalculatePointLightContribution(float4 lightColor, float lightIntensity, float lightDistance, float3 lightAttenuation)
{
	return lightColor * lightIntensity / (lightAttenuation.x + lightAttenuation.y * lightDistance + lightAttenuation.z * lightDistance * lightDistance);
}

/*float CalculateSpotLightFactor()
{
	float angle = degrees(acos(dot(-tempLight.direction, lightDir)));
	float spotFactor = 0.0f;
	if (angle < tempLight.spotInnerAngle)
	{
		spotFactor = 1.0f;
	}
	else if (angle < tempLight.spotOuterAngle)
	{
		spotFactor = 1.0f - smoothstep(tempLight.spotInnerAngle, tempLight.spotOuterAngle, angle);
	}
	return spotFactor;
}*/

void Apply(inout float4 colorAcculmulator, float3 lightMultiplyer, float3 normal, float3 lightDir, float3 viewDir)//should take material properties later
{
    float4 ambient = float4(0.1f, 0.1f, 0.1f, 1.0f);
    float4 diffuse = wow;
    float4 specular = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float smoothness = 16.0f;

    float lambertian = saturate(dot(normal, lightDir));
    float specularIntensity = 0.0f;
    if (lambertian > 0.0f)
    {
        specularIntensity = pow(saturate(dot(normal, GetHalfwayVec(viewDir, lightDir))), smoothness); //blinn-phong
    }
    
    colorAcculmulator += ambient + (diffuse * lambertian + specular * specularIntensity) * float4(lightMultiplyer, 1);
}

float4 frag(v2f input) : SV_TARGET
{
    float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    float3 viewDir = normalize(_WorldSpaceCameraPos - input.worldPos.xyz);
	float3 lightDir = float3(0, 0, 0);
    float3 lightMultiplyer = float3(0.0f, 0.0f, 0.0f);
    
    int i = 0;
	int roof = nrOfDirectionalLights;
    for (; i < roof; ++i) //directional
    {
        lightDir = lights[i].direction;
        lightMultiplyer = lights[i].color * lights[i].intensity;
        Apply(finalColor, lightMultiplyer, input.normal, lightDir, viewDir);
    }
    roof += nrOfPointLights;
    for (; i < roof; ++i) //point
    {
        lightDir = lights[i].position - input.worldPos.xyz;
        float lightDistance = length(lightDir);
        lightDir = normalize(lightDir);

        lightMultiplyer = lights[i].color * lights[i].intensity / (lights[i].attenuation.x + lights[i].attenuation.y * lightDistance + lights[i].attenuation.z * lightDistance * lightDistance);
        Apply(finalColor, lightMultiplyer, input.normal, lightDir, viewDir);
    }
    roof += nrOfSpotLights;
    for (; i < roof; ++i) //spot
    {
        lightDir = lights[i].position - input.worldPos.xyz;
        float lightDistance = length(lightDir);
        lightDir = normalize(lightDir);

        float angle = degrees(acos(dot(lights[i].direction, lightDir)));
        float spotFactor = 0.0f;
        if (angle < lights[i].spotInnerAngle)
        {
            spotFactor = 1.0f;
        }
        else if (angle < lights[i].spotOuterAngle)
        {
            spotFactor = 1.0f - smoothstep(lights[i].spotInnerAngle, lights[i].spotOuterAngle, angle);
        }

        lightMultiplyer = spotFactor * lights[i].color * lights[i].intensity / (lights[i].attenuation.x + lights[i].attenuation.y * lightDistance + lights[i].attenuation.z * lightDistance * lightDistance);
        Apply(finalColor, lightMultiplyer, input.normal, lightDir, viewDir);
    }

    return saturate(finalColor);

}


technique11 Standard {
	pass P0 {
		VERT(vert());
		SetGeometryShader(NULL);
		FRAG(frag());
		SetDepthStencilState(EnableDepth, 0);
		SetRasterizerState(TestRasterizer);
		SetBlendState(AlphaBlendingOn, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}

}