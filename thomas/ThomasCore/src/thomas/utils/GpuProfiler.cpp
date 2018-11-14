#include "GpuProfiler.h"
#include "../Common.h"
#include "../ThomasTime.h"
#include "..\ThomasCore.h"
namespace thomas
{
	namespace utils
	{
		namespace profiling
		{
			GpuProfiler::GpuProfiler()
				: m_frameQuery(0), m_frameCollect(-1), m_frameCountAvg(0),
				m_beginAvg(0.0f), m_drawCalls(0), m_totalVertexCount(0), m_memoryUsage(0.0f)
			{
				memset(m_queryDisjoint, 0, sizeof(m_queryDisjoint));
				memset(m_queryTimestamp, 0, sizeof(m_queryTimestamp));
				memset(m_timings, 0, sizeof(m_timings));
				memset(m_avgTimings, 0, sizeof(m_avgTimings));
				memset(m_avgTimings, 0, sizeof(m_avgTimings));
			}
			bool GpuProfiler::Init()
			{
				// Create all the queries we'll need
				utils::D3D* device = utils::D3D::Instance();

				if (!device->CreateQuery(D3D11_QUERY_TIMESTAMP_DISJOINT, m_queryDisjoint[0]))
				{
					return false;
				}

				if (!device->CreateQuery(D3D11_QUERY_TIMESTAMP_DISJOINT, m_queryDisjoint[1]))
				{
					return false;
				}

				for (GTS gts = GTS_BEGIN_FRAME; gts < GTS_MAX; gts = GTS(gts + 1))
				{
					if (!device->CreateQuery(D3D11_QUERY_TIMESTAMP, m_queryTimestamp[gts][0]))
					{
						return false;
					}

					if (!device->CreateQuery(D3D11_QUERY_TIMESTAMP, m_queryTimestamp[gts][1]))
					{
						return false;
					}
				}

				IDXGIDevice* dxgiDevice = nullptr;
				HRESULT hr = device->GetDevice()->QueryInterface(__uuidof(IDXGIDevice), (void **)& dxgiDevice);
				if (SUCCEEDED(hr))
				{
					IDXGIAdapter* dxgiAdapter = nullptr;
					hr = dxgiDevice->GetAdapter(&dxgiAdapter);
					hr = dxgiAdapter->QueryInterface(__uuidof(IDXGIAdapter4), (void**)&m_dxgiAdapter4);
					if (SUCCEEDED(hr))
					{
						//Use this for setting memory cap of vram at 512 mega bytes
						//m_dxgiAdapter4->SetVideoMemoryReservation(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, 512000000);
						
						DXGI_QUERY_VIDEO_MEMORY_INFO info;
						if (SUCCEEDED(m_dxgiAdapter4->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info)))
						{
							m_memoryUsage = float(info.CurrentUsage * 0.001f * 0.001f);
							m_totalMemory = float(info.Budget * 0.001f * 0.001f);
						};

						SAFE_RELEASE(dxgiDevice);
						SAFE_RELEASE(dxgiAdapter);
						return true;
					}

					SAFE_RELEASE(dxgiAdapter);
				}

				SAFE_RELEASE(dxgiDevice);

				return false;
			}

			void GpuProfiler::Destroy()
			{
				SAFE_RELEASE(m_dxgiAdapter4);
				SAFE_RELEASE(m_queryDisjoint[0]);
				SAFE_RELEASE(m_queryDisjoint[1])


				for (GTS gts = GTS_BEGIN_FRAME; gts < GTS_MAX; gts = GTS(gts + 1))
				{
					SAFE_RELEASE(m_queryTimestamp[gts][0]);
					SAFE_RELEASE(m_queryTimestamp[gts][1])
				}
			}

			void GpuProfiler::BeginFrame()
			{
				m_drawCalls = 0;
				m_totalVertexCount = 0;
				utils::D3D::Instance()->GetDeviceContext()->Begin(m_queryDisjoint[m_frameQuery]);
				Timestamp(GTS_BEGIN_FRAME);
			}

			void GpuProfiler::Timestamp(GTS gts)
			{

				UINT64 timestamp;
				HRESULT hr = utils::D3D::Instance()->GetDeviceContext()->GetData(m_queryTimestamp[gts][m_frameQuery], &timestamp, sizeof(UINT64), 0);
				if (hr == S_FALSE)
					return;

				utils::D3D::Instance()->GetDeviceContext()->End(m_queryTimestamp[gts][m_frameQuery]);
			}

			void GpuProfiler::EndFrame()
			{
				Timestamp(GTS_END_FRAME);
				utils::D3D::Instance()->GetDeviceContext()->End(m_queryDisjoint[m_frameQuery]);
				++m_frameQuery &= 1; //Fancy 0/1 toggle.
			}

			void profiling::GpuProfiler::AddDrawCall(int vertexCount)
			{
				m_totalVertexCount += vertexCount;
				m_drawCalls++;
			}

			void GpuProfiler::WaitForDataAndUpdate()
			{
		
				ID3D11DeviceContext* context = utils::D3D::Instance()->GetDeviceContext();
				if (m_frameCollect < 0)
				{
					// Haven't run enough frames yet to have data
					m_frameCollect = 0;
					return;
				}

				// Wait for data
				while (context->GetData(m_queryDisjoint[m_frameCollect], NULL, 0, 0) == S_FALSE)
				{
					Sleep(1);
				}

				int iFrame = m_frameCollect;
				++m_frameCollect &= 1;

				D3D11_QUERY_DATA_TIMESTAMP_DISJOINT timestampDisjoint;
				HRESULT hr = context->GetData(m_queryDisjoint[iFrame], &timestampDisjoint, sizeof(timestampDisjoint), 0);
				if (hr == S_FALSE)
				{
					//LOG("Couldn't retrieve timestamp disjoint query data");
					return;
				}

				if (timestampDisjoint.Disjoint)
				{
					// Throw out this frame's data
					LOG("Timestamps disjoint");
					return;
				}

				UINT64 timestampPrev;
				hr = context->GetData(m_queryTimestamp[GTS_BEGIN_FRAME][iFrame], &timestampPrev, sizeof(UINT64), 0);
				if (hr == S_FALSE)
				{
					return;
				}

				for (GTS gts = GTS(GTS_BEGIN_FRAME + 1); gts < GTS_MAX; gts = GTS(gts + 1))
				{
					UINT64 timestamp;
					HRESULT hr = context->GetData(m_queryTimestamp[gts][iFrame], &timestamp, sizeof(UINT64), 0);
					if (hr == S_OK)
					{
						m_timings[gts] = float(timestamp - timestampPrev) / float(timestampDisjoint.Frequency);
						timestampPrev = timestamp;

						m_avgTimingsTotal[gts] += m_timings[gts];
					}
				}

				++m_frameCountAvg;
				if (ThomasTime::GetElapsedTime() > m_beginAvg + 0.5f)
				{
					for (GTS gts = GTS_BEGIN_FRAME; gts < GTS_MAX; gts = GTS(gts + 1))
					{
						m_avgTimings[gts] = m_avgTimingsTotal[gts] / m_frameCountAvg;
						m_avgTimingsTotal[gts] = 0.0f;
					}

					m_frameCountAvg = 0;
					m_beginAvg = ThomasTime::GetElapsedTime();
				}
			}

			float profiling::GpuProfiler::GetAverageTiming(GTS gts)
			{
				return m_avgTimings[gts];
			}

			float profiling::GpuProfiler::GetDrawTotal()
			{
				float drawTotal = 0.0f;
				for (GTS gts = GTS_BEGIN_FRAME; gts < GTS_END_FRAME; gts = GTS(gts + 1))
					drawTotal += GetAverageTiming(gts);
				return drawTotal;
			}

			float profiling::GpuProfiler::GetFrameTime()
			{
				return GetDrawTotal() + GetAverageTiming(GTS_END_FRAME);
			}

			float profiling::GpuProfiler::GetMemoryUsage()
			{
				DXGI_QUERY_VIDEO_MEMORY_INFO info;
				if (SUCCEEDED(m_dxgiAdapter4->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info)))
					m_memoryUsage = float(info.CurrentUsage * 0.001f * 0.001f);

				return m_memoryUsage;
			}

			float profiling::GpuProfiler::GetTotalMemory()
			{
				return m_totalMemory;
			}

			int profiling::GpuProfiler::GetNumberOfDrawCalls()
			{
				return m_drawCalls;
			}

			int profiling::GpuProfiler::GetVertexCount()
			{
				return m_totalVertexCount;
			}

		}
	}
}
