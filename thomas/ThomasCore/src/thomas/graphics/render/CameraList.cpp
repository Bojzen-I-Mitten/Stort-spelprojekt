#include "CameraList.h"
#include <algorithm>
#include "../../object/component/Camera.h"


namespace thomas
{
	namespace graphics
	{
		namespace render
		{


			CameraList::CameraList() :
				m_sysCamera(),
				m_idCounter(1)
			{
			}

			CameraList::~CameraList()
			{
			}

			object::component::Camera * CameraList::getCamera(uint32_t cameraID)
			{
				struct IDComp
				{
					uint32_t eval_id;
					IDComp(uint32_t id) : eval_id(id){}
					bool operator()(object::component::Camera* c)
					{
						return c->ID() == eval_id;
					}
				};
				object::component::Camera * cam;
				if (m_sysCamera.find<IDComp>(IDComp(cameraID), cam))
					return cam;
				return nullptr;
			}
			uint32_t CameraList::add(object::component::Camera * c) const
			{
				m_sysCamera.add(c);
				return m_idCounter++;
			}
			void CameraList::remove(object::component::Camera * c) const
			{
				m_sysCamera.rmv(c);
			}
			void CameraList::syncUpdate()
			{
				size_t edits = m_sysCamera.sync();
				if (edits)
				{
					std::sort(m_sysCamera.begin(), m_sysCamera.end(), [](object::component::Camera* a, object::component::Camera* b)
					{
						return a->GetTargetDisplayIndex() < b->GetTargetDisplayIndex();
					});
				}
			}
			const util::atomics::SynchronizedList<object::component::Camera*>& CameraList::getCameras() const
			{
				return m_sysCamera;
			}
		}
	}
}
