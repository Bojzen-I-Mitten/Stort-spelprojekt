#include "ParticleSystem.h"
#include "../object/component/ParticleEmitterComponent.h"
#include "../resource/ComputeShader.h"
#include <random>
#include <time.h>
namespace thomas
{
	namespace graphics
	{

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
			resource::ComputeShader* pShittyComputeShader = (resource::ComputeShader*)resource::ComputeShader::CreateShader("../Data/FXIncludes/appendIndexes.fx");
			

			//PARTICLE BUFFERS
			m_bufferSpawn =			std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(object::component::ParticleEmitterComponent::InitParticleBufferStruct), 1, DYNAMIC_BUFFER);//ammount of emiting emitters supported at once			
			m_bufferUpdate =		std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(object::component::ParticleEmitterComponent::ParticleStruct), maxNrOfParticles, STATIC_BUFFER, D3D11_BIND_FLAG(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS));//ammount of particles supported for entire system 
			m_bufferBillboard =		std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(BillboardStruct), maxNrOfParticles, STATIC_BUFFER, D3D11_BIND_FLAG(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS));

			//INDEXING APPEND CONSUME BUFFERS
			m_bufferDeadList = std::make_unique<utils::buffers::AppendConsumeBuffer>(nullptr, sizeof(unsigned int), maxNrOfParticles);
			m_bufferAliveListPing = std::make_unique<utils::buffers::AppendConsumeBuffer>(nullptr, sizeof(unsigned int), maxNrOfParticles);
			m_bufferAliveListPong = std::make_unique<utils::buffers::AppendConsumeBuffer>(nullptr, sizeof(unsigned int), maxNrOfParticles);
			
			//INIT DEAD LIST
			pShittyComputeShader->SetGlobalInt("maxNrOfParticles", maxNrOfParticles);
			pShittyComputeShader->SetGlobalUAV("deadlist", m_bufferDeadList->GetUAV());

			pShittyComputeShader->Bind();
			pShittyComputeShader->SetPass(0);
			pShittyComputeShader->Dispatch((maxNrOfParticles - 1) / 512 + 1);
			
			ID3D11UnorderedAccessView* const s_nullUAV[1] = { NULL };
			utils::D3D::Instance()->GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, s_nullUAV, nullptr); 

			resource::Shader::DestroyShader(pShittyComputeShader);
			
			//COUNTER BUFFER
			m_bufferCounters = std::make_unique<utils::buffers::ByteAddressBuffer>(sizeof(unsigned), 4);
			
			utils::D3D::Instance()->GetDeviceContext()->CopyStructureCount(m_bufferCounters->GetBuffer(), 0, m_bufferDeadList->GetUAV());
			utils::D3D::Instance()->GetDeviceContext()->CopyStructureCount(m_bufferCounters->GetBuffer(), 4, m_bufferAliveListPing->GetUAV());
			

			//INDIRECT ARGS BUFFER
			unsigned indirectArgs[3] = { 1, 1, 1 };

			m_bufferIndirectUpdateArgs = std::make_unique<utils::buffers::Buffer>(indirectArgs, sizeof(indirectArgs), (D3D11_BIND_FLAG)0, STATIC_BUFFER, sizeof(unsigned), D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS);

			m_pingpong = true;

			m_particleShader = resource::Shader::CreateShader("../Data/FXIncludes/particleShader.fx");

			m_emittedParticles = 0;



		}

		void ParticleSystem::Destroy()
		{
			SAFE_RELEASE(m_bufferAliveListPing);
			SAFE_RELEASE(m_bufferAliveListPong);
			SAFE_RELEASE(m_bufferDeadList);
			SAFE_RELEASE(m_bufferUpdate);
			SAFE_RELEASE(m_bufferSpawn);

			//m_updateParticlesCS 
			//m_emitParticlesCS
		}

		bool ParticleSystem::AddEmitterToSpawn(object::component::ParticleEmitterComponent* emitter)
		{
			for (object::component::ParticleEmitterComponent* e : m_spawningEmitters)
			{
				if (emitter == e)//emitter already added
					return false;
			}
			
			//m_spawningEmitterEmissionRate.push_back(emitter->GetEmissionRate() + 0.5f);
			m_spawningEmitters.push_back(emitter);
			return true;
		}

		bool ParticleSystem::AddEmitterToUpdate(object::component::ParticleEmitterComponent* emitter)
		{
			for (object::component::ParticleEmitterComponent* e : m_spawningEmitters)
			{
				if (emitter == e)//emitter already added
					return false;
			}

			m_updateEmitters.push_back(emitter);
			return true;
		}


		void ParticleSystem::SpawnParticles()
		{
			//if (m_emittedParticles < 1000)
			{
				std::vector<object::component::ParticleEmitterComponent::InitParticleBufferStruct> dataVec;
				object::component::ParticleEmitterComponent::InitParticleBufferStruct testInitData = {};
				testInitData.nrOfParticlesToEmit = 32;
				std::srand(time(NULL));
				testInitData.rand = std::rand();

				testInitData.spawnAtSphereEdge = (unsigned)false;
				testInitData.endSize = 3.0f;
				testInitData.endSpeed = 10.0f;
				testInitData.gravity = 0.0f;
				testInitData.maxLifeTime = 100.0f;
				testInitData.maxSize = 1.0f;
				testInitData.maxSpeed = 1.4f;
				testInitData.minLifeTime = 99.0f;
				testInitData.minSize = 0.5f;
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
				}
				*/

				dataVec.push_back(testInitData);

				m_bufferSpawn->SetData(dataVec);
			

				m_emitParticlesCS->SetGlobalResource("initparticles", m_bufferSpawn->GetSRV());
				m_emitParticlesCS->SetGlobalUAV("particles", m_bufferUpdate->GetUAV());
				m_emitParticlesCS->SetGlobalUAV("deadlist", m_bufferDeadList->GetUAV());
					
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
					
				m_emitParticlesCS->Dispatch(1);

				m_emittedParticles += testInitData.nrOfParticlesToEmit;

				ID3D11UnorderedAccessView* const s_nullUAV[8] = { NULL };
				utils::D3D::Instance()->GetDeviceContext()->CSSetUnorderedAccessViews(0, 8, s_nullUAV, nullptr);


				ID3D11ShaderResourceView* const s_nullSRV[2] = { NULL };
				utils::D3D::Instance()->GetDeviceContext()->CSSetShaderResources(0, 2, s_nullSRV);
				
			}
		}

		void ParticleSystem::UpdateParticles()
		{
			m_pingpong = !m_pingpong;
			if (m_pingpong)
			{
				m_updateParticlesCS->SetGlobalUAV("appendalivelist", m_bufferAliveListPing->GetUAV());
				m_updateParticlesCS->SetGlobalUAV("consumealivelist", m_bufferAliveListPong->GetUAV());

				utils::D3D::Instance()->GetDeviceContext()->CopyStructureCount(m_bufferCounters->GetBuffer(), 4, m_bufferAliveListPong->GetUAV());
			}
			else
			{
				m_updateParticlesCS->SetGlobalUAV("appendalivelist", m_bufferAliveListPong->GetUAV());
				m_updateParticlesCS->SetGlobalUAV("consumealivelist", m_bufferAliveListPing->GetUAV());

				utils::D3D::Instance()->GetDeviceContext()->CopyStructureCount(m_bufferCounters->GetBuffer(), 4, m_bufferAliveListPing->GetUAV());
			}
			
			m_updateParticlesCS->SetGlobalUAV("deadlist", m_bufferDeadList->GetUAV());
			utils::D3D::Instance()->GetDeviceContext()->CopyStructureCount(m_bufferCounters->GetBuffer(), 0, m_bufferDeadList->GetUAV());



			m_updateParticlesCS->SetGlobalUAV("counterbuffer", m_bufferCounters->GetUAV());

			m_updateParticlesCS->SetGlobalUAV("particles", m_bufferUpdate->GetUAV());
			m_updateParticlesCS->SetGlobalUAV("billboards", m_bufferBillboard->GetUAV());

			m_updateParticlesCS->Bind();
			m_updateParticlesCS->SetPass(0);

			int test = (m_emittedParticles + 31u) / 32u;
			if (test > 0)
				//m_updateParticlesCS->Dispatch(test);




			m_updateParticlesCS->DispatchIndirect(m_bufferIndirectUpdateArgs->GetBuffer(), 0);

			ID3D11UnorderedAccessView* const s_nullUAV[8] = { NULL };
			utils::D3D::Instance()->GetDeviceContext()->CSSetUnorderedAccessViews(0, 8, s_nullUAV, nullptr);


		}

		void ParticleSystem::DrawParticles()
		{
			m_particleShader->BindPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //TODO: refactor to triangle strip?
			

			m_particleShader->SetGlobalResource("billboards", m_bufferBillboard->GetSRV());
			
			m_particleShader->Bind();
			m_particleShader->SetPass(0);
			
			utils::D3D::Instance()->GetDeviceContext()->Draw(6 * m_emittedParticles, 0);

			ID3D11ShaderResourceView* const s_nullSRV[1] = { NULL };
			utils::D3D::Instance()->GetDeviceContext()->VSSetShaderResources(0, 1, s_nullSRV);

		}

		
	}
}

