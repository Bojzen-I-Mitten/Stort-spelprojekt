#pragma once

#include <vector>
#include <stdint.h>
#include "GizmoRenderCommand.h"
#include "../../utils/allocator/LinearAllocator.h"


namespace thomas {
	namespace editor {
		namespace gizmo {
			constexpr uint32_t GIZMO_BUFFER_MEMORY = 4096;
			constexpr uint32_t GIZMO_DRAW_COMMAND_INIT = 79;

			class GizmoRenderBuffer
			{
			public:
				GizmoRenderBuffer(uint32_t bufferMem = GIZMO_BUFFER_MEMORY, uint32_t drawCommandMax = GIZMO_DRAW_COMMAND_INIT);
				~GizmoRenderBuffer();

				GizmoRenderBuffer(const GizmoRenderBuffer& copy) = delete;
				GizmoRenderBuffer& operator=(const GizmoRenderBuffer& copy) = delete;
				/* Submit gizmo command.
				*/
				void submitCmd(GizmoRenderCommand cmd);
				/* Reset allocations. */
				void clear();

			public:

				uint32_t Count() { return m_count; }
				GizmoRenderCommand& operator[](uint32_t index) { return m_list[index]; }

			private:
				utility::allocator::LinearAllocator m_alloc;	// Memory buffer (Vertex data)
				GizmoRenderCommand* m_list;						// Command buffer
				uint32_t m_count;
				uint32_t m_capacity;

			};
		}
	}
}