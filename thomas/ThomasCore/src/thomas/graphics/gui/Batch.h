#pragma once

// DirectXTK
#include <DirectXTK/SpriteBatch.h>

using namespace DirectX;

namespace thomas
{
	class Batch
	{
	public:
		Batch();
		~Batch();

	public:
		void BeginBatch();
		void EndBatch();

	private:
		std::unique_ptr<SpriteBatch> m_spriteBatch;
	};
}