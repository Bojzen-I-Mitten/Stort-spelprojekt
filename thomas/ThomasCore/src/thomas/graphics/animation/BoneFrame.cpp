#include "BoneFrame.h"

namespace thomas {
	namespace animation {

		BoneFrame::BoneFrame()
			: KeyFrame<3>(SIZE_BONE_CHANNEL)
		{
		}

		BoneFrame::~BoneFrame()
		{
		}


		void BoneFrame::init(const math::Matrix& pose) {
			math::Vector3 pos, scale;
			math::Quaternion rot;
			DirectX::XMVECTOR s, r, t;
			DirectX::XMMatrixDecompose(&s, &r, &t, pose);
			_from[0].set(0.f, pos);
			_from[1].set(0.f, scale);
			_from[2].set(0.f, rot);
			_to[0].set(FLT_MAX, pos);
			_to[1].set(FLT_MAX, scale);
			_to[2].set(FLT_MAX, rot);
		}

		void BoneFrame::storeState(unsigned int type, float eT) {
			if (type == 0)
				_from[type].set(lerpVec3(_from[type], _to[type], lerpAmount(type, eT)));
			else if (type == 1)
				_from[type].set(lerpVec3(_from[type], _to[type], lerpAmount(type, eT)));
			else
				_from[type].set(slerp(_from[type], _to[type], lerpAmount(type, eT)));
		}

		math::Matrix BoneFrame::lerp(float eT) {
			checkUpdate(eT);
			//Calc lerp amount
			float amount[3];
			for (unsigned int i = 0; i < NUM_BONE_CHANNEL; i++)
				amount[i] = lerpAmount(i, eT);
			//Transform between the channel values
			math::Matrix mat;
			mat = math::Matrix::CreateScale(lerpVec3(_from[1], _to[1], amount[1]));
			mat = math::Matrix::CreateFromQuaternion(slerp(_from[2], _to[2], amount[2])) * mat;
			mat.Translation(lerpVec3(_from[0], _to[0], amount[0]));

			return mat;
		}
	}
}