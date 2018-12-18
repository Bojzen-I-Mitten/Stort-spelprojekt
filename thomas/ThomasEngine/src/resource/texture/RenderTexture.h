#pragma once
#pragma unmanaged
#include <thomas\resource\texture\RenderTexture.h>
#pragma managed
#include "Texture2D.h"
#include "../../ThomasManaged.h"
#include "../../GUI/game/Canvas.h"
namespace ThomasEngine
{
	[DataContractAttribute]
	public ref class RenderTexture : public Texture2D
	{
	public:
		RenderTexture(int width, int height) : Texture2D(new thomas::resource::RenderTexture(width, height)) {};
		virtual ~RenderTexture() {}
	internal:
		RenderTexture(thomas::resource::RenderTexture* nativePtr) : Texture2D(nativePtr) {};
	public:
		
		void WriteCanvas(Canvas^ canvas)
		{
			ThomasWrapper::WaitForRender();
			((thomas::resource::RenderTexture*)m_nativePtr)->WriteCanvas(canvas->nativePtr);
		}
	};
}
