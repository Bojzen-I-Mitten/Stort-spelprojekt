#pragma once
#include <vector>
#include <memory>
#include "..\utils\Buffers.h"


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
	}
	namespace graphics
	{
		class ParticleSystem
		{
		private:
			
			

			
		public:
			ParticleSystem();
			~ParticleSystem();

			void Initialize();
			void Destroy();

			bool AddEmitterToSpawn(object::component::ParticleEmitterComponent* emitter);
			bool AddEmitterToUpdate(object::component::ParticleEmitterComponent* emitter);

			void SpawnParticles();
			void UpdateParticles();
			void DrawParticles();

		private:
			unsigned m_currentNrOfParticles;
			


			resource::ComputeShader* m_emitParticlesCS;
			resource::ComputeShader* m_updateParticlesCS;

			
			std::unique_ptr<utils::buffers::StructuredBuffer> m_particleSpawn;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_particleUpdatePing;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_particleUpdatePong;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_particleDeadList;
			bool m_pingpong;


			std::vector<object::component::ParticleEmitterComponent*> m_spawningEmitters;
			std::vector<object::component::ParticleEmitterComponent*> m_updateEmitters; 
			
		public:

		};

	
	}
}