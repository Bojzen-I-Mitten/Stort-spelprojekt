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
		class Texture2D;
		class Texture2DArray;
	}
	namespace graphics
	{
		class ParticleSystem
		{
		public:
			struct InitParticleBufferStruct
			{
				math::Vector3 position;
				float distanceFromSphereCenter;

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

				math::Vector3 direction;
				float pad;

				unsigned nrOfParticlesToEmit;
				unsigned spawnAtSphereEdge;
				unsigned rand;
				unsigned textureIndex;

			};
			enum BLEND_STATE
			{
				ALPHA = 0,
				ADDITIVE = 1
			};


		private:
			struct BillboardStruct
			{
				math::Vector3 quad[2][3];
				float texIndex;
				float pad;
				math::Vector2 uvs[2][3];
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
			static void InitializeGlobalSystems();
			static std::shared_ptr<ParticleSystem> GetGlobalAlphaBlendingSystem();
			static std::shared_ptr<ParticleSystem> GetGlobalAdditiveBlendingSystem();
			static void DestroyGlobalSystems();
			ParticleSystem();
			~ParticleSystem();

			void Initialize(unsigned maxNrOfParticles, BLEND_STATE blendState);
			void Destroy();
			
			void AddEmitterToSpawn(InitParticleBufferStruct& emitterInitData);
			unsigned AddTexture(resource::Texture2D* tex);
			void DeRefTexFromTexArray(unsigned i);
			
			void UpdateParticleSystem();
			void DrawParticles();
			
		private:

			void SpawnParticles();
			void UpdateAliveCount();
		

		private:
			static std::shared_ptr<ParticleSystem> s_globalAlphaBlendingSystem;
			static std::shared_ptr<ParticleSystem> s_globalAdditiveBlendingSystem;

			unsigned m_maxNrOfParticles;

			std::unique_ptr<utils::buffers::ByteAddressBuffer> m_bufferCounters;//struct{uint deadcount, uint alivecount , uint maxcount, -}

			resource::ComputeShader* m_emitParticlesCS;
			resource::ComputeShader* m_updateParticlesCS;
			resource::ComputeShader* m_calculateEmitCountCS;
			std::unique_ptr<utils::buffers::ByteAddressBuffer> m_bufferIndirectArgs;//updateDispatchIndirect[3] + drawIndirectArgs[4]

			std::unique_ptr<utils::buffers::StructuredBuffer> m_bufferSpawn;
			std::unique_ptr<utils::buffers::Buffer>			  m_bufferSpawnIndex;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_bufferUpdate;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_bufferDeadList;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_bufferAliveListPing;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_bufferAliveListPong;
			std::unique_ptr<utils::buffers::StructuredBuffer> m_bufferBillboard;
			bool m_pingpong;

			BLEND_STATE m_blendState;
			ID3D11CommandList* m_commandList;

			std::vector<InitParticleBufferStruct> m_emitters;
			
			resource::Shader* m_particleShader;
			resource::Texture2DArray* m_texArr;

			static const unsigned int UPDATE_THREAD_DIM_X = 256;
			static const unsigned int EMIT_THREAD_DIM_X = 128;
		};

	
	}
}