#include "GizmoRenderBuffer.h"
#include "../../ThomasCore.h"
#include "../../resource/MemoryAllocation.h"

namespace thomas {
	namespace editor {
		namespace gizmo {
			
			GizmoRenderBuffer::GizmoRenderBuffer(uint32_t bufferMem, uint32_t drawCommandMax)
				: m_alloc(bufferMem, ThomasCore::Core().Memory()->allocSystemMemory(bufferMem, 64)), 
				m_list(reinterpret_cast<gizmo::GizmoRenderCommand*>(ThomasCore::Core().Memory()->allocSystemMemory(drawCommandMax * sizeof(gizmo::GizmoRenderCommand), 64))),
				m_count(0), m_capacity(drawCommandMax)
			{
				assert(m_list); // Allocation failed.
			}
			GizmoRenderBuffer::~GizmoRenderBuffer()
			{
			}
			void GizmoRenderBuffer::submitCmd(GizmoRenderCommand cmd)
			{
				if (m_count >= m_capacity) 
					return;	// No capacity
				// Local data copy.
				uint32_t bytes = cmd.numVertex * sizeof(math::Vector3);
				math::Vector3 * alloc = reinterpret_cast<math::Vector3*>(m_alloc.allocate(bytes, sizeof(float)));
				std::memcpy(alloc, cmd.vertexData, bytes);
				cmd.vertexData = alloc;
				// Increment
				m_list[m_count++] = cmd;
			}
			void GizmoRenderBuffer::clear()
			{
				m_count = 0;
				m_alloc.clear();
			}
		}
	}
}