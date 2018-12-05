#include "LightComponent.h"
#include "../../utils/d3d.h"
#include "../GameObject.h"
#include "Transform.h"
#include "../../graphics/Mesh.h"
#include "Camera.h"
#include "../../graphics/render/Frame.h"
#include "../../resource/texture/Texture2D.h"


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
				m_lightComponentData.colorDiffuse = math::Vector3(1.0f, 1.0f, 1.0f);
				m_lightComponentData.colorSpecular = math::Vector3(1.0f, 1.0f, 1.0f);
				m_lightComponentData.position = math::Vector3(1.0f, 1.0f, 1.0f);
				m_lightComponentData.direction = math::Vector3(0.0f, 0.0f, -1.0f);
				m_lightComponentData.right = math::Vector3(1.0f, 0.0f, 0.0f);
				m_lightComponentData.up = math::Vector3(0.0f, 1.0f, 0.0f);
				m_lightComponentData.intensity = 0.3f;
				m_lightComponentData.spotInnerAngle = 0.0f;
				m_lightComponentData.spotOuterAngle = 20.0f;
				m_lightComponentData.rectangleDimensions = math::Vector2(1.0f, 1.0f);
				m_castsShadows = false;
				m_lightComponentData.shadowMapIndex = -1;
				m_lightComponentData.shadowHardness = 0.5f;
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
				m_lightComponentData.position = m_gameObject->GetTransform()->GetPosition();
				m_lightComponentData.direction = m_gameObject->GetTransform()->Forward();
				m_lightComponentData.right = m_gameObject->GetTransform()->Right();
				m_lightComponentData.up = m_gameObject->GetTransform()->Up();
			}

			void LightComponent::OnEnable()
			{
				graphics::LightManager::AddLight(this);
			}

			void LightComponent::OnDisable()
			{
				graphics::LightManager::RemoveLight(this);
			}

			void LightComponent::OnDestroy()
			{
				SetCastShadows(false);//returns the shadowmap
				graphics::LightManager::RemoveLight(this);
			}

			void LightComponent::UpdateShadowBox(Camera* camera)
			{
				m_shadowMap.UpdateShadowBox(m_gameObject->GetTransform(), camera);
			}

			void LightComponent::BindShadowMapDepthTexture()
			{
				m_shadowMap.Bind();
			}

			void LightComponent::DrawShadow(graphics::render::RenderCommand renderCommand)
			{
				m_shadowMap.Draw(renderCommand);
			}

			math::Matrix LightComponent::GetVPMat()
			{
				return m_shadowMap.GetVPMat();
			}

			int LightComponent::GetShadowMapIndex() const
			{
				return m_lightComponentData.shadowMapIndex;
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

			thomas::math::Color LightComponent::GetColorDiffuse()
			{
				return thomas::math::Color(m_lightComponentData.colorDiffuse.x, m_lightComponentData.colorDiffuse.y, m_lightComponentData.colorDiffuse.z, 1.0f);
			}

			void LightComponent::SetColorDiffuse(thomas::math::Color other)
			{
				m_lightComponentData.colorDiffuse = other.ToVector3();
			}

			thomas::math::Color LightComponent::GetColorSpecular()
			{
				return thomas::math::Color(m_lightComponentData.colorSpecular.x, m_lightComponentData.colorSpecular.y, m_lightComponentData.colorSpecular.z, 1.0f);
			}

			void LightComponent::SetColorSpecular(thomas::math::Color other)
			{
				m_lightComponentData.colorSpecular = other.ToVector3();
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

			bool LightComponent::CastsShadows() const
			{
				return m_castsShadows;
			}

			void LightComponent::SetCastShadows(bool const & value)
			{
				m_castsShadows = value;

				if (m_castsShadows)
				{
					ID3D11DepthStencilView* dsv;
					m_lightComponentData.shadowMapIndex = graphics::LightManager::GetFreeShadowMapView(dsv);
					if (m_lightComponentData.shadowMapIndex == -1)
					{
						m_castsShadows = false;

					}
					else
						m_shadowMap.SetShadowMapDepthStencilView(dsv);
				}
				else
				{
					graphics::LightManager::ResturnShadowMapView(m_shadowMap.GetShadowMapDepthStencilView());
					m_shadowMap.SetShadowMapDepthStencilView(nullptr);
					m_lightComponentData.shadowMapIndex = -1;
				}
				
			}

			float LightComponent::GetShadowHardness() const
			{
				return m_lightComponentData.shadowHardness;
			}

			void LightComponent::SetShadowHardness(float const & value)
			{
				m_lightComponentData.shadowHardness = value;
			}

}
	}
}