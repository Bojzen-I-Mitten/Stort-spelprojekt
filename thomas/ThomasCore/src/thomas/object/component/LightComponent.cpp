#include "LightComponent.h"
#include "../../utils/d3d.h"
#include "../GameObject.h"
#include "Transform.h"

namespace thomas
{
	namespace object
	{
		namespace component
		{

			LightComponent::LightComponent()
			{
				type = LIGHT_TYPES::POINT;
			}
			LightComponent::~LightComponent()
			{
			}
			
			void LightComponent::Update()
			{
				if (type == LIGHT_TYPES::POINT)
				{
					//s_lightManager.AddPointLight(lightComponentData);
				}
				else if (type == LIGHT_TYPES::DIRECTIONAL)
				{
					//s_lightManager.AddPointLight(lightComponentData);
				}
				else if (type == LIGHT_TYPES::SPOT)
				{
					//s_lightManager.AddPointLight(lightComponentData);
				}
			}
			

			LightComponent::LIGHT_TYPES LightComponent::GetType()
			{
				return type;
			}

			void LightComponent::SetType(LIGHT_TYPES other)
			{
				type = other;
			}

			thomas::math::Color LightComponent::GetColor()
			{
				return lightComponentData.color;
			}

			void LightComponent::SetColor(thomas::math::Color other)
			{
				lightComponentData.color = other.ToVector3();
			}

			float LightComponent::GetIntensity()
			{
				return lightComponentData.intensity;
			}

			void LightComponent::SetIntensity(float value)
			{
				lightComponentData.intensity = value;
			}

			float LightComponent::GetSpotInnerAngle()
			{
				return lightComponentData.spotInnerAngle;
			}

			void LightComponent::SetSpotInnerAngle(float value)
			{
				lightComponentData.spotInnerAngle = value;
			}

			float LightComponent::GetSpotOuterAngle()
			{
				return lightComponentData.spotOuterAngle;
			}

			void LightComponent::SetSpotOuterAngle(float value)
			{
				lightComponentData.spotOuterAngle = value;
			}

			float LightComponent::GetConstantAttenuation()
			{
				return lightComponentData.attenuation.x;
			}

			void LightComponent::SetConstantAttenuation(float value)
			{
				lightComponentData.attenuation.x = value;
			}

			float LightComponent::GetLinearAttenuation()
			{
				return lightComponentData.attenuation.y;
			}

			void LightComponent::SetLinearAttenuation(float value)
			{
				lightComponentData.attenuation.y = value;
			}

			float LightComponent::GetQuadraticAttenuation()
			{
				return lightComponentData.attenuation.z;
			}

			void LightComponent::SetQuadraticAttenuation(float value)
			{
				lightComponentData.attenuation.z = value;
			}

		}
	}
}