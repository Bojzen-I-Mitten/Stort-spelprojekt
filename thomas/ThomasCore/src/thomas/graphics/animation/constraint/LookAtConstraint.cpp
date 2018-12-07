#include "LookAtConstraint.h"
#include "../data/Skeleton.h"
#include "../../../editor/gizmos/Gizmos.h"

namespace thomas {
	namespace graphics {
		namespace animation {
			LookAtConstraint::LookAtConstraint(AxisConstraint)
				: m_weight(1.f), m_target(), m_axis(AxisConstraint::AxisXYZ), m_faceAxis(math::Axis::AxisZ)
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
			void LookAtConstraint::execute(Skeleton & skel, math::Matrix * objectPose, TransformComponents* comp, uint32_t boneInd)
			{
				math::Matrix pInv = objectPose[skel.getBone(boneInd)._parentIndex].Invert();				// Solve in space of parent bone
				math::Matrix relative = objectPose[boneInd] * pInv;											// Relative transform of bone (from parent) 
				math::Vector3 local_dir = math::Vector3::Transform(m_target, pInv) - relative.Translation();// Target offset from bone
				local_dir.Normalize();
				math::Vector3 faceAxis = math::getAxis(relative, m_faceAxis);								// Axis rotated
				faceAxis.Normalize();
				math::Vector3 up = relative.Up();
				up.Normalize();

				math::Quaternion rotation;
				switch (m_axis) {
					case AxisConstraint::AxisX:
					{
						float x = faceAxis.Dot(local_dir);								// Project on ZY plane
						float y = up.Dot(local_dir);
						float angle = std::atan2f(y, x);							// Angle diff on ZY plane
						angle *= m_weight;											// Weighted
						math::Vector3 r = relative.Right();							// X
						r.Normalize();
						rotation = math::Quaternion::CreateFromAxisAngle(r, angle);	// Create rotation around x
					}
						break;
					case AxisConstraint::AxisY:
					case AxisConstraint::AxisZ:
						// Not implemented
					case AxisConstraint::AxisXYZ:
						rotation = math::getRotationTo(faceAxis, local_dir);
						break;
				}
				// Apply
				math::Matrix orient = math::Matrix::CreateFromQuaternion(rotation);
				objectPose[boneInd] = relative * orient * objectPose[skel.getBone(boneInd)._parentIndex];
#ifdef _DEBUG
#ifdef _EDITOR
				editor::Gizmos::Gizmo().DrawMatrixBasis(objectPose[boneInd]);
				editor::Gizmos::Gizmo().SetColor(math::Color(1.f, 1.f, 0.f));
				editor::Gizmos::Gizmo().DrawLine(objectPose[boneInd].Translation(), m_target, 0.1f);
#endif
#endif
			}
			bool LookAtConstraint::apply(Skeleton & skel, uint32_t boneInd)
			{
				return true;
			}
		}
	}
}