#pragma once
#include "../Component.h"

namespace thomas { namespace object { namespace component { class ParticleEmitterComponent; } } }
namespace ThomasEngine
{
	ref class Texture2D;
	[ExecuteInEditor]
	public ref class ParticleEmitter : public Component
	{
	private:
		[Xml::Serialization::XmlIgnoreAttribute]
		property thomas::object::component::ParticleEmitterComponent* particleEmitter {
			thomas::object::component::ParticleEmitterComponent* get();
		}
	public:
		ParticleEmitter();

		property float MinSpeed {
			float get();
			void set(float value);
		}
		property float MaxSpeed {
			float get();
			void set(float value);
		}
		property float EndSpeed {
			float get();
			void set(float value);
		}
		property float MinSize {
			float get();
			void set(float value);
		}
		property float MaxSize {
			float get();
			void set(float value);
		}
		property float EndSize {
			float get();
			void set(float value);
		}
		property float MinLifeTime {
			float get();
			void set(float value);
		}
		property float MaxLifeTime {
			float get();
			void set(float value);
		}
		property float MinRotationSpeed {
			float get();
			void set(float value);
		}
		property float MaxRotationSpeed {
			float get();
			void set(float value);
		}
		property float Radius {
			float get();
			void set(float value);
		}
		property float Gravity {
			float get();
			void set(float value);
		}
		property float DistanceFromSphereCenter {
			float get();
			void set(float value);
		}
		property bool SpawnAtEdge {
			bool get();
			void set(bool value);
		}
		property unsigned EmissionRate {
			unsigned get();
			void set(unsigned value);
		}
		property Texture2D^ Texture {
			Texture2D^ get();
			void set(Texture2D^ value);
		}
		property bool Emit {
			bool get();
			void set(bool value);
		}
	};
}