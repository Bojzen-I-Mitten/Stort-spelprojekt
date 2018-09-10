#include "LightManager.h"
#include "..\utils\D3D.h"
#include "..\resource\Shader.h"

namespace thomas
{
	namespace graphics
	{
		ID3D11Buffer* LightManager::s_lightBuffer;




		void LightManager::Destroy()
		{
			SAFE_RELEASE(s_lightBuffer);
			s_lightBuffer = NULL;
			s_lightstruct.nrOfDirectionalLights = 0;
			s_lightstruct.nrOfPointLights = 0;
		}
		void LightManager::AddPointLight(const LightStruct & light)
		{
			lightsCounts.nrOfPointLights++;
			pointLights.push_back(light);
		}
		void LightManager::AddSpotLight(const LightStruct & light)
		{
			lightsCounts.nrOfSpotLights++;
			spotLights.push_back(light);
		}
		void LightManager::AddDirectionalLight(const LightStruct & light)
		{
			lightsCounts.nrOfDirectionalLights++;
			directionalLights.push_back(light);
		}
		void LightManager::BindLights()
		{
			allLights.insert(allLights.end(), pointLights.begin(), pointLights.end());
			allLights.insert(allLights.end(), spotLights.begin(), spotLights.end());
			allLights.insert(allLights.end(), directionalLights.begin(), directionalLights.end());



		}
	}
}