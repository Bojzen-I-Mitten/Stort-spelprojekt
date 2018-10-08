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
					float pad2;
					float pad3;

					unsigned nrOfParticlesToEmit;
					unsigned spawnAtSphereEdge;
					unsigned rand;
					unsigned pad4;

				};

				
				
			private:
				
			public:
				void Update();
				void OnEnable();
				void OnDisable();
				ParticleEmitterComponent();
				~ParticleEmitterComponent();


				void SetSpread(float const& other);

				void SetSpeed(float const& speed);
				void SetMinSpeed(float const& other);
				void SetMaxSpeed(float const& other);
				void SetEndSpeed(float const& other);
				float GetMaxSpeed() const;
				float GetMinSpeed() const;
				float GetEndSpeed() const;
				
				void SetSize(float const& size);
				void SetMinSize(float const& other);
				void SetMaxSize(float const& other);
				void SetEndSize(float const& other);
				float GetMaxSize() const;
				float GetMinSize() const;
				float GetEndSize() const;

				void SetLifeTime(float const& lifeTime);
				void SetMaxLifeTime(float const& other);
				void SetMinLifeTime(float const& other);
				float GetMaxLifeTime() const;
				float GetMinLifeTime() const;

				void SetRotationSpeed(float const& other);
				void SetMinRotationSpeed(float const& other);
				void SetMaxRotationSpeed(float const& other);
				float GetMinRotationSpeed() const;
				float GetMaxRotationSpeed() const;

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
	
				void SetEmissionRate(float const& other);
				float GetEmissionRate() const;


				//void ExportEmitter(std::string path);
				//void ImportEmitter(std::string path);

			private:

				resource::Material* m_material;

				InitParticleBufferStruct m_particleBufferStruct;

				bool m_isEmitting;
				bool m_looping;

				float m_emissionRate;

				
			};
		}
	}
}
