#include "AnimatedSkeleton.h"
#include "AnimationNode.h"
#include "AnimPlayback.h"
#include "data/AnimationData.h"
#include "../../resource/Animation.h"
#include "data/Skeleton.h"
#include "BaseAnimationTime.h"
#include "BoneConstraint.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			AnimatedSkeleton::AnimatedSkeleton(Skeleton& ref) :
				_ref(ref), _root(), _pose(ref.getNumBones()), _skin(ref.getNumBones()), m_constraint(new ConstraintList[ref.getNumBones()])
			{
				clearConstraints();
				clearBlendTree();
				updateSkeleton();
			}


			AnimatedSkeleton::~AnimatedSkeleton()
			{
			}

			void AnimatedSkeleton::clearConstraints() 
			{
				std::memset(m_constraint.get(), 0, sizeof(ConstraintList) * _ref.getNumBones());
			}


			void AnimatedSkeleton::update(float dT) {
				_root->update(dT);
				updateSkeleton();
			}
			void AnimatedSkeleton::updateSkeleton()
			{
				//Update animation tree
				// Update skin transforms
				_pose[0] = _root->calcLocalTransform(0) * _ref.getRoot();				//	Update root pose
				_skin[0] = _ref.getBone(0)._invBindPose * _pose[0];					//	Update root skin
				applyConstraint(0);
				for (unsigned int i = 1; i < boneCount(); i++)
				{
					const Bone& bone = _ref.getBone(i);
					_pose[i] = _root->calcLocalTransform(i) * _pose[bone._parentIndex];	//	Update root pose
					applyConstraint(i);
					_skin[i] = bone._invBindPose * _pose[i];							//	Update root skin
				}
			}
			void AnimatedSkeleton::applyConstraint(uint32_t index)
			{
				BoneConstraint** ptr = (m_constraint.get() + index)->m_list;
				while (*ptr != NULL) {
					(*ptr)->execute(_ref, _pose.data(), index);
					ptr++;
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
				return _skin.data();
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

			bool AnimatedSkeleton::getBoneIndex(uint32_t boneNameHash, unsigned int &boneIndex) const
			{
				return _ref.findBoneIndex(boneNameHash, boneIndex);
			}

			const std::string & AnimatedSkeleton::getBoneName(unsigned int bone) const
			{
				return _ref.getBone(bone)._boneName;
			}
			void AnimatedSkeleton::addConstraint(BoneConstraint * bC, uint32_t boneIndex)
			{
				m_constraint.get()[boneIndex].add(bC);
			}
			void AnimatedSkeleton::rmvConstraint(BoneConstraint * bC, uint32_t boneIndex)
			{
				m_constraint.get()[boneIndex].rmv(bC);
			}
			void ConstraintList::add(BoneConstraint * bC)
			{
				for (uint32_t i = 0; i < MAX_CONSTRAIN_COUNT; i++) {
					if (m_list[i] == NULL) {
						m_list[i] = bC;
						return;
					}
				}
				// No remaining slot.
			}
			void ConstraintList::rmv(BoneConstraint * bC)
			{
				for (uint32_t i = 0; i < MAX_CONSTRAIN_COUNT; i++) {
					if (m_list[i] == bC) {
						m_list[i] = NULL;
						// Find last slot:
						BoneConstraint** last = m_list + i;
						while (++last != NULL) {}
						last--; // Step back to non-empty
						// Swap
						m_list[i] = *last;
						*last = NULL;
						return;
					}
				}
				// Not found.
			}
}
	}
}