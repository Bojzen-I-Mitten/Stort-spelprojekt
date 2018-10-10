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
				m_particleSystem = graphics::ParticleSystem::GetGlobalSystem();
				m_emissionRate = 32;
				m_emissionThreshold = 0.0;
				m_looping = false;
				m_isEmitting = false;
				m_particleBufferStruct = {};

				m_particleBufferStruct.position = math::Vector3(0, 0, 0);
				m_particleBufferStruct.spread = 0.0f;

				//m_particleBufferStruct.directionMatrix = math::Matrix::CreateLookAt(math::Vector3(0, 0, 0), math::Vector3(1, 0, 0), math::Vector3::Up).Transpose();

				m_particleBufferStruct.radius = 0;
				m_particleBufferStruct.maxSpeed = 0.0f;
				m_particleBufferStruct.minSpeed = 0.0f;
				m_particleBufferStruct.endSpeed = 0.0f;

				m_particleBufferStruct.maxSize = 1.0f;
				m_particleBufferStruct.minSize = 1.0f;
				m_particleBufferStruct.endSize = 1.0f;
				m_particleBufferStruct.gravity = 0.0f;

				m_particleBufferStruct.maxLifeTime = 1.0f;
				m_particleBufferStruct.minLifeTime = 1.0f;
				m_particleBufferStruct.minRotationSpeed = 0.0f;
				m_particleBufferStruct.maxRotationSpeed = 0.0f;

				XMStoreFloat3x3(&m_particleBufferStruct.directionMatrix, DirectX::XMMatrixLookAtRH(math::Vector3(0, 2, 0), math::Vector3(1.0f, 2.0f, 0.0f), math::Vector3::Up));
				m_particleBufferStruct.endRotationSpeed = 0.0f;

				m_particleBufferStruct.nrOfParticlesToEmit = 0;
				m_particleBufferStruct.spawnAtSphereEdge = (unsigned)false;
				m_particleBufferStruct.rand = std::rand();
				
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
				m_emissionThreshold += ThomasTime::GetDeltaTime() * (double)m_emissionRate;
				if (m_emissionThreshold > 1.0)
				{
					unsigned temp = (unsigned)m_emissionThreshold;
					m_emissionThreshold = 0.0;
					return temp;
				}
				return 0;
			}
			
			void ParticleEmitterComponent::Update()
			{
				m_particleBufferStruct.position = m_gameObject->m_transform->GetPosition();

				if (m_isEmitting)
				{
					unsigned nrOfParticlesToEmit = NrOfParticlesToEmitThisFrame();
					if (nrOfParticlesToEmit > 0)
					{
						m_particleBufferStruct.nrOfParticlesToEmit = nrOfParticlesToEmit;
						m_particleBufferStruct.rand = std::rand();
						m_particleSystem->AddEmitterToSpawn(m_particleBufferStruct);
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