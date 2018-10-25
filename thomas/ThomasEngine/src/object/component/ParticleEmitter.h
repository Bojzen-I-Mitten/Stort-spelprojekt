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
		enum class BLEND_STATES
		{
			ALPHA,
			ADDITIVE,
			UNKNOWN
		};

	public:
		ParticleEmitter();

		[CategoryAttribute("Speed")]
		property float MinSpeed {
			float get();
			void set(float value);
		}
		[CategoryAttribute("Speed")]
		property float MaxSpeed {
			float get();
			void set(float value);
		}
		[CategoryAttribute("Speed")]
		property float EndSpeed {
			float get();
			void set(float value);
		}
		[CategoryAttribute("Size")]
		property float MinSize {
			float get();
			void set(float value);
		}
		[CategoryAttribute("Size")]
		property float MaxSize {
			float get();
			void set(float value);
		}
		[CategoryAttribute("Size")]
		property float EndSize {
			float get();
			void set(float value);
		}
		[CategoryAttribute("LifeTime")]
		property float MinLifeTime {
			float get();
			void set(float value);
		}
		[CategoryAttribute("LifeTime")]
		property float MaxLifeTime {
			float get();
			void set(float value);
		}
		[CategoryAttribute("Rotation")]
		property float MinRotationSpeed {
			float get();
			void set(float value);
		}
		[CategoryAttribute("Rotation")]
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
		property BLEND_STATES BlendState {
			BLEND_STATES get();
			void set(BLEND_STATES value);
		}
		property Texture2D^ Texture {
			Texture2D^ get();
			void set(Texture2D^ value);
		}
		property bool Emit {
			bool get();
			void set(bool value);
		}

		void EmitOneShot(unsigned nrToEmit);
	};
}