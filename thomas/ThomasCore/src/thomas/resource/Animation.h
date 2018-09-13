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

		class THOMAS_API AnimationData : public Resource {
		private:
		public:
			AnimationData(std::string path);

			graphics::animation::AnimationData* GetAnimation();

			~AnimationData();

		public:
		private:
			std::shared_ptr<graphics::animation::AnimationData> m_data;
		};
	}
}