#pragma once

#include <vector>
#include <stdint.h>
#include "GizmoRenderCommand.h"
#include "../../utils/allocator/LinearAllocator.h"

namespace thomas {
	namespace editor {
		namespace gizmo {

			class GizmoRenderBuffer
			{
			public:
				GizmoRenderBuffer();
				~GizmoRenderBuffer();
				/* Submit gizmo command.
				*/
				void submitCmd(GizmoRenderCommand cmd);
				/* Reset allocations. */
				void clear();

			private:
				utility::allocator::LinearAllocator m_alloc;	// Memory buffer (Vertex data)
				std::vector<GizmoRenderCommand> m_cmd;			// Command buffer

			};
		}
	}
}