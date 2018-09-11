#include "LightManager.h"
#include "..\utils\D3D.h"
#include "..\resource\Shader.h"

namespace thomas
{
	namespace graphics
	{
		void LightManager::Initialize()
		{
			m_lightBuffer = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(LightStruct), 25, DYNAMIC_BUFFER);

			m_lightsCounts.nrOfDirectionalLights = 0;
			m_lightsCounts.nrOfSpotLights = 0;
			m_lightsCounts.nrOfPointLights = 0;
			m_lightsCounts.pad = 0;
			//m_lightsCountsBuffer = std::make_unique<utils::buffers::StructuredBuffer>(&m_lightsCounts, sizeof(LightCountsStruct), 1, DYNAMIC_BUFFER);
		}
		void LightManager::Reset()
		{
			m_lightsCounts.nrOfDirectionalLights = 0;
			m_lightsCounts.nrOfSpotLights = 0;
			m_lightsCounts.nrOfPointLights = 0;

			m_directionalLights.clear();
			m_spotLights.clear();
			m_pointLights.clear();
			m_allLights.clear();
		}
		void LightManager::Destroy()
		{
			m_lightBuffer.release();
		}
		void LightManager::AddPointLight(const LightStruct & light)
		{
			m_lightsCounts.nrOfPointLights++;
			m_pointLights.push_back(light);
		}
		void LightManager::AddSpotLight(const LightStruct & light)
		{
			m_lightsCounts.nrOfSpotLights++;
			m_spotLights.push_back(light);
		}
		void LightManager::AddDirectionalLight(const LightStruct & light)
		{
			m_lightsCounts.nrOfDirectionalLights++;
			m_directionalLights.push_back(light);
		}
		void LightManager::BindLights()
		{
			m_allLights.insert(m_allLights.end(), m_pointLights.begin(), m_pointLights.end());
			m_allLights.insert(m_allLights.end(), m_spotLights.begin(), m_spotLights.end());
			m_allLights.insert(m_allLights.end(), m_directionalLights.begin(), m_directionalLights.end());

			m_lightBuffer->SetData(m_allLights);
			m_lightsCountsBuffer->SetData(&m_lightsCounts, sizeof(LightCountsStruct));
		}
	}
}