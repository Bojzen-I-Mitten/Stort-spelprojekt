#pragma once
#include "RenderComponent.h"
#include "../../resource/ShaderProperty/ShaderPropertyMatrix.h"
#include "../../utils/Math.h"
namespace thomas
{
	namespace graphics { namespace animation { class AnimatedSkeleton; } }
	namespace object
	{
		class Camera;
		namespace component
		{
			class THOMAS_API RenderSkinnedComponent : public RenderComponent
			{
			private:
			public:
				RenderSkinnedComponent();
				virtual ~RenderSkinnedComponent();
				virtual void Update();
			protected:
			public:
				virtual void SetModel(resource::Model* model);

			public:
			private:
				resource::shaderproperty::ShaderPropertyMatrixArray m_skinArray;
				std::unique_ptr<graphics::animation::AnimatedSkeleton> m_skeleton;
			};
		}
	}
}