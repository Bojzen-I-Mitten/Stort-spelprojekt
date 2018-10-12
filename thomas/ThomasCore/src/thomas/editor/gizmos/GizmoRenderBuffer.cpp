#include "GizmoRenderBuffer.h"
#include "../../ThomasCore.h"
#include "../../resource/MemoryAllocation.h"

namespace thomas {
	namespace editor {
		namespace gizmo {
			
			GizmoRenderBuffer::GizmoRenderBuffer(uint32_t bufferMem, uint32_t drawCommandMax)
				: m_alloc(bufferMem, ThomasCore::Core().Memory()->allocSystemMemory(bufferMem, 64)), m_cmd()
			{
				m_cmd.reserve(drawCommandMax);
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