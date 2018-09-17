#include "Animation.h"

#include "../utils/AssimpLoader.h"
#include "../graphics/animation/data/AnimationData.h"

namespace thomas {
	namespace resource {

		Animation::Animation(std::string path) : Resource(path)
		{
			OnChanged();	// Load
		}

		Animation::~Animation()
		{}
		
		bool Animation::HasAnimation()
		{
			return m_data.operator bool();
		}
		graphics::animation::AnimationData * Animation::GetAnimation()
		{
			return m_data.get();
		}
		
		void Animation::OnChanged()
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