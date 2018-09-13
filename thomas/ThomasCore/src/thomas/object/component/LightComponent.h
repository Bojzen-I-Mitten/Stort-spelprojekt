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
				

				float GetIntensity();
				void SetIntensity(float value);
				
				float GetSpotInnerAngle();
				void SetSpotInnerAngle(float value);

				float GetSpotOuterAngle();
				void SetSpotOuterAngle(float value);

				float GetConstantAttenuation();
				void SetConstantAttenuation(float value);

				float GetLinearAttenuation();
				void SetLinearAttenuation(float value);

				float GetQuadraticAttenuation();
				void SetQuadraticAttenuation(float value);

				


			};

		}
	}

}