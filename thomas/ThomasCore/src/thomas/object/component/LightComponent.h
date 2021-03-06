#pragma once

#include "Component.h"

#include "../../utils/Math.h"
#include "../../graphics/LightManager.h"
#include "../../graphics/ChadowMapping.h"
/**
LightComponent class
*/


namespace thomas
{
	namespace graphics {
		class Mesh; namespace render { struct RenderCommand; }
	}
	namespace resource { namespace texture { class Texture2D; class Texture2DArray; } }
	namespace object 
	{
		namespace component
		{
			class Camera;
			class LightComponent : public Component
			{
			public:
			private:
				graphics::LightManager::LightStruct m_lightComponentData;

				graphics::LightManager::LIGHT_TYPES m_type;
				bool m_castsShadows;
				graphics::ShadowMap m_shadowMap;
			public:
				LightComponent();
				~LightComponent();

				virtual void Update() override;
				virtual void OnEnable() override;
				virtual void OnDisable() override;
				virtual void OnDestroy() override;
			private:

			public: // Get / Set
				void UpdateShadowBox(Camera* camera);
				void BindShadowMapDepthTexture();
				void DrawShadow(graphics::render::RenderCommand renderCommand);
				math::Matrix GetVPMat();
				int GetShadowMapIndex() const;

				graphics::LightManager::LightStruct GetData();

				graphics::LightManager::LIGHT_TYPES GetType();
				void SetType(graphics::LightManager::LIGHT_TYPES other);

				thomas::math::Color GetColorDiffuse();
				void SetColorDiffuse(thomas::math::Color other);
				thomas::math::Color GetColorSpecular();
				void SetColorSpecular(thomas::math::Color other);

				float GetIntensity() const;
				void SetIntensity(float const& value);
				
				float GetSpotInnerAngle() const;
				void SetSpotInnerAngle(float const& value);

				float GetSpotOuterAngle() const;
				void SetSpotOuterAngle(float const& value);

				float GetConstantAttenuation() const;
				void SetConstantAttenuation(float const& value);

				float GetLinearAttenuation() const;
				void SetLinearAttenuation(float const& value);

				float GetQuadraticAttenuation() const;
				void SetQuadraticAttenuation(float const& value);

				math::Vector2 GetRectangleDimensions() const;
				void SetRectangleDimensions(math::Vector2 const& value);

				bool CastsShadows() const;
				void SetCastShadows(bool const& value);

				float GetShadowHardness() const;
				void SetShadowHardness(float const& value);
			};

		}
	}

}