#pragma once
#include "Component.h"
#include "../../utils/Math.h"
/**
*Camera class
*/
namespace thomas
{
	namespace utils { struct Ray; }
	namespace graphics { class Skybox; }
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
			private:
				CAMERA_FRAME_DATA m_frameData;
				math::Matrix m_projMatrix;
				float m_fov;
				float m_near;
				float m_far;
				int m_targetDisplay;
				math::Viewport m_viewport;
				math::BoundingFrustum m_frustrum;

			};
		}
	}
}