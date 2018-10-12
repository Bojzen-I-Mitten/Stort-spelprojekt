#pragma once
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
			void DrawArc(math::Vector3 origin, math::Vector3 majorAxis, math::Vector3 minorAxis);
			void DrawLine(math::Vector3 from, math::Vector3 to);
			void DrawSphere(math::Vector3 center, float radius);
			void DrawWireSphere(math::Vector3 center, float radius);
			void DrawRay(math::Vector3 from, math::Vector3 direction);
			void DrawRay(math::Ray ray);

			void DrawFrustum(math::Vector3 center, float fov, float maxRange, float minRange, float aspect);
			void DrawFrustum(math::BoundingFrustum& frustrum);

			void SetColor(math::Color color);
			void SetMatrix(math::Matrix matrix);

			void DrawPing(std::string ping);
			void ImguiStringUpdate(std::string text, math::Vector2 size, math::Vector2 pos);
			void ImguiStringUpdate(std::string text, math::Vector2 pos);
		private:

			std::unique_ptr<gizmo::GizmoRenderBuffer> m_render_buffers[MAX_NUM_THREAD];

			resource::Material* m_gizmoMaterial;
			utils::buffers::VertexBuffer* m_vertexBuffer;
			math::Matrix m_matrix;
			math::Color s_color;
			int m_imguiNumber;


		};
	}
}