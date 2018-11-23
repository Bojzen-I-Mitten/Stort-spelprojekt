#pragma once
#include "..\utils\Math.h"
#include <vector>
#include <memory>
#include "render/ShaderList.h"
#include <d3d11.h>

namespace thomas
{
	namespace object {
		namespace component {
			class LightComponent;
			class Camera;
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
			struct CameraRenderQueue;
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
				float shadowMapIndex;
				float shadowHardness;
			};

		public:
			static void Initialize();
			static void Destroy();
			static void AddLight(object::component::LightComponent* light);
			static bool RemoveLight(object::component::LightComponent* light);
			static void Update();

			static void DrawShadows(render::CameraRenderQueue& renderQueue, object::component::Camera* camera);
			static void BindLights(render::ShaderList* shaders);
			static void BindShadows(render::ShaderList* shaders);
			static int GetFreeShadowMapView(ID3D11DepthStencilView*& dsv);
			static bool ResturnShadowMapView(ID3D11DepthStencilView * dsv);
		private:
			static const unsigned s_nrOfShadowMapsSupported = 8;
			static ID3D11DepthStencilView* s_shadowMapViews[s_nrOfShadowMapsSupported];
			static math::Matrix s_lightMatrices[s_nrOfShadowMapsSupported];
			static std::vector<unsigned> s_freeShadowMapViewIndexes;

			static bool SortLights(object::component::LightComponent* light1, object::component::LightComponent* light2);

			static std::vector<object::component::LightComponent*> s_lights;
			static std::unique_ptr<utils::buffers::StructuredBuffer> s_lightBuffer;
			static LightCountsStruct s_lightCounts;

			
			static resource::Texture2DArray* s_shadowMapTextures;
			static unsigned s_shadowMapSize;
		};
	}
}