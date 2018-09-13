#include "Animation.h"


namespace thomas {
	namespace resource {

		AnimationData::AnimationData(std::string path) : Resource(path)
		{
		}
		graphics::animation::AnimationData * AnimationData::GetAnimation()
		{
			return m_data.get();
		}
	}
}