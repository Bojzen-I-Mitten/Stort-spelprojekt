#pragma once

#include <vector>
#include <stdint.h>
#include "GizmoRenderCommand.h"
#include "../../utils/allocator/LinearAllocator.h"


namespace thomas {
	namespace editor {
		namespace gizmo {
			constexpr uint32_t GIZMO_BUFFER_MEMORY = 4096;
			constexpr uint32_t GIZMO_DRAW_COMMAND_INIT = 200;

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

				std::vector<GizmoRenderCommand>::iterator begin() { return m_cmd.begin(); }
				std::vector<GizmoRenderCommand>::iterator end() { return m_cmd.end(); }
				std::vector<GizmoRenderCommand>::const_iterator begin() const { return m_cmd.begin(); }
				std::vector<GizmoRenderCommand>::const_iterator end() const { return m_cmd.end(); }

			private:
				utility::allocator::LinearAllocator m_alloc;	// Memory buffer (Vertex data)
				std::vector<GizmoRenderCommand> m_cmd;			// Command buffer



			};
		}
	}
}