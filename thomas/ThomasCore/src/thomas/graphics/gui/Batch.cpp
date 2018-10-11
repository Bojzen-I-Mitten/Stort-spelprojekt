#include "Batch.h"

// Thomas
#include "../../utils/d3d.h"

namespace thomas
{
	Batch::Batch()
	{
		m_spriteBatch = std::make_unique<SpriteBatch>(utils::D3D::Instance()->GetDeviceContext());
	}

	Batch::~Batch()
	{
		m_spriteBatch.reset();
	}

	void Batch::BeginBatch()
	{
		// TODO: Add different modes for begin
		m_spriteBatch->Begin();
	}

	void Batch::EndBatch()
	{
		m_spriteBatch->End();
	}
}