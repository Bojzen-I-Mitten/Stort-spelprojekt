#pragma unmanaged
#include <thomas/graphics/GUI/GUIManager.h>
#pragma managed
#include "GUIManager.h"
#include "GUIElements.h"
#include "../../Utility.h"

void ThomasEngine::GUIManager::AddImage(Texture2D ^ texture)
{
	thomas::graphics::GUI::GUIElement* element =
		nativePtr->AddGUIElement((thomas::resource::Texture2D*)texture->m_nativePtr);

	GUIElements.Add(gcnew Image(element));
}

void ThomasEngine::GUIManager::AddText(String ^ text)
{
	thomas::graphics::GUI::GUIElement* newText = 
		nativePtr->AddGUIElement(Utility::ConvertString(text));

	GUIElements.Add(gcnew Text(newText));
}

void ThomasEngine::GUIManager::DeleteGUIElement(GUIElement ^ element)
{
	nativePtr->DeleteGUIElement(element->nativePtr);
	GUIElements.Remove(element);
}
