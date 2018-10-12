#include "GizmoRenderBuffer.h"
#include "../../ThomasCore.h"
#include "../../resource/MemoryAllocation.h"

namespace thomas {
	namespace editor {
		namespace gizmo {

			constexpr uint32_t GIZMO_BUFFER_MEMORY = 4096;
			constexpr uint32_t GIZMO_DRAW_COMMAND_INIT = 200;

			GizmoRenderBuffer::GizmoRenderBuffer()
				: m_alloc(GIZMO_BUFFER_MEMORY, ThomasCore::Core().Memory()->allocSystemMemory(GIZMO_BUFFER_MEMORY, 64)), m_cmd()
			{
				m_cmd.reserve(GIZMO_DRAW_COMMAND_INIT);
			}
			GizmoRenderBuffer::~GizmoRenderBuffer()
			{
			}
			void GizmoRenderBuffer::submitCmd(GizmoRenderCommand cmd)
			{
				// Local data copy.
				uint32_t bytes = cmd.numVertex * sizeof(math::Vector3);
				math::Vector3 * alloc = reinterpret_cast<math::Vector3*>(m_alloc.allocate(bytes, sizeof(float)));
				std::memcpy(alloc, cmd.vertexData, bytes);
				cmd.vertexData = alloc;
				// Increment
				m_cmd.push_back(cmd);
			}
			void GizmoRenderBuffer::clear()
			{
				m_cmd.clear();
				m_alloc.clear();
			}
		}
	}
}