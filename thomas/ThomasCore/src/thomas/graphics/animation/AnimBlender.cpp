#include "AnimBlender.h"
#include "../../utils/Utility.h"
#include "../../Common.h"
#include "data/Skeleton.h"
#include "WeightMixer.h"
#include "../../ThomasCore.h"
#include "../../resource/MemoryAllocation.h"

namespace thomas {
	namespace graphics {
		namespace animation {
			AnimBlender::AnimBlender(Skeleton& skel)
				: AnimBlender(skel, 0)
			{
			}
			AnimBlender::AnimBlender(Skeleton & skel, uint32_t numBlendedNodes)
				: AnimationNode(skel), m_NumNode(numBlendedNodes), m_weights()
			{
				for (uint32_t i = 0; i < numBlendedNodes; i++)
					m_nodes[i] = skel.nullPlayback();
			}
			AnimBlender::~AnimBlender()
			{
			}

			uint32_t AnimBlender::numAnimations()
			{
				return m_NumNode;
			}

			void AnimBlender::setWeightMixer(WeightMixer * mixer)
			{
				m_weights = std::unique_ptr<WeightMixer>(mixer);
				constructMapping();
			}

			void AnimBlender::generateLinearMixer(float durationPerNode)
			{
				setWeightMixer(new WeightLinearPerChannel(m_NumNode, durationPerNode));
			}

			void AnimBlender::constructMapping() 
			{
				// Construct a (temporary) list of enabled bones
				std::vector<int> mapping(m_ref.getNumBones());
				for (uint32_t i = 0; i < m_NumNode; i++)
					mapping[i] = -1;
				for (uint32_t i = 0; i < m_NumNode; i++)
				{
					AnimationNode * node = m_nodes[i];
					for (unsigned int b = 0; b < node->NumChannel(); b++) {
						int bone = node->m_boneMapping[b];
						mapping[bone] = bone;	// Currently mapping 1->1 in Blender node(s), ignoring unsused.
					}
				}
				// Construct the mapping over active bones
				m_boneMapping.clear();
				m_boneMapping.resize(m_ref.getNumBones());
				for (uint32_t i = 0; i < m_ref.getNumBones(); i++) {
					if (mapping[i] >= 0)	// If active
						m_boneMapping.push_back(i);
				}
			}

			void AnimBlender::pushAnimation(AnimationNode * node)
			{
				if (m_NumNode >= MAX_ANIMATION_BLEND_NODE)
				{
					LOG("Warning! Not possible to push more blend nodes.");
					return;
				}
				m_nodes[m_NumNode++] = node;
				constructMapping(); // Re-construct
			}

			void AnimBlender::setAnimation(uint32_t index, AnimationNode * n)
			{
				assert(index < m_NumNode);
				m_nodes[index] = n;
				constructMapping(); // Re-construct
			}



			void AnimBlender::update(float dT)
			{
				if (isUpdated()) return;
				m_weights->update(dT);
				for (unsigned int i = 0; i < m_NumNode; i++)
					m_nodes[i]->update(dT);
				markUpdated();
			}
			void AnimBlender::resetUpdate()
			{
				if (utility::noFlag(m_flag, ANIM_UPDATED_FLAG)) return;
				AnimationNode::resetUpdate();
				for (unsigned int i = 0; i < m_NumNode; i++)
					m_nodes[i]->resetUpdate();
			}
			void AnimBlender::calcFrame(TransformComponents* result) 
			{

				TransformComponents *tmp_arr = (TransformComponents*)ThomasCore::Core().Memory()->stack(0).allocate(sizeof(TransformComponents) * m_ref.getNumBones(), sizeof(float));
				//assert(m_weights->numWeights() >= m_NumNode);
				const WeightTripple *weights = m_weights->getWeights();
				const WeightMixer::Mode *mode = m_weights->getMode();
				
				AnimationNode * node = m_nodes[0];

				// Initiate data with bind pose (if initial anim node does not write to all channels).
				if(node->NumChannel() < this->NumChannel())
					std::memcpy(result, m_ref.getBindComponents(), sizeof(TransformComponents) * m_ref.getNumBones());

				// Overwrite pose with data from the first animation node
				node->calcFrame(result);
				weights += (mode[0] == WeightMixer::PerChannel ? node->NumChannel() : 1u);

				// Blend remaining nodes
				for (unsigned int i = 1; i < m_NumNode; i++)
				{
					if (*mode == WeightMixer::PerChannel)
					{
						node = m_nodes[i];
						node->calcFrame(tmp_arr);
						// Blend bones (and map each to skeleton index) 
						for (unsigned int b = 0; b < node->NumChannel(); b++)
							result[node->m_boneMapping[b]].blendTo(tmp_arr[b], *weights++);
					}
					else if (*mode == WeightMixer::Additive) {
						// Additive blending
						if (weights->isWeighted()) {
							node = m_nodes[i];
							node->calcFrame(tmp_arr);
							// Blend bones (and map each to skeleton index) 
							for (unsigned int b = 0; b < node->NumChannel(); b++)
								result[node->m_boneMapping[b]].addFrom(tmp_arr[node->m_boneMapping[b]], *weights);
						}
						weights++; // Step next node weight
					}
					else 
					{
						// Blend with 
						if (weights->isWeighted()) {
							node = m_nodes[i];
							node->calcFrame(tmp_arr);
							// Blend bones (and map each to skeleton index) 
							for (unsigned int b = 0; b < node->NumChannel(); b++)
								result[node->m_boneMapping[b]].blendTo(tmp_arr[node->m_boneMapping[b]], *weights);
						}
						weights++; // Step next node weight
					}
					mode++;
				}
				ThomasCore::Core().Memory()->stack(0).deallocate(tmp_arr);
			}

			AnimationNode * AnimBlender::getAnimNode(uint32_t i)
			{
				if (i >= m_NumNode) return nullptr;
				return m_nodes[i];
			}



			math::Vector3 AnimBlender::calcBonePosition(unsigned int bone)
			{
				return math::Vector3();
			}
			math::Vector3 AnimBlender::calcBoneScale(unsigned int bone)
			{
				return math::Vector3();
			}
			math::Quaternion AnimBlender::calcBoneRot(unsigned int bone)
			{
				return math::Quaternion();
			}
			void AnimBlender::calcFrame(unsigned int bone, math::Vector3 & trans, math::Vector3 & scale, math::Quaternion & rot)
			{
			}
		}
	}
}