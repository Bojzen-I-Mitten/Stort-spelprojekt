#pragma once
#include "Component.h"
#include "..\..\utils\math.h"
#include <d3d11.h>
#include "../../graphics/ParticleSystem.h"

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

				
			private:
				unsigned NrOfParticlesToEmitThisFrame();
			public:
				void Update();
				void OnEnable();
				void OnDisable();
				ParticleEmitterComponent();
				~ParticleEmitterComponent();

				void SetParticleSystem(std::shared_ptr<graphics::ParticleSystem> particleSystem);


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
				void StopEmitting();
				bool IsEmitting() const;
				
				void SetMaterial(resource::Material* material);
				resource::Material* GetMaterial() const;
	
				void SetEmissionRate(unsigned const& other);
				unsigned GetEmissionRate() const;


				//void ExportEmitter(std::string path);
				//void ImportEmitter(std::string path);
				
			private:
				resource::Material* m_material;
				std::shared_ptr<graphics::ParticleSystem> m_particleSystem;

				graphics::ParticleSystem::InitParticleBufferStruct m_particleBufferStruct;

				bool m_isEmitting;

				bool m_looping;

				unsigned m_emissionRate; //Particles per second
				double m_emissionThreshold; //Only emitt when the rate*dt is above one particle

				
			};
		}
	}
}
