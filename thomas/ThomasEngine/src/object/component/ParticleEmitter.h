#pragma once

#pragma unmanaged
#include <thomas\object\component\ParticleEmitterComponent.h>

#pragma managed
#include "../Component.h"
namespace ThomasEngine
{
	[ExecuteInEditor]
	public ref class ParticleEmitter : public Component
	{
	private:

	public:
		ParticleEmitter() : Component(new thomas::object::component::ParticleEmitterComponent()) {}
		property Vector2 MinMaxSpeed {
			Vector2 get() {
				//Vector2 minMaxSpeed(((thomas::object::component::ParticleEmitterComponent*)nativePtr)->GetMinSpeed(), ((thomas::object::component::ParticleEmitterComponent*)nativePtr)->GetMaxSpeed());
				return Vector2(0, 1);// minMaxSpeed;
			}
			void set(Vector2 value) {
				//((thomas::object::component::ParticleEmitterComponent*)nativePtr)->SetSpeed(value.x, value.y);
			}
		}
		property float EndSpeed {
			float get() {
				//Vector2 minMaxSpeed(((thomas::object::component::ParticleEmitterComponent*)nativePtr)->GetMinSpeed(), ((thomas::object::component::ParticleEmitterComponent*)nativePtr)->GetMaxSpeed());
				return 0.0f;// minMaxSpeed;
			}
			void set(float value) {
				//((thomas::object::component::ParticleEmitterComponent*)nativePtr)->SetSpeed(value.x, value.y);
			}
		}
		property Vector2 MinMaxSize {
			Vector2 get() {
				//Vector2 minMaxSpeed(((thomas::object::component::ParticleEmitterComponent*)nativePtr)->GetMinSpeed(), ((thomas::object::component::ParticleEmitterComponent*)nativePtr)->GetMaxSpeed());
				return Vector2(0, 1);// minMaxSpeed;
			}
			void set(Vector2 value) {
				//((thomas::object::component::ParticleEmitterComponent*)nativePtr)->SetSpeed(value.x, value.y);
			}
		}
		property float EndSize {
			float get() {
				//Vector2 minMaxSpeed(((thomas::object::component::ParticleEmitterComponent*)nativePtr)->GetMinSpeed(), ((thomas::object::component::ParticleEmitterComponent*)nativePtr)->GetMaxSpeed());
				return 0.0f;// minMaxSpeed;
			}
			void set(float value) {
				//((thomas::object::component::ParticleEmitterComponent*)nativePtr)->SetSpeed(value.x, value.y);
			}
		}
		property Vector2 MinMaxLifeTime {
			Vector2 get() {
				//Vector2 minMaxSpeed(((thomas::object::component::ParticleEmitterComponent*)nativePtr)->GetMinSpeed(), ((thomas::object::component::ParticleEmitterComponent*)nativePtr)->GetMaxSpeed());
				return Vector2(0, 1);// minMaxSpeed;
			}
			void set(Vector2 value) {
				//((thomas::object::component::ParticleEmitterComponent*)nativePtr)->SetSpeed(value.x, value.y);
			}
		}
		property float RotationSpeed {
			float get() {
				//Vector2 minMaxSpeed(((thomas::object::component::ParticleEmitterComponent*)nativePtr)->GetMinSpeed(), ((thomas::object::component::ParticleEmitterComponent*)nativePtr)->GetMaxSpeed());
				return 0.0f;// minMaxSpeed;
			}
			void set(float value) {
				//((thomas::object::component::ParticleEmitterComponent*)nativePtr)->SetSpeed(value.x, value.y);
			}
		}
		property float Gravity {
			float get() {
				//Vector2 minMaxSpeed(((thomas::object::component::ParticleEmitterComponent*)nativePtr)->GetMinSpeed(), ((thomas::object::component::ParticleEmitterComponent*)nativePtr)->GetMaxSpeed());
				return 0.0f;// minMaxSpeed;
			}
			void set(float value) {
				//((thomas::object::component::ParticleEmitterComponent*)nativePtr)->SetSpeed(value.x, value.y);
			}
		}
		property bool Looping{
			bool get() {
				//Vector2 minMaxSpeed(((thomas::object::component::ParticleEmitterComponent*)nativePtr)->GetMinSpeed(), ((thomas::object::component::ParticleEmitterComponent*)nativePtr)->GetMaxSpeed());
				return 0.0f;// minMaxSpeed;
			}
			void set(bool value) {
				//((thomas::object::component::ParticleEmitterComponent*)nativePtr)->SetSpeed(value.x, value.y);
			}
		}

		property Color StartColor {
			Color get() {
				//Vector2 minMaxSpeed(((thomas::object::component::ParticleEmitterComponent*)nativePtr)->GetMinSpeed(), ((thomas::object::component::ParticleEmitterComponent*)nativePtr)->GetMaxSpeed());
				return Color(0.0f, 0.0f, 0.0f, 0.0f);// minMaxSpeed;
			}
			void set(Color value) {
				//((thomas::object::component::ParticleEmitterComponent*)nativePtr)->SetSpeed(value.x, value.y);
			}
		}

		property Color EndColor {
			Color get() {
				//Vector2 minMaxSpeed(((thomas::object::component::ParticleEmitterComponent*)nativePtr)->GetMinSpeed(), ((thomas::object::component::ParticleEmitterComponent*)nativePtr)->GetMaxSpeed());
				return Color(0.0f, 0.0f, 0.0f, 0.0f);// minMaxSpeed;
			}
			void set(Color value) {
				//((thomas::object::component::ParticleEmitterComponent*)nativePtr)->SetSpeed(value.x, value.y);
			}
		}

		void Update() override
		{
			//((thomas::object::component::ParticleEmitterComponent*)nativePtr)->Update();
		}
	};
}