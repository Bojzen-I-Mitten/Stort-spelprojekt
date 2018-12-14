#pragma once
#include "D3D.h"

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
				GTS_SHADOWS_BEGIN,
				GTS_SHADOWS_END,
				GTS_MAIN_OBJECTS_BEGIN,
				GTS_MAIN_OBJECTS_END,
				GTS_PARTICLES_BEGIN,
				GTS_PARTICLES_END,
				GTS_GIZMO_OBJECTS_BEGIN,
				GTS_GIZMO_OBJECTS_END,
				GTS_GUI_BEGIN,
				GTS_GUI_END,
				GTS_END_FRAME,

				GTS_MAX
			};
			class GpuProfiler
			{
			public:
				static GpuProfiler* Instance();
				~GpuProfiler() = default;

				bool Init();
				void Destroy();

				void BeginFrame();
				void Timestamp(GTS gts);
				void EndFrame();
				void AddDrawCall(int faceCount, int vertexCount);

				// Wait on GPU for last frame's data (not this frame's) to be available
				void RetriveTimeStamps();

			public:
				int GetNumOfDrawCalls();
				float GetTimeStamp(GTS gts);
				float GetFrameTime();
				float GetMemoryUsage();
				float GetCurrentMemory();
				float GetTotalMemory();
				int GetNumberOfDrawCalls();
				int GetVertexCount();
				int GetFaceCount();

			private:
				GpuProfiler() = default;

				int m_frameQuery;								
				ID3D11Query * m_queryDisjoint[FRAME_BUFFERS];				// "Timestamp disjoint" query; records whether timestamps are valid
				ID3D11Query * m_queryTimestamp[GTS_MAX][FRAME_BUFFERS];		// Individual timestamp queries for each relevant point in the frame
				
				int m_memoryQuery;
				DXGI_QUERY_VIDEO_MEMORY_INFO m_info[2];

				float m_timings[GTS_MAX];						// Last frame's timings (each relative to previous GTS)

			int m_totalVertexCount;
			int m_totalFaceCount;
			int m_drawCalls;
			
			float m_memoryUsage;
			float m_totalMemory;
			};
		}
	}
}