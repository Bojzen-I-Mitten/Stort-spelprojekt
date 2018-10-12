#pragma once

#include <vector>
#include "../../utils/Math.h"

namespace thomas {
	namespace editor {
		namespace gizmo {

			enum class GizmoPasses
			{
				SOLID = 0,
				WIREFRAME = 1,
			};
			struct GizmoRenderCommand
			{
				math::Matrix matrix;
				math::Vector3* vertexData;
				uint32_t numVertex;
				math::Color color;
				D3D_PRIMITIVE_TOPOLOGY topology;
				GizmoPasses pass;

				GizmoRenderCommand(math::Vector3* v, uint32_t numVertex, math::Matrix m, math::Color c, D3D_PRIMITIVE_TOPOLOGY t, GizmoPasses p) :
					vertexData(v), numVertex(numVertex), matrix(m), color(c), topology(t), pass(p) {};
			};

		}
	}
}