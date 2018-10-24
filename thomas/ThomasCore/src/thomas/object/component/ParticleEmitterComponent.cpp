#include "ParticleEmitterComponent.h"
#include "../GameObject.h"
#include "../../utils/d3d.h"

#include "..\..\resource\Material.h"
#include "..\..\graphics\ParticleSystem.h"
#include "Transform.h"
#include "../../ThomasTime.h"
#include "../../resource/texture/Texture2D.h"

#include "../../editor/gizmos/Gizmos.h"
#include <imgui/imgui.h>

namespace thomas
{
	namespace object
	{
		namespace component
		{
			thomas::object::component::ParticleEmitterComponent::ParticleEmitterComponent()
			{
				m_particleSystem = graphics::ParticleSystem::GetGlobalAlphaBlendingSystem();
				m_blendState = graphics::ParticleSystem::BLEND_STATE::ALPHA;
				m_emissionRate = 32;
				m_emissionThreshold = 0.0;
				m_isEmitting = false;
				m_emitOneShot = false;
				m_particleBufferStruct = {};

				m_particleBufferStruct.position = math::Vector3::Zero;
				m_particleBufferStruct.distanceFromSphereCenter = 0.0f;

				m_particleBufferStruct.radius = 1;
				m_particleBufferStruct.maxSpeed = 1.0f;
				m_particleBufferStruct.minSpeed = 1.0f;
				m_particleBufferStruct.endSpeed = 1.0f;

				m_particleBufferStruct.maxSize = 1.0f;
				m_particleBufferStruct.minSize = 1.0f;
				m_particleBufferStruct.endSize = 1.0f;
				m_particleBufferStruct.gravity = 0.0f;

				m_particleBufferStruct.maxLifeTime = 1.0f;
				m_particleBufferStruct.minLifeTime = 1.0f;
				m_particleBufferStruct.minRotationSpeed = 0.0f;
				m_particleBufferStruct.maxRotationSpeed = 0.0f;

				m_particleBufferStruct.direction = math::Vector3::Forward;

				m_particleBufferStruct.nrOfParticlesToEmit = 0;
				m_particleBufferStruct.spawnAtSphereEdge = (unsigned)false;
				m_particleBufferStruct.rand = std::rand();
				
				SetTexture(resource::Texture2D::GetWhiteTexture());
			}

			ParticleEmitterComponent::~ParticleEmitterComponent()
			{
				
			}


			void ParticleEmitterComponent::OnDrawGizmosSelected()
			{
				editor::Gizmos::Gizmo().SetColor(math::Color(1, 1, 0));
				editor::Gizmos::Gizmo().SetMatrix(m_gameObject->m_transform->GetWorldMatrix());
				
				math::Vector3 sphereCenter = math::Vector3::Forward * m_particleBufferStruct.distanceFromSphereCenter; 
				editor::Gizmos::Gizmo().DrawBoundingSphere(math::BoundingSphere(sphereCenter, m_particleBufferStruct.radius));
				
				editor::Gizmos::Gizmo().DrawLine(sphereCenter + math::SphericalCoordinate(math::DegreesToRadians(180), math::DegreesToRadians(0), m_particleBufferStruct.radius), math::Vector3::Zero);
				editor::Gizmos::Gizmo().DrawLine(sphereCenter + math::SphericalCoordinate(math::DegreesToRadians(0), math::DegreesToRadians(90), m_particleBufferStruct.radius), math::Vector3::Zero);
				editor::Gizmos::Gizmo().DrawLine(sphereCenter + math::SphericalCoordinate(math::DegreesToRadians(90), math::DegreesToRadians(90), m_particleBufferStruct.radius), math::Vector3::Zero);
				editor::Gizmos::Gizmo().DrawLine(sphereCenter + math::SphericalCoordinate(math::DegreesToRadians(180), math::DegreesToRadians(90), m_particleBufferStruct.radius), math::Vector3::Zero);
				editor::Gizmos::Gizmo().DrawLine(sphereCenter + math::SphericalCoordinate(math::DegreesToRadians(270), math::DegreesToRadians(90), m_particleBufferStruct.radius), math::Vector3::Zero);
				editor::Gizmos::Gizmo().DrawLine(sphereCenter + math::SphericalCoordinate(math::DegreesToRadians(0), math::DegreesToRadians(180), m_particleBufferStruct.radius), math::Vector3::Zero);
				
				//ImGui::SetNextWindowPos(ImVec2(5, 5));
				
				
				//ImGui::Begin("Overlay", NULL, ImVec2(100, 0), 0.3f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
					//ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
				/*
				std::string stop = "alt + 254";
				
				ImGui::Button(stop.c_str());*/
				//if (ImGui::ArrowButton("DisplayParticles", ImGuiDir_Right));
				//{
					//StartEmitting();
				//}
				//ImGui::SameLine();
				//ImGui::SetCursorPos(ImVec2(52, 10)); // Center buttons
				//ImGui::Text("Status asdfasdfasdf ");
				//ImGui::End();
				//StartEmitting();
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
				m_particleBufferStruct.direction = m_gameObject->m_transform->Forward();

				if (m_emitOneShot)
				{
					if (m_particleBufferStruct.nrOfParticlesToEmit > 0)
					{
						m_particleBufferStruct.rand = std::rand();
						m_particleSystem->AddEmitterToSpawn(m_particleBufferStruct);
					}
					m_emitOneShot = false;
				}
				else if (m_isEmitting)
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

			void ParticleEmitterComponent::OnDestroy()
			{
				m_particleSystem->DeRefTexFromTexArray(m_particleBufferStruct.textureIndex);
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

			void ParticleEmitterComponent::SetDistanceFromSphereCenter(float const & other)
			{
				m_particleBufferStruct.distanceFromSphereCenter = other;
			}

			float ParticleEmitterComponent::GetDistanceFromSphereCenter() const
			{
				return m_particleBufferStruct.distanceFromSphereCenter;
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
				m_particleBufferStruct.spawnAtSphereEdge = (unsigned)other;
				
			}

			bool ParticleEmitterComponent::IsSpawningAtSphereEdge() const
			{
				return (bool)m_particleBufferStruct.spawnAtSphereEdge;
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

			void ParticleEmitterComponent::EmitOneShot(unsigned const & nrOfPaticles)
			{
				m_emitOneShot = true;
				m_particleBufferStruct.nrOfParticlesToEmit = nrOfPaticles;
			}

			void ParticleEmitterComponent::SetBlendState(graphics::ParticleSystem::BLEND_STATE const & blendState)
			{
				if (blendState != m_blendState)
				{
					m_blendState = blendState;
					if (blendState == graphics::ParticleSystem::BLEND_STATE::ALPHA)
					{
						m_particleSystem = graphics::ParticleSystem::GetGlobalAlphaBlendingSystem();
					}
					else if (blendState == graphics::ParticleSystem::BLEND_STATE::ADDITIVE)
					{
						m_particleSystem = graphics::ParticleSystem::GetGlobalAdditiveBlendingSystem();
					}
				}
			}

			graphics::ParticleSystem::BLEND_STATE ParticleEmitterComponent::GetBlendState()
			{
				return m_blendState;
			}

			void ParticleEmitterComponent::SetTexture(resource::Texture2D * other)
			{
				if (other == m_texture)
					return;

				m_particleBufferStruct.textureIndex = m_particleSystem->AddTexture(other);

				m_texture = other;
			}

			resource::Texture2D * ParticleEmitterComponent::GetTexture() const
			{
				return m_texture;
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