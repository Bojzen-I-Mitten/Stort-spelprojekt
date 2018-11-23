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
#include "../resource/texture/Texture2DArray.h"
#include "render/Frame.h"

namespace thomas
{
	namespace graphics
	{
		
		std::vector<object::component::LightComponent*> LightManager::s_lights;
		std::unique_ptr<utils::buffers::StructuredBuffer> LightManager::s_lightBuffer;
		resource::Texture2DArray* LightManager::s_shadowMapTextures;
		LightManager::LightCountsStruct LightManager::s_lightCounts;
		utils::buffers::StructuredBuffer* LightManager::s_shadowLightVPMatrices;

		const unsigned LightManager::s_nrOfShadowMapsSupported;
		unsigned LightManager::s_shadowMapSize;
		ID3D11DepthStencilView* LightManager::s_shadowMapViews[LightManager::s_nrOfShadowMapsSupported];
		math::Matrix LightManager::s_lightMatrices[LightManager::s_nrOfShadowMapsSupported];
		std::vector<unsigned> LightManager::s_freeShadowMapViewIndexes;

		void LightManager::Initialize()
		{
			s_lightBuffer = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(LightStruct), 25, DYNAMIC_BUFFER);

			s_lightCounts.nrOfDirectionalLights = 0;
			s_lightCounts.nrOfSpotLights = 0;
			s_lightCounts.nrOfPointLights = 0;
			s_lightCounts.nrOfAreaLights = 0;


			s_shadowMapSize = 1024;
			s_shadowMapTextures = new resource::Texture2DArray(s_shadowMapSize, s_shadowMapSize, DXGI_FORMAT_R32_TYPELESS, s_nrOfShadowMapsSupported, true);


			D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc = {};
			depthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
			depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			depthViewDesc.Texture2DArray.ArraySize = 1u;
			for (unsigned i = 0; i < s_nrOfShadowMapsSupported; ++i)
			{
				depthViewDesc.Texture2DArray.FirstArraySlice = i;
				
				HRESULT hr = utils::D3D::Instance()->GetDevice()->CreateDepthStencilView(s_shadowMapTextures->GetResource(), &depthViewDesc, &s_shadowMapViews[i]);

				s_freeShadowMapViewIndexes.push_back(i);
				s_lightMatrices[i] = math::Matrix::Identity;
			}



			ShadowMap::InitStatics(s_shadowMapSize);
		}
		
		void LightManager::Destroy()
		{
			for (unsigned i = 0; i < s_nrOfShadowMapsSupported; ++i)
			{
				SAFE_RELEASE(s_shadowMapViews[i]);
			}
			//delete s_shadowMapTextures;
			SAFE_RELEASE(s_lightBuffer);
			s_lights.clear();
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
					
					*it._Ptr = nullptr;
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

		void LightManager::DrawShadows(render::CameraRenderQueue & renderQueue, object::component::Camera* camera)
		{
			for (object::component::LightComponent* l : s_lights)
			{
				if (l)
				{

					if (l->CastsShadows())
					{
						if (l->GetType() != LIGHT_TYPES::DIRECTIONAL)//right now only works for dir lights
							return;


						l->UpdateShadowBox(camera);
						l->BindShadowMapDepthTexture();


						for (auto & perMaterialQueue : renderQueue.m_commands3D)
						{

							for (auto & perMeshCommand : perMaterialQueue.second)
							{
								{

									l->DrawShadow(perMeshCommand);
								}
							}
						}
					}
				}
			}
		}

		void LightManager::BindLights(render::ShaderList* shaders)
		{
			shaders->SetGlobalInt("nrOfPointLights", s_lightCounts.nrOfPointLights);
			shaders->SetGlobalInt("nrOfDirectionalLights", s_lightCounts.nrOfDirectionalLights);
			shaders->SetGlobalInt("nrOfSpotLights", s_lightCounts.nrOfSpotLights);
			shaders->SetGlobalInt("nrOfAreaLights", s_lightCounts.nrOfAreaLights);
			shaders->SetGlobalInt("nrOfShadowMaps", s_nrOfShadowMapsSupported - s_freeShadowMapViewIndexes.size());
			shaders->SetGlobalResource("lights", s_lightBuffer->GetSRV());
		}

		void LightManager::BindShadows(render::ShaderList * shaders)
		{
			int nrOfUsedShadowMaps = s_nrOfShadowMapsSupported - s_freeShadowMapViewIndexes.size();
			if (nrOfUsedShadowMaps == 0)
				return;


			shaders->SetGlobalTexture2DArray("ShadowMaps", s_shadowMapTextures);
			
			for (object::component::LightComponent* l : s_lights)
			{
				if (l->CastsShadows())
				{
					s_lightMatrices[l->GetShadowMapIndex()] = l->GetVPMat();
				}
			}

			shaders->SetGlobalMatrixArray("LightMatricesVP", s_lightMatrices, s_nrOfShadowMapsSupported);
		}

		/*ID3D11DepthStencilView * LightManager::GetFreeShadowMapView()
		{
			if (s_freeShadowMapViews.size() <= 0)
			{
				//LOG("Not enough shadowmaps");
				return nullptr;
			}
			ID3D11DepthStencilView* dsv = s_freeShadowMapViews[s_freeShadowMapViews.size() - 1];
			s_usedShadowMapViews.push_back(dsv);
			s_freeShadowMapViews.pop_back();
			return dsv;
		}*/

		int LightManager::GetFreeShadowMapView(ID3D11DepthStencilView*& dsv)
		{
			if (s_freeShadowMapViewIndexes.size() == 0)
			{
				//LOG("Not enough shadowmaps");
				return -1;
			}
			auto it = s_freeShadowMapViewIndexes.begin();
			unsigned freeIndex = *it._Ptr;
			s_freeShadowMapViewIndexes.erase(it);

			dsv = s_shadowMapViews[freeIndex];
			return freeIndex;
		}

		bool LightManager::ResturnShadowMapView(ID3D11DepthStencilView * dsv)
		{
			for (unsigned i = 0; i < s_nrOfShadowMapsSupported; ++i)
			{
				if (dsv == s_shadowMapViews[i])
				{
					s_freeShadowMapViewIndexes.push_back(i);
					return true;
				}
			}
			return false;
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