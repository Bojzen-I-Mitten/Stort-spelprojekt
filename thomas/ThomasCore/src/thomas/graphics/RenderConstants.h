#pragma once

#include <string>

namespace thomas {
	namespace graphics {

		const static std::string THOMAS_DELTA_TIME("thomas_ObjectToWorld");


		const static std::string THOMAS_MATRIX_PROJECTION("thomas_MatrixP");
		const static std::string THOMAS_MATRIX_VIEW("thomas_MatrixV");
		const static std::string THOMAS_MATRIX_VIEW_INV("thomas_MatrixInvV");
		const static std::string THOMAS_MATRIX_VIEW_PROJ("thomas_MatrixVP");

		const static std::string THOMAS_VECTOR_CAMERA_POS("_WorldSpaceCameraPos");
		const static std::string THOMAS_VECTOR_CAMERA_DIR("_WorldSpaceCameraDir");

		const static std::string THOMAS_MATRIX_WORLD("thomas_ObjectToWorld");
		const static std::string THOMAS_MATRIX_WORLD_INV("thomas_WorldToObject");
		const static std::string THOMAS_MATRIX_SKIN_ARRAY("thomas_Bone_Array");
	}
}