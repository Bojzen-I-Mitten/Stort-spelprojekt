#include "ParticleSystem.h"
#include "../object/component/ParticleEmitterComponent.h"
#include "../resource/ComputeShader.h"

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
			m_emitParticlesCS = (resource::ComputeShader*)resource::ComputeShader::CreateShader("../Data/oldShaders/emitParticlesCS.hlsl");
			m_updateParticlesCS = (resource::ComputeShader*)resource::ComputeShader::CreateShader("../Data/oldShaders/updateParticlesCS.hlsl");

			
			m_spawnBuffer = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(object::component::ParticleEmitterComponent::InitParticleBufferStruct), 10, DYNAMIC_BUFFER);//ammount of emiting emitters supported at once			
			m_updateBuffer = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(object::component::ParticleEmitterComponent::ParticleStruct), maxNrOfParticles, STATIC_BUFFER, D3D11_BIND_FLAG(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS));//ammount of particles supported for entire system 
			m_billboardBuffer = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(object::component::ParticleEmitterComponent::BillboardStruct), maxNrOfParticles, STATIC_BUFFER, D3D11_BIND_FLAG(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS));

			m_aliveListPing = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(unsigned), 1, D3D11_BUFFER_UAV_FLAG_APPEND);
			m_aliveListPong = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(unsigned), 1, D3D11_BUFFER_UAV_FLAG_APPEND);
			m_deadList = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(unsigned), maxNrOfParticles, D3D11_BUFFER_UAV_FLAG_APPEND);
			
			m_pingpong = true;

			m_particleShader = resource::Shader::CreateShader("../Data/oldShaders/particleShader.fx");
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
			if (m_emittedParticles < 512)
			{
				std::vector<object::component::ParticleEmitterComponent::InitParticleBufferStruct> dataVec;

				unsigned test = (unsigned)false;
				test = (unsigned)true;

				object::component::ParticleEmitterComponent::InitParticleBufferStruct testInitData = {};
				testInitData.nrOfParticlesToEmit = 20;
				testInitData.rand = 1.0f;
				testInitData.spawnAtSphereEdge = (unsigned)false;
				testInitData.endSize = 3.0f;
				testInitData.endSpeed = 10.0f;
				testInitData.gravity = 10.0f;
				testInitData.maxLifeTime = 20.0f;
				testInitData.maxSize = 1.0f;
				testInitData.maxSpeed = 2.0f;
				testInitData.minLifeTime = 20.0f;
				testInitData.minSize = 1.0f;
				testInitData.minSpeed = 10.0f;
				testInitData.position = math::Vector3(0.0f, 2.0f, 0.0f);
				testInitData.rotation = 0.0f;
				testInitData.rotationSpeed = 1.0f;
				testInitData.spread = 1.0f;
				XMStoreFloat3x3(&testInitData.directionMatrix, DirectX::XMMatrixLookAtRH(math::Vector3(0, 0, 0), math::Vector3(0.0f, 1.0f, 0.0f), math::Vector3::Up));

				/*for (object::component::ParticleEmitterComponent* e : m_spawningEmitters)
				{
					dataVec.push_back(e->GetInitData());
				}
				*/

				dataVec.push_back(testInitData);

				m_spawnBuffer->SetData(dataVec);

				m_emitParticlesCS->SetGlobalResource("initParticles", m_spawnBuffer->GetSRV());
				m_emitParticlesCS->SetGlobalUAV("particlesWrite", m_updateBuffer->GetUAV());
				m_emitParticlesCS->SetGlobalUAV("deadList", m_deadList->GetUAV());

				m_pingpong = !m_pingpong;
				if (m_pingpong)
				{
					m_emitParticlesCS->SetGlobalUAV("aliveList", m_aliveListPing->GetUAV());
				}
				else
				{
					m_emitParticlesCS->SetGlobalUAV("aliveList", m_aliveListPong->GetUAV());
				}

				//m_spawningEmitterEmissionRate[0] * 

				m_emitParticlesCS->Dispatch(32);
				m_emittedParticles += 32;
			}
		}

		void ParticleSystem::UpdateParticles()
		{
			if (m_pingpong)
			{
				m_updateParticlesCS->SetGlobalUAV("appendAliveList", m_aliveListPing->GetUAV());
				m_updateParticlesCS->SetGlobalUAV("consumeAliveList", m_aliveListPong->GetUAV());
			}
			else
			{
				m_updateParticlesCS->SetGlobalUAV("consumeAliveList", m_aliveListPing->GetUAV());
				m_updateParticlesCS->SetGlobalUAV("appendAliveList", m_aliveListPong->GetUAV());
			}

			m_updateParticlesCS->SetGlobalUAV("deadList", m_deadList->GetUAV());
			m_updateParticlesCS->SetGlobalUAV("billboards", m_billboardBuffer->GetUAV());
			m_updateParticlesCS->SetGlobalUAV("particles", m_updateBuffer->GetUAV());



			m_updateParticlesCS->Dispatch(m_emittedParticles / 256.0f);

		}

		void ParticleSystem::DrawParticles()
		{
			//m_particleShader->Bind();
			m_particleShader->BindPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //TODO: refactor to triangle strip?
			m_particleShader->SetGlobalResource("billboards", m_billboardBuffer->GetSRV());
			m_particleShader->Bind();
			ThomasCore::GetDeviceContext()->Draw(m_emittedParticles * 6, 0);


		}

		
	}
}

