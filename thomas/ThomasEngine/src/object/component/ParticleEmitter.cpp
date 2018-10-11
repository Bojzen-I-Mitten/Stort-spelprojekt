#pragma unmanaged
#include <thomas\object\component\ParticleEmitterComponent.h>
#pragma managed
#include "ParticleEmitter.h"


namespace ThomasEngine
{
	ParticleEmitter::ParticleEmitter() : Component(new thomas::object::component::ParticleEmitterComponent()) {}

	thomas::object::component::ParticleEmitterComponent* ParticleEmitter::particleEmitter::get() { return (thomas::object::component::ParticleEmitterComponent*)nativePtr; }


	float ParticleEmitter::MinSpeed::get() { return particleEmitter->GetMinSpeed(); }
	void ParticleEmitter::MinSpeed::set(float value) { particleEmitter->SetMinSpeed(value); }
	float ParticleEmitter::MaxSpeed::get() { return particleEmitter->GetMaxSpeed(); }
	void ParticleEmitter::MaxSpeed::set(float value) { particleEmitter->SetMaxSpeed(value); }
	float ParticleEmitter::EndSpeed::get() { return particleEmitter->GetEndSpeed(); }
	void ParticleEmitter::EndSpeed::set(float value) { particleEmitter->SetEndSpeed(value); }

	float ParticleEmitter::MinSize::get() { return particleEmitter->GetMinSize(); }
	void ParticleEmitter::MinSize::set(float value) { particleEmitter->SetMinSize(value); }
	float ParticleEmitter::MaxSize::get() { return particleEmitter->GetMaxSize(); }
	void ParticleEmitter::MaxSize::set(float value) { particleEmitter->SetMaxSize(value); }
	float ParticleEmitter::EndSize::get() { return particleEmitter->GetEndSize(); }
	void ParticleEmitter::EndSize::set(float value) { particleEmitter->SetEndSize(value); }

	float ParticleEmitter::MinLifeTime::get() { return particleEmitter->GetMinLifeTime(); }
	void ParticleEmitter::MinLifeTime::set(float value) { particleEmitter->SetMinLifeTime(value); }
	float ParticleEmitter::MaxLifeTime::get() { return particleEmitter->GetMaxLifeTime(); }
	void ParticleEmitter::MaxLifeTime::set(float value) { particleEmitter->SetMaxLifeTime(value); }
	
	float ParticleEmitter::MinRotationSpeed::get() { return particleEmitter->GetMinRotationSpeed(); }
	void ParticleEmitter::MinRotationSpeed::set(float value) { particleEmitter->SetMinRotationSpeed(value); }
	float ParticleEmitter::MaxRotationSpeed::get() { return particleEmitter->GetMaxRotationSpeed(); }
	void ParticleEmitter::MaxRotationSpeed::set(float value) { particleEmitter->SetMaxRotationSpeed(value); }

	float ParticleEmitter::Radius::get() { return particleEmitter->GetRadius(); }
	void ParticleEmitter::Radius::set(float value) { particleEmitter->SetRadius(value); }

	float ParticleEmitter::Gravity::get() { return particleEmitter->GetGravity(); }
	void ParticleEmitter::Gravity::set(float value) { particleEmitter->SetGravity(value); }

	float ParticleEmitter::Distance::get() { return particleEmitter->GetDistance(); }
	void ParticleEmitter::Distance::set(float value) { particleEmitter->SetDistance(value); }

	bool ParticleEmitter::SpawnAtEdge::get() { return particleEmitter->IsSpawningAtSphereEdge(); }
	void ParticleEmitter::SpawnAtEdge::set(bool value) { particleEmitter->SpawnAtSphereEdge(value); }

	unsigned ParticleEmitter::EmissionRate::get() { return particleEmitter->GetEmissionRate(); }
	void ParticleEmitter::EmissionRate::set(unsigned value) { particleEmitter->SetEmissionRate(value); }

	bool ParticleEmitter::Emit::get() { return particleEmitter->IsEmitting(); }
	void ParticleEmitter::Emit::set(bool value) { particleEmitter->StartEmitting(); }
}