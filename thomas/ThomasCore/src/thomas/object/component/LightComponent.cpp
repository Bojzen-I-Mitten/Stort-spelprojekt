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
				m_type = graphics::LightManager::DIRECTIONAL;

				m_lightComponentData.attenuation = math::Vector3(1.0f, 0.2f, 0.0f);
				m_lightComponentData.color = math::Vector3(1.0f, 1.0f, 1.0f);
				m_lightComponentData.position = math::Vector3(1.0f, 1.0f, 1.0f);
				m_lightComponentData.direction = math::Vector3(1.0f, 1.0f, 1.0f);
				m_lightComponentData.intensity = 1.0f;
				m_lightComponentData.spotInnerAngle = 0.0f;
				m_lightComponentData.spotOuterAngle = 20.0f;
			}
			LightComponent::~LightComponent()
			{
			}
			

			void LightComponent::Update()
			{
				m_lightComponentData.position = m_gameObject->m_transform->GetPosition();
				m_lightComponentData.direction = m_gameObject->m_transform->Forward();
				
			}

			void LightComponent::OnEnable()
			{
				graphics::LightManager::AddLight(this);
			}

			void LightComponent::OnDisable()
			{
				graphics::LightManager::RemoveLight(this);
			}

			
			

			graphics::LightManager::LIGHT_TYPES LightComponent::GetType()
			{
				return m_type;
			}

			void LightComponent::SetType(graphics::LightManager::LIGHT_TYPES other)
			{
				graphics::LightManager::RemoveLight(this);
				m_type = other;
				graphics::LightManager::AddLight(this);
			}

			thomas::math::Color LightComponent::GetColor()
			{
				return thomas::math::Color(m_lightComponentData.color.x, m_lightComponentData.color.y, m_lightComponentData.color.z, 1.0f);
			}

			void LightComponent::SetColor(thomas::math::Color other)
			{
				m_lightComponentData.color = other.ToVector3();
			}

			float LightComponent::GetIntensity()
			{
				return m_lightComponentData.intensity;
			}

			void LightComponent::SetIntensity(float value)
			{
				m_lightComponentData.intensity = value;
			}

			float LightComponent::GetSpotInnerAngle()
			{
				return m_lightComponentData.spotInnerAngle;
			}

			void LightComponent::SetSpotInnerAngle(float value)
			{
				m_lightComponentData.spotInnerAngle = value;
			}

			float LightComponent::GetSpotOuterAngle()
			{
				return m_lightComponentData.spotOuterAngle;
			}

			void LightComponent::SetSpotOuterAngle(float value)
			{
				m_lightComponentData.spotOuterAngle = value;
			}

			float LightComponent::GetConstantAttenuation()
			{
				return m_lightComponentData.attenuation.x;
			}

			void LightComponent::SetConstantAttenuation(float value)
			{
				m_lightComponentData.attenuation.x = value;
			}

			float LightComponent::GetLinearAttenuation()
			{
				return m_lightComponentData.attenuation.y;
			}

			void LightComponent::SetLinearAttenuation(float value)
			{
				m_lightComponentData.attenuation.y = value;
			}

			float LightComponent::GetQuadraticAttenuation()
			{
				return m_lightComponentData.attenuation.z;
			}

			void LightComponent::SetQuadraticAttenuation(float value)
			{
				m_lightComponentData.attenuation.z = value;
			}

		}
	}
}