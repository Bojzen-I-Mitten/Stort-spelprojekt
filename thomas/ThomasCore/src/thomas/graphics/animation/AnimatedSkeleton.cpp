#include "AnimatedSkeleton.h"
#include "AnimationNode.h"
#include "AnimPlayback.h"
#include "data/Skeleton.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			AnimatedSkeleton::AnimatedSkeleton(Skeleton& ref)
				: _ref(ref), _root(), _pose(ref.getNumBones()), _skinTransform(ref.getNumBones())
			{
				clearBlendTree();
				updateSkeleton();
			}


			AnimatedSkeleton::~AnimatedSkeleton()
			{
			}


			void AnimatedSkeleton::update(float dT) {
				_root->update(dT);
				updateSkeleton();
			}
			void AnimatedSkeleton::updateSkeleton()
			{
				//Update animation tree
				// Update skin transforms
				_pose[0] = _root->calcLocalTransform(0);
				_skinTransform[0] = _pose[0] * _ref.getBone(0)._invBindPose;
				for (unsigned int i = 1; i < boneCount(); i++)
				{
					const Bone& bone = _ref.getBone(i);
					_pose[i] = _pose[bone._parentIndex] * _root->calcLocalTransform(i);
					_skinTransform[i] = _pose[i] * bone._invBindPose;
				}
			}
			/* Freeze the current animation */
			void AnimatedSkeleton::stopAnimation()
			{
				_root = NULL;
			}

			void AnimatedSkeleton::setBlendTree(std::unique_ptr<AnimationNode> &blendTree)
			{
				if (!blendTree)
					clearBlendTree();
				else
					_root = std::move(blendTree);
			}

			void AnimatedSkeleton::clearBlendTree() {
				_root = std::unique_ptr<AnimationNode>(new AnimPlayback(_ref));
			}


			const std::vector<math::Matrix>& AnimatedSkeleton::getSkin() const {
				return _skinTransform;
			}
			unsigned int AnimatedSkeleton::boneCount() const {
				return _pose.size();
			}

			/* Get the local matrix of a bone
			*/
			const math::Matrix& AnimatedSkeleton::getBoneMatrix(unsigned int bone) const
			{
				assert(bone < _pose.size());
				return _pose[bone];
			}
		}
	}
}