#pragma once
#include "../../resource/ShaderProperty/ShaderProperty.h"

#include<vector>


namespace thomas {


	namespace resource
	{
		namespace shaderproperty {
			class ShaderProperty;
		}
	}


	namespace graphics {

		namespace render {

			class FrameAllocation {
			public:
				FrameAllocation(unsigned int size);
				~FrameAllocation();

			private:
				std::vector< thomas::resource::shaderproperty::ShaderProperty*> m_alloc;
			};

		}
	}
}