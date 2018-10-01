#include "AnimBlender.h"
#include "data/Skeleton.h"
#include "WeightMixer.h"

namespace thomas {
	namespace graphics {
		namespace animation {
			AnimBlender::AnimBlender(Skeleton & skel)
				: AnimationNode(skel), m_NumNode(0), m_weights()
			{
			}
			AnimBlender::~AnimBlender()
			{
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
					for (unsigned int b = 0; b < node->m_numChannel; b++) {
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
				m_nodes[m_NumNode++] = node;
				constructMapping(); // Re-construct
			}



			void AnimBlender::update(float dT)
			{
				m_weights->update(dT);
			}
			void AnimBlender::calcFrame(TransformComponents* result) 
			{
				std::unique_ptr<TransformComponents> tmp_arr(new TransformComponents[m_ref.getNumBones()]);

				//assert(m_weights->numWeights() >= m_NumNode);
				const WeightTripple *weights = m_weights->getWeights();
				const WeightMixer::Mode *mode = m_weights->getMode();
				
				AnimationNode * node = m_nodes[0];

				// Initiate data with bind pose (if initial anim node does not write to all channels).
				if(node->m_numChannel < this->m_numChannel)
					std::memcpy(result, m_ref.getBindComponents(), sizeof(TransformComponents) * m_ref.getNumBones());

				// Overwrite pose with data from the first animation node
				node->calcFrame(result);
				weights += (mode[0] == WeightMixer::PerChannel ? node->m_numChannel : 1u);

				// Blend remaining nodes
				for (unsigned int i = 1; i < m_NumNode; i++)
				{
					if (*mode == WeightMixer::PerChannel)
					{
						node = m_nodes[i];
						node->calcFrame(tmp_arr.get());
						// Blend bones (and map each to skeleton index) 
						for (unsigned int b = 0; b < node->m_numChannel; b++)
							result[node->m_boneMapping[b]].blendTo(tmp_arr.get()[b], *weights++);
					}
					else if (*mode == WeightMixer::Additive) {
						// Additive blending
						if (weights->isWeighted()) {
							node = m_nodes[i];
							node->calcFrame(tmp_arr.get());
							// Blend bones (and map each to skeleton index) 
							for (unsigned int b = 0; b < node->m_numChannel; b++)
								result[node->m_boneMapping[b]].addFrom(tmp_arr.get()[node->m_boneMapping[b]], *weights);
						}
						weights++; // Step next node weight
					}
					else 
					{
						// Blend with 
						if (weights->isWeighted()) {
							node = m_nodes[i];
							node->calcFrame(tmp_arr.get());
							// Blend bones (and map each to skeleton index) 
							for (unsigned int b = 0; b < node->m_numChannel; b++)
								result[node->m_boneMapping[b]].blendTo(tmp_arr.get()[node->m_boneMapping[b]], *weights);
						}
						weights++; // Step next node weight
					}
					mode++;
				}
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