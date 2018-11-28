#ifndef THOMAS_LIGHTS
#define THOMAS_LIGHTS

#include <ThomasCG.hlsl>

cbuffer LightMatrices
{
    float4x4 LightMatricesVP[8] : MATRIXARRAY;
};

inline float4 WorldToLightClipPos(in float3 pos, in float shadowMapIndex)//, uint lightIndex)//temp dirlight id
{
    return mul(LightMatricesVP[(int) shadowMapIndex], float4(pos, 1.0));
}


SamplerState StandardClampSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = CLAMP;
    AddressV = CLAMP;
};
Texture2DArray ShadowMaps : SHADOWMAPS;


TextureCube environmentMap : ENVIRONMENTMAP;
SamplerState EnvironmentSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
    
};


//Texture2D ShadowMap : SHADOWMAP;

cbuffer LightCountsStruct
{
    uint nrOfDirectionalLights;
    uint nrOfPointLights;
    uint nrOfSpotLights;
    uint nrOfAreaLights;
    uint nrOfShadowMaps;
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
    
    float shadowMapIndex;
    float shadowHardness;
};


StructuredBuffer<LightStruct> lights;

inline float3 GetHalfwayVec(float3 lightDir, float3 viewDir)
{
    return normalize(viewDir + lightDir);
}

inline float3 Apply(float lightMultiplyer, float3 normal, float3 lightDir, float3 viewDir, float3 diffuse, float3 specular, float smoothness) 
{
    float lambertian = saturate(dot(normal, lightDir));
    float specularIntensity = 0.0f;
    if (lambertian > 0.0f)
    {
        specularIntensity = pow(saturate(dot(normal, GetHalfwayVec(viewDir, lightDir))), smoothness); //blinn-phong
    }
    
    return (diffuse * lambertian + specular * specularIntensity) * lightMultiplyer;
}
/* Fresnel approximation
*/
float3 FresnelSchlick(float3 specCol, float3 L, float3 H)
{
    const float OneOnLN2_x6 = 8.656170; // == 1/ln(2) * 6   (6 is SpecularPower of 5 + 1)
    // In this case SphericalGaussianApprox(1.0f - saturate(dot(L, H)), OneOnLN2_x6) is equal to exp2(-OneOnLN2_x6 * x)
    return specCol + (1.0f - specCol) * exp2(-OneOnLN2_x6 * saturate(dot(L, H)));
}
//E = L
float3 FresnelSchlickWithRoughness(float3 SpecularColor, float3 E, float3 N, float Gloss)
{
    return SpecularColor + (max(float3(Gloss,Gloss,Gloss), SpecularColor) - SpecularColor) * pow(1 - saturate(dot(E, N)), 5);
}

/* Ambient calculation
*/
float3 ambientSpec(in float3 V, in float3 N, in float3 R_world, in float3 specCol, in float specPow, in float gloss)
{
    float MipmapIndex = -1.66096404744368 * log(specPow) + 5.5;
    float3 IndirectSpecular = environmentMap.SampleLevel(EnvironmentSampler, R_world, MipmapIndex).rgb * specCol;
    return FresnelSchlickWithRoughness(specCol, N, V, gloss) * IndirectSpecular;
}

float3 ambient(in float3 V, in float3 N, in float3 R_world, in float3 diffCol, in float3 specCol, in float specPow, in float gloss, in float ambiTerm)
{
    return ambientSpec(V, N, R_world, specCol, specPow, gloss) + diffCol * ambiTerm;
}
/* Light calculation
*/
float3 light(in float3 V, in float3 N, in float3 L, in float3 lightDiff, in float3 diffCol, in float3 specCol, in float specPow)
{
    float dotNL = max(dot(L, N), 0.0f);
    float3 H = normalize(L + V);
    return lightDiff * (diffCol * dotNL + FresnelSchlick(specCol, L, H) * ((specPow + 2) / 8)
            * pow(saturate(dot(N, H)), specPow) * dotNL);
}

inline float3 AddLights(float3 worldPos, float3 worldNormal, float3 surfaceDiffuse, float3 specular, float smoothness)
{
    float3 viewDir = normalize(_WorldSpaceCameraPos - worldPos);
    float3 lightDir = float3(0, 0, 0);
    float lightMultiplyer = 0.0;
    
    const int poissonDiskSize = 4;
    static float2 poissonDisk[poissonDiskSize] = { float2(-0.94201624, -0.39906216), float2(0.94558609, -0.76890725), float2(-0.094184101, -0.92938870), float2(0.34495938, 0.29387760) };
    
    float specPow = exp2(10 * smoothness + 1); // smoothness: gloss
    float3 R_world = reflect(-viewDir, worldNormal);
    float3 colorAcculmulator = ambient(viewDir, worldNormal, R_world, surfaceDiffuse, specular, specPow, smoothness, 0.f);

    int i = 0;
    if (nrOfDirectionalLights) //directional
    {
        lightDir = lights[i].direction; //should be normalized already
        
        float shadowFactor = 1.0;
        float shadowMapIndex = lights[i].shadowMapIndex + 0.1;
        if (shadowMapIndex > 0)
        {
            float shadowHardnessFactor = lights[i].shadowHardness / (float) poissonDiskSize;

            float4 sampleCoordLS = WorldToLightClipPos(worldPos, shadowMapIndex);
            sampleCoordLS.xyz /= sampleCoordLS.w;
            sampleCoordLS.x = sampleCoordLS.x * 0.5 + 0.5;
            sampleCoordLS.y = 0.5 - sampleCoordLS.y * 0.5;
        
            float bias = 0.005 * tan(acos(saturate(dot(worldNormal, lightDir))));
            bias = clamp(bias, 0, 0.01);
            
            for (int si = 0; si < poissonDiskSize; ++si)
            {
                shadowFactor -= (float) (ShadowMaps.Sample(StandardClampSampler, float3(sampleCoordLS.xy + poissonDisk[si] / 700.0, shadowMapIndex)).x < sampleCoordLS.z - bias) * shadowHardnessFactor;
            }
        }
        colorAcculmulator = light(viewDir, worldNormal, lightDir, lights[i].colorDiffuse, surfaceDiffuse, specular, specPow) * shadowFactor;
    }
    /*
    // Puck multiple directional lights.
    int roof = nrOfPointLights;
    for (; i < roof; ++i) //point
    {
        lightDir = lights[i].position - worldPos;
        float lightDistance = length(lightDir);
        lightDir /= lightDistance;

        float3 atten = lights[i].attenuation;
        lightMultiplyer = lights[i].intensity / (atten.x + atten.y * lightDistance + atten.z * lightDistance * lightDistance);
        //colorAcculmulator += Apply(lightMultiplyer, worldNormal, lightDir, viewDir, surfaceDiffuse * lights[i].colorDiffuse, specularMapFactor * lights[i].colorSpecular, smoothness);
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
        //colorAcculmulator += Apply(lightMultiplyer, worldNormal, lightDir, viewDir, surfaceDiffuse * lights[i].colorDiffuse, specularMapFactor * lights[i].colorSpecular, smoothness);
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
        //colorAcculmulator += Apply(lightMultiplyer, worldNormal, lightDir, viewDir, surfaceDiffuse * lights[i].colorDiffuse, specularMapFactor * lights[i].colorSpecular, smoothness);
    }
    */
    return colorAcculmulator;
}

inline float2 Intensity(float3 normal, float3 worldPos)
{
    float3 lightDir = float3(0, 0, 0);
    float lightMultiplyer = 0.0f;
    float lambertian = 0.f;
    int i = 0;

    int roof = nrOfDirectionalLights;
    for (; i < roof; ++i) //directional
    {
        lightDir = lights[i].direction; //should be normalized already
        lightMultiplyer = lights[i].intensity;
        lambertian = saturate(dot(normal, lightDir));      
    }

    roof += nrOfPointLights;
    for (; i < roof; ++i) //point
    {
        lightDir = lights[i].position - worldPos;
        float lightDistance = length(lightDir);
        lightDir /= lightDistance;

        float3 atten = lights[i].attenuation;
        lightMultiplyer = lights[i].intensity / (atten.x + atten.y * lightDistance + atten.z * lightDistance * lightDistance);
        lambertian += saturate(dot(normal, lightDir));
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
        lambertian += saturate(dot(normal, lightDir));
    }

    roof += nrOfAreaLights;
    for (; i < roof; ++i)
    {
        float3 lightPos = lights[i].position + lights[i].direction * 0.0001f;

        //project the pixelpoint in the direction of the normal until it hits the area light plane
        float distanceForProjection = dot(normal, worldPos - lightPos);
        float3 projectionPos = worldPos - distanceForProjection * normal;
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
        lambertian = saturate(dot(normal, lightDir));
    }

    return float2(lambertian, lambertian);
}


#endif // THOMAS_LIGHTS
