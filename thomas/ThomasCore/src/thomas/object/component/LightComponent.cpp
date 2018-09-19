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
				m_lightComponentData.direction = math::Vector3(0.0f, 0.0f, -1.0f);
				m_lightComponentData.right = math::Vector3(1.0f, 0.0f, 0.0f);
				m_lightComponentData.up = math::Vector3(0.0f, 1.0f, 0.0f);
				m_lightComponentData.intensity = 1.0f;
				m_lightComponentData.spotInnerAngle = 0.0f;
				m_lightComponentData.spotOuterAngle = 20.0f;
				m_lightComponentData.rectangleDimensions = math::Vector2(1.0f, 1.0f);
			}
			LightComponent::~LightComponent()
			{
			}

			graphics::LightManager::LightStruct LightComponent::GetData()
			{
				return m_lightComponentData;
			}
			

			void LightComponent::Update()
			{
				m_lightComponentData.position = m_gameObject->m_transform->GetPosition();
				m_lightComponentData.direction = m_gameObject->m_transform->Forward();
				m_lightComponentData.right = m_gameObject->m_transform->Right();
				m_lightComponentData.up = m_gameObject->m_transform->Up();
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
				if (graphics::LightManager::RemoveLight(this))
				{
					m_type = other;
					graphics::LightManager::AddLight(this);
				}
				else
					m_type = other;
			}

			thomas::math::Color LightComponent::GetColor()
			{
				return thomas::math::Color(m_lightComponentData.color.x, m_lightComponentData.color.y, m_lightComponentData.color.z, 1.0f);
			}

			void LightComponent::SetColor(thomas::math::Color other)
			{
				m_lightComponentData.color = other.ToVector3();
			}

			float LightComponent::GetIntensity() const
			{
				return m_lightComponentData.intensity;
			}

			void LightComponent::SetIntensity(float const& value)
			{
				m_lightComponentData.intensity = value;
			}

			float LightComponent::GetSpotInnerAngle() const
			{
				return m_lightComponentData.spotInnerAngle;
			}

			void LightComponent::SetSpotInnerAngle(float const& value)
			{
				m_lightComponentData.spotInnerAngle = value;
			}

			float LightComponent::GetSpotOuterAngle() const
			{
				return m_lightComponentData.spotOuterAngle;
			}

			void LightComponent::SetSpotOuterAngle(float const& value)
			{
				m_lightComponentData.spotOuterAngle = value;
			}

			float LightComponent::GetConstantAttenuation() const
			{
				return m_lightComponentData.attenuation.x;
			}

			void LightComponent::SetConstantAttenuation(float const& value)
			{
				m_lightComponentData.attenuation.x = value;
			}

			float LightComponent::GetLinearAttenuation() const
			{
				return m_lightComponentData.attenuation.y;
			}

			void LightComponent::SetLinearAttenuation(float const& value)
			{
				m_lightComponentData.attenuation.y = value;
			}

			float LightComponent::GetQuadraticAttenuation() const
			{
				return m_lightComponentData.attenuation.z;
			}

			void LightComponent::SetQuadraticAttenuation(float const& value)
			{
				m_lightComponentData.attenuation.z = value;
			}

			math::Vector2 LightComponent::GetRectangleDimensions() const
			{
				return m_lightComponentData.rectangleDimensions;
			}

			void LightComponent::SetRectangleDimensions(math::Vector2 const& value)
			{
				m_lightComponentData.rectangleDimensions = value;
			}

}
	}
}