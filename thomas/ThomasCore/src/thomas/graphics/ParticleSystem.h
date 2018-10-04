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
			
			struct IndirectBufferArgsStruct
			{
				unsigned args[3];
			};

			struct ParticleCounters
			{
				unsigned deadCount;
				unsigned aliveCount;
				unsigned realEmitCount;
				unsigned aliveCountAfterSimulation;
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

			std::unique_ptr<utils::buffers::ByteAddressBuffer> m_bufferCounters;

			resource::ComputeShader* m_emitParticlesCS;
			resource::ComputeShader* m_updateParticlesCS;
			resource::ComputeShader* m_calculateEmitCountCS;
			std::unique_ptr<utils::buffers::ByteAddressBuffer> m_bufferIndirectUpdateArgs;

			std::unique_ptr<utils::buffers::StructuredBuffer> m_bufferSpawn;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_bufferUpdate;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_bufferDeadList;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_bufferAliveListPing;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_bufferAliveListPong;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_bufferBillboard;
			bool m_pingpong;

			std::vector<object::component::ParticleEmitterComponent*> m_spawningEmitters;
			std::vector<unsigned> m_spawningEmitterEmissionRate;
			std::vector<object::component::ParticleEmitterComponent*> m_updateEmitters; 
			
			resource::Shader* m_particleShader;

		public:

		};

	
	}
}