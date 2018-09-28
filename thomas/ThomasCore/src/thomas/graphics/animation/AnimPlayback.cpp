#include "AnimPlayback.h"
#include "Playback.h"
#include "BaseAnimationTime.h"
#include "data/Skeleton.h"
#include "../../Common.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			/* Generate an animation pose
			*/
			AnimPlayback::AnimPlayback(Skeleton &ref)
				: AnimationNode(ref),
				m_playback(new BaseAnimationTime(0.f,0.f, graphics::animation::PlayType::None)), m_anim(NULL), m_channel(ref.getNumBones())
			{
				m_boneMapping.reserve(ref.getNumBones());
				//Each channel needs to be initiated so it can generate it's own frames.
				for (unsigned int i = 0; i < ref.getNumBones(); i++) {
					m_channel[i].init(ref.getBone(i)._bindPose);
					m_boneMapping[i] = i;
				}
				m_numChannel = ref.getNumBones();
			}
			/* Generate an animation player
			*/
			AnimPlayback::AnimPlayback(Skeleton &ref, std::unique_ptr<Playback> &playback, AnimationData &anim)
				: AnimationNode(ref),
				m_playback(std::move(playback)), m_anim(&anim), m_channel()
			{
				m_channel.reserve(anim.numChannel());
				m_boneMapping.reserve(anim.numChannel());
				//Each channel needs to be initiated so it can generate it's own frames.
				for (size_t i = 0; i < anim.numChannel(); i++) {
					const ObjectChannel& ch = anim[i];
					unsigned int boneInd;
					if(ch.numNodeChannels() == 0 ||
						!ref.findBoneIndex(ch.getHash(), boneInd))
						continue; // Empty or culled
					else {
						//m_boneMapping[boneInd] = m_channel.size();
						m_boneMapping.push_back(boneInd);
						m_channel.push_back(BoneFrame());
						m_channel.back().newAnimation(&anim[i], m_playback->m_elapsedTime);
					}
				}
				m_numChannel = m_channel.size();
			}

			AnimPlayback::~AnimPlayback()
			{
			}

			void AnimPlayback::update(float dT)
			{
				m_playback->timeStep(dT);
			}
			math::Vector3 AnimPlayback::calcBonePosition(unsigned int index)
			{
				return m_channel[index].lerpCoordinate(m_playback->m_elapsedTime);
			}

			math::Vector3 AnimPlayback::calcBoneScale(unsigned int index)
			{
				return m_channel[index].lerpScale(m_playback->m_elapsedTime);
			}

			math::Quaternion AnimPlayback::calcBoneRot(unsigned int index)
			{
				return m_channel[index].lerpRotation(m_playback->m_elapsedTime);
			}


			void AnimPlayback::calcFrame(unsigned int index, math::Vector3& trans, math::Vector3 &scale, math::Quaternion &rot) {
				float eT = m_playback->m_elapsedTime;
				m_channel[index].lerpFrame(eT, trans, scale, rot);
			}
			/*
			bool AnimatedSkeleton::setAnim(const std::string& name, PlayType runType) {
				//Change animation, it can be null
				_animation = _ref.getAnimation(name);
				if (_animation)
				{
					_animDuration = _animation->m_duration;
					for (unsigned int i = 0; i < _channel.size(); i++)
						_channel[i].newAnimation(_elapAnimTime, &_animation->operator[](i), 0.f);
					_elapAnimTime = 0.f;
					_endState = runType;
					return true;
				}
				//No animation found, set current to null.
				stopAnimation();
				return false;
			}

			/* Pose the skeleton at the specific point of animation. Blends into the pose over the animation time specified. *
			bool AnimatedSkeleton::setAnimPose(const std::string& name, float poseAt, float animTime)
			{
				//Change animation, it may fail
				_animation = _ref.getAnimation(name);
				if (_animation)
				{
					for (unsigned int i = 0; i < _channel.size(); i++)
						_channel[i].poseAnimation(_elapAnimTime, &_animation->operator[](i), poseAt, animTime);
					_elapAnimTime = 0.f;
					_animDuration = animTime;
					_endState = Once;
					return true;
				}
				//No animation found, set current to null.
				stopAnimation();
				return false;
			}
			/* Recalculates the end time when animation is looped *
			void AnimatedSkeleton::loopRefit() {
				_elapAnimTime = (float)fmod((double)_elapAnimTime, (double)_animation->m_duration); //loop
				for (unsigned int i = 0; i < boneCount(); i++)
					_channel[i].loop(_elapAnimTime, _animation->m_duration);
			}
			*/
		}
	}
}
