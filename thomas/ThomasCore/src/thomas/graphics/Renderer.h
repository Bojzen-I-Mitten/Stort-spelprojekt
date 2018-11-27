#pragma once
#include "..\utils\Math.h"
#include <vector>
#include <map>
#include <memory>
#include "render/ShaderList.h"
#include "render/CameraList.h"
#include "render/FrameData.h"


namespace thomas 
{
	class Scene;
	namespace object
	{
		class GameObject;
		namespace component
		{
			class Camera;
			class Transform;
			class RenderComponent;
		}
	}

	namespace resource
	{
		class Material;
		namespace shaderproperty {
			class ShaderProperty;
		}
	}

	namespace graphics
	{				
		class Mesh;

		namespace render {
			class Frame;
			struct RenderCommand;
		}
		
		class Renderer 
		{
		private:
			Renderer();
			~Renderer();

			void BindFrame();
			void BindObject(render::RenderCommand & rC);
			bool BindCameraRenderTarget(const render::CAMERA_FRAME_DATA & frameData);
			bool BindCameraBackBuffer(const render::CAMERA_FRAME_DATA& frameData);

		public:
			void init();
			void PostRender();
			void Destroy();

			/* Renderer Singleton */
			static Renderer* Instance();
			void ProcessCommands();
			/* Clear front buffer. */
			void ClearCommands();
			/* Clear both front, and back buffer. */
			void ClearAllCommands();
			void SubmitCamera(object::component::Camera * cam);
			void SubmitCommand(render::RenderCommand& command);
			render::Frame& getAllocator();

			void TransferCommandList();

		public:
			/* Access shaders 
			*/
			const render::ShaderList& getShaderList();
			const render::CameraList& getCameraList();
			resource::Shader* GetStandardShader();

		private:
			std::unique_ptr<render::Frame> m_frame;
			std::unique_ptr<render::Frame> m_prevFrame;
			render::ShaderList m_shaders;
			render::CameraList m_cameras;
			bool m_enableShadows;
		};
	}
}