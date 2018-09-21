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
			

			
		public:
			ParticleSystem();
			~ParticleSystem();

			void Initialize();
			void Destroy();

			void AddEmitterToInit(object::component::ParticleEmitterComponent* emitter);
			void AddEmitterToUpdate(object::component::ParticleEmitterComponent* emitter);

			void SpawnParticles();
			void UpdateParticles();
			void DrawParticles();

		private:
			resource::Shader m_updateParticlesCS;
			resource::Shader m_emitParticlesCS;

			std::vector<object::component::ParticleEmitterComponent*> m_initEmitters; 
			std::vector<object::component::ParticleEmitterComponent*> m_updateEmitters; 
		public:

		};

	
	}
}