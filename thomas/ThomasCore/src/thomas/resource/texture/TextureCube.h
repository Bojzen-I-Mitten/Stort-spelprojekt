#pragma once

#include "Texture2D.h"

namespace thomas
{
	namespace resource
	{
		class TextureCube : public Texture
		{
		private:
			void LoadTextureCubeFromFile(std::string path);
		public:
			TextureCube(std::string path);
			TextureCube(Texture2D* tex);
		private:

		};
	}
}