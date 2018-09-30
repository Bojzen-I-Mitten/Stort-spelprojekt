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
			m_updateParticlesCS = (resource::ComputeShader*)resource::ComputeShader::CreateShader("../Data/FXIncludes/updateParticlesCS.fx");

			
			m_spawnBuffer = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(object::component::ParticleEmitterComponent::InitParticleBufferStruct), 10, DYNAMIC_BUFFER);//ammount of emiting emitters supported at once			
			m_updateBuffer = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(object::component::ParticleEmitterComponent::ParticleStruct), maxNrOfParticles, STATIC_BUFFER, D3D11_BIND_FLAG(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS));//ammount of particles supported for entire system 
			m_billboardBuffer = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(BillboardStruct), 1/*maxNrOfParticles*/, STATIC_BUFFER, D3D11_BIND_FLAG(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS));


			m_aliveListPing = std::make_unique<utils::buffers::AppendConsumeBuffer>(sizeof(unsigned), maxNrOfParticles);
			m_aliveListPong = std::make_unique<utils::buffers::AppendConsumeBuffer>(sizeof(unsigned), maxNrOfParticles);
			m_deadList = std::make_unique<utils::buffers::AppendConsumeBuffer>(sizeof(unsigned), maxNrOfParticles);
			
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
			if (m_emittedParticles < 1)
			{
				std::vector<object::component::ParticleEmitterComponent::InitParticleBufferStruct> dataVec;

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

				m_emitParticlesCS->Dispatch(1);
				m_emittedParticles += 1;
			}
		}

		void ParticleSystem::UpdateParticles()
		{
			/*
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

			m_updateParticlesCS->SetGlobalUAV("deadList", m_deadList->GetUAV());*/
			m_updateParticlesCS->SetGlobalUAV("billboards", m_billboardBuffer->GetUAV());
			m_updateParticlesCS->SetGlobalUAV("particles", m_updateBuffer->GetUAV());

			ID3D11DeviceContext* pDC = ThomasCore::GetDeviceContext();
			//ID3D11UnorderedAccessView* pUAV = m_billboardBuffer->GetUAV();
			ID3D11UnorderedAccessView* const s_nullUAV[1] = { NULL };
			



			m_updateParticlesCS->SetPass(0);
			m_updateParticlesCS->Bind();

			m_updateParticlesCS->Dispatch(1);//m_emittedParticles / 256u);

			
			pDC->CSSetUnorderedAccessViews(0, 1, s_nullUAV, nullptr);

		}

		void ParticleSystem::DrawParticles()
		{
			//m_particleShader->Bind();
			m_particleShader->BindPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); //TODO: refactor to triangle strip?
			
			m_particleShader->SetGlobalResource("billboards", m_billboardBuffer->GetSRV());
			m_particleShader->SetPass(0);
			m_particleShader->Bind();
			
			
			//m_emittedParticles * 

			ThomasCore::GetDeviceContext()->Draw(6, 0);

			ID3D11ShaderResourceView* const s_nullSRV[1] = { NULL };
			ThomasCore::GetDeviceContext()->VSSetShaderResources(0, 1, s_nullSRV);
			//m_particleShader->UnbindGlobalSRV("billboards");
		}

		
	}
}

