#pragma once
#include "..\utils\Math.h"
#include "LightManager.h"
#include <vector>
#include <map>
#include <memory>

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
			static LightManager s_lightManager;
			void BindFrame();
			void BindObject(render::RenderCommand & rC);

		public:
			Renderer();

			void BindCamera(thomas::object::component::Camera* camera);
			void ProcessCommands();
			void ClearCommands();
			void SubmitCommand(render::RenderCommand& command);
			void TransferCommandList();

		public:
			static Renderer s_renderer;

		private:
			std::unique_ptr<render::Frame> m_frame;
			std::unique_ptr<render::Frame> m_prevFrame;
		};
	}
}