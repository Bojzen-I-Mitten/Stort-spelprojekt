#pragma unmanaged
#include <thomas\object\component\LightComponent.h>
#pragma managed
#include "LightComponent.h"

namespace ThomasEngine
{
	LightComponent::LightComponent() : Component(new thomas::object::component::LightComponent()) {}

	thomas::object::component::LightComponent* LightComponent::light::get() { return (thomas::object::component::LightComponent*)nativePtr; }

	LightComponent::LIGHT_TYPES LightComponent::Type::get() { return LIGHT_TYPES(light->GetType()); }
	void LightComponent::Type::set(LightComponent::LIGHT_TYPES value) { light->SetType((thomas::graphics::LightManager::LIGHT_TYPES)value); }

	Color LightComponent::DiffuseColor::get() { return Utility::Convert(light->GetColorDiffuse()); }
	void LightComponent::DiffuseColor::set(Color value) { light->SetColorDiffuse(Utility::Convert(value)); }

	Color LightComponent::SpecularColor::get() { return Utility::Convert(light->GetColorSpecular()); }
	void LightComponent::SpecularColor::set(Color value) { light->SetColorSpecular(Utility::Convert(value)); }

	float LightComponent::Intensity::get() { return light->GetIntensity(); }
	void LightComponent::Intensity::set(float value) { light->SetIntensity(value); }

	float LightComponent::SpotInnerAngle::get() { return light->GetSpotInnerAngle(); }
	void LightComponent::SpotInnerAngle::set(float value) { light->SetSpotInnerAngle(value); }

	float LightComponent::SpotOuterAngle::get() { return light->GetSpotOuterAngle(); }
	void LightComponent::SpotOuterAngle::set(float value) { light->SetSpotOuterAngle(value); }

	float LightComponent::ConstantAttenuation::get() { return light->GetConstantAttenuation(); }
	void LightComponent::ConstantAttenuation::set(float value) { light->SetConstantAttenuation(value); }

	float LightComponent::LinearAttenuation::get() { return light->GetLinearAttenuation(); }
	void LightComponent::LinearAttenuation::set(float value) { light->SetLinearAttenuation(value); }

	float LightComponent::QuadraticAttenuation::get() { return light->GetQuadraticAttenuation(); }
	void LightComponent::QuadraticAttenuation::set(float value) { light->SetQuadraticAttenuation(value); }

	Vector2 LightComponent::AreaRectangle::get() { return Utility::Convert(light->GetRectangleDimensions()); }
	void LightComponent::AreaRectangle::set(Vector2 value) { light->SetRectangleDimensions(Utility::Convert(value)); }




}

