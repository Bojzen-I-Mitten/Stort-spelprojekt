#pragma once
#include "../../resource/ShaderProperty/ShaderPropertyStruct.h"

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
				FrameAllocation(unsigned int numStruct, unsigned int numDataBytes);
				~FrameAllocation();
				/*	Give ownership of the pointer to the allocation
				*/ 
				const resource::shaderproperty::ShaderPropertyStatic* allocate(const resource::shaderproperty::ShaderPropertyStatic* alloc);
				const resource::shaderproperty::ShaderPropertyStatic* allocate(const resource::shaderproperty::ShaderPropertyStatic* alloc, unsigned int num);
				void clear();

				FrameAllocation(const FrameAllocation& copy) = delete;
				FrameAllocation(FrameAllocation&& move) = delete;
				const FrameAllocation& operator=(const FrameAllocation& copy) = delete;
				FrameAllocation& operator=(FrameAllocation&& move);

			private:
				bool canAllocate(uint32_t bytes);
				void * allocData(uint32_t off);
				/* Create a frame copy of the data in the struct. 
				*/
				void copy(resource::shaderproperty::ShaderPropertyStruct& info);
			private:
				/* Raw Data allocation
				*/
				void * m_dataAlloc;
				uint32_t m_allocatedBytes, m_dataTotalBytes;
				/* Struct allocation
				*/
				std::vector< resource::shaderproperty::ShaderPropertyStruct> m_alloc;
			};

		}
	}
}