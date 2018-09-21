#include "AnimatedSkeleton.h"
#include "AnimationNode.h"
#include "AnimPlayback.h"
#include "data/AnimationData.h"
#include "../../resource/Animation.h"
#include "data/Skeleton.h"
#include "BaseAnimationTime.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			AnimatedSkeleton::AnimatedSkeleton(Skeleton& ref, resource::shaderproperty::ShaderPropertyMatrixArray& skin_ref) :
				_ref(ref), _root(), _pose(ref.getNumBones()), _skin(&skin_ref)
			{
				_skin->resize(ref.getNumBones());
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
				math::Matrix *skin_arr = _skin->GetValue();
				_pose[0] = _root->calcLocalTransform(0) * _ref.getRoot();				//	Update root pose
				skin_arr[0] = _ref.getBone(0)._invBindPose * _pose[0];					//	Update root skin
				for (unsigned int i = 1; i < boneCount(); i++)
				{
					const Bone& bone = _ref.getBone(i);
					_pose[i] = _root->calcLocalTransform(i) * _pose[bone._parentIndex];	//	Update root pose
					math::Matrix m = bone._invBindPose * _pose[i];
					skin_arr[i] = bone._invBindPose * _pose[i];							//	Update root skin
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
				_root = std::unique_ptr<AnimationNode>(new BindPoseNode(_ref));
				update(0.1f);
			}

			void AnimatedSkeleton::playSingle(thomas::resource::Animation * anim)
			{
				if (!anim->HasAnimation()) {
					clearBlendTree();
					return;
				}
				AnimationData &animRef = *anim->GetAnimation();
				std::unique_ptr<Playback> playback(new BaseAnimationTime(0.f, animRef.m_duration, PlayType::Loop));
				_root = std::unique_ptr<AnimationNode>(new AnimPlayback(_ref, playback, animRef));
			}


			const math::Matrix* AnimatedSkeleton::getSkin() const {
				return _skin->GetValue();
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
			const std::string & AnimatedSkeleton::getBoneName(unsigned int bone) const
			{
				return _ref.getBone(bone)._boneName;
			}
			const resource::shaderproperty::ShaderPropertyMatrixArray * AnimatedSkeleton::getShaderProperty()
			{
				return _skin;
			}
		}
	}
}