#include "ParticleEmitterComponent.h"
#include <cstdlib>
#include <ctime>
#include "../GameObject.h"
#include <fstream>
#include "../../utils/d3d.h"

#include "..\..\resource\Material.h"
#include "..\..\graphics\ParticleSystem.h"
#include "Transform.h"
#include "../../ThomasTime.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{
			thomas::object::component::ParticleEmitterComponent::ParticleEmitterComponent()
			{

				m_emissionRate = 1.0;
				m_looping = false;
				m_isEmitting = false;

				m_particleBufferStruct = m_particleSystem->AddEmitterToSystem();
				
			}

			ParticleEmitterComponent::~ParticleEmitterComponent()
			{
				
			}

			void ParticleEmitterComponent::SetParticleSystem(std::shared_ptr<graphics::ParticleSystem> particleSystem)
			{
				m_particleSystem = particleSystem;
			}

			unsigned ParticleEmitterComponent::NrOfParticlesToEmitThisFrame()
			{
				return m_emissionRate * ThomasTime::GetDeltaTime();
			}

			void ParticleEmitterComponent::Update()
			{
				m_particleBufferStruct.position = m_gameObject->m_transform->GetPosition();

				unsigned nrOfParticlesToEmit = NrOfParticlesToEmitThisFrame();
				if (m_isEmitting)
				{
					if (nrOfParticlesToEmit > 0)
					{
						m_particleBufferStruct.nrOfParticlesToEmit = nrOfParticlesToEmit;
						//m_particleSystem->AddEmitterToSpawn(this);
					}
				
				}
				
			}

			void ParticleEmitterComponent::OnEnable()
			{
			}

			void ParticleEmitterComponent::OnDisable()
			{
			}


			void ParticleEmitterComponent::SetSpread(float const& other)
			{
				m_particleBufferStruct.spread = other;
			}

			
			void ParticleEmitterComponent::SetSpeed(float const& speed)
			{
				SetMinSpeed(speed);
				SetMaxSpeed(speed);
				SetEndSpeed(speed);
			}
			void ParticleEmitterComponent::SetMaxSpeed(float const& other)
			{
				m_particleBufferStruct.maxSpeed = other;
			}
			void ParticleEmitterComponent::SetMinSpeed(float const& other)
			{
				m_particleBufferStruct.minSpeed = other;
			}
			void ParticleEmitterComponent::SetEndSpeed(float const& other)
			{
				m_particleBufferStruct.endSpeed = other;
			}

			float ParticleEmitterComponent::GetMaxSpeed() const
			{
				return m_particleBufferStruct.maxSpeed;
			}

			float ParticleEmitterComponent::GetMinSpeed() const
			{
				return m_particleBufferStruct.minSpeed;
			}

			float ParticleEmitterComponent::GetEndSpeed() const
			{
				return m_particleBufferStruct.endSpeed;
			}

			
			void ParticleEmitterComponent::SetSize(float const& size)
			{
				SetMinSize(size);
				SetMaxSize(size);
				SetEndSize(size);
			}
			void ParticleEmitterComponent::SetMaxSize(float const& other)
			{
				m_particleBufferStruct.maxSize = other;
			}
			void ParticleEmitterComponent::SetMinSize(float const& other)
			{
				m_particleBufferStruct.minSize = other;
			}
			void ParticleEmitterComponent::SetEndSize(float const& other)
			{
				m_particleBufferStruct.endSize = other;
			}

			float ParticleEmitterComponent::GetMaxSize() const
			{
				return m_particleBufferStruct.maxSize;
			}

			float ParticleEmitterComponent::GetMinSize() const
			{
				return m_particleBufferStruct.minSize;
			}

			float ParticleEmitterComponent::GetEndSize() const
			{
				return m_particleBufferStruct.endSize;
			}


			void ParticleEmitterComponent::SetLifeTime(float const& lifeTime)
			{
				SetMinLifeTime(lifeTime);
				SetMaxLifeTime(lifeTime);
			}
			void ParticleEmitterComponent::SetMinLifeTime(float const& other)
			{
				m_particleBufferStruct.minLifeTime = other;
			}
			float ParticleEmitterComponent::GetMaxLifeTime() const
			{
				return m_particleBufferStruct.maxLifeTime;
			}
			float ParticleEmitterComponent::GetMinLifeTime() const
			{
				return m_particleBufferStruct.minLifeTime;
			}
			void ParticleEmitterComponent::SetMaxLifeTime(float const& other)
			{
				m_particleBufferStruct.maxLifeTime = other;
			}

			void ParticleEmitterComponent::SetRotationSpeed(float const& other)
			{
				SetMinRotationSpeed(other);
				SetMaxRotationSpeed(other);
			}

			void ParticleEmitterComponent::SetMinRotationSpeed(float const & other)
			{
				m_particleBufferStruct.minRotationSpeed = other;
			}

			void ParticleEmitterComponent::SetMaxRotationSpeed(float const & other)
			{
				m_particleBufferStruct.maxRotationSpeed = other;
			}

			float ParticleEmitterComponent::GetMinRotationSpeed() const
			{
				return m_particleBufferStruct.minRotationSpeed;
			}

			float ParticleEmitterComponent::GetMaxRotationSpeed() const
			{
				return m_particleBufferStruct.maxRotationSpeed;
			}
			

			void ParticleEmitterComponent::SetLooping(bool const& other)
			{
				m_looping = other;
			}

			bool ParticleEmitterComponent::IsLooping() const
			{
				return m_looping;
			}

			void ParticleEmitterComponent::SetGravity(float const& other)
			{
				m_particleBufferStruct.gravity = other;
			}

			float ParticleEmitterComponent::GetGravity() const
			{
				return m_particleBufferStruct.gravity;
			}

			void ParticleEmitterComponent::SetRadius(float const& radius)
			{
				m_particleBufferStruct.radius = radius;
			}

			float ParticleEmitterComponent::GetRadius() const
			{
				return m_particleBufferStruct.radius;
			}

			void ParticleEmitterComponent::SpawnAtSphereEdge(bool const& other)
			{
				m_particleBufferStruct.spawnAtSphereEdge = other;
			}

			bool ParticleEmitterComponent::IsSpawningAtSphereEdge() const
			{
				return m_particleBufferStruct.spawnAtSphereEdge;
			}


			void ParticleEmitterComponent::StartEmitting()
			{
				m_isEmitting = true;
			}

			void ParticleEmitterComponent::StopEmitting()
			{
				m_isEmitting = false;
			}


			bool ParticleEmitterComponent::IsEmitting() const
			{
				return m_isEmitting;
			}


			void ParticleEmitterComponent::SetMaterial(resource::Material * material)
			{
				m_material = material;
			}

			resource::Material * ParticleEmitterComponent::GetMaterial() const
			{
				return m_material;
			}


			void ParticleEmitterComponent::SetEmissionRate(unsigned const& other)
			{
				m_emissionRate = other;
			}


			unsigned ParticleEmitterComponent::GetEmissionRate() const
			{
				return m_emissionRate;
			}

		
			
		}
	}
}