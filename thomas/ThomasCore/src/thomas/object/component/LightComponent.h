#pragma once

#include "Component.h"

#include "../../utils/Math.h"
#include "../../graphics/LightManager.h"
/**
LightComponent class
*/


namespace thomas
{
	namespace object 
	{
		namespace component
		{
			
			class THOMAS_API LightComponent : public Component
			{
			public:
			private:
				graphics::LightManager::LightStruct m_lightComponentData;

				graphics::LightManager::LIGHT_TYPES m_type;
				
			public:
				void Update();
				void OnEnable();
				void OnDisable();
				LightComponent();
				~LightComponent();

				graphics::LightManager::LightStruct GetData();

				graphics::LightManager::LIGHT_TYPES GetType();
				void SetType(graphics::LightManager::LIGHT_TYPES other);

				thomas::math::Color GetColor();
				void SetColor(thomas::math::Color other);
				

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


			};

		}
	}

}