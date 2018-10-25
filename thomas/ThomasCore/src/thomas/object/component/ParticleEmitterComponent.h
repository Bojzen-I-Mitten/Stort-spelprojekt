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

			private:
				unsigned NrOfParticlesToEmitThisFrame();
			public:
				void Update();
				void OnEnable();
				void OnDisable();
				void OnDestroy();
				ParticleEmitterComponent();
				~ParticleEmitterComponent();

				void OnDrawGizmosSelected();


				void SetParticleSystem(std::shared_ptr<graphics::ParticleSystem> particleSystem);


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

				void SetDistanceFromSphereCenter(float const& other);
				float GetDistanceFromSphereCenter() const;

				void SetGravity(float const& other);
				float GetGravity() const;

				void SetRadius(float const& radius);
				float GetRadius() const;

				void SpawnAtSphereEdge(bool const& other);
				bool IsSpawningAtSphereEdge() const;
				
				void SetEmissionRate(unsigned const& other);
				unsigned GetEmissionRate() const;
				void StartEmitting();
				void StopEmitting();
				bool IsEmitting() const;
				void EmitOneShot(unsigned const& nrOfPaticles);

				void SetBlendState(graphics::ParticleSystem::BLEND_STATE const& blendState);
				graphics::ParticleSystem::BLEND_STATE GetBlendState();

				void SetTexture(resource::Texture2D* other);
				resource::Texture2D* GetTexture() const;
	
				


				//void ExportEmitter(std::string path);
				//void ImportEmitter(std::string path);
				
			private:
				resource::Texture2D* m_texture;
				std::shared_ptr<graphics::ParticleSystem> m_particleSystem;

				graphics::ParticleSystem::InitParticleBufferStruct m_particleBufferStruct;

				bool m_isEmitting;
				bool m_emitOneShot;

				graphics::ParticleSystem::BLEND_STATE m_blendState;

				unsigned m_emissionRate; //Particles per second
				double m_emissionThreshold; //Only emitt when the rate*dt is above one particle

				
			};
		}
	}
}
