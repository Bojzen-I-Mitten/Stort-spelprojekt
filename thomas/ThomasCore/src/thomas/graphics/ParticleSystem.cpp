#include "ParticleSystem.h"
#include "../resource/ComputeShader.h"
#include <random>
#include <time.h>
namespace thomas
{
	namespace graphics
	{
		std::shared_ptr<ParticleSystem> ParticleSystem::s_globalSystem;

		void ParticleSystem::InitializeGlobalSystem()
		{
			s_globalSystem = std::make_shared<ParticleSystem>();
			s_globalSystem->Initialize(65536);
		}
		std::shared_ptr<ParticleSystem> ParticleSystem::GetGlobalSystem()
		{
			return s_globalSystem;
		}
		ParticleSystem::ParticleSystem()
		{

		}

		ParticleSystem::~ParticleSystem()
		{

		}

		void ParticleSystem::Initialize(unsigned maxNrOfParticles)
		{
			m_maxNrOfParticles = maxNrOfParticles;
			m_emitParticlesCS = (resource::ComputeShader*)resource::ComputeShader::CreateShader("../Data/FXIncludes/emitParticlesCS.fx");
			m_updateParticlesCS = (resource::ComputeShader*)resource::ComputeShader::CreateShader("../Data/FXIncludes/updateParticlesCS.fx");
			resource::ComputeShader* initCS = (resource::ComputeShader*)resource::ComputeShader::CreateShader("../Data/FXIncludes/appendIndexes.fx");
			m_calculateEmitCountCS = (resource::ComputeShader*)resource::ComputeShader::CreateShader("../Data/FXIncludes/calculateEmitCountCS.fx");

			//PARTICLE BUFFERS
			m_bufferSpawn =			std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(InitParticleBufferStruct), 1, DYNAMIC_BUFFER);//ammount of emiting emitters supported at once			
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
			

			//m_updateParticlesCS 
			//m_emitParticlesCS
		}

		void ParticleSystem::AddEmitterToSpawn(InitParticleBufferStruct & emitterInitData)
		{
			m_emitters.push_back(emitterInitData);
		}

		int asdf = 0;
		void ParticleSystem::SpawnParticles()
		{
			
			std::vector<InitParticleBufferStruct> dataVec;

			InitParticleBufferStruct testInitData = {};
			testInitData.nrOfParticlesToEmit = 5;
			std::srand(time(NULL));
			testInitData.rand = std::rand();

			testInitData.spawnAtSphereEdge = (unsigned)false;
			testInitData.endSize = 1.5f;
			testInitData.endSpeed = 10.0f;
			testInitData.gravity = 0.0f;
			testInitData.maxLifeTime = 10.0f;
			testInitData.maxSize = 1.0f;
			testInitData.maxSpeed = 1.4f;
			testInitData.minLifeTime = 5.0f;
			testInitData.minSize = 0.1f;
			testInitData.minSpeed = 0.5f;
			testInitData.radius = 10.0f;
			testInitData.position = math::Vector3(0.0f, 2.0f, 0.0f);
			testInitData.maxRotationSpeed = 3.0f;
			testInitData.minRotationSpeed = 0.3f;
			testInitData.spread = 0.7f;
			XMStoreFloat3x3(&testInitData.directionMatrix, DirectX::XMMatrixLookAtRH(math::Vector3(0, 2, 0), math::Vector3(1.0f, 2.0f, 0.0f), math::Vector3::Up));

			/*for (object::component::ParticleEmitterComponent* e : m_spawningEmitters)
			{
				dataVec.push_back(e->GetInitData());
			}*/
			


			dataVec.push_back(testInitData);
			/*
			if (m_emitters.size() == 0)
				return;

			if (m_emitters.size() > 1)
				int stopper = 0;*/
asdf++;
			if (asdf != 3000 && asdf != 3400 && asdf != 3401)
				return;
			
			m_bufferSpawn->SetData(dataVec);
			

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
					
			m_emitParticlesCS->Dispatch(1);					//EMIT

			resource::ComputeShader::UnbindAllUAVs();
			resource::ComputeShader::UnbindOneSRV(0);

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

			//UPDATE EMIT COUNT BEFORE DRAW
			if (m_pingpong)
			{
				utils::D3D::Instance()->GetDeviceContext()->CopyStructureCount(m_bufferCounters->GetBuffer(), 4, m_bufferAliveListPing->GetUAV());
				utils::D3D::Instance()->GetDeviceContext()->CopyStructureCount(m_bufferCounters->GetBuffer(), 12, m_bufferAliveListPong->GetUAV());
			}
			else
			{
				utils::D3D::Instance()->GetDeviceContext()->CopyStructureCount(m_bufferCounters->GetBuffer(), 4, m_bufferAliveListPong->GetUAV());

				utils::D3D::Instance()->GetDeviceContext()->CopyStructureCount(m_bufferCounters->GetBuffer(), 12, m_bufferAliveListPing->GetUAV());
			}

			utils::D3D::Instance()->GetDeviceContext()->CopyStructureCount(m_bufferCounters->GetBuffer(), 0, m_bufferDeadList->GetUAV());

			m_calculateEmitCountCS->SetGlobalUAV("indirectargs", m_bufferIndirectArgs->GetUAV());
			m_calculateEmitCountCS->SetGlobalResource("counterbuffer", m_bufferCounters->GetSRV());
			m_calculateEmitCountCS->Bind();
			m_calculateEmitCountCS->SetPass(0);

			m_calculateEmitCountCS->Dispatch(1);

			resource::ComputeShader::UnbindOneUAV(0);
			resource::ComputeShader::UnbindOneSRV(0);

		}

		void ParticleSystem::DrawParticles()
		{
			m_particleShader->BindPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			

			m_particleShader->SetGlobalResource("billboards", m_bufferBillboard->GetSRV());
			
			m_particleShader->Bind();
			m_particleShader->SetPass(0);
			
			utils::D3D::Instance()->GetDeviceContext()->DrawInstancedIndirect(m_bufferIndirectArgs->GetBuffer(), 12);

			ID3D11ShaderResourceView* const s_nullSRV[1] = { NULL };
			utils::D3D::Instance()->GetDeviceContext()->VSSetShaderResources(0, 1, s_nullSRV);

		}

		
	}
}

