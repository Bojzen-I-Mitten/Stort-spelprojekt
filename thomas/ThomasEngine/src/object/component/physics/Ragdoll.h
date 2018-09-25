#pragma once
#pragma managed
#include "../../Component.h"
namespace ThomasEngine
{
	[DisallowMultipleComponent]
	public ref class Ragdoll : public Component
	{
	internal:
	public:
		Ragdoll();
	//	void Update() override;

	};
}