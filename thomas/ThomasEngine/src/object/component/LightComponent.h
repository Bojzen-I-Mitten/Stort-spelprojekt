#pragma once
#include "../Component.h"
#include "../../Utility.h"

namespace thomas { namespace object { namespace component { class LightComponent; } } }
namespace ThomasEngine
{
	[ExecuteInEditor]
	public ref class LightComponent : public Component
	{
	private:
		[Newtonsoft::Json::JsonIgnoreAttribute]
		property thomas::object::component::LightComponent* light {
			thomas::object::component::LightComponent* get();
		}
	public:
		enum class LIGHT_TYPES
		{
			DIRECTIONAL = 0,
			POINT = 1,
			SPOT = 2,
			AREA = 3
		};
		
		LightComponent();

		property LIGHT_TYPES Type {
			LIGHT_TYPES get();
			void set(LIGHT_TYPES value);
		}

		property Color DiffuseColor {
			Color get();
			void set(Color value);
		}

		property Color SpecularColor {
			Color get();
			void set(Color value);
		}
		property float Intensity {
			float get();
			void set(float value);
		}
		property float SpotInnerAngle {
			float get();
			void set(float value);
		}
		property float SpotOuterAngle {
			float get();
			void set(float value);
		}
		property float ConstantAttenuation {
			float get();
			void set(float value);
		}
		property float LinearAttenuation {
			float get();
			void set(float value);
		}
		property float QuadraticAttenuation {
			float get();
			void set(float value);
		}
		property Vector2 AreaRectangle {
			Vector2 get();
			void set(Vector2 value);
		}
		property bool CastShadows {
			bool get();
			void set(bool value);
		}
		property float ShadowHardness {
			float get();
			void set(float value);
		}
	};
}