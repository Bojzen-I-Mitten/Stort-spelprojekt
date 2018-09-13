#include "Animation.h"

#include "../utils/AssimpLoader.h"
#include "../graphics/animation/data/AnimationData.h"

namespace thomas {
	namespace resource {

		AnimationData::AnimationData(std::string path) : Resource(path)
		{
			OnChanged();	// Load
		}
		graphics::animation::AnimationData * AnimationData::GetAnimation()
		{
			return m_data.get();
		}
		void AnimationData::OnChanged()
		{
			// Load on change & construction
			std::vector<std::shared_ptr<graphics::animation::AnimationData>> anims = utils::AssimpLoader::LoadAnimation(m_path);
			if (anims.size() == 0) {
				m_data = NULL;
				std::string err("Error loading Animation. No animation data in file: " + m_path);
				LOG(err);
			}
			else
				m_data = anims.front();
		}
	}
}