#pragma once
#include <vector>
#include "../../Common.h"
#include "../../Constants.h"
#include "../../utils/Math.h"
namespace thomas
{
	namespace resource { class Model; class Material; }
	namespace utils { struct Ray; namespace buffers { class VertexBuffer; } }
	namespace editor
	{
		namespace gizmo {
			class GizmoRenderBuffer;
		}
		class Gizmos
		{

		public:		
			static Gizmos& Gizmo();
		public:

			Gizmos();
			~Gizmos();


			void DrawLines(std::vector<math::Vector3> lines, D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST);
			void DrawLines(math::Vector3 * lines, uint32_t num, D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST);
			void TransferGizmoCommands();
			void RenderGizmos();
			void ClearGizmos();
			void Init();
			void Destroy();
			void DrawModel(resource::Model* model, int meshIndex, math::Vector3 position, math::Quaternion rotation, math::Vector3 scale);
			void DrawModel(resource::Model* model, math::Vector3 position, math::Quaternion rotation, math::Vector3 scale);

			void DrawWireModel(resource::Model* model, int meshIndex, math::Vector3 position, math::Quaternion rotation, math::Vector3 scale);
			void DrawWireModel(resource::Model* model, math::Vector3 position, math::Quaternion rotation, math::Vector3 scale);

			void DrawBoundingCapsule(math::Vector3 center, float radius, float height);
			void DrawCube(math::Vector3 center, math::Vector3 size);
			void DrawWireCube(math::Vector3 center, math::Vector3 size);
			void DrawBoundingOrientedBox(const math::BoundingOrientedBox& obb);
			void DrawBoundingSphere(const math::BoundingSphere& sphere);
			void DrawRing(math::Vector3 origin, math::Vector3 majorAxis, math::Vector3 minorAxis);
			void DrawRing(math::Vector3 origin, math::Vector3 aroundAxis, float radius);
			void DrawArc(math::Vector3 origin, math::Vector3 majorAxis, math::Vector3 minorAxis);
			void DrawLine(math::Vector3 from, math::Vector3 to);
			/* Draw line of length toward the target vector
			*/
			void DrawLine(math::Vector3 from, math::Vector3 to, float len);	
			void DrawMatrixBasis(const math::Matrix & mat, float len = 0.05f);
			void DrawMatrixBasis(const math::Matrix & mat, math::Vector3 axisLenghts);
			void DrawSphere(math::Vector3 center, float radius);
			void DrawWireSphere(math::Vector3 center, float radius);
			void DrawRay(math::Vector3 from, math::Vector3 direction);
			void DrawRay(math::Vector3 from, math::Vector3 direction, float len);
			void DrawRay(math::Ray ray);

			void DrawFrustum(math::Vector3 center, float fov, float maxRange, float minRange, float aspect);
			void DrawFrustum(math::BoundingFrustum& frustrum);

			void SetColor(math::Color color);
			void SetMatrix(math::Matrix matrix);

			void DrawPing(std::string ping);
			void ImguiStringUpdate(std::string text, math::Vector2 size, math::Vector2 pos);
			void ImguiStringUpdate(std::string text, math::Vector2 pos);
		private:

			gizmo::GizmoRenderBuffer* m_update_buffers[MAX_NUM_THREAD];
			gizmo::GizmoRenderBuffer* m_render_buffers[MAX_NUM_THREAD];

			resource::Material* m_gizmoMaterial;
			utils::buffers::VertexBuffer* m_vertexBuffer;
			math::Matrix m_matrix;
			math::Color s_color;
			int m_imguiNumber;


		};
	}
}