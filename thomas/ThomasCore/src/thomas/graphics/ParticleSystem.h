#pragma once
#include <vector>
#include <memory>
#include "..\utils\Buffers.h"
#include "../utils/Math.h"


namespace thomas
{
	
	namespace resource
	{
		class ComputeShader;
		class Shader;
	}
	namespace graphics
	{
		class ParticleSystem
		{
		public:
			struct InitParticleBufferStruct
			{
				math::Vector3 position;
				float spread;

				float radius;
				float maxSpeed;
				float minSpeed;
				float endSpeed;

				float maxSize;
				float minSize;
				float endSize;
				float maxLifeTime;

				float minLifeTime;
				float gravity;
				float minRotationSpeed;
				float maxRotationSpeed;

				DirectX::XMFLOAT3X3 directionMatrix;
				float endRotationSpeed;
				float pad;
				float pad2;

				unsigned nrOfParticlesToEmit;
				unsigned spawnAtSphereEdge;
				unsigned rand;
				unsigned pad3;

			};


		private:
			struct BillboardStruct
			{
				math::Vector3 positions[2][3];
				math::Vector2 pad2;
				math::Vector2 uvs[2][3];
				//math::Vector4 colorFactor;
			};
			struct ParticleStruct
			{
				math::Vector3 position;
				float gravity;

				math::Vector3 direction;
				float speed;

				float endSpeed;
				float size;
				float endSize;
				float lifeTime;

				float lifeTimeLeft;
				float rotationSpeed;
				float rotation;
				float pad;
			};

			

		public:
			static void InitializeGlobalSystem();
			static std::shared_ptr<ParticleSystem> GetGlobalSystem();
			ParticleSystem();
			~ParticleSystem();

			void Initialize(unsigned maxNrOfParticles);
			void Destroy();
			
			void AddEmitterToSpawn(InitParticleBufferStruct& emitterInitData);
			
			void UpdateParticleSystem();
			void DrawParticles();
		private:

			void SpawnParticles();
			void UpdateAliveCount();
		

		private:
			static std::shared_ptr<ParticleSystem> s_globalSystem;

			unsigned m_maxNrOfParticles;

			std::unique_ptr<utils::buffers::ByteAddressBuffer> m_bufferCounters;//struct{uint deadcount, uint alivecount , uint maxcount, -}
			

			resource::ComputeShader* m_emitParticlesCS;
			resource::ComputeShader* m_updateParticlesCS;
			resource::ComputeShader* m_calculateEmitCountCS;
			std::unique_ptr<utils::buffers::ByteAddressBuffer> m_bufferIndirectArgs;//updateDispatchIndirect[3] + drawIndirectArgs[4]

			std::unique_ptr<utils::buffers::StructuredBuffer> m_bufferSpawn;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_bufferUpdate;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_bufferDeadList;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_bufferAliveListPing;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_bufferAliveListPong;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_bufferBillboard;
			bool m_pingpong;

			std::vector<InitParticleBufferStruct> m_emitters;
			
			resource::Shader* m_particleShader;


		};

	
	}
}