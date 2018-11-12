#pragma once
#include "..\utils\Math.h"
#include <vector>
#include <memory>
#include "render/ShaderList.h"

namespace thomas
{
	namespace object {
		namespace component {
			class LightComponent;
		}
	}
	namespace utils {
		namespace buffers {
			class StructuredBuffer;
		}
	}

	namespace graphics
	{
		namespace render {
			class ShaderList;
		}

		class LightManager
		{
		public:
			/*MATCH ON GPU*/
			struct LightCountsStruct
			{
				unsigned nrOfDirectionalLights;
				unsigned nrOfPointLights;
				unsigned nrOfSpotLights;
				unsigned nrOfAreaLights;
			};

			enum LIGHT_TYPES
			{
				DIRECTIONAL = 0,
				POINT = 1,
				SPOT = 2,
				AREA = 3
			};


			struct LightStruct
			{
				thomas::math::Vector3  colorDiffuse;
				thomas::math::Vector3  colorSpecular;
				float   intensity;
				thomas::math::Vector3  position;
				float   spotOuterAngle;
				thomas::math::Vector3  direction;
				float   spotInnerAngle;
				thomas::math::Vector3  attenuation;
				//for area lights
				thomas::math::Vector3 right;
				thomas::math::Vector3 up;
				thomas::math::Vector2 rectangleDimensions;
				thomas::math::Vector2 pad;
			};

		public:
			static void Initialize();
			static void Destroy();
			static void AddLight(object::component::LightComponent* light);
			static bool RemoveLight(object::component::LightComponent* light);
			static void Update();
			static std::vector<object::component::LightComponent*> GetLightsCastingShadows();
			static void Bind(render::ShaderList* shaders);
		private:

			static bool SortLights(object::component::LightComponent* light1, object::component::LightComponent* light2);

			static std::vector<object::component::LightComponent*> s_lights;

			static std::unique_ptr<utils::buffers::StructuredBuffer> s_lightBuffer;

			static LightCountsStruct s_lightCounts;
		};
	}
}