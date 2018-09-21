#pragma once

#pragma unmanaged
#include <thomas\object\component\LightComponent.h>

#pragma managed
#include "../Component.h"
#include "../../Utility.h"
namespace ThomasEngine
{
	[ExecuteInEditor]
	public ref class LightComponent : public Component
	{
	private:

	public:
		enum class LIGHT_TYPES
		{
			DIRECTIONAL = 0,
			POINT = 1,
			SPOT = 2,
			AREA = 3
		};
		
		
		LightComponent() : Component(new thomas::object::component::LightComponent()) {}

		property LIGHT_TYPES Type {
			LIGHT_TYPES get() {
				return LIGHT_TYPES(((thomas::object::component::LightComponent*)nativePtr)->GetType());
			}
			void set(LIGHT_TYPES value) {
				((thomas::object::component::LightComponent*)nativePtr)->SetType((thomas::graphics::LightManager::LIGHT_TYPES)value);
			}
		}

		property Color DiffuseColor {
			Color get() {
				return Utility::Convert(((thomas::object::component::LightComponent*)nativePtr)->GetColorDiffuse());
			}
			void set(Color value) {
				((thomas::object::component::LightComponent*)nativePtr)->SetColorDiffuse(Utility::Convert(value));
			}
		}

		property Color SpecularColor {
			Color get() {
				return Utility::Convert(((thomas::object::component::LightComponent*)nativePtr)->GetColorSpecular());
			}
			void set(Color value) {
				((thomas::object::component::LightComponent*)nativePtr)->SetColorSpecular(Utility::Convert(value));
			}
		}
		property float Intensity {
			float get() {
				return ((thomas::object::component::LightComponent*)nativePtr)->GetIntensity();
			}
			void set(float value) {
				((thomas::object::component::LightComponent*)nativePtr)->SetIntensity(value);
			}
		}
		property float SpotInnerAngle {
			float get() {
				return ((thomas::object::component::LightComponent*)nativePtr)->GetSpotInnerAngle();
			}
			void set(float value) {
				((thomas::object::component::LightComponent*)nativePtr)->SetSpotInnerAngle(value);
			}
		}
		property float SpotOuterAngle {
			float get() {
				return ((thomas::object::component::LightComponent*)nativePtr)->GetSpotOuterAngle();
			}
			void set(float value) {
				((thomas::object::component::LightComponent*)nativePtr)->SetSpotOuterAngle(value);
			}
		}
		property float ConstantAttenuation {
			float get() {
				return ((thomas::object::component::LightComponent*)nativePtr)->GetConstantAttenuation();
			}
			void set(float value) {
				((thomas::object::component::LightComponent*)nativePtr)->SetConstantAttenuation(value);
			}
		}
		property float LinearAttenuation {
			float get() {
				return ((thomas::object::component::LightComponent*)nativePtr)->GetLinearAttenuation();
			}
			void set(float value) {
				((thomas::object::component::LightComponent*)nativePtr)->SetLinearAttenuation(value);
			}
		}
		property float QuadraticAttenuation {
			float get() {
				return ((thomas::object::component::LightComponent*)nativePtr)->GetQuadraticAttenuation();
			}
			void set(float value) {
				((thomas::object::component::LightComponent*)nativePtr)->SetQuadraticAttenuation(value);
			}
		}
		
		property Vector2 AreaRectangle {
			Vector2 get() {
				return Utility::Convert(((thomas::object::component::LightComponent*)nativePtr)->GetRectangleDimensions());
			}
			void set(Vector2 value) {
				((thomas::object::component::LightComponent*)nativePtr)->SetRectangleDimensions(Utility::Convert(value));
			} 
		}
		
		void Update() override
		{
			((thomas::object::component::LightComponent*)nativePtr)->Update();
		}
	};
}