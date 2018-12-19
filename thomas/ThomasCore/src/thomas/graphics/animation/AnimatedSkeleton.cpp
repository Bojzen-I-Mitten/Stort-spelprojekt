#include "AnimatedSkeleton.h"
#include "AnimationNode.h"
#include "AnimPlayback.h"
#include "data/AnimationData.h"
#include "../../resource/Animation.h"
#include "data/Skeleton.h"
#include "BaseAnimationTime.h"
#include "constraint/BoneConstraint.h"
#include "../../resource/MemoryAllocation.h"
#include "../../ThomasCore.h"

namespace thomas {
	namespace graphics {
		namespace animation {
			using namespace utils::atomics;

			AnimatedSkeleton::AnimatedSkeleton(Skeleton& ref) :
				_ref(ref), _root(),  _pose(ref.getNumBones()), _skin(ref.getNumBones()), m_constraint(new ConstraintList[ref.getNumBones()])
			{
				clearConstraints();
				clearBlendTree();
			}


			AnimatedSkeleton::~AnimatedSkeleton()
			{
			}

			void AnimatedSkeleton::clearConstraints() 
			{
				std::memset(m_constraint.get(), 0, sizeof(ConstraintList) * _ref.getNumBones());
			}

			void AnimatedSkeleton::update(float dT) {
				Lock lck(m_lock);
				if (_root.empty()) return;		// Verify blend node
				_root->update(dT);				// Update each node ONCE.
				updateSkeleton();				// Apply skeleton.
				_root->resetUpdate();			// Clear dirty update flags.
			}

			void AnimatedSkeleton::setPose()
			{
				_pose[0] = _ref.getBone(0)._bindPose * _ref.getRoot();			//	Set root bind pose
				_skin[0] = _ref.getBone(0)._invBindPose * _pose[0];
				for (uint32_t i = 1; i < boneCount(); i++)
				{
					const Bone& bone = _ref.getBone(i);
					_pose[i] = bone._bindPose * _pose[bone._parentIndex];		//	Set bind pose
					_skin[i] = bone._invBindPose * _pose[i];
				}
			}

			void AnimatedSkeleton::updateSkeleton()
			{
				TransformComponents *frame_tmp = reinterpret_cast<TransformComponents*>(
					ThomasCore::Core().Memory()->stack(0).allocate(sizeof(TransformComponents)* _ref.getNumBones(), sizeof(float)));
				//Update animation tree
				if (_root->NumChannel() < _ref.getNumBones())	// Fill with default if not enough.
					std::memcpy(frame_tmp, _ref.getBindComponents(), sizeof(TransformComponents) * _ref.getNumBones());
				_root->calcFrame(frame_tmp);
				// Update skin transforms
				_pose[0] = frame_tmp[0].createTransform() * _ref.getRoot();						//	Update root pose
				applyConstraint(0, frame_tmp);
				for (uint32_t i = 1; i < boneCount(); i++)
				{
					const Bone& bone = _ref.getBone(i);
					_pose[i] = frame_tmp[i].createTransform() * _pose[bone._parentIndex];		//	Update root pose
					applyConstraint(i, frame_tmp);
				}
				for (uint32_t i = 0; i < boneCount(); i++)
					_skin[i] = _ref.getBone(i)._invBindPose * _pose[i];							//	Update skin
				ThomasCore::Core().Memory()->stack(0).deallocate(frame_tmp);
			}
			void AnimatedSkeleton::applyConstraint(uint32_t index, TransformComponents *transformInfo)
			{
				BoneConstraint** ptr = (m_constraint.get() + index)->m_list;
				while (*ptr != NULL) {
					(*ptr)->execute(_ref, _pose.data(), transformInfo, index);
					ptr++;
				}
			}
			/* Freeze the current animation */
			void AnimatedSkeleton::stopAnimation()
			{
				Lock lck(m_lock);
				_root.unassign();
			}

			void AnimatedSkeleton::setBlendTree(AnimationNode *blendTree)
			{
				if (!blendTree)
					clearBlendTree();
				else
				{
					Lock lck(m_lock);
					_root.assign(blendTree, thomas::utility::allocator::OWNED_BY_ENGINE);
				}
			}

			void AnimatedSkeleton::clearBlendTree() {
				_root.unassign();
				setPose();
			}

			void AnimatedSkeleton::playSingle(thomas::resource::Animation * anim)
			{
				if (!anim->HasAnimation()) {
					clearBlendTree();
					return;
				}
				AnimationData &animRef = *anim->GetAnimation();
				std::unique_ptr<Playback> playback(new BaseAnimationTime(0.f, animRef.m_duration, PlayType::Loop));
				Lock lck(m_lock);
				_root.assign(new AnimPlayback(_ref, playback, animRef), thomas::utility::allocator::OWNED_BY_CORE);
			}


			const math::Matrix* AnimatedSkeleton::getSkin() const {
				return _skin.data();
			}
			unsigned int AnimatedSkeleton::boneCount() const {
				return (uint32_t)_pose.size();
			}

			/* Get the local matrix of a bone
			*/
			const math::Matrix& AnimatedSkeleton::getBoneMatrix(unsigned int bone) const
			{
				assert(bone < _pose.size());
				return _pose[bone];
			}

			math::Matrix AnimatedSkeleton::getBoneOrientation(unsigned int bone) const
			{
				assert(bone < _pose.size());
				return math::normalizeBasisAxis(_pose[bone]);
			}

			bool AnimatedSkeleton::getBoneIndex(uint32_t boneNameHash, unsigned int &boneIndex) const
			{
				return _ref.findBoneIndex(boneNameHash, boneIndex);
			}

			const std::string & AnimatedSkeleton::getBoneName(unsigned int bone) const
			{
				return _ref.getBone(bone)._boneName;
			}
			const Bone & AnimatedSkeleton::getBoneInfo(unsigned int bone)
			{
				return _ref.getBone(bone);
			}
			void AnimatedSkeleton::addConstraint(BoneConstraint * bC, uint32_t boneIndex)
			{
				Lock lck(m_lock);
				if (!bC->apply(_ref, boneIndex))
				{
					std::string s("Constraint failed to be applied at bone index: ");
					s += std::to_string(boneIndex);
					LOG(s);
				}
				else
					m_constraint.get()[boneIndex].add(bC);
			}
			void AnimatedSkeleton::rmvConstraint(BoneConstraint * bC, uint32_t boneIndex)
			{
				Lock lck(m_lock);
				m_constraint.get()[boneIndex].rmv(bC);
			}
			void ConstraintList::add(BoneConstraint * bC)
			{
				for (uint32_t i = 0; i < MAX_CONSTRAINT_COUNT; i++) {
					if (m_list[i] == NULL) {
						m_list[i] = bC;
						return;
					}
				}
				// No remaining slot.
				LOG("No remaining constraint slot.");
			}
			void ConstraintList::rmv(BoneConstraint * bC)
			{
				for (uint32_t i = 0; i < MAX_CONSTRAINT_COUNT; i++) {
					if (m_list[i] == bC) {
						m_list[i] = NULL;
						// Find last slot:
						uint32_t last = i;
						while (++last < MAX_CONSTRAINT_COUNT && m_list[i] != NULL) {}
						last--; // Step back to non-empty
						if (last)
						{
							// Swap out
							m_list[i] = m_list[last];
							m_list[last] = NULL;
						}
						return;
					}
				}
				// Not found.
			}
}
	}
}