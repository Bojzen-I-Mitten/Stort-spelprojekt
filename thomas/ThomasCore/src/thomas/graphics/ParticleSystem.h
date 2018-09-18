#pragma once
#include "../utils/Math.h"
#include <d3d11.h>


namespace thomas
{
	namespace object
	{
		namespace component
		{
			class ParticleEmitterComponent;
			class Transform;
			class Camera;
			
		}
	}
	namespace resource
	{
		class ComputeShader;
	}
	namespace graphics
	{
		
		class Mesh;
		class ParticleSystem
		{
		private:
			
			static void SwapUAVsandSRVs(object::component::ParticleEmitterComponent * emitter);//ping pong
			
		public:
			ParticleSystem();
			~ParticleSystem();

			static void CreateBillboardUAVandSRV(int maxAmountOfParticles, ID3D11Buffer*& buffer, ID3D11UnorderedAccessView*& uav, ID3D11ShaderResourceView*& srv);
			static void Initialize();
			static void Destroy();
			static void SpawnParticles(object::component::ParticleEmitterComponent* emitter, int amountOfParticles);
			static void UpdateParticles(object::component::ParticleEmitterComponent* emitter);
			static void DrawParticles(object::component::ParticleEmitterComponent* emitter);

		private:

			struct BillboardStruct
			{
				math::Vector3 positions[2][3];
				math::Vector2 pad2;
				math::Vector2 uvs[2][3];
				math::Vector4 colorFactor;
			};
			
			
			resource::Shader s_updateParticlesCS;
			resource::Shader s_emitParticlesCS;

			ID3D11UnorderedAccessView* s_activeParticleUAV;
			ID3D11ShaderResourceView* s_activeParticleSRV;

		public:

		};

	
	}
}