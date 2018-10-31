#pragma once


#include <vector>
#include "../../utils/atomic/SyncronizedList.h"


namespace thomas
{
	namespace object
	{
		namespace component
		{
			class Camera;
		}
	}
	namespace graphics
	{
		namespace render
		{

			class CameraList
			{
			public:
				CameraList();
				~CameraList();

				object::component::Camera* getCamera(uint32_t cameraID);

				uint32_t add(object::component::Camera* c) const;
				void remove(object::component::Camera* c) const;
				void syncUpdate();
				const utils::atomics::SynchronizedList<object::component::Camera*>& getCameras() const;
			private:
				mutable std::atomic<uint32_t> m_idCounter;
				mutable utils::atomics::SynchronizedList<object::component::Camera*> m_sysCamera;
			};
		}
	}
}