#pragma once
#include "..\utils\Math.h"
#include <vector>
#include <map>
#include <memory>
#include "InstanceRenderer.h"

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

	namespace graphics
	{				
		namespace render 
		{
			class Frame;
			struct RenderCommand;
		}
		
		class Renderer 
		{
		private:
			Renderer();

			void BindFrame();
			void BindObject(render::RenderCommand & rC, math::Matrix* value);
			void BindObject(render::RenderCommand & rC, int count, math::Matrix* matrix, math::Matrix* inverseMatrix);
			void BindObject(render::RenderCommand & rC, int count, math::Matrix* matrix);

		public:
			~Renderer() = default;

			static Renderer* Instance();
			void BindCamera(thomas::object::component::Camera* camera);
			void ProcessCommands();
			/* Clear front buffer. */
			void ClearCommands();
			/* Clear both front, and back buffer. */
			void ClearAllCommands();
			void SubmitCommand(render::RenderCommand& command);
			render::Frame& getAllocator();

			void TransferCommandList();

			InstanceRenderer& Instancing();

		private:
			static Renderer s_renderer;
			InstanceRenderer m_instanceRenderer;
			
		private:
			std::unique_ptr<render::Frame> m_frame;
			std::unique_ptr<render::Frame> m_prevFrame;
			
		};
	}
}