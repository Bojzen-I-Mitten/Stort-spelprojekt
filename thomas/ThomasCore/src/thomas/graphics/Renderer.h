#pragma once
#include "..\utils\Math.h"
#include <vector>
#include <map>
#include <memory>
#include "render/ShaderList.h"

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

		public:
			void init();
			void PostRender();
			void Destroy();

			/* Renderer Singleton */
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

		public:
			/* Access shaders 
			*/
			const render::ShaderList& getShaderList();
			resource::Shader* GetStandardShader();

		private:
			std::unique_ptr<render::Frame> m_frame;
			std::unique_ptr<render::Frame> m_prevFrame;
			render::ShaderList m_shaders;
			
		};
	}
}