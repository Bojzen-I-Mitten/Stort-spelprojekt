#ifndef THOMAS_LIGHTS
#define THOMAS_LIGHTS

#include <ThomasCG.hlsl>

cbuffer LightMatrices
{
    float4x4 lightMatrixVP; //[4];
};

inline float4 ObjectToLightClipPos(in float3 pos)//, uint lightIndex)//temp dirlight id
{
    return mul(lightMatrixVP, mul(thomas_ObjectToWorld, float4(pos, 1.0)));
}

inline float4 WorldToLightClipPos(in float3 pos)//, uint lightIndex)//temp dirlight id
{
    return mul(lightMatrixVP, float4(pos, 1.0));
}


SamplerState StandardClampSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};
//Texture2DArray ShadowMaps;
Texture2D ShadowMap : SHADOWMAP;

cbuffer LightCountsStruct
{
    uint nrOfDirectionalLights;
    uint nrOfPointLights;
    uint nrOfSpotLights;
    uint nrOfAreaLights;
};

struct LightStruct
{
    float3 colorDiffuse;
    float3 colorSpecular;
    float intensity;

    float3 position;
    float spotOuterAngle;

    float3 direction;
    float spotInnerAngle;

    float3 attenuation;
    
    //For area lights
    float3 right;
    float3 up;
    float2 rectangleDimension;
    float2 pad;
};


StructuredBuffer<LightStruct> lights;

inline float3 GetHalfwayVec(float3 lightDir, float3 viewDir)
{
    return normalize(viewDir + lightDir);
}

inline void Apply(inout float3 colorAcculmulator, float lightMultiplyer, float3 normal, float3 lightDir, float3 viewDir, float3 diffuse, float3 specular, float smoothness) 
{
    float lambertian = saturate(dot(normal, lightDir));
    float specularIntensity = 0.0f;
    if (lambertian > 0.0f)
    {
        specularIntensity = pow(saturate(dot(normal, GetHalfwayVec(viewDir, lightDir))), smoothness); //blinn-phong
    }
    
    colorAcculmulator += (diffuse * lambertian + specular * specularIntensity) * lightMultiplyer;
}

inline float3 AddLights(float3 worldPos, float3 worldNormal, float3 surfaceDiffuse, float specularMapFactor, float smoothness)
{
    float3 viewDir = normalize(_WorldSpaceCameraPos - worldPos);
    float3 ambient = float3(0.1f, 0.1f, 0.1f);
    float3 colorAcculmulator = ambient;
    float3 lightDir = float3(0, 0, 0);
    float lightMultiplyer = 0.0f;
    
    int i = 0;
    int roof = nrOfDirectionalLights;
    for (; i < roof; ++i) //directional
    {
        lightDir = lights[i].direction; //should be normalized already
        
        float4 sampleCoordLS = WorldToLightClipPos(worldPos);
        sampleCoordLS.xyz /= sampleCoordLS.w;
        sampleCoordLS.x = (sampleCoordLS.x + 1.0f) / 2.0f;
        sampleCoordLS.y = 1.0 - (sampleCoordLS.y + 1.0f) / 2.0f;
        
        float shadowFactor = 1.0 - (float) (ShadowMap.Sample(StandardClampSampler, sampleCoordLS.xy).x < sampleCoordLS.z - 0.0005)*0.85;
        

        lightMultiplyer = lights[i].intensity * shadowFactor;
        Apply(colorAcculmulator, lightMultiplyer, worldNormal, lightDir, viewDir, surfaceDiffuse * lights[i].colorDiffuse, specularMapFactor * lights[i].colorSpecular, smoothness);
    }
    roof += nrOfPointLights;
    for (; i < roof; ++i) //point
    {
        lightDir = lights[i].position - worldPos;
        float lightDistance = length(lightDir);
        lightDir /= lightDistance;

        float3 atten = lights[i].attenuation;
        lightMultiplyer = lights[i].intensity / (atten.x + atten.y * lightDistance + atten.z * lightDistance * lightDistance);
        Apply(colorAcculmulator, lightMultiplyer, worldNormal, lightDir, viewDir, surfaceDiffuse * lights[i].colorDiffuse, specularMapFactor * lights[i].colorSpecular, smoothness);
    }
    roof += nrOfSpotLights;
    for (; i < roof; ++i) //spot
    {
        lightDir = lights[i].position - worldPos;
        float lightDistance = length(lightDir);
        lightDir /= lightDistance;

        float angle = degrees(acos(dot(lights[i].direction, lightDir))); //optimize
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
        
        float3 atten = lights[i].attenuation;
        lightMultiplyer = spotFactor * lights[i].intensity / (atten.x + atten.y * lightDistance + atten.z * lightDistance * lightDistance);
        Apply(colorAcculmulator, lightMultiplyer, worldNormal, lightDir, viewDir, surfaceDiffuse * lights[i].colorDiffuse, specularMapFactor * lights[i].colorSpecular, smoothness);
    }
    roof += nrOfAreaLights;
    for (; i < roof; ++i)
    {
        float3 lightPos = lights[i].position + lights[i].direction * 0.0001f;

        //project the pixelpoint in the direction of the normal until it hits the area light plane
        float distanceForProjection = dot(worldNormal, worldPos - lightPos);
        float3 projectionPos = worldPos - distanceForProjection * worldNormal;
        float3 lightToPoint = projectionPos - lightPos;

        //clamp light boundaries
        float3 lightRight = lights[i].right;
        float3 lightUp = lights[i].up;
        float2 rectDimension = lights[i].rectangleDimension;
        float2 rectInLightPlane = float2(dot(lightToPoint, lightRight), dot(lightToPoint, lightUp));
        float2 clampedRect = float2(clamp(rectInLightPlane.x, -rectDimension.x, rectDimension.x), clamp(rectInLightPlane.y, -rectDimension.y, rectDimension.y));
        float3 nearestPoint = lightPos + lightRight * clampedRect.x + lightUp * clampedRect.y;
    
        lightDir = normalize(nearestPoint - worldPos);
        float lightDistance = length(lightDir);
        lightDir /= lightDistance;

        float3 atten = lights[i].attenuation;
        lightMultiplyer = lights[i].intensity / (atten.x + atten.y * lightDistance + atten.z * lightDistance * lightDistance);
        Apply(colorAcculmulator, lightMultiplyer, worldNormal, lightDir, viewDir, surfaceDiffuse * lights[i].colorDiffuse, specularMapFactor * lights[i].colorSpecular, smoothness);
    }
    return colorAcculmulator;
}

#endif // THOMAS_LIGHTS
