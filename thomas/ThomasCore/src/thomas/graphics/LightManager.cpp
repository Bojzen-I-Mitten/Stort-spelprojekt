#include "LightManager.h"
#include "..\utils\D3D.h"
#include "..\resource\Shader.h"
#include "..\object\Object.h"
#include "..\object\component\LightComponent.h"

#include <algorithm>
namespace thomas
{
	namespace graphics
	{
	
		std::vector<object::component::LightComponent*> LightManager::s_lights;
		std::shared_ptr<utils::buffers::StructuredBuffer> LightManager::m_lightBuffer;

		LightManager::LightCountsStruct LightManager::m_lightsCounts;


		void LightManager::Initialize()
		{
			m_lightBuffer = std::make_shared<utils::buffers::StructuredBuffer>(nullptr, sizeof(LightStruct), 24, DYNAMIC_BUFFER);

			m_lightsCounts.nrOfDirectionalLights = 0;
			m_lightsCounts.nrOfSpotLights = 0;
			m_lightsCounts.nrOfPointLights = 0;
		}
		
		void LightManager::Destroy()
		{
			
		}
		void LightManager::AddLight(object::component::LightComponent* light)
		{
			switch (light->GetType())
			{
			case DIRECTIONAL:
				m_lightsCounts.nrOfDirectionalLights++;
				break;
			case POINT:
				m_lightsCounts.nrOfPointLights++;
				break;
			case SPOT:
				m_lightsCounts.nrOfSpotLights++;
				break;
			default:
				break;
			}
			s_lights.push_back(light);
			std::sort(s_lights.begin(), s_lights.end(), SortLights);
			int stoppper = 0;
		}
		
		void LightManager::RemoveLight(object::component::LightComponent * light)
		{
			auto it = s_lights.begin();

			while (it != s_lights.end())
			{
				if (*it._Ptr == light)
				{
					switch (light->GetType())
					{
					case DIRECTIONAL:
						m_lightsCounts.nrOfDirectionalLights--;
						break;
					case POINT:
						m_lightsCounts.nrOfPointLights--;
						break;
					case SPOT:
						m_lightsCounts.nrOfSpotLights--;
						break;
					default:
						break;
					}

					s_lights.erase(it);
					break;
				}
				it++;
			}
		}

		void LightManager::Update()
		{
			std::vector<LightStruct> m_allLights;

			for (object::component::LightComponent* light : s_lights)
			{
				m_allLights.push_back(light->GetData());
			}
			
			m_lightBuffer->SetData(m_allLights);
		}
		void LightManager::Bind()
		{
			resource::Shader::SetGlobalInt("nrOfPointLights", m_lightsCounts.nrOfPointLights);
			resource::Shader::SetGlobalInt("nrOfDirectionalLights", m_lightsCounts.nrOfDirectionalLights);
			resource::Shader::SetGlobalInt("nrOfSpotLights", m_lightsCounts.nrOfSpotLights);
			resource::Shader::SetGlobalResource("lights", m_lightBuffer->GetSRV());
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