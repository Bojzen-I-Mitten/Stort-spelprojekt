#pragma once

#include <map>
#include <vector>
#include "../../utils/Math.h"
#include "FrameAllocation.h"
#include "../../resource/ShaderProperty/ShaderPropertyStruct.h"

namespace thomas {

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
	}

	namespace graphics
	{
		class Mesh;
		class Frame;

		namespace render {


			struct RenderCommand
			{
				object::component::Camera* camera;										// Camera rendered from
				math::Matrix worldMatrix;												// World matrix, make local?
				Mesh* mesh;																// Rendered mesh
				resource::Material* material;											// Material used for rendering
				unsigned int num_local_prop;											// Number of local properties
				const resource::shaderproperty::ShaderPropertyStatic * local_prop;	// Properties local to rendered object

				RenderCommand() {}
				RenderCommand(math::Matrix world, Mesh* m, resource::Material* mat, object::component::Camera* cam) :
					camera(cam), worldMatrix(world), mesh(m), material(mat), num_local_prop(0), local_prop(NULL) {};
				RenderCommand(math::Matrix world, Mesh* m, resource::Material* mat, object::component::Camera* cam, unsigned int num_local_prop, const resource::shaderproperty::ShaderPropertyStatic * local_prop) :
					camera(cam), worldMatrix(world), mesh(m), material(mat), num_local_prop(num_local_prop), local_prop(local_prop) {};
			};
			struct GUIRenderCommand
			{

			};

			struct MaterialSorter
			{
				bool operator() (resource::Material* mat1, resource::Material* mat2) const;
			};
			struct CameraRenderQueue
			{
				std::map<resource::Material*, std::vector<RenderCommand>, MaterialSorter> m_commands3D;
				//std::vector<GUIRenderCommand> m_guiCommands;
			};
			typedef std::map<object::component::Camera*, CameraRenderQueue> CommandQueue;

			/* Render frame container
			*/
			class Frame {
			public:

				Frame(unsigned int numStruct, unsigned int constantDataAlloc);

				CommandQueue m_queue;
				FrameAllocation m_alloc;

				void clear();
			};
		}
	}
}
