/*
	Assembly class of the engine (creates, updates and destroys objects).
*/

#pragma once
#include <d3d11_4.h>
#include <vector>
#include <string>
#include <imgui\imgui.h>
#include <dxgidebug.h>
#include <tchar.h>

namespace thomas
{
	class ThomasCore
	{
	public:	
		static bool Init();
		static bool Initialized();
		static void Update();
		static bool Destroy();
		static void Exit();

	public:
		static ID3D11Device* GetDevice();
		static ID3D11DeviceContext* GetDeviceContext();
		static std::vector<std::string> GetLogOutput();

	public:
		static void LogOutput(const std::string & message);
		static void ClearLogOutput();

	public:
		template<UINT TNameLength>
		static void SetDebugObjectName(ID3D11DeviceChild* ComObject, const char(&name)[TNameLength]);
		template<UINT TNameLength>
		static void SetDebugObjectName(IDXGIObject* ComObject, const char(&name)[TNameLength]);
		template<UINT TNameLength>
		static void SetDebugObjectName(ID3D11Resource* ComObject, const char(&name)[TNameLength]);

	private:
		static bool InitDirectX();
		static bool CreateDeviceAndContext();
		static void CreateDebugInterface();

	private:
		static bool s_initialized;
		static std::vector<std::string> s_logOutput;
		static bool s_clearLog;

	private:
		static ID3D11Device* s_device;
		static ID3D11DeviceContext* s_context;
		static IDXGIDebug* s_debug;
		static IDXGIInfoQueue* s_infoQueue;
		static ImGuiContext* s_imGuiContext;
	};

	template<UINT TNameLength>
	inline void ThomasCore::SetDebugObjectName(ID3D11DeviceChild* ComObject, const char(&name)[TNameLength])
	{
#if defined(_DEBUG_DX)
		ComObject->SetPrivateData(WKPDID_D3DDebugObjectName, TNameLength - 1, name);
#endif
	}
	template<UINT TNameLength>
	inline void ThomasCore::SetDebugObjectName(IDXGIObject* ComObject, const char(&name)[TNameLength])
	{
#if defined(_DEBUG_DX)
		ComObject->SetPrivateData(WKPDID_D3DDebugObjectName, TNameLength - 1, name);
#endif
	}
	template<UINT TNameLength>
	inline void ThomasCore::SetDebugObjectName(ID3D11Resource* ComObject, const char(&name)[TNameLength])
	{
#if defined(_DEBUG_DX)
		ComObject->SetPrivateData(WKPDID_D3DDebugObjectName, TNameLength - 1, name);
#endif
	}
}