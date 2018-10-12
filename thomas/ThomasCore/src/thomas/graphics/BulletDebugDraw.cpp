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
			editor::Gizmos::Gizmo().SetMatrix(math::Matrix::Identity);
			editor::Gizmos::Gizmo().SetColor(math::Color(1, 1, 0));
			editor::Gizmos::Gizmo().DrawLine(thomas::Physics::ToSimple(from), thomas::Physics::ToSimple(to));
		}

		void BulletDebugDraw::drawLine(const btVector3 & from, const btVector3 & to, const btVector3 & color)
		{
			drawLine(from, to, color, color);
		}

		void BulletDebugDraw::drawSphere(const btVector3 & p, btScalar radius, const btVector3 & color)
		{
			editor::Gizmos::Gizmo().SetMatrix(math::Matrix::Identity);
			editor::Gizmos::Gizmo().SetColor(math::Color(1, 1, 0));
			editor::Gizmos::Gizmo().DrawBoundingSphere(math::BoundingSphere(thomas::Physics::ToSimple(p), radius));
		}

		void BulletDebugDraw::drawTriangle(const btVector3 & a, const btVector3 & b, const btVector3 & c, const btVector3 & color, btScalar alpha)
		{
			editor::Gizmos::Gizmo().SetMatrix(math::Matrix::Identity);
			editor::Gizmos::Gizmo().SetColor(math::Color(1, 1, 0));
			editor::Gizmos::Gizmo().DrawLine(thomas::Physics::ToSimple(a), thomas::Physics::ToSimple(b));
			editor::Gizmos::Gizmo().DrawLine(thomas::Physics::ToSimple(b), thomas::Physics::ToSimple(c));
			editor::Gizmos::Gizmo().DrawLine(thomas::Physics::ToSimple(c), thomas::Physics::ToSimple(a));
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
		/*
		void BulletDebugDraw::drawLineFinal()
		{
			if (m_lines.positions.empty())
				if (m_linePositions.empty())
					return;
			DirectX::CommonStates states(utils::D3D::Instance()->GetDevice());
			utils::D3D::Instance()->GetDeviceContext()->OMSetBlendState(states.Opaque(), nullptr, 0xFFFFFFFF);
			utils::D3D::Instance()->GetDeviceContext()->OMSetDepthStencilState(states.DepthNone(), 0);
			utils::D3D::Instance()->GetDeviceContext()->RSSetState(states.CullNone());
			// Set the data and send to GPU
			m_vertexBufferPos->SetData(m_linePositions);
			m_vertexBufferColor->SetData(m_lineColors);
			m_material->m_topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
			m_material->GetShader()->BindVertexBuffers({ m_vertexBufferPos.get(), m_vertexBufferColor.get() });
			m_material->SetMatrix("viewProjection", m_viewProjection);
			m_material->Bind();
			m_material->Draw(m_linePositions.size(), 0);

			// Clear the memory
			m_linePositions.clear();
			m_lineColors.clear();
		}
		*/
	}
}