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
			
			m_spawnBuffer =			std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(object::component::ParticleEmitterComponent::InitParticleBufferStruct), 1, DYNAMIC_BUFFER);//ammount of emiting emitters supported at once			
			m_updateBuffer =		std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(object::component::ParticleEmitterComponent::ParticleStruct), maxNrOfParticles, STATIC_BUFFER, D3D11_BIND_FLAG(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS));//ammount of particles supported for entire system 
			m_billboardBuffer =		std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(BillboardStruct), maxNrOfParticles, STATIC_BUFFER, D3D11_BIND_FLAG(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS));

			m_deadList = std::make_unique<utils::buffers::AppendConsumeBuffer>(nullptr, sizeof(unsigned int), maxNrOfParticles);

			pShittyComputeShader->SetGlobalInt("maxNrOfParticles", maxNrOfParticles);
			pShittyComputeShader->SetGlobalUAV("deadlist", m_deadList->GetUAV());
			pShittyComputeShader->Bind();
			pShittyComputeShader->SetPass(0);
			pShittyComputeShader->Dispatch((maxNrOfParticles - 1) / 512 + 1);
			

			ID3D11UnorderedAccessView* const s_nullUAV[1] = { NULL };
			ThomasCore::GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, s_nullUAV, nullptr);


			m_aliveListPing =	std::make_unique<utils::buffers::AppendConsumeBuffer>(nullptr, sizeof(unsigned int), maxNrOfParticles);
			m_aliveListPong =	std::make_unique<utils::buffers::AppendConsumeBuffer>(nullptr, sizeof(unsigned int), maxNrOfParticles);

			m_pingpong = true;

			m_particleShader = resource::Shader::CreateShader("../Data/FXIncludes/particleShader.fx");

			m_emittedParticles = 0;



		}

		void ParticleSystem::Destroy()
		{
			SAFE_RELEASE(m_aliveListPing);
			SAFE_RELEASE(m_aliveListPong);
			SAFE_RELEASE(m_deadList);
			SAFE_RELEASE(m_updateBuffer);
			SAFE_RELEASE(m_spawnBuffer);

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
			
			m_spawningEmitterEmissionRate.push_back(emitter->GetEmissionRate() + 0.5f);
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
				testInitData.maxLifeTime = 20.0f;
				testInitData.maxSize = 1.0f;
				testInitData.maxSpeed = 1.4f;
				testInitData.minLifeTime = 20.0f;
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

				m_spawnBuffer->SetData(dataVec);
				dank = true;
				

				m_emitParticlesCS->SetGlobalResource("initparticles", m_spawnBuffer->GetSRV());
				m_emitParticlesCS->SetGlobalUAV("particles", m_updateBuffer->GetUAV());
					

				m_emitParticlesCS->SetGlobalUAV("deadlist", m_deadList->GetUAV());

					
				if (m_pingpong)
				{
					m_emitParticlesCS->SetGlobalUAV("alivelist", m_aliveListPing->GetUAV());
				}
				else
				{
					m_emitParticlesCS->SetGlobalUAV("alivelist", m_aliveListPong->GetUAV());
				}
					

				m_emitParticlesCS->Bind();
				m_emitParticlesCS->SetPass(0);
					
				m_emitParticlesCS->Dispatch(1);
				//m_emittedParticles += testInitData.nrOfParticlesToEmit;

				ID3D11UnorderedAccessView* const s_nullUAV[4] = { NULL };
				ThomasCore::GetDeviceContext()->CSSetUnorderedAccessViews(0, 4, s_nullUAV, nullptr);


				ID3D11ShaderResourceView* const s_nullSRV[2] = { NULL };
				ThomasCore::GetDeviceContext()->CSSetShaderResources(0, 2, s_nullSRV);
				
			}
		}

		void ParticleSystem::UpdateParticles()
		{
			m_pingpong = !m_pingpong;
			if (m_pingpong)
			{
				m_updateParticlesCS->SetGlobalUAV("appendalivelist", m_aliveListPing->GetUAV());
				m_updateParticlesCS->SetGlobalUAV("consumealivelist", m_aliveListPong->GetUAV());
			}
			else
			{
				m_updateParticlesCS->SetGlobalUAV("appendalivelist", m_aliveListPong->GetUAV());
				m_updateParticlesCS->SetGlobalUAV("consumealivelist", m_aliveListPing->GetUAV());
				
			}
			
			m_updateParticlesCS->SetGlobalUAV("deadlist", m_deadList->GetUAV());
			
			m_emitParticlesCS->SetGlobalUAV("particles", m_updateBuffer->GetUAV());
			m_emitParticlesCS->SetGlobalUAV("billboards", m_billboardBuffer->GetUAV());

			m_updateParticlesCS->Bind();
			m_updateParticlesCS->SetPass(0);

			m_updateParticlesCS->Dispatch(1);//m_emittedParticles / 256u);


			ID3D11UnorderedAccessView* const s_nullUAV[8] = { NULL };
			ThomasCore::GetDeviceContext()->CSSetUnorderedAccessViews(0, 8, s_nullUAV, nullptr);

			ID3D11ShaderResourceView* const s_nullSRV[1] = { NULL };
			ThomasCore::GetDeviceContext()->CSSetShaderResources(0, 1, s_nullSRV);

		}

		void ParticleSystem::DrawParticles()
		{
			m_particleShader->BindPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //TODO: refactor to triangle strip?
			

			m_particleShader->SetGlobalResource("billboards", m_billboardBuffer->GetSRV());
			
			m_particleShader->Bind();
			m_particleShader->SetPass(0);
			
			//m_emittedParticles * 

			ThomasCore::GetDeviceContext()->Draw(6 * 32, 0);

			ID3D11ShaderResourceView* const s_nullSRV[1] = { NULL };
			ThomasCore::GetDeviceContext()->VSSetShaderResources(0, 1, s_nullSRV);

		}

		
	}
}

