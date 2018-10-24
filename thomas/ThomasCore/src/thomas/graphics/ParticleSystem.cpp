#include "ParticleSystem.h"
#include "../resource/ComputeShader.h"
#include <random>
#include <time.h>
#include "../resource/texture/Texture2DArray.h"

namespace thomas
{
	namespace graphics
	{
		std::shared_ptr<ParticleSystem> ParticleSystem::s_globalAlphaBlendingSystem;
		std::shared_ptr<ParticleSystem> ParticleSystem::s_globalAdditiveBlendingSystem;

		void ParticleSystem::InitializeGlobalSystems()
		{
			s_globalAlphaBlendingSystem = std::make_shared<ParticleSystem>();
			s_globalAlphaBlendingSystem->Initialize(8192, ALPHA);

			s_globalAdditiveBlendingSystem = std::make_shared<ParticleSystem>();
			s_globalAdditiveBlendingSystem->Initialize(8192, ADDITIVE);

			std::srand(time(NULL));
		}


		std::shared_ptr<ParticleSystem> ParticleSystem::GetGlobalAlphaBlendingSystem()
		{
			return s_globalAlphaBlendingSystem;
		}

		std::shared_ptr<ParticleSystem> ParticleSystem::GetGlobalAdditiveBlendingSystem()
		{
			return s_globalAdditiveBlendingSystem;
		}

		void ParticleSystem::DestroyGlobalSystems()
		{
			s_globalAlphaBlendingSystem->Destroy();
			s_globalAdditiveBlendingSystem->Destroy();
		}
		ParticleSystem::ParticleSystem()
		{

		}

		ParticleSystem::~ParticleSystem()
		{

		}

		void ParticleSystem::Initialize(unsigned maxNrOfParticles, BLEND_STATE blendState)
		{
			m_maxNrOfParticles = maxNrOfParticles;
			m_emitParticlesCS = (resource::ComputeShader*)resource::ComputeShader::CreateShader("../Data/FXIncludes/emitParticlesCS.fx");
			m_updateParticlesCS = (resource::ComputeShader*)resource::ComputeShader::CreateShader("../Data/FXIncludes/updateParticlesCS.fx");
			resource::ComputeShader* initCS = (resource::ComputeShader*)resource::ComputeShader::CreateShader("../Data/FXIncludes/appendIndexes.fx");
			m_calculateEmitCountCS = (resource::ComputeShader*)resource::ComputeShader::CreateShader("../Data/FXIncludes/calculateEmitCountCS.fx");

			//PARTICLE BUFFERS
			m_bufferSpawn =			std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(InitParticleBufferStruct), 32, DYNAMIC_BUFFER);//ammount of emiting emitters supported at once			
			m_bufferSpawnIndex =	std::make_unique<utils::buffers::Buffer>(nullptr, sizeof(int) * 4, D3D11_BIND_CONSTANT_BUFFER, DYNAMIC_BUFFER);

			m_bufferUpdate =		std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(ParticleStruct), maxNrOfParticles, STATIC_BUFFER, D3D11_BIND_FLAG(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS));//ammount of particles supported for entire system 
			m_bufferBillboard =		std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(BillboardStruct), maxNrOfParticles, STATIC_BUFFER, D3D11_BIND_FLAG(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS));

			//INDEXING APPEND CONSUME BUFFERS
			m_bufferDeadList =		std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(unsigned int), maxNrOfParticles, STATIC_BUFFER, D3D11_BIND_FLAG(D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE), D3D11_BUFFER_UAV_FLAG_APPEND);
			m_bufferAliveListPing = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(unsigned int), maxNrOfParticles, STATIC_BUFFER, D3D11_BIND_FLAG(D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE), D3D11_BUFFER_UAV_FLAG_APPEND);
			m_bufferAliveListPong = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(unsigned int), maxNrOfParticles, STATIC_BUFFER, D3D11_BIND_FLAG(D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE), D3D11_BUFFER_UAV_FLAG_APPEND);
			
			//INIT DEAD LIST
			initCS->SetGlobalInt("maxNrOfParticles", maxNrOfParticles);
			initCS->SetGlobalUAV("deadlist", m_bufferDeadList->GetUAV());

			initCS->Bind();
			initCS->SetPass(0);
			initCS->Dispatch((maxNrOfParticles + 511) / 512);
			
			resource::ComputeShader::UnbindOneUAV(0);

			resource::Shader::DestroyShader(initCS);
			

			//COUNTER BUFFER
			unsigned* counterInitVals = new unsigned[4] {0, maxNrOfParticles, maxNrOfParticles, 0};
			
			m_bufferCounters = std::make_unique<utils::buffers::ByteAddressBuffer>(sizeof(unsigned), 4, counterInitVals, D3D11_BIND_FLAG(D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE));
			//utils::D3D::Instance()->GetDeviceContext()->CopyStructureCount(m_bufferCounters->GetBuffer(), 4, m_bufferAliveListPing->GetUAV());

			delete counterInitVals;
			
			//INDIRECT ARGS BUFFER
			unsigned indirectArgs[7] = { 0, 1, 1, 0, 1, 0, 0 };

			m_bufferIndirectArgs = std::make_unique<utils::buffers::ByteAddressBuffer>(sizeof(unsigned), 7, indirectArgs);

			m_pingpong = true;

			m_particleShader = resource::Shader::CreateShader("../Data/FXIncludes/particleShader.fx");

			m_texArr = new resource::Texture2DArray(256, 256, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);

			m_blendState = blendState;
		}

		void ParticleSystem::Destroy()
		{
			
			SAFE_RELEASE(m_bufferAliveListPing);
			SAFE_RELEASE(m_bufferAliveListPong);
			SAFE_RELEASE(m_bufferDeadList);
			SAFE_RELEASE(m_bufferUpdate);
			SAFE_RELEASE(m_bufferSpawn);
			SAFE_RELEASE(m_bufferBillboard);
			SAFE_RELEASE(m_bufferIndirectArgs);
			SAFE_RELEASE(m_bufferCounters);
			SAFE_RELEASE(m_bufferSpawnIndex);

			delete m_texArr;
			//m_updateParticlesCS 
			//m_emitParticlesCS
		}

		void ParticleSystem::AddEmitterToSpawn(InitParticleBufferStruct & emitterInitData)
		{
			m_emitters.push_back(emitterInitData);
		}

		unsigned ParticleSystem::AddTexture(resource::Texture2D * tex)
		{
			return m_texArr->AddTexture(tex);
		}

		void ParticleSystem::DeRefTexFromTexArray(unsigned i)
		{
			m_texArr->DeRefTexture(i);
		}
		
		void ParticleSystem::SpawnParticles()
		{
			if (m_emitters.size() == 0)
				return;

			m_bufferSpawn->SetData(m_emitters);
			
			m_emitParticlesCS->SetGlobalResource("initparticles", m_bufferSpawn->GetSRV());

			m_emitParticlesCS->SetGlobalUAV("particles", m_bufferUpdate->GetUAV());
			m_emitParticlesCS->SetGlobalUAV("deadlist", m_bufferDeadList->GetUAV());
			m_emitParticlesCS->SetGlobalUAV("counterbuffer", m_bufferCounters->GetUAV());
					
			if (m_pingpong)
			{
				m_emitParticlesCS->SetGlobalUAV("alivelist", m_bufferAliveListPing->GetUAV());
			}
			else
			{
				m_emitParticlesCS->SetGlobalUAV("alivelist", m_bufferAliveListPong->GetUAV());
			}
					

			m_emitParticlesCS->Bind();
			m_emitParticlesCS->SetPass(0);

			ID3D11Buffer* cBuffer[1] = { m_bufferSpawnIndex->GetBuffer() };
			utils::D3D::Instance()->GetDeviceContext()->CSSetConstantBuffers(7, 1, cBuffer);

			for (unsigned i = 0; i < m_emitters.size(); ++i)
			{
				D3D11_MAPPED_SUBRESOURCE mappedResource = {};
				HRESULT ttttt = utils::D3D::Instance()->GetDeviceContext()->Map(m_bufferSpawnIndex->GetBuffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
				memcpy(mappedResource.pData, &i, sizeof(int));
				utils::D3D::Instance()->GetDeviceContext()->Unmap(m_bufferSpawnIndex->GetBuffer(), 0);

				m_emitParticlesCS->Dispatch((m_emitters[i].nrOfParticlesToEmit + EMIT_THREAD_DIM_X - 1) / EMIT_THREAD_DIM_X);					//EMIT
			}
			resource::ComputeShader::UnbindAllUAVs();
			resource::ComputeShader::UnbindOneSRV(0);

			
		}

		void ParticleSystem::UpdateAliveCount()
		{
			//UPDATE EMITCOUNT
			if (m_pingpong)
			{
				utils::D3D::Instance()->GetDeviceContext()->CopyStructureCount(m_bufferCounters->GetBuffer(), 4, m_bufferAliveListPing->GetUAV());
			}
			else
			{
				utils::D3D::Instance()->GetDeviceContext()->CopyStructureCount(m_bufferCounters->GetBuffer(), 4, m_bufferAliveListPong->GetUAV());
			}

			utils::D3D::Instance()->GetDeviceContext()->CopyStructureCount(m_bufferCounters->GetBuffer(), 0, m_bufferDeadList->GetUAV());

			m_calculateEmitCountCS->SetGlobalUAV("indirectargs", m_bufferIndirectArgs->GetUAV());
			m_calculateEmitCountCS->SetGlobalResource("counterbuffer", m_bufferCounters->GetSRV());
			m_calculateEmitCountCS->Bind();
			m_calculateEmitCountCS->SetPass(0);

			m_calculateEmitCountCS->Dispatch(1);

			resource::ComputeShader::UnbindOneUAV(0);
			resource::ComputeShader::UnbindOneSRV(0);

			m_emitters.clear();
		}

		void ParticleSystem::UpdateParticleSystem()
		{
			SpawnParticles();
			UpdateAliveCount();

			m_pingpong = !m_pingpong;

			if (m_pingpong)
			{
				m_updateParticlesCS->SetGlobalUAV("appendalivelist", m_bufferAliveListPing->GetUAV());
				m_updateParticlesCS->SetGlobalUAV("consumealivelist", m_bufferAliveListPong->GetUAV());
			}
			else
			{
				m_updateParticlesCS->SetGlobalUAV("appendalivelist", m_bufferAliveListPong->GetUAV());
				m_updateParticlesCS->SetGlobalUAV("consumealivelist", m_bufferAliveListPing->GetUAV());
			}
			m_updateParticlesCS->SetGlobalUAV("deadlist", m_bufferDeadList->GetUAV());

			m_updateParticlesCS->SetGlobalUAV("counterbuffer", m_bufferCounters->GetUAV());
			m_updateParticlesCS->SetGlobalUAV("particles", m_bufferUpdate->GetUAV());
			m_updateParticlesCS->SetGlobalUAV("billboards", m_bufferBillboard->GetUAV());

			m_updateParticlesCS->Bind();
			m_updateParticlesCS->SetPass(0);

			m_updateParticlesCS->DispatchIndirect(m_bufferIndirectArgs->GetBuffer(), 0);


			resource::ComputeShader::UnbindAllUAVs();
			resource::ComputeShader::UnbindAllSRVs();
		}

		void ParticleSystem::DrawParticles()
		{
			m_particleShader->BindPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			
			m_particleShader->SetGlobalTexture2DArray("textures", m_texArr);
			m_particleShader->SetGlobalResource("billboards", m_bufferBillboard->GetSRV());
			
			m_particleShader->Bind();
			m_particleShader->SetPass(m_blendState);
			
			
			utils::D3D::Instance()->GetDeviceContext()->DrawInstancedIndirect(m_bufferIndirectArgs->GetBuffer(), 12);

			ID3D11ShaderResourceView* const s_nullSRV[8] = { NULL };
			utils::D3D::Instance()->GetDeviceContext()->VSSetShaderResources(0, 8, s_nullSRV);

		}

		
	}
}

