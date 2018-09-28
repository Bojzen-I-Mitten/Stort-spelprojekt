#pragma once
#include "Component.h"
#include "..\..\utils\math.h"
#include <d3d11.h>

namespace thomas
{
	namespace resource {
		class Material;
	}
	namespace object
	{
		namespace component
		{
			class ParticleEmitterComponent : public Component
			{
			public:
				enum class BlendStates {
					ADDITIVE,
					ALPHA_BLEND
				};

				struct D3DData {
					ID3D11UnorderedAccessView* particleUAV1;
					ID3D11ShaderResourceView* particleSRV1;
					ID3D11Buffer* particleBuffer1;
					ID3D11UnorderedAccessView* particleUAV2;
					ID3D11ShaderResourceView* particleSRV2;
					ID3D11Buffer* particleBuffer2;

					ID3D11Buffer* initParticleBuffer;

					ID3D11UnorderedAccessView* billboardsUAV;
					ID3D11ShaderResourceView* billboardsSRV;
					ID3D11Buffer* billboardBuffer;
				};

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
					float rand;
					float rotationSpeed;
					float rotation;

					DirectX::XMFLOAT3X3 directionMatrix;

					float gravity;
					float pad1;
					float pad2;

					unsigned nrOfParticlesToEmit;
					unsigned spawnAtSphereEdge;
					unsigned pad3;
					unsigned pad4;

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

				struct BillboardStruct
				{
					math::Vector3 positions[2][3];
					math::Vector2 pad2;
					math::Vector2 uvs[2][3];
					//math::Vector4 colorFactor;
				};
			private:
				void CreateParticleUAVsandSRVs();
				//void CreateInitBuffer();
				void CalculateMaxNrOfParticles();

				
				void SwapUAVsAndSRVs();
				void UpdateDirection();


			public:
				ParticleEmitterComponent();
				~ParticleEmitterComponent();

				void Update();

				void TogglePause();

				void SetSpread(float const& other);
				void SetSpeed(float const& min, float const& max);
				void SetSpeed(float const& speed);
				void SetMaxSpeed(float const& other);
				void SetMinSpeed(float const& other);
				float GetMaxSpeed() const;
				float GetMinSpeed() const;
				void SetEndSpeed(float const& other);
				float GetEndSpeed() const;
				
				void SetSize(float const& min, float const& max);
				void SetSize(float const& size);
				void SetMaxSize(float const& other);
				void SetMinSize(float const& other);
				float GetMaxSize() const;
				float GetMinSize() const;
				void SetEndSize(float const& other);
				float GetEndSize() const;

				void SetLifeTime(float const& min, float const& max);
				void SetLifeTime(float lifeTime);
				void SetMaxLifeTime(float const& other);
				void SetMinLifeTime(float const& other);
				float GetMaxLifeTime() const;
				float GetMinLifeTime() const;

				void SetRotationSpeed(float const& other);
				float GetRotationSpeed() const;
				void SetRotation(float const& other);
				void SetLooping(bool const& other);
				bool IsLooping() const;
				void SetGravity(float const& other);
				float GetGravity() const;
				void SetRadius(float const& radius);
				float GetRadius() const;


				void SpawnAtSphereEdge(bool const& other);
				bool IsSpawningAtSphereEdge() const;
				

				void StartEmitting();
				void StopEmitting(bool force=false);
				bool IsEmitting() const;
				
				void SetMaterial(resource::Material* material);
				resource::Material* GetMaterial();
	
				void SetEmissionRate(float const& emissionRate);
				void SetEmissionDuration(float const& duration);
				float GetEmissionRate() const;

				D3DData* GetD3DData();

				InitParticleBufferStruct& GetInitData();

				bool IsPaused() const;
				unsigned int GetNrOfMaxParticles() const;
				unsigned int GetSpawnedParticleCount() const;
				//void ExportEmitter(std::string path);
				//void ImportEmitter(std::string path);

			private:

				math::Vector3 m_offset;
				D3DData m_d3dData;
				bool m_swapUAVsAndSRVs;
				resource::Material* m_material;

				InitParticleBufferStruct m_particleBufferStruct;

				bool m_paused;
				float m_tempMaxLifeTime;
				float m_tempEmissionRate;

				bool m_isEmitting;
				bool m_looping;
				float m_emissionDuration;
				bool m_shouldUpdateResources;
				unsigned int m_maxNrOfParticles;
				unsigned int m_spawnedParticleCount;
				float m_emissionRate;
				float m_emissionTimer;
				float m_emissionTimeLeft;

				float m_drawTimer;


				ID3D11UnorderedAccessView* m_activeParticleUAV;
				ID3D11ShaderResourceView* m_activeParticleSRV;
				

				BlendStates m_blendState;

			};
		}
	}
}
