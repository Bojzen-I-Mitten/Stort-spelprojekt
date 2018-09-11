#include "LightComponent.h"
#include "../../utils/d3d.h"
#include "../GameObject.h"
#include "Transform.h"



thomas::graphics::LightManager thomas::object::component::LightComponent::s_lightManager;


namespace thomas
{
	namespace object
	{
		namespace component
		{

			

			LightComponent::LightComponent()
			{
				type = LIGHT_TYPES::POINT;

				lightComponentData.attenuation = math::Vector3(1.0f, 0.2f, 0.0f);
				lightComponentData.color = math::Vector3(1.0f, 1.0f, 1.0f);
				lightComponentData.intensity = 1.0f;
				lightComponentData.spotInnerAngle = 0.0f;
				lightComponentData.spotOuterAngle = 20.0f;
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
					//s_lightManager.AddDirectionalLight(lightComponentData);
				}
				else if (type == LIGHT_TYPES::SPOT)
				{
					//s_lightManager.AddSpotLight(lightComponentData);
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