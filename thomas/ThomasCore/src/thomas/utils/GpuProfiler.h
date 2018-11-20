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
				GTS_MAIN_CLEAR,
				GTS_MAIN_OBJECTS,
				GTS_PARTICLES,
				GTS_GIZMO_OBJECTS,
				GTS_GUI,
				GTS_SHADOWS,
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
				void WaitForDataAndUpdate();

			public:
				float GetAverageTiming(GTS gts);
				float GetDrawTotal();
				float GetFrameTime();
				float GetMemoryUsage();
				float GetTotalMemory();
				int GetNumberOfDrawCalls();
				int GetVertexCount();


			private:
				int m_frameQuery;								// Which of the two sets of queries are we currently issuing?
				int m_frameCollect;								// Which of the two did we last collect?
				ID3D11Query * m_queryDisjoint[2];				// "Timestamp disjoint" query; records whether timestamps are valid
				ID3D11Query * m_queryTimestamp[GTS_MAX][2];		// Individual timestamp queries for each relevant point in the frame

				IDXGIAdapter4* m_dxgiAdapter4 = NULL;			// Adapter for checking VRAM

				float m_timings[GTS_MAX];						// Last frame's timings (each relative to previous GTS)
				float m_avgTimings[GTS_MAX];					// Timings averaged over 0.5 second

				float m_avgTimingsTotal[GTS_MAX];				// Total timings thus far within this averaging period
				int m_frameCountAvg;							// Frames rendered in current averaging period
				float m_beginAvg;								// Time at which current averaging period started

				int m_totalVertexCount;
				int m_drawCalls;

				float m_memoryUsage;
				float m_totalMemory;
			};
		}
	}
}