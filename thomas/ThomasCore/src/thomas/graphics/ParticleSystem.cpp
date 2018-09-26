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

		void ParticleSystem::Initialize()
		{
			m_emitParticlesCS = (resource::ComputeShader*)resource::ComputeShader::CreateShader("../Data/oldShaders/emitParticlesCS.hlsl");
			m_updateParticlesCS = (resource::ComputeShader*)resource::ComputeShader::CreateShader("../Data/oldShaders/updateParticlesCS.hlsl");

			unsigned maxNrOfParticles = 25000;
			m_particleSpawn = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(object::component::ParticleEmitterComponent::InitParticleBufferStruct), 10, DYNAMIC_BUFFER);//ammount of emiting emitters supported at once			
			m_particleUpdatePing = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(object::component::ParticleEmitterComponent::ParticleStruct), maxNrOfParticles, true);//ammount of particles supported for entire system
			m_particleUpdatePong = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(object::component::ParticleEmitterComponent::ParticleStruct), maxNrOfParticles, true);//ammount of particles supported for entire system
			m_particleDeadList = std::make_unique<utils::buffers::StructuredBuffer>(nullptr, sizeof(unsigned), maxNrOfParticles * 2, D3D11_BUFFER_UAV_FLAG_APPEND);//appendconsumebuffer
			m_pingpong = true;


			/*

			D3D11_BLEND_DESC blendDesc;
			ZeroMemory(&blendDesc, sizeof(blendDesc));

			D3D11_RENDER_TARGET_BLEND_DESC blendState;
			ZeroMemory(&blendState, sizeof(blendState));

			//Additive

			blendState.BlendEnable = true;
			blendState.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendState.DestBlend = D3D11_BLEND_ONE;
			blendState.BlendOp = D3D11_BLEND_OP_ADD;
			blendState.SrcBlendAlpha = D3D11_BLEND_ONE;
			blendState.DestBlendAlpha = D3D11_BLEND_ZERO;
			blendState.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendState.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			blendDesc.RenderTarget[0] = blendState;

			HRESULT hr = ThomasCore::GetDevice()->CreateBlendState(&blendDesc, &s_blendStates.additive);

			//alpha blend
			blendState.BlendEnable = true;
			blendState.SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendState.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendState.BlendOp = D3D11_BLEND_OP_ADD;
			blendState.SrcBlendAlpha = D3D11_BLEND_ONE;
			blendState.DestBlendAlpha = D3D11_BLEND_ZERO;
			blendState.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendState.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			blendDesc.RenderTarget[0] = blendState;

			hr = ThomasCore::GetDevice()->CreateBlendState(&blendDesc, &s_blendStates.alphaBlend);


			D3D11_DEPTH_STENCIL_DESC mirrorDesc;
			mirrorDesc.DepthEnable = true;
			mirrorDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			mirrorDesc.DepthFunc = D3D11_COMPARISON_LESS;
			mirrorDesc.StencilEnable = false;
			mirrorDesc.StencilReadMask = 0xff;
			mirrorDesc.StencilWriteMask = 0xff;
			mirrorDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			mirrorDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			mirrorDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			mirrorDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
				
			ThomasCore::GetDevice()->CreateDepthStencilState(&mirrorDesc, &s_depthStencilState);*/


			//s_cameraBuffer = thomas::utils::D3d::CreateDynamicBufferFromStruct(s_cameraBufferStruct, D3D11_BIND_CONSTANT_BUFFER);
			return;
		}

		void ParticleSystem::Destroy()
		{
		}

		bool ParticleSystem::AddEmitterToSpawn(object::component::ParticleEmitterComponent* emitter)
		{
			for (object::component::ParticleEmitterComponent* e : m_spawningEmitters)
			{
				if (emitter == e)//emitter already added
					return false;
			}
			
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


			m_emitParticlesCS->SetGlobalUAV("deadlist", m_particleDeadList->GetUAV());
			//object::component::ParticleEmitterComponent::InitParticleBufferStruct data[m_spawningEmitters.size()];

			m_emitParticlesCS->Bind();
			
			//m_emitParticlesCS->
			//object::component::ParticleEmitterComponent::D3DData* emitterD3D = emitter->GetD3DData();
			
			//m_emitParticlesCS//SetBuffer("InitBuffer", *emitterD3D->particleBuffer);
			//s_emitParticlesCS->SetGlobalResource("particlesWrite", *emitterD3D->particleUAV2);
			//s_emitParticlesCS->SetUAV("particlesWrite2", *emitterD3D->particleUAV1);

			//s_emitParticlesCS->Dispatch(amountOfParticles, 1, 1);

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

			//bind CS
			//s_updateParticlesCS->SetUAV("particlesWrite", *s_activeParticleUAV);
			//s_updateParticlesCS->SetUAV("billboards", *emitter->GetD3DData()->billboardsUAV);
			//s_updateParticlesCS->SetResource("particlesRead", *s_activeParticleSRV);

			//s_updateParticlesCS->Dispatch(emitter->GetSpawnedParticleCount() / 256 + 1, 1, 1);

		}

		void ParticleSystem::DrawParticles()
		{

			//UpdateParticles(emitter);

			
			//emitter->GetMaterial()->m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			//emitter->GetMaterial()->GetShader()->BindVertexBuffer(NULL, 0, 0);

			//emitter->GetMaterial()->Draw(emitter->GetNrOfMaxParticles() * 6, 0);
						

//			ThomasCore::GetDeviceContext()->OMSetBlendState(NULL, NULL, 0xffffffff);

		}

		
	}
}

