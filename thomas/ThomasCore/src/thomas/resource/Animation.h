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
			class Animation;
		}
	}
	namespace resource {

		class THOMAS_API Animation : public Resource {
		private:
		public:
			Animation(std::string path);

			graphics::animation::Animation* GetAnimation();

			~Animation();

		public:
		private:
			std::shared_ptr<graphics::animation::Animation> m_data;
		};
	}
}