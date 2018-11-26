#pragma once

#include "Texture.h"

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
			virtual ~TextureCube();
		private:

		};
	}
}