#include "GpuProfiler.h"
#include "..\Common.h"
#include "..\ThomasTime.h"
#include "..\ThomasCore.h"

namespace thomas
{
	namespace utils
	{
		namespace profiling
		{
			GpuProfiler* GpuProfiler::Instance()
			{
				static GpuProfiler s_gpuProfiler;
				return &s_gpuProfiler;
			}

			bool GpuProfiler::Init()
			{
				m_frameQuery = 0;
				m_memoryQuery = 0;
				m_drawCalls = 0;
				m_totalVertexCount = 0;
				m_totalFaceCount = 0;
				m_memoryUsage = 0.0f;

				memset(m_queryDisjoint, 0, sizeof(m_queryDisjoint));
				memset(m_queryTimestamp, 0, sizeof(m_queryTimestamp));
				memset(m_timings, 0, sizeof(m_timings));

				for (int i = 0; i < FRAME_BUFFERS; i++)
				{
					if (!utils::D3D::Instance()->CreateQuery(D3D11_QUERY_TIMESTAMP_DISJOINT, m_queryDisjoint[i]))
					{
						return false;
					}

					for (GTS gts = GTS_BEGIN_FRAME; gts < GTS_MAX; gts = GTS(gts + 1))
					{
						if (!utils::D3D::Instance()->CreateQuery(D3D11_QUERY_TIMESTAMP, m_queryTimestamp[gts][i]))
						{
							return false;
						}
					}
				}
				
				//Use this for setting memory cap of vram at 512 mega bytes
				//utils::D3D::Instance()->GetDxgiAdapter()->SetVideoMemoryReservation(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, 512000000);
				DXGI_QUERY_VIDEO_MEMORY_INFO info;
				if (SUCCEEDED(utils::D3D::Instance()->GetDxgiAdapter()->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info)))
				{
					m_memoryUsage = float(info.CurrentUsage * 0.001f * 0.001f);
					m_totalMemory = 0.0f;
				};

				return true;
			}

			void GpuProfiler::Destroy()
			{
				for (int i = 0; i < FRAME_BUFFERS; i++)
				{
					SAFE_RELEASE(m_queryDisjoint[i]);

					for (GTS gts = GTS_BEGIN_FRAME; gts < GTS_MAX; gts = GTS(gts + 1))
					{
						SAFE_RELEASE(m_queryTimestamp[gts][i]);
					}
				}
			}

			void GpuProfiler::BeginFrame()
			{
				m_drawCalls = 0;
				m_totalVertexCount = 0;
				m_totalFaceCount = 0;
				utils::D3D::Instance()->GetDeviceContextDeffered()->Begin(m_queryDisjoint[m_frameQuery]);
				Timestamp(GTS_BEGIN_FRAME);
			}

			void GpuProfiler::Timestamp(GTS gts)
			{
				utils::D3D::Instance()->GetDeviceContextDeffered()->End(m_queryTimestamp[gts][m_frameQuery]);
			}

			void GpuProfiler::EndFrame()
			{
				Timestamp(GTS_END_FRAME);
				utils::D3D::Instance()->GetDeviceContextDeffered()->End(m_queryDisjoint[m_frameQuery]);
				m_frameQuery = (m_frameQuery + 1) % FRAME_BUFFERS; 
			}

			void profiling::GpuProfiler::AddDrawCall(size_t faceCount, size_t vertexCount)
			{
				m_totalVertexCount += vertexCount;
				m_totalFaceCount += faceCount;
				m_drawCalls++;
			}

			void GpuProfiler::RetriveTimeStamps()
			{
				ID3D11DeviceContext* context = utils::D3D::Instance()->GetDeviceContextImmediate();
				D3D11_QUERY_DATA_TIMESTAMP_DISJOINT timestampDisjoint;
				HRESULT hr = context->GetData(m_queryDisjoint[m_frameQuery], &timestampDisjoint, sizeof(timestampDisjoint), 0);
				if ( hr == S_FALSE)
				{
					LOG("Couldn't retrieve timestamp disjoint query data");
				}

				if (timestampDisjoint.Disjoint)
				{
					// Throw out this frame's data
					LOG("Timestamps disjoint");
				}

				UINT64 beginStamp;
				if (context->GetData(m_queryTimestamp[GTS_BEGIN_FRAME][m_frameQuery], &beginStamp, sizeof(UINT64), 0) == S_FALSE)
				{
					LOG("Couldn't retrieve timestamp query data for GTS " << GTS_BEGIN_FRAME);
				}

				for (GTS gts = GTS(GTS_BEGIN_FRAME + 1); gts < GTS_MAX; gts = GTS(gts + 1))
				{
					UINT64 timestamp;
					if (context->GetData(m_queryTimestamp[gts][m_frameQuery], &timestamp, sizeof(UINT64), 0) == S_FALSE)
					{
						LOG("Couldn't retrieve timestamp query data for GTS " << gts);
					}
					else
					{
						m_timings[gts] = float(timestamp - beginStamp) / float(timestampDisjoint.Frequency);
					}
				}
			}

			float profiling::GpuProfiler::GetTimeStamp(GTS gts)
			{
				float timeStamp = m_timings[gts] - m_timings[gts - 1];
				return timeStamp;
			}

			int GpuProfiler::GetNumOfDrawCalls()
			{
				return m_drawCalls;
			}

			float profiling::GpuProfiler::GetFrameTime()
			{
				return m_timings[GTS::GTS_END_FRAME];
			}

			float profiling::GpuProfiler::GetMemoryUsage()
			{
				if (SUCCEEDED(utils::D3D::Instance()->GetDxgiAdapter()->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &m_info[m_memoryQuery])))
				{
					m_memoryUsage = float(m_info[m_memoryQuery].CurrentUsage);

					m_memoryQuery = (m_memoryQuery + 1) % 2;

					float usage = float(m_info[m_memoryQuery].CurrentUsage);
					if (m_memoryUsage > usage)
					{
						m_totalMemory += (m_memoryUsage - usage) * 0.001f * 0.001f;
						return (m_memoryUsage - usage) * 0.001f * 0.001f;
					}
				}

				return 0.0f;
			}

			float profiling::GpuProfiler::GetTotalMemory()
			{
				return m_totalMemory;
			}

			int profiling::GpuProfiler::GetNumberOfDrawCalls()
			{
				return (int)m_drawCalls;
			}

			int profiling::GpuProfiler::GetVertexCount()
			{
				return m_totalVertexCount;
			}

			int profiling::GpuProfiler::GetFaceCount()
			{
				return m_totalFaceCount;
			}
		}
	}
}
