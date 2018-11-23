#include "EmptyPlayback.h"



namespace thomas {
	namespace graphics {
		namespace animation {
			EmptyPlayback::EmptyPlayback(Skeleton & skel) :
				AnimationNode(skel, 0)
			{
			}
			EmptyPlayback::~EmptyPlayback()
			{
			}
			void EmptyPlayback::update(float)
			{
			}
			math::Vector3 EmptyPlayback::calcBonePosition(unsigned int bone)
			{
				return math::Vector3();
			}
			math::Vector3 EmptyPlayback::calcBoneScale(unsigned int bone)
			{
				return math::Vector3();
			}
			math::Quaternion EmptyPlayback::calcBoneRot(unsigned int bone)
			{
				return math::Quaternion();
			}
			void EmptyPlayback::calcFrame(unsigned int bone, math::Vector3 & trans, math::Vector3 & scale, math::Quaternion & rot)
			{
			}
			void EmptyPlayback::calcFrame(TransformComponents * result)
			{
			}
		}
	}
}