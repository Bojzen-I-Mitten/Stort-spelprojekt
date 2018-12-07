#pragma once
#include "Component.h"
#include "../../utils/Math.h"
#include "../../graphics/render/FrameData.h"
/**
*Camera class
*/
namespace thomas
{
	namespace utils { struct Ray; }
	namespace graphics { class SkyBox; namespace GUI { class Canvas; struct GUIElement; }}
	namespace resource { class TextureCube; class RenderTexture; }

	namespace object
	{
		namespace component
		{
			class Camera : public Component
			{
			private:
				void UpdateProjMatrix();

			public:
				Camera();
				Camera(int target_display);
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
				math::Vector2 GetViewportScale();
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
				const graphics::render::CAMERA_FRAME_DATA& GetFrameData();

				uint32_t ID();

				graphics::GUI::Canvas* AddCanvas();
				graphics::GUI::Canvas* AddCanvas(math::Viewport viewport);

				void RenderGUI();

				void SetSkyMap(resource::TextureCube* tex);
				resource::TextureCube* GetSkyMap();
				void DrawSkyBox();
				bool hasSkybox();
				math::Vector3 WorldToViewport(math::Vector3 position, math::Matrix world);

				void SetRenderTexture(resource::RenderTexture* texture);
				resource::RenderTexture* GetRenderTexture();

			private:
				uint32_t m_ID;
				graphics::render::CAMERA_FRAME_DATA m_frameData;
				math::Matrix m_projMatrix;
				float m_fov;
				float m_near;
				float m_far;
				int m_targetDisplay;
				bool m_renderGUI;
				math::Viewport m_viewport;
				math::BoundingFrustum m_frustrum;
				std::vector<std::unique_ptr<graphics::GUI::Canvas>> m_canvases;
				
				resource::RenderTexture* m_renderTexture = nullptr;
				std::unique_ptr<graphics::SkyBox> m_skybox;
			};
		}
	}
}