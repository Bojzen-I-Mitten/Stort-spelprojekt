#pragma once
/*  Mattias F 2017
	- Modified 2018
*/
#include <vector>
#include <cfloat>
#include "ChannelKeys.h"
#include "Channel.h"
#include "AnimationData.h"
#include "../../../utils/Math.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			/* Lerp the keys as vec3 with the specific amount */
			math::Vector3 lerpVec3(const ChannelKey& from, const ChannelKey& to, float amount);
			/* Slerp the keys as quaternions with the specific amount */
			math::Quaternion slerp(const ChannelKey& from, const ChannelKey& to, float amount);

			/* Contains the node's currently active keys for a node.
			 * Allowing each node to maintain an individual
			*/
			template<unsigned int N>
			class KeyFrame
			{
			private:
				unsigned int m_keyIndex[N];	// Tracks current keyframe index for every channel
			protected:
				/* Currently active channel */
				const ObjectChannel *m_channel;
				/* Determines if the to keys belong to a custom channel animation and should persist over loop/change */
				bool _custom[N];
				/* The last key frames.
				*/
				ChannelKeyMemory m_from[N];
				/* The next key frames.
				*/
				ChannelKeyMemory m_to[N];
			private:
				/* Find and set the next to key */
				void nextKey(unsigned int type, float eT);
				/* Find and set the previous key of the animation */
				void previousKey(unsigned int chnlInd, float eT);
				/* Find and set a the first key of the animation */
				void newKey(unsigned int chnlInd, float startAT);
				/* Adjusts a custom frame over a animation change */
				void keepCustom(unsigned int chnlInd, float eT);
			protected:
				/* Get the lerp amount for a specific node channel */
				float lerpAmount(unsigned int type, float eT);
				/* Store the current animation state in the specific node channel
				*/
				virtual void storeState(unsigned int type, float eT) = 0;
			public:
				KeyFrame();
				KeyFrame(const unsigned int* nodeChannelSize);
				~KeyFrame();
				bool isActive();
				/* Deactivate the channel, */
				void deactivate();
				/* Set new animation in the frame. Specify the animation time to start from */
				void newAnimation(float oldET, const ObjectChannel *channel, float startAt);
				/* Initiate to initial timeframe. */
				void newAnimation(const ObjectChannel *channel, float eT);
				/* Pose the animation at a specified frame. Blend between the current anim pose over the animation time param.*/
				void poseAnimation(float oldET, const ObjectChannel *channel, float poseAt, float animTime);
				/* Loop the animation */
				void loop(float eT, float animDuration);
				/* Update related key frame if necesary */
				void updateFrame(unsigned int key, float eT);
				/* Update each key necesary */
				void updateFrame(float eT);
			};


			template<unsigned int N>
			KeyFrame<N>::KeyFrame()
			{
				for (unsigned int i = 0; i < N; i++)
					_custom[i] = false;
			}
			template<unsigned int N>
			KeyFrame<N>::KeyFrame(const unsigned int* nodeChannelSize)
				: m_to(), m_from()
			{
				for (unsigned int i = 0; i < N; i++)
				{
					m_to[i] = ChannelKeyMemory(nodeChannelSize[i]);
					m_from[i] = ChannelKeyMemory(nodeChannelSize[i]);
					_custom[i] = false;
				}
			}

			template<unsigned int N>
			KeyFrame<N>::~KeyFrame()
			{
			}

			template<unsigned int N>
			float KeyFrame<N>::lerpAmount(unsigned int type, float eT) {
				float delta = (m_to[type]._time - m_from[type]._time);
				return delta <= 0 ? 1.f : (eT - m_from[type]._time) / delta;
			}
			template<unsigned int N>
			void KeyFrame<N>::deactivate() {
				m_channel = nullptr;
			}
			template<unsigned int N>
			bool  KeyFrame<N>::isActive() {
				return m_channel;
			}
			template<unsigned int N>
			void KeyFrame<N>::newAnimation(float oldET, const ObjectChannel *channel, float startAt) {
				m_channel = channel;
				if (m_channel) {
					for (unsigned int i = 0; i < N; i++) {
						//Store current anim pose
						storeState(i, oldET);
						m_from[i]._time = startAt;
						//Fetch the first key of the animation
						if (_custom[i])
							keepCustom(i, oldET);
						else
							newKey(i, startAt);
					}
				}
			}

			/* Pose the animation at a specified frame. Blend between the current anim pose over the animation time param.*/
			template<unsigned int N>
			void KeyFrame<N>::poseAnimation(float oldET, const ObjectChannel *channel, float poseAt, float animTime)
			{
				m_channel = channel;
				if (m_channel) {
					for (unsigned int i = 0; i < N; i++) {
						//Store current anim pose
						storeState(i, oldET);
						//Temporarily store state
						ChannelKeyMemory tmpCpy(m_from[i]);
						//Fetch and create pose keys
						newKey(i, poseAt);
						previousKey(i, poseAt);
						storeState(i, poseAt);
						//Setup the animation, note that this is not a 'custom' animation as it affects all channels.
						m_to[i] = m_from[i];
						m_from[i] = tmpCpy;
						m_from[i]._time = 0.f;
						m_to[i]._time = animTime;
					}
					m_channel = nullptr;
				}
			}
			template<unsigned int N>
			inline void KeyFrame<N>::newAnimation(const ObjectChannel *channel, float eT)
			{
				m_channel = channel;
				assert(eT >= 0.f);
				for (unsigned int i = 0; i < N; i++) {
					m_keyIndex[i] = 0;
					if (m_channel->numKeys(i) == 0) {
						// Default initiation...
						assert(false);
					}
					else {
						m_keyIndex[i] = 0;
						newKey(i, eT);
						previousKey(i, eT);
					}
				}
			}
			template<unsigned int N>
			void  KeyFrame<N>::updateFrame(unsigned int chInd, float eT) {
				//Update keys if necesary
				if (eT > m_to[chInd]._time)
					nextKey(chInd, eT);
				else if (eT < m_from[chInd]._time)
					previousKey(chInd, eT);
			}

			template<unsigned int N>
			void  KeyFrame<N>::updateFrame(float eT) {
				for (unsigned int i = 0; i < N; i++)
					updateFrame(i, eT);
			}
			template<unsigned int N>
			void  KeyFrame<N>::nextKey(unsigned int chnlInd, float eT)
			{
				if (m_channel->getNext(eT, chnlInd, m_keyIndex[chnlInd]))			//If key is found, set it
					m_to[chnlInd] = m_channel->getKey(m_keyIndex[chnlInd], chnlInd);
				else {
					m_to[chnlInd]._time = FLT_MAX;									//Let the frame elapse until the animation end
					_custom[chnlInd] = false;										//This is not a custom key.
				}
			}
			template<unsigned int N>
			void  KeyFrame<N>::previousKey(unsigned int chInd, float eT)
			{
				if (m_channel->getPrevious(eT, chInd, m_keyIndex[chInd]))		// If key is found, set it
					m_from[chInd] = m_channel->getKey(m_keyIndex[chInd] - 1, chInd);
				else {
					m_from[chInd]._time = eT;						// Animate from current value (and time)
					_custom[chInd] = false;						// This is not a custom key.
				}
			}
			/* Find and set a the first key of the animation */
			template<unsigned int N>
			void  KeyFrame<N>::newKey(unsigned int chInd, float startAT)
			{
				if (m_channel->getNext(startAT, chInd, m_keyIndex[chInd]))			//	If key is found, set it
					m_to[chInd] = m_channel->getKey(m_keyIndex[chInd], chInd);	
				else {																	//	There is no key in the animation channel, set the current value to continue to the endtimes.
					m_to[chInd] = m_channel->getLast(chInd);
					m_to[chInd]._time = FLT_MAX;
					_custom[chInd] = false;
				}
			}

			/* Adjusts a custom frame over a animation change */
			template<unsigned int N>
			void  KeyFrame<N>::keepCustom(unsigned int chnlInd, float eT)
			{
				//Let custom frame persist, adjusting the time relation
				m_from[chnlInd]._time -= eT;
				m_to[chnlInd]._time -= eT;
			}

			template<unsigned int N>
			void  KeyFrame<N>::loop(float eT, float animDuration) {
				for (unsigned int i = 0; i < N; i++)
				{
					if (_custom[i])
						keepCustom(i, animDuration);
					else {
						//Animate from last frame to first
						m_from[i] = m_to[i];
						m_from[i]._time = 0.f;
						nextKey(i, eT);
					}
				}
			}
		}
	}
}