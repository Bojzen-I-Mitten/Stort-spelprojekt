#include "BulletDebugDraw.h"
#include "../utils/Math.h"
#include "../Physics.h"
#include "../editor/gizmos/Gizmos.h"
namespace thomas
{
	namespace graphics
	{
		BulletDebugDraw::BulletDebugDraw()
		{
			
		}

		BulletDebugDraw::~BulletDebugDraw()
		{

		}

		void BulletDebugDraw::drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & fromColor, const btVector3 & toColor)
		{
			editor::Gizmos::SetMatrix(math::Matrix::Identity);
			editor::Gizmos::SetColor(math::Color(1, 1, 0));
			editor::Gizmos::DrawLine(thomas::Physics::ToSimple(from), thomas::Physics::ToSimple(to));
		}

		void BulletDebugDraw::drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & color)
		{
			drawLine(from, to, color, color);
		}

		void BulletDebugDraw::drawSphere(const btVector3 & p, btScalar radius, const btVector3 & color)
		{
			editor::Gizmos::SetMatrix(math::Matrix::Identity);
			editor::Gizmos::SetColor(math::Color(1, 1, 0));
			editor::Gizmos::DrawBoundingSphere(math::BoundingSphere(thomas::Physics::ToSimple(p), radius));
		}

		void BulletDebugDraw::drawTriangle(const btVector3 & a, const btVector3 & b, const btVector3 & c, const btVector3 & color, btScalar alpha)
		{
			editor::Gizmos::SetMatrix(math::Matrix::Identity);
			editor::Gizmos::SetColor(math::Color(1, 1, 0));
			editor::Gizmos::DrawLine(thomas::Physics::ToSimple(a), thomas::Physics::ToSimple(b));
			editor::Gizmos::DrawLine(thomas::Physics::ToSimple(b), thomas::Physics::ToSimple(c));
			editor::Gizmos::DrawLine(thomas::Physics::ToSimple(c), thomas::Physics::ToSimple(a));
		}

		void BulletDebugDraw::drawContactPoint(const btVector3 & PointOnB, const btVector3 & normalOnB, btScalar distance, int lifeTime, const btVector3 & color)
		{
		}

		void BulletDebugDraw::reportErrorWarning(const char * warningString)
		{
			LOG("BULLET: " << warningString);
		}

		void BulletDebugDraw::setDebugMode(int debugMode)
		{
			m_debugMode = debugMode;
		}

		
	}
}