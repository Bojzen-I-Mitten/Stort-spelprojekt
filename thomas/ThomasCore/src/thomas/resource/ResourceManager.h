#pragma once

namespace DirectX
{
	class SpriteBatch;
	class CommonStates;
}

namespace thomas
{
	namespace resource
	{
		class Texture2D;
		class TextureCube;
		class Font;
		/* Sad state of a resource manager. Contains only default parameters in core. One day it can hopefully be something more ;<
		*/
		class ResourceManager
		{
		/* Default parameters
		*/
		public:

			static void Init();
			static void Destroy();


			static Texture2D* GetBlackTexture();
			static Texture2D* GetWhiteTexture();
			static Texture2D* GetNormalTexture();
			static TextureCube* GetCubeDefault();
			static DirectX::CommonStates* GetSpriteStates();
			static DirectX::SpriteBatch* GetSpriteBatch();
			static Font* GetDefaultFont();

		private:
			static Texture2D* s_blackTexture;
			static Texture2D* s_whiteTexture;
			static Texture2D* s_normalTexture;
			static TextureCube* s_cubeMap;
			static DirectX::CommonStates* s_spriteStates;
			static DirectX::SpriteBatch* s_spriteBatch;
			static Font* s_defaultFont;
		};

	}
}