#pragma once
#include "..\utils\Math.h"
#include <vector>
#include <memory>
#include "..\utils\Buffers.h"
#include "..\resource\Shader.h"
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
			static void Initialize();
			static void Reset();//call at the beginning of a frame, before the scripts update
			static void Destroy();
			static void AddPointLight(const LightStruct &light);
			static void AddSpotLight(const LightStruct &light);
			static void AddDirectionalLight(const LightStruct &light);

			static void Update();

			static void Bind();
		private:

			static std::vector<LightStruct> m_pointLights;
			static std::vector<LightStruct> m_spotLights;
			static std::vector<LightStruct> m_directionalLights;
			static std::vector<LightStruct> m_allLights;

			static std::shared_ptr<utils::buffers::StructuredBuffer> m_lightBuffer;

			static LightCountsStruct m_lightsCounts;
		};
	}
}