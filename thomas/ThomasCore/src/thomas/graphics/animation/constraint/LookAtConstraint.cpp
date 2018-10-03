#include "LookAtConstraint.h"


namespace thomas {
	namespace graphics {
		namespace animation {
			LookAtConstraint::LookAtConstraint(AxisConstraint)
				: m_weight(1.f), m_target(), m_axis(AxisConstraint::AxisXYZ)
			{
			}
			LookAtConstraint::~LookAtConstraint()
			{

			}
			/* Project onto the plane orthogonal to the axis and the plane intersect origo
			 * Warning not tested!
			*/
			math::Vector3 projectAlongAxis(math::Vector3 plane_norm, math::Vector3 vec) {
				float d = plane_norm.Dot(vec);
				return vec - d * plane_norm;
			}
			/* Project onto the plane orthogonal to the axis and the plane intersects the point
			 * Warning not tested!
			*/
			math::Vector3 projectAlongAxis(math::Vector3 plane_norm, math::Vector3 plane_point, math::Vector3 vec) {
				float d = plane_norm.Dot(vec);
				d -= plane_norm.Dot(plane_point);
				return vec - d * plane_norm;
			}
			/* Project on to an axis
			 * Warning not tested!
			*/
			math::Vector3 projectOntoAxis(math::Vector3 axis, math::Vector3 vec) {
				return axis * axis.Dot(vec);
			}
			void LookAtConstraint::execute(Skeleton & skel, math::Matrix * objectPose, uint32_t boneInd)
			{
				math::Vector3 dir = m_target - objectPose[boneInd].Translation();
				dir.Normalize();
				math::Vector3 forward = objectPose[boneInd].Forward();
				forward.Normalize();
				math::Vector3 up = objectPose[boneInd].Up();
				up.Normalize();
				math::Quaternion rotation;
				switch (m_axis) {
					case AxisConstraint::AxisX:
					{
						float x = forward.Dot(dir);									// Project on ZY plane
						float y = up.Dot(dir);
						float angle = std::atan2f(y, x);							// Angle diff on ZY plane
						angle *= m_weight;											// Weighted
						math::Vector3 r = objectPose->Right();						// X
						r.Normalize();
						rotation = math::Quaternion::CreateFromAxisAngle(r, angle);	// Create rotation around x
					}
						break;
					case AxisConstraint::AxisY:
					case AxisConstraint::AxisZ:
						// Not implemented
					case AxisConstraint::AxisXYZ:
						rotation = math::getRotationTo(forward, dir);
						break;
				}
				// Apply
				objectPose[boneInd] *= math::Matrix::CreateFromQuaternion(rotation);
			}
		}
	}
}