#include "BoneFrame.h"

namespace thomas {
	namespace animation {

		BoneFrame::BoneFrame()
			: KeyFrame<3>(SIZE_BONE_CHANNEL), m_last_frame(0)
		{
		}

		BoneFrame::~BoneFrame()
		{
		}


		void BoneFrame::init(math::Vector3 pos, math::Vector3 scale, math::Vector3 rot) {
			_from[0].set(0.f, pos);
			_from[1].set(0.f, scale);
			_from[2].set(0.f, rot);
			_to[0].set(FLT_MAX, pos);
			_to[1].set(FLT_MAX, scale);
			_to[2].set(FLT_MAX, rot);
		}
		void BoneFrame::init(const math::Matrix& pose) {
			DirectX::XMVECTOR s, r, t;
			DirectX::XMMatrixDecompose(&s, &r, &t, pose);
			init(t, s, r);
		}

		void BoneFrame::storeState(unsigned int type, float eT) {
			if (type == 0)
				_from[type].set(lerpVec3(_from[type], _to[type], lerpAmount(type, eT)));
			else if (type == 1)
				_from[type].set(lerpVec3(_from[type], _to[type], lerpAmount(type, eT)));
			else
				_from[type].set(slerp(_from[type], _to[type], lerpAmount(type, eT)));
		}

		math::Vector3 BoneFrame::lerpCoordinate(float eT) {
			updateFrame(0, eT);										// Step keyframes
			return lerpVec3(_from[0], _to[0], lerpAmount(0, eT));   // Interpolate
		}
		math::Vector3 BoneFrame::lerpScale(float eT) {
			updateFrame(1, eT);										// Step keyframes
			return lerpVec3(_from[1], _to[1], lerpAmount(1, eT));   // Interpolate
		}
		math::Quaternion BoneFrame::lerpRotation(float eT) {
			updateFrame(2, eT);										// Step keyframes
			return slerp(_from[2], _to[2], lerpAmount(2, eT));   // Interpolate
		}
		math::Matrix BoneFrame::lerp(float eT) {
			updateFrame(eT);										// Step keyframes
			math::Matrix mat;										// Interpolated transform
			mat = math::Matrix::CreateScale(lerpVec3(_from[1], _to[1], lerpAmount(1, eT)));
			mat = math::Matrix::CreateFromQuaternion(slerp(_from[2], _to[2], lerpAmount(2, eT))) * mat;
			mat.Translation(lerpVec3(_from[0], _to[0], lerpAmount(0, eT)));
			return mat;
		}
	}
}