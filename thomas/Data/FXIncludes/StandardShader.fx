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
    uint nrOfAreaLights;
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
    //For area lights
    float3 right;
    float pad1;

    float3 up;
    float pad2;

    float2 rectangleDimension;
    float2 pad3;
};

StructuredBuffer<LightStruct> lights;

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
    
    colorAcculmulator += ambient + (diffuse * lambertian) + (specular * specularIntensity) * float4(lightMultiplyer, 1);
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
        lightDir = lights[i].direction; //should be normalized already
        lightMultiplyer = lights[i].color * lights[i].intensity;
        Apply(finalColor, lightMultiplyer, input.normal, lightDir, viewDir);
    }
    roof += nrOfPointLights;
    for (; i < roof; ++i) //point
    {
        lightDir = lights[i].position - input.worldPos.xyz;
        float lightDistance = length(lightDir);
        lightDir /= lightDistance;

        float3 attenuation = lights[i].attenuation;
        lightMultiplyer = lights[i].color * lights[i].intensity / (attenuation.x + attenuation.y * lightDistance + attenuation.z * lightDistance * lightDistance);
        Apply(finalColor, lightMultiplyer, input.normal, lightDir, viewDir);
    }
    roof += nrOfSpotLights;
    for (; i < roof; ++i) //spot
    {
        lightDir = lights[i].position - input.worldPos.xyz;
        float lightDistance = length(lightDir);
        lightDir /= lightDistance;

        float angle = degrees(acos(dot(lights[i].direction, lightDir)));//optimize
        float spotFactor = 0.0f;
        float innerAngle = lights[i].spotInnerAngle;
        
        if (angle < innerAngle)
        {
            spotFactor = 1.0f;
        }
        else 
        {
            float outerAngle = lights[i].spotOuterAngle;
            if (angle < outerAngle)
            spotFactor = 1.0f - smoothstep(innerAngle, outerAngle, angle);
        }

        float3 attenuation = lights[i].attenuation;
        lightMultiplyer = spotFactor * lights[i].color * lights[i].intensity / (attenuation.x + attenuation.y * lightDistance + attenuation.z * lightDistance * lightDistance);
        Apply(finalColor, lightMultiplyer, input.normal, lightDir, viewDir);
    }
    roof += nrOfAreaLights;
    for (; i < roof; ++i)
    {
        float3 lightPos = lights[i].position;
        float3 worldPos = input.worldPos.xyz;
        float3 worldNormal = input.normal;

        //project the pixelpoint in the direction of the normal until it hits the area light plane
        float distanceForProjection = dot(worldNormal, worldPos - lightPos);
        float3 projectionPos = worldPos - distanceForProjection * worldNormal;
        float3 lightToPointDir = projectionPos - lightPos;

        //clamp light boundaries
        float3 lightRight = lights[i].right;
        float3 lightUp = lights[i].up;
        float2 rectDimension = lights[i].rectangleDimension;
        float2 rectInLightPlane = float2(dot(lightToPointDir, lightRight), dot(lightToPointDir, lightUp));
        float2 clampedRect = float2(clamp(rectInLightPlane.x, -rectDimension.x, rectDimension.x), clamp(rectInLightPlane.y, -rectDimension.y, rectDimension.y));
        float3 nearestPoint = lightPos + lightRight * clampedRect.x + lightUp * clampedRect.y;
    
        lightDir = nearestPoint - lightPos;
        float lightDistance = length(lightDir);
        lightDir /= lightDistance;

        float3 attenuation = lights[i].attenuation;
        lightMultiplyer = lights[i].color; // * lights[i].intensity / (attenuation.x + attenuation.y * lightDistance + attenuation.z * lightDistance * lightDistance);
        Apply(finalColor, lightMultiplyer, input.normal, lightDir, viewDir);
    }
    
    //float3 pnormal = lights[i].direction;
/*
    float2 dim = float2(0.35f, 2.67f);

    float3 right = float3(1.0, 0, 0);
    float3 up = float3(0, 0, -1);
    float3 ldir = float3(0, -1, 0);
    
    float3 pos = input.worldPos.xyz;
    float3 nor = input.normal;
    

    
    float distance = dot(nor, input.worldPos.xyz - lights[i].position);
    float3 projection = input.worldPos.xyz - distance * nor;
    float3 directionLightToPoint = projection - lights[i].position;

    float2 diagonal = float2(dot(directionLightToPoint, right), dot(directionLightToPoint, up));
    float2 nearest2D = float2(clamp(diagonal.x, -dim.x, dim.x), clamp(diagonal.y, -dim.y, dim.y));
    float3 nearestPointInside = lights[i].position + right * nearest2D.x + up * nearest2D.y;

    //if (i > 0)
    {
    
        float lightDistance = 10;
//        length(nearestPointInside - input.worldPos.xyz);
        lightDir = normalize(nearestPointInside - input.worldPos.xyz);
        float3 attenuation = lights[i].attenuation;
        lightMultiplyer = lights[i].color * 0.5f;
//        lights[i].intensity / (attenuation.x + attenuation.y * lightDistance + attenuation.z * lightDistance * lightDistance);
        //finalColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
        Apply(finalColor, lightMultiplyer, input.normal, lightDir, viewDir);
    }
    float det = dot(-nor, planeNor);
    float t = dot(planeNor, input.worldPos.xyz - testAreaLight.p0) / det;

    float3 pa = input.worldPos.xyz + nor * t;

    if (det > 0.0f)
        return float4(1.0f, 0.0f, 0.0f, 1.0f);*/
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