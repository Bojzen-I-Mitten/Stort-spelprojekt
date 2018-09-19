#pragma once
#include "../Common.h"
#include "../utils/Math.h"
#include "Resource.h"
#include <memory>
namespace thomas {
	namespace graphics
	{
		class Mesh;
		namespace animation {
			class Skeleton;
			class AnimationData;
		}
	}
	namespace resource {

		class THOMAS_API Animation : public Resource {
		private:
		public:
			Animation(std::string path);

			bool HasAnimation();

			graphics::animation::AnimationData* GetAnimation();

			virtual void OnChanged();

			virtual ~Animation();
			
		public:
		private:
			std::shared_ptr<graphics::animation::AnimationData> m_data;
		};
	}
}