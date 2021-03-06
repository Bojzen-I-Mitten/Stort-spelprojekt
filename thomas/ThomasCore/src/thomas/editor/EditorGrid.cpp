#include "EditorGrid.h"
#include "../utils/d3d.h"
#include "../resource/Material.h"
#include "../utils/Buffers.h"
#include "../resource/Shader.h"
#include "../object/component/Camera.h"
#include "../graphics/Renderer.h"
#include "../graphics/render/Frame.h"
namespace thomas
{
	namespace editor
	{
		void EditorGrid::AddLine(math::Vector3 from, math::Vector3 to, math::Vector4 color, float viewDistance)
		{


			m_lines.positions.push_back(math::Vector4(from.x, from.y, from.z, viewDistance));
			m_lines.colors.push_back(color);

			m_lines.positions.push_back(math::Vector4(to.x, to.y, to.z, viewDistance));
			m_lines.colors.push_back(color);

		}

		EditorGrid::EditorGrid(int gridSize, float cellSize, int internalGridSize)
		{
			m_gridSize = gridSize;
			m_cellSize = cellSize;
			m_internalGridSize = internalGridSize;
			for (float i = -m_gridSize / 2.f; i <= m_gridSize / 2.f; i += cellSize)
			{
				math::Vector3 from(i, 0.0f, -m_gridSize / 2.f);
				math::Vector3 to(i, 0.0f, m_gridSize / 2.f);
				AddLine(from, to, math::Vector4(0.40625f, 0.40625f, 0.40625f, 1.0f), 25);
				from = math::Vector3(-m_gridSize / 2.f, 0.0f, i);
				to = math::Vector3(m_gridSize / 2.f, 0.0f, i);
				AddLine(from, to, math::Vector4(0.40625f, 0.40625f, 0.40625f, 1.0f), 25);

				for (float j = cellSize / internalGridSize; j < cellSize; j += cellSize / internalGridSize)
				{
					from = math::Vector3(i + j, 0.0f, -m_gridSize / 2.f);
					to = math::Vector3(i + j, 0.0f, m_gridSize / 2.f);
					AddLine(from, to, math::Vector4(0.3046875f, 0.3046875f, 0.3046875f, 1.0f), 1.5f);
					from = math::Vector3(-m_gridSize / 2.f, 0.0f, i + j);
					to = math::Vector3(m_gridSize / 2.f, 0.0f, i + j);
					AddLine(from, to, math::Vector4(0.3046875f, 0.3046875f, 0.3046875f, 1.0f), 1.5f);
				}

			}

			resource::Shader* shader = graphics::Renderer::Instance()->getShaderList().CreateShader("../Data/FXIncludes/EditorGridShader.fx");
			if (shader)
			{
				m_material = new resource::Material(shader);
				m_material->m_renderQueue = 1;
				m_material->m_topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
				m_mesh = std::shared_ptr<graphics::Mesh>(new graphics::Mesh(m_lines, {}, "grid"));
				m_lines.positions.clear();
				m_lines.colors.clear();
			}


		}

		void EditorGrid::Draw(object::component::Camera* camera)
		{
			if (m_material)
			{
				math::Vector3 cameraPos = camera->GetPosition();
				int scale = (int)log10(((abs((cameraPos.y*m_internalGridSize) / 2) + 1) / m_cellSize)*m_cellSize);
				scale = (int)pow(10.0f, scale);
				math::Matrix worldMatrix = math::Matrix::CreateScale((scale)*m_cellSize) * math::Matrix::CreateTranslation(
					std::floorf((cameraPos.x / scale))*scale,	//Must be int here, grid moves with camera otherwise
					0.0f,
					std::floorf((cameraPos.z / scale))*scale	//Must be int here, grid moves with camera otherwise
				);
				scale = int(scale * m_cellSize);
				math::Vector4 cameraScaleMatrix(cameraPos.x, cameraPos.y, cameraPos.z, 0);
				m_material->SetVector("cameraPos", math::Vector4(cameraScaleMatrix));
				m_material->SetInt("gridScale", scale);

				graphics::render::RenderCommand cmd(worldMatrix, m_mesh.get(), m_material, camera->ID());
				graphics::Renderer::Instance()->SubmitCommand(cmd);
			}

		}

		EditorGrid::~EditorGrid()
		{
		}

	}
}
