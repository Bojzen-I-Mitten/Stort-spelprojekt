#include "Gizmos.h"
#include "../../resource/Model.h"
#include "../../resource/Material.h"
#include "../../resource/Shader.h"
#include "../../graphics/Mesh.h"
#include "../../utils/d3d.h"
#include "../../utils/Buffers.h"
#include "GizmoRenderCommand.h"
#include "GizmoRenderBuffer.h"
#include "../../ThomasCore.h"
namespace thomas
{
	namespace editor
	{
		Gizmos::Gizmos() : 
			m_gizmoMaterial(NULL),
			m_vertexBuffer(),
			m_matrix(),
			m_imguiNumber()
		{
			for (uint32_t i = 0; i < MAX_NUM_THREAD; i++)
			{
				m_update_buffers[i] = new gizmo::GizmoRenderBuffer();
				m_render_buffers[i] = new gizmo::GizmoRenderBuffer();
			}
		}

		Gizmos::~Gizmos()
		{
		}

		Gizmos & Gizmos::Gizmo()
		{
			static Gizmos gizmo;
			return gizmo;
		}

		void Gizmos::DrawModel(resource::Model * model, math::Vector3 position = math::Vector3::Zero, math::Quaternion rotation = math::Quaternion::Identity, math::Vector3 scale = math::Vector3::One)
		{
			DrawModel(model, -1, position, rotation, scale);
		}


		void Gizmos::DrawModel(resource::Model * model, int meshIndex, math::Vector3 position = math::Vector3::Zero, math::Quaternion rotation = math::Quaternion::Identity, math::Vector3 scale = math::Vector3::One)
		{
			//std::vector<graphics::Mesh*> meshes = model->GetMeshes();
			//if (meshIndex == -1)
			//{
			//	for (graphics::Mesh* mesh : meshes)
			//		
			//}
			//else
			//{
			//	s_gizmoMaterial->Draw(meshes[meshIndex]);
			//}*/
			

			/*s_gizmoMaterial->SetShaderPass((int)GizmoPasses::SOLID);
			s_gizmoMaterial->m_topology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			s_gizmoMaterial->SetMatrix("gizmoMatrix", math::CreateMatrix(position, rotation, scale));
			s_gizmoMaterial->Bind();
			
			if (meshIndex == -1)
			{
				for (graphics::Mesh* mesh : meshes)
					s_gizmoMaterial->Draw(mesh);
			}
			else
			{
				s_gizmoMaterial->Draw(meshes[meshIndex]);
			}*/
		}

		void Gizmos::DrawWireModel(resource::Model * model, math::Vector3 position, math::Quaternion rotation, math::Vector3 scale)
		{
			DrawWireModel(model, -1, position, rotation, scale);
		}

		void Gizmos::DrawWireModel(resource::Model * model, int meshIndex, math::Vector3 position, math::Quaternion rotation, math::Vector3 scale)
		{
			/*s_gizmoMaterial->SetShaderPass((int)GizmoPasses::WIREFRAME);
			s_gizmoMaterial->m_topology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			s_gizmoMaterial->SetMatrix("gizmoMatrix", math::CreateMatrix(position, rotation, scale));
			s_gizmoMaterial->Bind();
			std::vector<graphics::Mesh*> meshes = model->GetMeshes();
			if (meshIndex == -1)
			{
				for (graphics::Mesh* mesh : meshes)
					s_gizmoMaterial->Draw(mesh);
			}
			else
			{
				s_gizmoMaterial->Draw(meshes[meshIndex]);
			}*/
		}


		void Gizmos::DrawBoundingCapsule(math::Vector3 center, float radius, float height)
		{
			
			math::Vector3 xAxis = math::Vector3::UnitX * radius;
			math::Vector3 yAxis = math::Vector3::UnitY * radius;
			math::Vector3 zAxis = math::Vector3::UnitZ * radius;
						
			math::Vector3 offset = math::Vector3(0, (height / 2.0f), 0);
			math::Vector3 top = center + offset;
			math::Vector3 bottom = center - offset;

			DrawRing(top, xAxis, zAxis);
			DrawArc(top, xAxis, yAxis);
			DrawArc(top, zAxis, yAxis);

			DrawRing(bottom, xAxis, zAxis);
			DrawArc(bottom, xAxis, -yAxis);
			DrawArc(bottom, zAxis, -yAxis);
			
			DrawLine(top + xAxis, bottom + xAxis);
			DrawLine(top - xAxis, bottom - xAxis);

			DrawLine(top + zAxis, bottom + zAxis);
			DrawLine(top - zAxis, bottom - zAxis);

		}

		void Gizmos::DrawCube(math::Vector3 center, math::Vector3 size)
		{			
			DrawModel(utils::Primitives::GetPrimitive(utils::Primitives::Type::Cube), center, math::Quaternion::Identity, size);
		}

		void Gizmos::DrawWireCube(math::Vector3 center, math::Vector3 size)
		{
			/*s_gizmoMaterial->SetShaderPass((int)GizmoPasses::SOLID);
			s_gizmoMaterial->m_topology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			s_gizmoMaterial->Bind();*/
			//s_gizmoMaterial->Draw(graphics::Model::GetPrimitive(graphics::PrimitiveType::Cube))
		}

		void Gizmos::DrawBoundingOrientedBox(const math::BoundingOrientedBox & obb)
		{
			math::Vector3 corners[math::BoundingOrientedBox::CORNER_COUNT];
			obb.GetCorners(corners);

			std::vector<math::Vector3> lines(24);

			lines[0] = corners[0];
			lines[1] = corners[1];
			lines[2] = corners[1];
			lines[3] = corners[2];
			lines[4] = corners[2];
			lines[5] = corners[3];
			lines[6] = corners[3];
			lines[7] = corners[0];

			lines[8] = corners[0];
			lines[9] = corners[4];
			lines[10] = corners[1];
			lines[11] = corners[5];
			lines[12] = corners[2];
			lines[13] = corners[6];
			lines[14] = corners[3];
			lines[15] = corners[7];

			lines[16] = corners[4];
			lines[17] = corners[5];
			lines[18] = corners[5];
			lines[19] = corners[6];
			lines[20] = corners[6];
			lines[21] = corners[7];
			lines[22] = corners[7];
			lines[23] = corners[4];

			DrawLines(lines);
		}

		void Gizmos::DrawBoundingSphere(const math::BoundingSphere & sphere)
		{
			math::Vector3 origin = sphere.Center;
			const float radius = sphere.Radius;

			math::Vector3 xAxis = math::Vector3::UnitX * radius;
			math::Vector3 yAxis = math::Vector3::UnitY * radius;
			math::Vector3 zAxis = math::Vector3::UnitZ * radius;

			DrawRing(origin, xAxis, zAxis);
			DrawRing(origin, xAxis, yAxis);
			DrawRing(origin, zAxis, yAxis);
		}

		void Gizmos::DrawRing(math::Vector3 origin, math::Vector3 majorAxis, math::Vector3 minorAxis)
		{
			const size_t ringSegments = 32;

			float angleDelta = math::PI*2.0f / float(ringSegments);

			std::vector<math::Vector3> lines(ringSegments + 1);

			math::Vector3 cosDelta = math::Vector3(cosf(angleDelta));
			math::Vector3 sinDelta = math::Vector3(sinf(angleDelta));
			math::Vector3 incrementalSin = math::Vector3::Zero;
			math::Vector3 incrementalCos = math::Vector3::One;
			for (size_t i = 0; i < ringSegments; i++)
			{
				math::Vector3 pos = majorAxis * incrementalCos + origin;
				pos = minorAxis * incrementalSin + pos;
				lines[i] = pos;
				// Standard formula to rotate a vector.
				math::Vector3 newCos = incrementalCos * cosDelta - incrementalSin * sinDelta;
				math::Vector3 newSin = incrementalCos * sinDelta + incrementalSin * cosDelta;
				incrementalCos = newCos;
				incrementalSin = newSin;
			}
			lines[ringSegments] = lines[0];


			DrawLines(lines, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		}

		void Gizmos::DrawArc(math::Vector3 origin, math::Vector3 majorAxis, math::Vector3 minorAxis)
		{
			const size_t ringSegments = 16;

			float angleDelta = math::PI / float(ringSegments);

			std::vector<math::Vector3> lines(ringSegments + 1);

			math::Vector3 cosDelta = math::Vector3(cosf(angleDelta));
			math::Vector3 sinDelta = math::Vector3(sinf(angleDelta));
			math::Vector3 incrementalSin = math::Vector3::Zero;
			math::Vector3 incrementalCos = math::Vector3::One;
			for (size_t i = 0; i <= ringSegments; i++)
			{
				math::Vector3 pos = majorAxis * incrementalCos + origin;
				pos = minorAxis * incrementalSin + pos;
				lines[i] = pos;
				// Standard formula to rotate a vector.
				math::Vector3 newCos = incrementalCos * cosDelta - incrementalSin * sinDelta;
				math::Vector3 newSin = incrementalCos * sinDelta + incrementalSin * cosDelta;
				incrementalCos = newCos;
				incrementalSin = newSin;
			}


			DrawLines(lines, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		}

		void Gizmos::DrawLine(math::Vector3 from, math::Vector3 to)
		{
			std::vector<math::Vector3> corners(2);
			corners[0] = from;
			corners[1] = to;

			DrawLines(corners);
		}

		void Gizmos::DrawSphere(math::Vector3 center, float radius)
		{
			/*s_gizmoMaterial->SetShaderPass((int)GizmoPasses::SOLID);
			s_gizmoMaterial->m_topology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			s_gizmoMaterial->Bind();*/
			//s_gizmoMaterial->Draw(graphics::Model::GetPrimitive(graphics::PrimitiveType::Cube))
		}

		void Gizmos::DrawWireSphere(math::Vector3 center, float radius)
		{
			/*s_gizmoMaterial->SetShaderPass((int)GizmoPasses::WIREFRAME);
			s_gizmoMaterial->m_topology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
			s_gizmoMaterial->Bind();*/
			//s_gizmoMaterial->Draw(graphics::Model::GetPrimitive(graphics::PrimitiveType::Cube))
		}

		void Gizmos::DrawRay(math::Vector3 from, math::Vector3 direction)
		{

			std::vector<math::Vector3> corners(2);
			corners[0] = from;
			corners[1] = from + direction * 1000;

			DrawLines(corners);
		}

		void Gizmos::DrawRay(math::Ray ray)
		{
			std::vector<math::Vector3> corners(2);
			corners[0] = ray.position;
			corners[1] = ray.position + ray.direction * 1000;

			DrawLines(corners);
		}

		void Gizmos::DrawFrustum(math::Vector3 center, float fov, float maxRange, float minRange, float aspect)
		{
			math::Matrix proj = math::Matrix::CreatePerspectiveFieldOfView(fov, aspect, minRange, maxRange);
			DrawFrustum(math::BoundingFrustum(proj));
		}

		void Gizmos::DrawFrustum(math::BoundingFrustum& frustrum)
		{

			math::Vector3 corners[DirectX::BoundingFrustum::CORNER_COUNT];
			frustrum.GetCorners(corners);

			std::vector<math::Vector3> lines(24);

			lines[0] = corners[0];
			lines[1] = corners[1];
			lines[2] = corners[1];
			lines[3] = corners[2];
			lines[4] = corners[2];
			lines[5] = corners[3];
			lines[6] = corners[3];
			lines[7] = corners[0];

			lines[8] = corners[0];
			lines[9] = corners[4];
			lines[10] = corners[1];
			lines[11] = corners[5];
			lines[12] = corners[2];
			lines[13] = corners[6];
			lines[14] = corners[3];
			lines[15] = corners[7];

			lines[16] = corners[4];
			lines[17] = corners[5];
			lines[18] = corners[5];
			lines[19] = corners[6];
			lines[20] = corners[6];
			lines[21] = corners[7];
			lines[22] = corners[7];
			lines[23] = corners[4];


			DrawLines(lines);
		}
		
		void Gizmos::DrawLines(std::vector<math::Vector3> lines, D3D_PRIMITIVE_TOPOLOGY topology)
		{
			// Submit to thread buffer:
			m_render_buffers[ThomasCore::Core().Thread_Index()]->submitCmd(
				gizmo::GizmoRenderCommand(
					lines.data(), lines.size(),				// Vertex info
					m_matrix, s_color,						// Transform, color (Transform 'should'/could be applied to the vertex data).
					topology, gizmo::GizmoPasses::SOLID));	// 
		}

		void Gizmos::TransferGizmoCommands()
		{
			for (uint32_t i = 0; i < MAX_NUM_THREAD; i++) 
				std::swap(m_render_buffers[i], m_update_buffers[i]);
		}

		void Gizmos::RenderGizmos()
		{
			for (uint32_t i = 0; i < MAX_NUM_THREAD; i++)
			{
				gizmo::GizmoRenderBuffer & buf = *m_render_buffers[i];
				for (uint32_t i = 0; i < buf.Count(); i++)
				{
					gizmo::GizmoRenderCommand& command = buf[i];
					m_gizmoMaterial->SetShaderPass((int)command.pass);
					m_gizmoMaterial->SetMatrix("gizmoMatrix", command.matrix);
					m_gizmoMaterial->SetColor("gizmoColor", command.color);
					m_gizmoMaterial->m_topology = command.topology;

					m_vertexBuffer->SetData(command.vertexData, command.numVertex);
					m_gizmoMaterial->GetShader()->BindVertexBuffer(m_vertexBuffer);
					m_gizmoMaterial->Bind();
					m_gizmoMaterial->Draw(command.numVertex, 0);
				}
			}
		}

		void Gizmos::ClearGizmos()
		{
			for (uint32_t i = 0; i < MAX_NUM_THREAD; i++)
				m_update_buffers[i]->clear();
		}

		void Gizmos::Init()
		{
			resource::Shader* shader = resource::Shader::CreateShader("../Data/FXIncludes/GizmoShader.fx");
			if (shader)
			{
				m_gizmoMaterial = new resource::Material(shader);
				SetColor(math::Color(1, 1, 1));
				SetMatrix(math::Matrix::Identity);
			}

			m_vertexBuffer = new utils::buffers::VertexBuffer(nullptr, sizeof(math::Vector3), 500, DYNAMIC_BUFFER); //500 hardcoded here :/

		}

		void Gizmos::Destroy()
		{
			delete m_gizmoMaterial;
			delete m_vertexBuffer;
		}


		void Gizmos::SetColor(math::Color color)
		{
			s_color = color;
			//s_gizmoMaterial->SetColor("gizmoColor", color);
		}

		void Gizmos::SetMatrix(math::Matrix matrix)
		{
			m_matrix = matrix;
			//s_gizmoMaterial->SetMatrix("gizmoMatrix", matrix);
		}

		void Gizmos::DrawPing(std::string ping)
		{
			ImGui::SetNextWindowSize(ImVec2(300, 0));
			ImGui::Begin("ping", nullptr, ImVec2(0, 0), 0.0f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			ImGui::Text(ping.c_str());
			ImGui::End();
		}

		void Gizmos::ImguiStringUpdate(std::string text, math::Vector2 size, math::Vector2 pos)
		{
			ImGui::SetNextWindowSize(ImVec2(size.x, size.y));
			ImGui::SetNextWindowPos(ImVec2(pos.x, pos.y));
			std::string ImguiName = std::to_string(pos.x)+","+std::to_string(pos.y);

			ImGui::Begin(ImguiName.c_str(), nullptr, ImVec2(0, 0), 0.0f, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
				ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings);
			ImGui::Text(text.c_str());
			ImGui::End();
		}

		void Gizmos::ImguiStringUpdate(std::string text, math::Vector2 pos)
		{

			ImguiStringUpdate(text, math::Vector2(text.size() * 8.f + 5.f, 0.f), pos);
		}

	}
}