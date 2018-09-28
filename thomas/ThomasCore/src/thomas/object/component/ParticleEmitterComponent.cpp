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
				m_offset = math::Vector3(0, 0, 0);
				m_shouldUpdateResources = true;
				m_emissionDuration = 1.0;
				m_emissionTimeLeft = m_emissionDuration;
				m_emissionTimer = 0;
				m_emissionRate = 1.0;
				m_tempEmissionRate = 1.0;
				m_looping = false;
				m_maxNrOfParticles = 0;//256 * 100 + 254;
				m_isEmitting = false;
				m_particleBufferStruct.position = math::Vector3(0, 0, 0);
				m_particleBufferStruct.spread = 1.0f;
				//m_particleBufferStruct.directionMatrix = math::Matrix::CreateLookAt(math::Vector3(0, 0, 0), math::Vector3(1, 0, 0), math::Vector3::Up).Transpose();
				m_particleBufferStruct.maxSpeed = 0.0f;
				m_particleBufferStruct.minSpeed = 0.0f;
				m_particleBufferStruct.endSpeed = 0.0f;
				m_particleBufferStruct.maxSize = 1.0f;
				m_particleBufferStruct.minSize = 1.0f;
				m_particleBufferStruct.endSize = 1.0f;
				m_particleBufferStruct.maxLifeTime = 1.0f;
				m_particleBufferStruct.minLifeTime = 1.0f;
				m_tempMaxLifeTime = 1.0f;
				m_particleBufferStruct.rotationSpeed = 0.0f;
				m_particleBufferStruct.rotation = 0;
				//m_particleBufferStruct.currentParticleStartIndex = 0;
				m_particleBufferStruct.spawnAtSphereEdge = false;
				m_particleBufferStruct.radius = 0;
				m_particleBufferStruct.rand = (std::rand() % 1000) / 1000.f;
				m_particleBufferStruct.gravity = 0.0f;
				m_paused = false;
				
				m_swapUAVsAndSRVs = true;

				m_spawnedParticleCount = 0;
				CalculateMaxNrOfParticles();
				//CreateInitBuffer();
			}

			ParticleEmitterComponent::~ParticleEmitterComponent()
			{
				SAFE_RELEASE(m_d3dData.particleBuffer1);
				SAFE_RELEASE(m_d3dData.particleBuffer2);
				SAFE_RELEASE(m_d3dData.particleUAV1);
				SAFE_RELEASE(m_d3dData.particleUAV2);
				SAFE_RELEASE(m_d3dData.particleSRV1);
				SAFE_RELEASE(m_d3dData.particleSRV2);
				SAFE_RELEASE(m_d3dData.billboardBuffer);
				SAFE_RELEASE(m_d3dData.billboardsSRV);
				SAFE_RELEASE(m_d3dData.billboardsUAV);
			}

			void ParticleEmitterComponent::Update()
			{
				if (m_particleBufferStruct.maxLifeTime != m_tempMaxLifeTime || m_emissionRate != m_tempEmissionRate)
					CalculateMaxNrOfParticles();

				if (m_emissionTimeLeft < 0.0f)
				{
					m_drawTimer -= ThomasTime::GetDeltaTime();
					StopEmitting();
				}


				if (m_shouldUpdateResources)
				{
					m_shouldUpdateResources = false;
					CreateParticleUAVsandSRVs();

					//m_particleBufferStruct.currentParticleStartIndex = 0;
					
					//graphics::ParticleSystem::SpawnParticles(this, m_maxNrOfParticles);
					m_spawnedParticleCount = m_maxNrOfParticles;
					//graphics::ParticleSystem::UpdateParticles(this);
				}


				if (m_isEmitting && !m_paused)
				{
					if (!m_looping)
						m_emissionTimeLeft -= ThomasTime::GetDeltaTime();

					m_emissionTimer += ThomasTime::GetDeltaTime();
					UINT numberOfParticlesToEmit = UINT(m_emissionTimer / (1.0f / m_emissionRate));
					if (numberOfParticlesToEmit > 0)
					{
						m_emissionTimer = 0;
						m_particleBufferStruct.position = m_gameObject->m_transform->GetPosition() + math::Vector3::Transform(m_offset, math::Matrix::CreateFromQuaternion(m_gameObject->m_transform->GetRotation())) ;
						
						UpdateDirection();
						
						m_particleBufferStruct.rand = (std::rand() % 1000) / 1000.f;

						//graphics::ParticleSystem::SpawnParticles(this, numberOfParticlesToEmit);
						//m_particleBufferStruct.currentParticleStartIndex = (m_particleBufferStruct.currentParticleStartIndex + numberOfParticlesToEmit) % m_maxNrOfParticles;
						m_spawnedParticleCount += numberOfParticlesToEmit;
						m_spawnedParticleCount = min(m_spawnedParticleCount, m_maxNrOfParticles);
					}
				}
			}

			void ParticleEmitterComponent::TogglePause()
			{
				m_paused = !m_paused;
			}

			void ParticleEmitterComponent::SetSpread(float const& other)
			{
				m_particleBufferStruct.spread = other;
			}
			void ParticleEmitterComponent::SwapUAVsAndSRVs()
			{
				m_swapUAVsAndSRVs = !m_swapUAVsAndSRVs;

				if (m_swapUAVsAndSRVs)
				{
					m_activeParticleUAV = m_d3dData.particleUAV1;
					m_activeParticleSRV = m_d3dData.particleSRV2;
				}
				else
				{
					m_activeParticleUAV = m_d3dData.particleUAV2;
					m_activeParticleSRV = m_d3dData.particleSRV1;
				}
			}
			void ParticleEmitterComponent::UpdateDirection()
			{
				math::Vector3 temp = math::Vector3::TransformNormal(this->m_gameObject->m_transform->Forward(), this->m_gameObject->m_transform->GetWorldMatrix());
				
				//m_particleBufferStruct.directionMatrix = math::Matrix::CreateLookAt(math::Vector3(0, 0, 0), -temp, math::Vector3::Up).Invert().Transpose();
			}

			void ParticleEmitterComponent::SetSpeed(float const& min, float const& max)
			{
				SetMinSpeed(min);
				SetMaxSpeed(max);
			}
			void ParticleEmitterComponent::SetSpeed(float const& speed)
			{
				SetSpeed(speed, speed);
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
			float ParticleEmitterComponent::GetMaxSpeed() const
			{
				return m_particleBufferStruct.maxSpeed;
			}
			float ParticleEmitterComponent::GetMinSpeed() const
			{
				return m_particleBufferStruct.minSpeed;
			}
			void ParticleEmitterComponent::SetEndSpeed(float const& other)
			{
				m_particleBufferStruct.endSpeed = other;
			}
			float ParticleEmitterComponent::GetEndSpeed() const
			{
				return m_particleBufferStruct.endSpeed;
			}
			
			void ParticleEmitterComponent::SetSize(float const& min, float const& max)
			{
				SetMinSize(min);
				SetMaxSize(max);
			}
			void ParticleEmitterComponent::SetSize(float const& size)
			{
				SetSize(size, size);
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
			float ParticleEmitterComponent::GetMaxSize() const
			{
				return m_particleBufferStruct.maxSize;
			}
			float ParticleEmitterComponent::GetMinSize() const
			{
				return m_particleBufferStruct.minSize;
			}
			void ParticleEmitterComponent::SetEndSize(float const& other)
			{
				m_particleBufferStruct.endSize = other;
			}
			float ParticleEmitterComponent::GetEndSize() const
			{
				return m_particleBufferStruct.endSize;
			}
			void ParticleEmitterComponent::SetLifeTime(float const& min, float const& max)
			{
				SetMinLifeTime(min);
				SetMaxLifeTime(max);
			}
			void ParticleEmitterComponent::SetLifeTime(float lifeTime)
			{
				SetLifeTime(lifeTime, lifeTime);

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
				m_tempMaxLifeTime = other;
				CalculateMaxNrOfParticles();
			}
			void ParticleEmitterComponent::SetRotationSpeed(float const& other)
			{
				m_particleBufferStruct.rotationSpeed = other;
			}
			float ParticleEmitterComponent::GetRotationSpeed() const
			{
				return m_particleBufferStruct.rotationSpeed;
			}
			void ParticleEmitterComponent::SetRotation(float const& other)
			{
				m_particleBufferStruct.rotation = other;
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
				//m_drawTimer = m_particleBufferStruct.maxLifeTime + m_particleBufferStruct.maxDelay;
				if (!m_isEmitting)
				{
					m_isEmitting = true;
					m_emissionTimeLeft = m_emissionDuration;
				}
			}

			void ParticleEmitterComponent::StopEmitting(bool force)
			{
				m_isEmitting = false;
				if (force)
				{
					//m_particleBufferStruct.currentParticleStartIndex = 0;

					//graphics::ParticleSystem::SpawnParticles(this, m_maxNrOfParticles);

				}
			}

			bool ParticleEmitterComponent::IsEmitting() const
			{
				return m_isEmitting;
			}


			void ParticleEmitterComponent::CreateParticleUAVsandSRVs()
			{
				SAFE_RELEASE(m_d3dData.particleBuffer1);
				SAFE_RELEASE(m_d3dData.particleBuffer2);
				SAFE_RELEASE(m_d3dData.particleUAV1);
				SAFE_RELEASE(m_d3dData.particleUAV2);
				SAFE_RELEASE(m_d3dData.particleSRV1);
				SAFE_RELEASE(m_d3dData.particleSRV2);
				SAFE_RELEASE(m_d3dData.billboardBuffer);
				SAFE_RELEASE(m_d3dData.billboardsSRV);
				SAFE_RELEASE(m_d3dData.billboardsUAV);


				UINT bytewidth = sizeof(ParticleStruct) * m_maxNrOfParticles;
				UINT structurebytestride = sizeof(ParticleStruct);

				utils::D3d::CreateBufferAndUAV(NULL, bytewidth, structurebytestride, m_d3dData.particleBuffer1, m_d3dData.particleUAV1, m_d3dData.particleSRV1);
				utils::D3d::CreateBufferAndUAV(NULL, bytewidth, structurebytestride, m_d3dData.particleBuffer2, m_d3dData.particleUAV2, m_d3dData.particleSRV2);


				bytewidth = sizeof(BillboardStruct) * m_maxNrOfParticles;
				structurebytestride = sizeof(BillboardStruct);
				thomas::utils::D3d::CreateBufferAndUAV(NULL, bytewidth, structurebytestride, m_d3dData.billboardBuffer, m_d3dData.billboardsUAV, m_d3dData.billboardsSRV);

			}



			void ParticleEmitterComponent::CalculateMaxNrOfParticles()
			{
				m_particleBufferStruct.maxLifeTime = m_tempMaxLifeTime;
				m_emissionRate = m_tempEmissionRate;
				//m_maxNrOfParticles = unsigned((m_particleBufferStruct.maxLifeTime + m_particleBufferStruct.maxDelay)*m_emissionRate);
				m_shouldUpdateResources = true;

			}

	
			void ParticleEmitterComponent::SetEmissionRate(float const& emissionRate)
			{
				m_emissionRate = emissionRate;
				m_tempEmissionRate = emissionRate;
				m_emissionTimer = 1.0f/m_emissionRate;
				CalculateMaxNrOfParticles();
			}

			void ParticleEmitterComponent::SetEmissionDuration(float const& duration)
			{
				m_emissionDuration = duration;
				m_emissionTimeLeft = m_emissionDuration;
			}

			float ParticleEmitterComponent::GetEmissionRate() const
			{
				return m_emissionRate;
			}

			ParticleEmitterComponent::D3DData * ParticleEmitterComponent::GetD3DData() 
			{
				return &m_d3dData;
			}

			ParticleEmitterComponent::InitParticleBufferStruct& ParticleEmitterComponent::GetInitData()
			{
				return m_particleBufferStruct;
			}

			bool ParticleEmitterComponent::IsPaused() const
			{
				return m_paused;
			}


			unsigned int ParticleEmitterComponent::GetNrOfMaxParticles() const
			{
				return m_maxNrOfParticles;
			}

			unsigned int ParticleEmitterComponent::GetSpawnedParticleCount() const
			{
				return m_spawnedParticleCount;
			}
			/*
			void ParticleEmitterComponent::ExportEmitter(std::string path)
			{
				//Header size and texture size
				int texturesize;

				std::string matName = "";
				//Texturesize is how long the string is * bytes
				int matSize = matName.length() * sizeof(char);

				//Opens file
				std::ofstream file;
				file.open(path, std::ios::binary | std::ios::out);

				//Header
				file.write(reinterpret_cast<char*>(&matSize), sizeof(int));
				file.write(matName.c_str(), matSize);

				//Particle System variables
				file.write(reinterpret_cast<char*>(&m_offset), sizeof(math::Vector3));
				file.write(reinterpret_cast<char*>(&m_looping), sizeof(bool));
				file.write(reinterpret_cast<char*>(&m_directionVector), sizeof(math::Vector3));
				file.write(reinterpret_cast<char*>(&m_emissionDuration), sizeof(float));
				file.write(reinterpret_cast<char*>(&m_emissionRate), sizeof(float));
				file.write(reinterpret_cast<char*>(&m_blendState), sizeof(BlendStates));

				file.write(reinterpret_cast<char*>(&m_particleBufferStruct), sizeof(ParticleEmitterComponent::InitParticleBufferStruct));
				file.close();
			}

			void ParticleEmitterComponent::ImportEmitter(std::string path)
			{
				//Test reading file
				std::ifstream file;
				file.open(path, std::ios::binary | std::ios::in);

				int materialSize;
				file.read((char*)&materialSize, sizeof(int));

				//Read texture name
				char* materialName = (char*)malloc(materialSize + 1);
				file.read(materialName, sizeof(char) * materialSize);
				materialName[materialSize] = 0;

				//read data
				file.read((char*)&m_offset, sizeof(math::Vector3));
				file.read((char*)&m_looping, sizeof(bool));
				file.read((char*)&m_directionVector, sizeof(math::Vector3));
				file.read((char*)&m_emissionDuration, sizeof(float));
				file.read((char*)&m_emissionRate, sizeof(float));
				file.read((char*)&m_blendState, sizeof(BlendStates));
				

				//Read Particle struct
				file.read((char*)&m_particleBufferStruct, sizeof(ParticleEmitterComponent::InitParticleBufferStruct));
				//file.read((char*)&variable, sizeof(vartype));
				file.close();

				m_particleBufferStruct.currentParticleStartIndex = 0;


				m_tempEmissionRate = m_emissionRate;
				m_particleBufferStruct.maxDelay = m_tempMaxDelay;
				m_particleBufferStruct.maxLifeTime = m_tempMaxLifeTime;
				

				CalculateMaxNrOfParticles();

			}
			*/

		}
	}
}