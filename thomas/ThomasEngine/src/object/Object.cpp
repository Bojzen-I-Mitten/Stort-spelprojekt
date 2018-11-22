#pragma unmanaged
#include <assert.h>
#include <thomas\object\Object.h>
#include <thomas\object\GameObject.h>
#pragma managed
#include "../Utility.h"
#include "Object.h"
#include "GameObject.h"
#include "Component.h"

namespace ThomasEngine
{
	Object::Object(thomas::object::Object* ptr)
	{
		nativePtr = ptr;
		m_guid = Guid::NewGuid();
		nativePtr->m_guid = Utility::Convert(m_guid);
	}
	Object::~Object()
	{
		Delete();
	}
	void Object::Delete()
	{
#ifdef _DEBUG
		assert(!m_Destroyed);
		m_Destroyed = true;
#endif
		nativePtr->Destroy();
	
		if(this->GetType() != GameObject::typeid)
			delete nativePtr;
	}
	
	void Object::Destroy()
	{
		Delete();
	}




	bool Object::operator ==(Object^ a, Object^ b)
	{
		if (Object::ReferenceEquals(nullptr, a))
			return Object::ReferenceEquals(nullptr, b);

		if (Object::ReferenceEquals(nullptr, b))
			return false;

		return a->nativePtr == b->nativePtr;
	}

	bool Object::operator !=(Object^ a, Object^ b)
	{

		if (Object::ReferenceEquals(nullptr, a))
			return !Object::ReferenceEquals(nullptr, b);

		if (Object::ReferenceEquals(nullptr, b))
			return true;

		return a->nativePtr != b->nativePtr;

	}

	Object::operator bool(Object ^ object)
	{
		return object != nullptr;
	}

	void Object::OnDeserializedObject(System::Runtime::Serialization::StreamingContext c)
	{
		nativePtr->m_guid = Utility::Convert(m_guid);
	}
}