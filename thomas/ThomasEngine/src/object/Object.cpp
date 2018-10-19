#pragma unmanaged
#include <assert.h>
#include <thomas\object\Object.h>
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
		s_objects.Add(this);
		m_guid = Guid::NewGuid();
		nativePtr->m_guid = Utility::Convert(m_guid);
	}
	Object::~Object()
	{
		Delete();
	}
	void Object::Delete()
	{
		OnDestroy();
		bool rmvd = s_objects.Remove(this);
		//assert(rmvd);
		nativePtr->Destroy();
		delete nativePtr;
	}
	
	void Object::Destroy()
	{
		Delete();
	}

	Object^ Object::Find(Guid guid)
	{
		for each(Object^ o in s_objects)
		{
			if (o->m_guid == guid)
				return o;
		}
		return nullptr;
	}

	List<Object^>^ Object::GetObjects()
	{
		return %s_objects;
	}

	Object^ Object::GetObject(thomas::object::Object* ptr)
	{
		for( int i=0; i < s_objects.Count; i++)
		{
			if (s_objects[i]->nativePtr == ptr)
				return s_objects[i];
		}
		return nullptr;
	}



	List<Object^>^ Object::GetObjectsOfType(Type^ type)
	{
		List<Object^>^ list = gcnew List<Object^>();
		for (int i = 0; i < s_objects.Count; i++) {
			if (s_objects[i]->GetType() == type) {
				if ((Component::typeid)->IsAssignableFrom(type)) {
					ThomasEngine::Component^ component = (Component^)s_objects[i];
					if (component->gameObject->inScene)
						list->Add((Object^)component);
				}
				else {
					list->Add((Object^)s_objects[i]);
				}
			}

		}
		return list;
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