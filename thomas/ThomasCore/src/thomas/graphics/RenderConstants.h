#pragma once

#include <string>
#include "../utils/Utility.h"

namespace thomas {
	namespace graphics {

		const static std::string THOMAS_DELTA_TIME("thomas_DeltaTime");


		const static std::string THOMAS_MATRIX_PROJECTION("thomas_MatrixP");
		const static std::string THOMAS_MATRIX_VIEW("thomas_MatrixV");
		const static std::string THOMAS_MATRIX_VIEW_INV("thomas_MatrixInvV");
		const static std::string THOMAS_MATRIX_VIEW_PROJ("thomas_MatrixVP");

		const static std::string THOMAS_VECTOR_CAMERA_POS("_WorldSpaceCameraPos");
		const static std::string THOMAS_VECTOR_CAMERA_DIR("_WorldSpaceCameraDir");

		const static std::string THOMAS_MATRIX_WORLD("thomas_ObjectToWorld");
		const static std::string THOMAS_MATRIX_WORLD_INV("thomas_WorldToObject");
		const static std::string THOMAS_MATRIX_SKIN_ARRAY("thomas_Bone_Array");
		const static uint32_t THOMAS_MATRIX_SKIN_ARRAY_HASH = utility::hash(THOMAS_MATRIX_SKIN_ARRAY);
	}
}