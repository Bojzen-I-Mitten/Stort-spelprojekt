#include "BoneFrame.h"

namespace thomas {
	namespace graphics {
		namespace animation {

			BoneFrame::BoneFrame()
				: KeyFrame<3>(SIZE_BONE_CHANNEL), m_last_frame(0)
			{
			}

			BoneFrame::~BoneFrame()
			{
			}


			void BoneFrame::init(math::Vector3 pos, math::Vector3 scale, math::Quaternion quat) {
				m_from[0].set(0.f, pos);
				m_from[1].set(0.f, scale);
				m_from[2].set(0.f, quat);
				m_to[0].set(FLT_MAX, pos);
				m_to[1].set(FLT_MAX, scale);
				m_to[2].set(FLT_MAX, quat);

			}
			void BoneFrame::init(const math::Matrix& pose) {
				DirectX::XMVECTOR s, r, t;
				DirectX::XMMatrixDecompose(&s, &r, &t, pose);

				math::Matrix m = math::Matrix::CreateScale(s);		// Test bind pose matrix							
				m = m * math::Matrix::CreateFromQuaternion(r);
				m.Translation(t);


				init(t, s, r);
			}

			void BoneFrame::storeState(unsigned int type, float eT) {
				if (type == 0)
					m_from[type].set(lerpVec3(m_from[type], m_to[type], lerpAmount(type, eT)));
				else if (type == 1)
					m_from[type].set(lerpVec3(m_from[type], m_to[type], lerpAmount(type, eT)));
				else
					m_from[type].set(slerp(m_from[type], m_to[type], lerpAmount(type, eT)));
			}

			math::Vector3 BoneFrame::lerpCoordinate(float eT) {
				updateFrame(0, eT);										// Step keyframes
				return lerpVec3(m_from[0], m_to[0], lerpAmount(0, eT));   // Interpolate
			}
			math::Vector3 BoneFrame::lerpScale(float eT) {
				updateFrame(1, eT);										// Step keyframes
				return lerpVec3(m_from[1], m_to[1], lerpAmount(1, eT));   // Interpolate
			}
			math::Quaternion BoneFrame::lerpRotation(float eT) {
				updateFrame(2, eT);										// Step keyframes
				return slerp(m_from[2], m_to[2], lerpAmount(2, eT));   // Interpolate
			}

			void BoneFrame::lerpFrame(float eT, math::Vector3& trans, math::Vector3 &scale, math::Quaternion &rot) {
				// Step keyframes
				updateFrame(0, eT);
				updateFrame(1, eT);				
				updateFrame(2, eT);					
				trans = lerpVec3(m_from[0], m_to[0], lerpAmount(0, eT));	// Interpolate
				scale = lerpVec3(m_from[1], m_to[1], lerpAmount(1, eT));	// Interpolate
				rot = slerp(m_from[2], m_to[2], lerpAmount(2, eT));			// Interpolate
			}
			math::Matrix BoneFrame::lerp(float eT) {
				updateFrame(eT);										// Step keyframes
				math::Matrix mat;										// Interpolated transform
				mat = math::Matrix::CreateScale(lerpVec3(m_from[1], m_to[1], lerpAmount(1, eT)));
				mat = math::Matrix::CreateFromQuaternion(slerp(m_from[2], m_to[2], lerpAmount(2, eT))) * mat;
				mat.Translation(lerpVec3(m_from[0], m_to[0], lerpAmount(0, eT)));
				return mat;
			}
		}
	}
}