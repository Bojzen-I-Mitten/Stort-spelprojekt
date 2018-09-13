#include "AnimatedSkeleton.h"
#include "AnimationNode.h"
#include "AnimPlayback.h"
#include "data/Skeleton.h"
#include "../RenderConstants.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			AnimatedSkeleton::AnimatedSkeleton(Skeleton& ref) : 
				resource::shaderproperty::ShaderPropertyMatrixArray(ref.getNumBones()),
				_ref(ref), _root(), _pose(ref.getNumBones())
			{
				SetName(THOMAS_MATRIX_SKIN_ARRAY);
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
				_pose[0] = _root->calcLocalTransform(0);								//	Update root pose
				Matrix(0) = _pose[0] * _ref.getBone(0)._invBindPose;					//	Update root skin
				for (unsigned int i = 1; i < boneCount(); i++)
				{
					const Bone& bone = _ref.getBone(i);
					_pose[i] = _pose[bone._parentIndex] * _root->calcLocalTransform(i);	//	Update root pose
					Matrix(i) = _pose[i] * bone._invBindPose;							//	Update root skin
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


			const math::Matrix* AnimatedSkeleton::getSkin() const {
				return m_value.get();
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
			const resource::shaderproperty::ShaderPropertyMatrixArray * AnimatedSkeleton::getShaderProperty()
			{
				return this;
			}
		}
	}
}