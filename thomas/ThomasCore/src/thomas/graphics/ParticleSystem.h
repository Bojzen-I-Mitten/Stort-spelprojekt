#pragma once
#include <vector>
#include <memory>
#include "..\utils\Buffers.h"
#include "../utils/Math.h"


namespace thomas
{
	namespace object
	{
		namespace component
		{
			class ParticleEmitterComponent;
		}
	}
	namespace resource
	{
		class ComputeShader;
		class Shader;
	}
	namespace graphics
	{
		class ParticleSystem
		{
		private:
			struct BillboardStruct
			{
				math::Vector3 positions[2][3];
				math::Vector2 pad2;
				math::Vector2 uvs[2][3];
				//math::Vector4 colorFactor;
			};
			

			
		public:
			ParticleSystem();
			~ParticleSystem();

			void Initialize(unsigned maxNrOfParticles = 25000);
			void Destroy();

			bool AddEmitterToSpawn(object::component::ParticleEmitterComponent* emitter);
			bool AddEmitterToUpdate(object::component::ParticleEmitterComponent* emitter);

			void SpawnParticles();
			void UpdateParticles();
			void DrawParticles();

		private:
			unsigned m_maxNrOfParticles;
			unsigned m_emittedParticles; 

			resource::ComputeShader* m_emitParticlesCS;
			resource::ComputeShader* m_updateParticlesCS;

			
			std::unique_ptr<utils::buffers::StructuredBuffer> m_spawnBuffer;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_updateBuffer;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_deadList;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_aliveListPing;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_aliveListPong;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_billboardBuffer;
			bool m_pingpong;

			std::vector<object::component::ParticleEmitterComponent*> m_spawningEmitters;
			std::vector<unsigned> m_spawningEmitterEmissionRate;
			std::vector<object::component::ParticleEmitterComponent*> m_updateEmitters; 
			
			resource::Shader* m_particleShader;

			bool dank;
			
		public:

		};

	
	}
}