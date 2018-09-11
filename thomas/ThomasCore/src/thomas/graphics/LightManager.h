#pragma once
#include "..\utils\Math.h"
#include <vector>
#include <memory>
#include "..\utils\Buffers.h"
//namespace thomas { namespace utils { namespace buffers { class StructuredBuffer; } } }
namespace thomas
{
	
	namespace graphics
	{
		class LightManager
		{
		public:
			/*MATCH ON GPU*/
			struct LightCountsStruct
			{
				unsigned nrOfPointLights;
				unsigned nrOfSpotLights;
				unsigned nrOfDirectionalLights;
				unsigned pad;
			};

			struct LightStruct
			{
				thomas::math::Vector3  color;
				float   intensity;
				thomas::math::Vector3  position;
				float   spotOuterAngle;
				thomas::math::Vector3  direction;
				float   spotInnerAngle;
				thomas::math::Vector3  attenuation;
				float   pad;
			};

		public:
			void Initialize();
			void Reset();//call at the beginning of a frame, before the scripts update
			void Destroy();
			void AddPointLight(const LightStruct &light);
			void AddSpotLight(const LightStruct &light);
			void AddDirectionalLight(const LightStruct &light);

			void Update();

			std::unique_ptr<utils::buffers::StructuredBuffer> m_lightBuffer;
			std::unique_ptr<utils::buffers::Buffer> m_lightsCountsBuffer;
		private:

			LightCountsStruct m_lightsCounts;
			std::vector<LightStruct> m_pointLights;
			std::vector<LightStruct> m_spotLights;
			std::vector<LightStruct> m_directionalLights;
			std::vector<LightStruct> m_allLights;

			
		};
	}
}