#pragma once
#pragma unmanaged
#include <thomas\resource\texture\Texture2D.h>
#pragma managed

#include "Texture.h"
namespace ThomasEngine
{
	[DataContractAttribute]
	public ref class Texture2D : public Texture
	{
	public:

	internal:
		Texture2D(String^ path) : Texture(path, new thomas::resource::Texture2D(Utility::ConvertString(path))) {};
		Texture2D(int width, int height, bool mipMap) : Texture("", new thomas::resource::Texture2D(width, height, mipMap)) {};
		Texture2D(thomas::resource::Texture2D* nativePtr) : Texture(Utility::ConvertString(nativePtr->GetPath()), nativePtr) {};
	public:
		virtual ~Texture2D() {}
		static property Texture2D^ blackTexture
		{
			Texture2D^ get() { return gcnew Texture2D(thomas::resource::Texture2D::GetBlackTexture()); }
		}

		static property Texture2D^ whiteTexture
		{
			Texture2D^ get() { return gcnew Texture2D(thomas::resource::Texture2D::GetWhiteTexture()); }
		}

		static property Texture2D^ normalTexture
		{
			Texture2D^ get() { return gcnew Texture2D(thomas::resource::Texture2D::GetNormalTexture()); }
		}

		IntPtr GetRawPixelData()
		{
			return IntPtr(((thomas::resource::Texture2D*)m_nativePtr)->GetRawBGRAPixels()); 
		}

		array<Color>^ GetPixels() 
		{
			array<Color>^ pixels = gcnew array<Color>(width*height);
			std::vector<thomas::math::Color>& nativePixels = ((thomas::resource::Texture2D*)m_nativePtr)->GetPixels();
			for(int i = 0; i < nativePixels.size(); i++)
			{
				pixels[i] = Utility::Convert(nativePixels[i]);
			}
			return pixels;
		}

		[OnDeserializedAttribute]
		void OnDeserialized(StreamingContext c)
		{
			
		}
	};
}
