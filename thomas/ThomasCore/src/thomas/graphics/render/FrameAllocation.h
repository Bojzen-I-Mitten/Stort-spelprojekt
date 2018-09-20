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
				/*	Give ownership of the pointer to the allocation
				*/ 
				resource::shaderproperty::ShaderProperty** allocate(resource::shaderproperty::ShaderProperty* alloc);
				resource::shaderproperty::ShaderProperty** allocate(resource::shaderproperty::ShaderProperty** alloc, unsigned int num);
				bool reserve(unsigned int num, resource::shaderproperty::ShaderProperty **& ptr);
				void clear();

				FrameAllocation(const FrameAllocation& copy) = delete;
				FrameAllocation(FrameAllocation&& move) = delete;
				const FrameAllocation& operator=(const FrameAllocation& copy) = delete;
				FrameAllocation& operator=(FrameAllocation&& move);

			private:
				std::vector< resource::shaderproperty::ShaderProperty*> m_alloc;
			};

		}
	}
}