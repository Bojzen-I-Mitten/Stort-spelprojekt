#include "ParticleSystem.h"
#include "../object/component/ParticleEmitterComponent.h"
#include "../object/component/Transform.h"
#include "../object/component/Camera.h"
#include "../object/GameObject.h"
//#include "../resource/ComputeShader.h"
#include "Renderer.h"
#include "../utils/d3d.h"
#include "../ThomasCore.h"
#include "../resource/texture/Texture.h"
#include "../ThomasTime.h"
#include "Mesh.h"

namespace thomas
{
	namespace graphics
	{

		resource::ComputeShader* ParticleSystem::s_updateParticlesCS;
		resource::ComputeShader* ParticleSystem::s_emitParticlesCS;

		ID3D11UnorderedAccessView* ParticleSystem::s_activeParticleUAV; //ping
		ID3D11ShaderResourceView* ParticleSystem::s_activeParticleSRV; //pong

		ParticleSystem::ParticleSystem()
		{


		}

		ParticleSystem::~ParticleSystem()
		{

		}

		void ParticleSystem::Initialize()
		{
			
			//s_emitParticlesCS = new resource::ComputeShader(resource::Shader::CreateShader("../Data/oldShaders/emitParticlesCS.hlsl"));
			//s_updateParticlesCS = new resource::ComputeShader(resource::Shader::CreateShader("../Data/oldShaders/updateParticlesCS.hlsl"));
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


		void ParticleSystem::SwapUAVsandSRVs(object::component::ParticleEmitterComponent * emitter)
		{
			object::component::ParticleEmitterComponent::D3DData* emitterD3D = emitter->GetD3DData();

			emitterD3D->swapUAVandSRV = !emitterD3D->swapUAVandSRV;
			
			if (emitterD3D->swapUAVandSRV)
			{
				s_activeParticleUAV = emitterD3D->particleUAV1;
				s_activeParticleSRV = emitterD3D->particleSRV2;
			}
			else
			{
				s_activeParticleUAV = emitterD3D->particleUAV2;
				s_activeParticleSRV = emitterD3D->particleSRV1;
			}

		}

		void ParticleSystem::SpawnParticles(object::component::ParticleEmitterComponent * emitter, int amountOfParticles)
		{
			object::component::ParticleEmitterComponent::D3DData* emitterD3D = emitter->GetD3DData();
			
			s_emitParticlesCS//SetBuffer("InitBuffer", *emitterD3D->particleBuffer);
			//s_emitParticlesCS->SetGlobalResource("particlesWrite", *emitterD3D->particleUAV2);
			//s_emitParticlesCS->SetUAV("particlesWrite2", *emitterD3D->particleUAV1);

			//s_emitParticlesCS->Dispatch(amountOfParticles, 1, 1);

		}

		void ParticleSystem::UpdateParticles(object::component::ParticleEmitterComponent * emitter)
		{

			SwapUAVsandSRVs(emitter);

			//bind CS
			//s_updateParticlesCS->SetUAV("particlesWrite", *s_activeParticleUAV);
			//s_updateParticlesCS->SetUAV("billboards", *emitter->GetD3DData()->billboardsUAV);
			//s_updateParticlesCS->SetResource("particlesRead", *s_activeParticleSRV);

			//s_updateParticlesCS->Dispatch(emitter->GetSpawnedParticleCount() / 256 + 1, 1, 1);

		}

		void ParticleSystem::DrawParticles(object::component::ParticleEmitterComponent* emitter)
		{

			UpdateParticles(emitter);

			
			//emitter->GetMaterial()->m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
			//emitter->GetMaterial()->GetShader()->BindVertexBuffer(NULL, 0, 0);

			//emitter->GetMaterial()->Draw(emitter->GetNrOfMaxParticles() * 6, 0);
						

			ThomasCore::GetDeviceContext()->OMSetBlendState(NULL, NULL, 0xffffffff);

		}

		void ParticleSystem::CreateBillboardUAVandSRV(int maxAmountOfParticles, ID3D11Buffer*& buffer, ID3D11UnorderedAccessView*& uav, ID3D11ShaderResourceView*& srv)
		{
			UINT bytewidth = sizeof(BillboardStruct) * maxAmountOfParticles;

			UINT structurebytestride = sizeof(BillboardStruct);
			thomas::utils::D3d::CreateBufferAndUAV(NULL, bytewidth, structurebytestride, buffer, uav, srv);

		}
	}
}

