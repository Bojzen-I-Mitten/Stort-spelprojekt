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
				: m_frameQuery(0), m_frameCollect(-1), m_frameCountAvg(0), m_beginAvg(0.0f),
				m_drawCalls(0), m_totalVertexCount(0), m_totalFaceCount(0),
				m_memoryUsage(0.0f), m_active(false),
				m_currentFrame(0), m_maxFrames(5)
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
					LOG("Could not create timestamp disjoint query for frame 0!");
					return false;
				}

				if (!device->CreateQuery(D3D11_QUERY_TIMESTAMP_DISJOINT, m_queryDisjoint[1]))
				{
					LOG("Could not create timestamp disjoint query for frame 1!");
					return false;
				}

				for (GTS gts = GTS_BEGIN_FRAME; gts < GTS_MAX; gts = GTS(gts + 1))
				{
					if (!device->CreateQuery(D3D11_QUERY_TIMESTAMP, m_queryTimestamp[gts][0]))
					{
						LOG("Could not create start-frame timestamp query for GTS " << gts << ", frame 0!");
						return false;
					}

					if (!device->CreateQuery(D3D11_QUERY_TIMESTAMP, m_queryTimestamp[gts][1]))
					{
						LOG("Could not create start-frame timestamp query for GTS " << gts << ", frame 1!");
						return false;
					}
				}

				IDXGIDevice* dxgiDevice = nullptr;
				HRESULT hr = device->GetDevice()->QueryInterface(__uuidof(IDXGIDevice), (void **)& dxgiDevice);
				if (FAILED(hr))
				{
					LOG("Could not query DXGIDevice");
					return false;
				}
				IDXGIAdapter* dxgiAdapter = nullptr;
				hr = dxgiDevice->GetAdapter(&dxgiAdapter);
				if (FAILED(hr))
				{

					SAFE_RELEASE(dxgiDevice);
					LOG("Could not query DXGIAdapter");
					return false;
				}
				hr = dxgiAdapter->QueryInterface(__uuidof(IDXGIAdapter4), (void**)&m_dxgiAdapter4);
				if (FAILED(hr))
				{
					SAFE_RELEASE(dxgiDevice);
					SAFE_RELEASE(dxgiAdapter);
					LOG("Could not query DXGIAdapter4");
					return false;
				}
				SAFE_RELEASE(dxgiDevice);
				SAFE_RELEASE(dxgiAdapter);

				return true;
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
				m_currentFrame = (m_currentFrame + 1) % 5;
				if (m_currentFrame > 0)
					return;

				m_drawCalls = 0;
				m_totalVertexCount = 0;
				m_totalFaceCount = 0;
				if (!m_active)
					return;
				utils::D3D::Instance()->GetDeviceContext()->Begin(m_queryDisjoint[m_frameQuery]);
				Timestamp(GTS_BEGIN_FRAME);
			}

			void GpuProfiler::Timestamp(GTS gts)
			{
				if (m_currentFrame > 0 || !m_active)
					return;
				utils::D3D::Instance()->GetDeviceContext()->End(m_queryTimestamp[gts][m_frameQuery]);
			}

			void GpuProfiler::EndFrame()
			{
				if (m_currentFrame > 0 || !m_active)
					return;
				Timestamp(GTS_END_FRAME);
				utils::D3D::Instance()->GetDeviceContext()->End(m_queryDisjoint[m_frameQuery]);
				++m_frameQuery &= 1; //Fancy 0/1 toggle.
			}

			void profiling::GpuProfiler::AddDrawCall(size_t faceCount, size_t vertexCount)
			{
				if (m_currentFrame > 0)
					return;
				m_totalVertexCount += vertexCount;
				m_totalFaceCount += faceCount;
				m_drawCalls++;
			}

			void GpuProfiler::WaitForDataAndUpdate()
			{
				if (m_currentFrame || !m_active)
					return;
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
				HRESULT HR = context->GetData(m_queryDisjoint[iFrame], &timestampDisjoint, sizeof(timestampDisjoint), 0);
				if (HR != S_OK)
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
				if (context->GetData(m_queryTimestamp[GTS_BEGIN_FRAME][iFrame], &timestampPrev, sizeof(UINT64), 0) != S_OK)
				{
					LOG("Couldn't retrieve timestamp query data for GTS " << GTS_BEGIN_FRAME);
					return;
				}

				for (GTS gts = GTS(GTS_BEGIN_FRAME + 1); gts < GTS_MAX; gts = GTS(gts + 1))
				{
					UINT64 timestamp;
					if (context->GetData(m_queryTimestamp[gts][iFrame], &timestamp, sizeof(UINT64), 0) != S_OK)
					{
						LOG("Couldn't retrieve timestamp query data for GTS " << gts);
						return;
					}

					m_timings[gts] = float(timestamp - timestampPrev) / float(timestampDisjoint.Frequency);
					timestampPrev = timestamp;

					m_avgTimingsTotal[gts] += m_timings[gts];
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

				//Update VRAM
				DXGI_QUERY_VIDEO_MEMORY_INFO info;

				if (SUCCEEDED(m_dxgiAdapter4->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info)))
				{
					m_memoryUsage = float(info.CurrentUsage / 1024.0 / 1024.0); //MiB
					m_totalMemory = float(info.Budget / 1024.0 / 1024.0);
				};
			}

			void profiling::GpuProfiler::SetActive(bool value)
			{
				m_active = value;
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

			size_t profiling::GpuProfiler::GetVertexCount()
			{
				return m_totalVertexCount;
			}

			size_t profiling::GpuProfiler::GetFaceCount()
			{
				return m_totalFaceCount;
			}
		}
	}
}
