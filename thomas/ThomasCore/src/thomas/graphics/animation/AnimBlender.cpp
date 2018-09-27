#include "AnimBlender.h"
#include "data/Skeleton.h"
#include "WeightMixer.h"

namespace thomas {
	namespace graphics {
		namespace animation {
			AnimBlender::AnimBlender(Skeleton & skel)
				: AnimationNode(skel)
			{
			}
			AnimBlender::~AnimBlender()
			{
			}
			void AnimBlender::update(float)
			{
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

			void AnimBlender::calcFrame(TransformComponents* result) {
				std::unique_ptr<TransformComponents> tmp_arr(new TransformComponents[m_ref.getNumBones()]);

				assert(m_weights->numWeights() >= m_nodes.size());
				const float *weights = m_weights->getWeights();
				
				float weight = weights[0];
				AnimationNode * node = m_nodes[0];
				node->calcFrame(tmp_arr.get());

				// Initiate data with bind pose (if initial anim node does not write to all channels).
				if(node->m_numChannel < this->m_numChannel)
					std::memcpy(tmp_arr.get(), m_ref.getBindComponents(), sizeof(TransformComponents) * m_ref.getNumBones());

				// Overwrite pose with data from the first animation node
				for (unsigned int b = 0; b < node->m_numChannel; b++)
					result[node->m_boneMapping[b]] = tmp_arr.get()[b];

				// Blend remaining nodes
				for (unsigned int i = 0; i < m_nodes.size(); i++) {
					weight = weights[i];
					node = m_nodes[i];
					node->calcFrame(tmp_arr.get());

					for (unsigned int b = 0; b < node->m_numChannel; b++)
						result[node->m_boneMapping[b]].blendTo(tmp_arr.get()[b], weight);
				}
			}
		}
	}
}