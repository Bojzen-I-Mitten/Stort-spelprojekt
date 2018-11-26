#pragma once
#include "D3D.h"
#include <dxgi1_6.h>
namespace thomas
{
	namespace utils
	{
		namespace profiling
		{
			//GPU Timestamp
			enum GTS
			{
				GTS_BEGIN_FRAME,
				GTS_MAIN_OBJECTS,
				GTS_PARTICLES,
				GTS_GIZMO_OBJECTS,
				GTS_GUI,
				GTS_END_FRAME,

				GTS_MAX
			};
			class GpuProfiler
			{
			public:

				GpuProfiler();
				bool Init();
				void Destroy();

				void BeginFrame();
				void Timestamp(GTS gts);
				void EndFrame();
				void AddDrawCall(int vertexCount);

				// Wait on GPU for last frame's data (not this frame's) to be available
				void RetriveTimeStamps();

			public:
				int GetNumOfDrawCalls();
				float GetTimeStamp(GTS gts);
				float GetFrameTime();
				float GetMemoryUsage();
				float GetTotalMemory();
				int GetNumberOfDrawCalls();
				int GetVertexCount();


			private:
				int m_frameQuery;								
				ID3D11Query * m_queryDisjoint[FRAME_BUFFERS];				// "Timestamp disjoint" query; records whether timestamps are valid
				ID3D11Query * m_queryTimestamp[GTS_MAX][FRAME_BUFFERS];		// Individual timestamp queries for each relevant point in the frame
				
				int m_memoryQuery;
				DXGI_QUERY_VIDEO_MEMORY_INFO m_info[2];

				float m_timings[GTS_MAX];						// Last frame's timings (each relative to previous GTS)

				int m_totalVertexCount;
				int m_drawCalls;

				float m_memoryUsage;
				float m_totalMemory;
			};
		}
	}
}