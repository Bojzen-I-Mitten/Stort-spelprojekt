#include "BoneNode.h"
#include "AnimatedSkeleton.h"


namespace thomas {
	namespace animation {
		/*
		BoneNode::BoneNode(const AnimatedSkeleton &skel, unsigned int bone)
			: _ref(&skel), _bone(bone)
		{
		}


		BoneNode::~BoneNode()
		{
		}

		void BoneNode::update(float dT)
		{
			calcModelMatrix();
			_modelMatrix = _ref->getBoneWorld(_bone) * _modelMatrix;
			updateChildren(dT);
		}
		*/
	}
}