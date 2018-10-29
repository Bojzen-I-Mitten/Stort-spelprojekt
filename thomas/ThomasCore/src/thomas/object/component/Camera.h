#pragma once
#include "Component.h"
#include "../../utils/Math.h"
#include "../../resource/texture/Texture2D.h"
#include "../../resource/Font.h"
/**
*Camera class
*/
namespace thomas
{
	namespace utils { struct Ray; }
	namespace graphics { class Skybox; namespace GUI { class Canvas; class GUIElement; } }
	
	namespace object
	{
		namespace component
		{
			class Camera : public Component
			{
			public:
				struct CAMERA_FRAME_DATA
				{
					int targetDisplay;
					math::Viewport viewport;
					math::Matrix projectionMatrix;
					math::Matrix viewMatrix;
					math::Vector4 position;
				};
			private:
				void UpdateProjMatrix();

			public:
				static std::vector<Camera*> s_allCameras;

				Camera();
				Camera(bool dontAddTolist);
				~Camera();

				void OnEnable() override;
				void OnDisable() override;
				void OnDestroy() override;

				math::Matrix GetViewMatrix();
				math::Matrix GetProjMatrix();
				math::Matrix GetViewProjMatrix();
				math::Vector3 GetPosition();
				math::Vector3 GetDirection();
				
				math::Ray ScreenPointToRay(math::Vector2 point);

				float GetFov();
				void SetFov(float fov);

				float GetNear();
				void SetNear(float viewNear);

				float GetFar();
				void SetFar(float viewFar);

				bool GetGUIRendering() const;
				void SetGUIRendering(bool rendering);

				math::Viewport GetViewport();
				void SetViewport(math::Viewport viewport);
				void SetViewport(float x, float y, float width, float height);

				float GetAspectRatio();

				void Render();
				void OnDrawGizmos();
				void OnDrawGizmosSelected();

				void SetTargetDisplay(int index);
				int GetTargetDisplayIndex();

				math::BoundingFrustum GetFrustrum();
				math::BoundingFrustum GetSubFrustrum(math::Rectangle rect);

				void CopyFrameData();
				CAMERA_FRAME_DATA& GetFrameData();
				
				graphics::GUI::Canvas* AddCanvas();
				graphics::GUI::Canvas* AddCanvas(Viewport viewport);

				void RenderGUI();

			private:
				CAMERA_FRAME_DATA m_frameData;
				math::Matrix m_projMatrix;
				float m_fov;
				float m_near;
				float m_far;
				int m_targetDisplay;
				bool m_renderGUI;
				math::Viewport m_viewport;
				math::BoundingFrustum m_frustrum;
				std::vector<std::unique_ptr<graphics::GUI::Canvas>> m_canvases;
			};
		}
	}
}