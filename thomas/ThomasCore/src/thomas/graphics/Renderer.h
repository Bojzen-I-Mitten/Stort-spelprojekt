/*
	Render class for the gameobjects and material assembly
*/

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
			class RenderCommand;
		}
		
		class Renderer {
		private:
			static LightManager s_lightManager;
			void BindFrame();
			void BindObject(RenderCommand & rC);

		public:


			void BindCamera(thomas::object::component::Camera* camera);
			void ProcessCommands();
			void ClearCommands();
			void SubmitCommand(RenderCommand& command);
			void allocate(resource::shaderproperty::ShaderProperty *prop);


			void TransferCommandList();


		private:
			std::unique_ptr<render::Frame> m_frame;
			std::unique_ptr<render::Frame> m_prevFrame;
		};
	}
}