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

			
			m_particleSpawn = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(object::component::ParticleEmitterComponent::InitParticleBufferStruct), 10, DYNAMIC_BUFFER);//ammount of emiting emitters supported at once			
			m_particleUpdatePing = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(object::component::ParticleEmitterComponent::ParticleStruct), maxNrOfParticles, STATIC_BUFFER, true, D3D11_BIND_FLAG(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS));//ammount of particles supported for entire system
			m_particleUpdatePong = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(object::component::ParticleEmitterComponent::ParticleStruct), maxNrOfParticles, STATIC_BUFFER, true, D3D11_BIND_FLAG(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS));//ammount of particles supported for entire system
			
			int vals[25000] = {};
			m_particleAliveList = std::make_unique<utils::buffers::StructuredBuffer>(&vals, sizeof(unsigned), maxNrOfParticles, D3D11_BUFFER_UAV_FLAG_APPEND);

			for (int i = 0; i < 25000; ++i)
				vals[i] = i;

			m_particleDeadList = std::make_unique<utils::buffers::StructuredBuffer>(&vals, sizeof(unsigned), maxNrOfParticles, D3D11_BUFFER_UAV_FLAG_APPEND);
			
			m_pingpong = true;

		}

		void ParticleSystem::Destroy()
		{
			SAFE_RELEASE(m_particleAliveList);
			SAFE_RELEASE(m_particleDeadList);
			SAFE_RELEASE(m_particleUpdatePong);
			SAFE_RELEASE(m_particleUpdatePing);
			SAFE_RELEASE(m_particleSpawn);

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
			std::vector<object::component::ParticleEmitterComponent::InitParticleBufferStruct> dataVec;

			for (object::component::ParticleEmitterComponent* e : m_spawningEmitters)
			{
				dataVec.push_back(e->GetInitData());
			}

			m_particleSpawn->SetData(dataVec);

			m_emitParticlesCS->SetGlobalResource("initParticles", m_particleSpawn->GetSRV());
			m_emitParticlesCS->SetGlobalUAV("particlesWrite", m_particleUpdatePing->GetUAV());
			m_emitParticlesCS->SetGlobalUAV("particlesWrite2", m_particleUpdatePong->GetUAV());
			m_emitParticlesCS->SetGlobalUAV("deadlist", m_particleDeadList->GetUAV());
			m_emitParticlesCS->SetGlobalUAV("alivelist", m_particleAliveList->GetUAV());

			m_emitParticlesCS->Dispatch(m_spawningEmitterEmissionRate[0] * 128);

		}

		void ParticleSystem::UpdateParticles()
		{
			m_pingpong = !m_pingpong;

			if (m_pingpong)
			{
				m_updateParticlesCS->SetGlobalUAV("particlesWrite", m_particleUpdatePing->GetUAV());
				m_updateParticlesCS->SetGlobalResource("particlesRead", m_particleUpdatePong->GetSRV());
				
			}
			else
			{
				m_updateParticlesCS->SetGlobalUAV("particlesWrite", m_particleUpdatePong->GetUAV());
				m_updateParticlesCS->SetGlobalResource("particlesRead", m_particleUpdatePing->GetSRV());
			}

			m_updateParticlesCS->SetGlobalUAV("deadlist", m_particleDeadList->GetUAV());

			m_updateParticlesCS->Dispatch(25000);

		}

		void ParticleSystem::DrawParticles()
		{


		}

		
	}
}

