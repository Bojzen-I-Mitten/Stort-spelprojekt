#include "LightManager.h"
#include "..\utils\D3D.h"
#include "..\resource\Shader.h"

namespace thomas
{
	namespace graphics
	{
		std::vector<LightManager::LightStruct> LightManager::m_pointLights;
		std::vector<LightManager::LightStruct> LightManager::m_spotLights;
		std::vector<LightManager::LightStruct> LightManager::m_directionalLights;
		std::vector<LightManager::LightStruct> LightManager::m_allLights;

		std::shared_ptr<utils::buffers::StructuredBuffer> LightManager::m_lightBuffer;

		LightManager::LightCountsStruct LightManager::m_lightsCounts;


		void LightManager::Initialize()
		{
			m_lightBuffer = std::make_shared<utils::buffers::StructuredBuffer>(nullptr, sizeof(LightStruct), 24, DYNAMIC_BUFFER);

			m_lightsCounts.nrOfDirectionalLights = 0;
			m_lightsCounts.nrOfSpotLights = 0;
			m_lightsCounts.nrOfPointLights = 0;
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
		void LightManager::Update()
		{
			if (m_directionalLights.size() > 0)
				m_allLights.insert(m_allLights.end(), m_directionalLights.begin(), m_directionalLights.end());
			if (m_pointLights.size() > 0)
				m_allLights.insert(m_allLights.end(), m_pointLights.begin(), m_pointLights.end());
			if (m_spotLights.size() > 0)
				m_allLights.insert(m_allLights.end(), m_spotLights.begin(), m_spotLights.end());
			
			m_lightBuffer->SetData(m_allLights);
		}
		void LightManager::Bind()
		{
			resource::Shader::SetGlobalInt("nrOfPointLights", m_lightsCounts.nrOfPointLights);
			resource::Shader::SetGlobalInt("nrOfDirectionalLights", m_lightsCounts.nrOfDirectionalLights);
			resource::Shader::SetGlobalInt("nrOfSpotLights", m_lightsCounts.nrOfSpotLights);
			resource::Shader::SetGlobalResource("lights", m_lightBuffer->GetSRV());
		}
	}
}