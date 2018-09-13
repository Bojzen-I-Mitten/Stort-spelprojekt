/*
	Render class for the gameobjects and material assembly
*/

#pragma once
#include "..\utils\Math.h"
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

		struct RenderCommand
		{
			object::component::Camera* camera;								// Camera rendered from
			math::Matrix worldMatrix;										// World matrix, make local?
			Mesh* mesh;														// Rendered mesh
			resource::Material* material;									// Material used for rendering
			unsigned int num_local_prop;									// Number of local properties
			const resource::shaderproperty::ShaderProperty ** local_prop;	// Properties local to rendered object

			RenderCommand(math::Matrix world, Mesh* m, resource::Material* mat, object::component::Camera* cam) :
				camera(cam), worldMatrix(world), mesh(m), material(mat), num_local_prop(0), local_prop(NULL) {};
		};

		struct MaterialSorter
		{
			bool operator() (resource::Material* mat1, resource::Material* mat2) const;
		};

		typedef std::map<object::component::Camera*, std::map<resource::Material*, std::vector<RenderCommand>, MaterialSorter>> CommandQueue;

		class Renderer {
		private:

			static void BindFrame();
			static void BindObject(RenderCommand & rC);
						
		public:
			static void BindCamera(thomas::object::component::Camera* camera);
			static void ProcessCommands();
			static void ClearCommands();
			static void SubmitCommand(RenderCommand command);
			static void TransferCommandList();


		private:
			static CommandQueue s_renderCommands;
			static CommandQueue s_lastFramesCommands;
		};
	}
}