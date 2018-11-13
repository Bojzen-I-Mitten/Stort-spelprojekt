#include "LightManager.h"
#include "..\utils\D3D.h"
#include "..\resource\Shader.h"
#include "..\object\Object.h"
#include "..\object\component\LightComponent.h"
#include "..\utils\Buffers.h"
#include "..\resource\Shader.h"
#include "render/ShaderList.h"
#include "ChadowMapping.h"
#include <algorithm>
namespace thomas
{
	namespace graphics
	{
		
		std::vector<object::component::LightComponent*> LightManager::s_lights;
		std::unique_ptr<utils::buffers::StructuredBuffer> LightManager::s_lightBuffer;

		LightManager::LightCountsStruct LightManager::s_lightCounts;
		

		void LightManager::Initialize()
		{
			s_lightBuffer = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(LightStruct), 15, DYNAMIC_BUFFER);

			s_lightCounts.nrOfDirectionalLights = 0;
			s_lightCounts.nrOfSpotLights = 0;
			s_lightCounts.nrOfPointLights = 0;
			s_lightCounts.nrOfAreaLights = 0;

			ShadowMap::InitStatics();
		}
		
		void LightManager::Destroy()
		{
			SAFE_RELEASE(s_lightBuffer);
		}
		void LightManager::AddLight(object::component::LightComponent* light)
		{
			switch (light->GetType())
			{
			case DIRECTIONAL:
				s_lightCounts.nrOfDirectionalLights++;
				break;
			case POINT:
				s_lightCounts.nrOfPointLights++;
				break;
			case SPOT:
				s_lightCounts.nrOfSpotLights++;
				break;
			case AREA:
				s_lightCounts.nrOfAreaLights++;
				break;
			default:
				break;
			}
			s_lights.push_back(light);
			std::sort(s_lights.begin(), s_lights.end(), SortLights);
		}
		
		bool LightManager::RemoveLight(object::component::LightComponent * light)//returns true if light was removed
		{
			auto it = s_lights.begin();

			while (it != s_lights.end())
			{
				if (*it._Ptr == light)
				{
					switch (light->GetType())
					{
					case DIRECTIONAL:
						s_lightCounts.nrOfDirectionalLights--;
						break;
					case POINT:
						s_lightCounts.nrOfPointLights--;
						break;
					case SPOT:
						s_lightCounts.nrOfSpotLights--;
						break;
					case AREA:
						s_lightCounts.nrOfAreaLights--;
						break;
					default:
						break;
					}
					
					s_lights.erase(it);
					
					return true;
				}
				it++;
			}
			return false;
		}

		void LightManager::Update()
		{
			std::vector<LightStruct> allLights;

			for (object::component::LightComponent* light : s_lights)
			{
				allLights.push_back(light->GetData());
			}

			s_lightBuffer->SetData(allLights);
		}

		std::vector<object::component::LightComponent*> LightManager::GetLightsCastingShadows()
		{
			return s_lights;
		}

		void LightManager::Bind(render::ShaderList* shaders)
		{
			shaders->SetGlobalInt("nrOfPointLights", s_lightCounts.nrOfPointLights);
			shaders->SetGlobalInt("nrOfDirectionalLights", s_lightCounts.nrOfDirectionalLights);
			shaders->SetGlobalInt("nrOfSpotLights", s_lightCounts.nrOfSpotLights);
			shaders->SetGlobalInt("nrOfAreaLights", s_lightCounts.nrOfAreaLights);
			shaders->SetGlobalResource("lights", s_lightBuffer->GetSRV());
		}
		
		bool LightManager::SortLights(object::component::LightComponent * light1, object::component::LightComponent * light2)
		{
			if (light1->GetType() == light2->GetType())
			{
				return light1 < light2;
			}
			return light1->GetType() < light2->GetType();
		}
	}
}