#pragma once
#include "..\utils\Math.h"
#include <vector>
#include <memory>
#include "..\utils\Buffers.h"
#include "..\resource\Shader.h"

namespace thomas
{
	namespace object {
		namespace component {
			class LightComponent;
		}
	}
	namespace graphics
	{
		class LightManager
		{
		public:
			/*MATCH ON GPU*/
			struct LightCountsStruct
			{
				unsigned nrOfDirectionalLights;
				unsigned nrOfPointLights;
				unsigned nrOfSpotLights;
			};

			enum LIGHT_TYPES
			{
				DIRECTIONAL = 0,
				POINT = 1,
				SPOT = 2
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
			LightManager() = default;
			~LightManager() = default;

			static void Initialize();
			static void Destroy();
			static void AddLight(object::component::LightComponent* light);
			static bool RemoveLight(object::component::LightComponent* light);
			static void Update();

			static void Bind();
		private:

			static bool SortLights(object::component::LightComponent* light1, object::component::LightComponent* light2);

			static std::vector<object::component::LightComponent*> s_lights;

			static std::unique_ptr<utils::buffers::StructuredBuffer> m_lightBuffer;

			static LightCountsStruct m_lightsCounts;
		};
	}
}