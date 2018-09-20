#pragma once

#include <map>
#include <vector>
#include "../../utils/Math.h"
#include "FrameAllocation.h"

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
		namespace shaderproperty {
			class ShaderProperty;
		}
	}

	namespace graphics
	{
		class Mesh;
		class Frame;

		namespace render {


			struct RenderCommand
			{
				object::component::Camera* camera;								// Camera rendered from
				math::Matrix worldMatrix;										// World matrix, make local?
				Mesh* mesh;														// Rendered mesh
				resource::Material* material;									// Material used for rendering
				size_t num_local_prop;									// Number of local properties
				const resource::shaderproperty::ShaderProperty ** local_prop;	// Properties local to rendered object

				RenderCommand(math::Matrix world, Mesh* m, resource::Material* mat, object::component::Camera* cam) :
					camera(cam), worldMatrix(world), mesh(m), material(mat), num_local_prop(0), local_prop(NULL) {};
				RenderCommand(math::Matrix world, Mesh* m, resource::Material* mat, object::component::Camera* cam, size_t num_local_prop, const resource::shaderproperty::ShaderProperty ** local_prop) :
					camera(cam), worldMatrix(world), mesh(m), material(mat), num_local_prop(num_local_prop), local_prop(local_prop) {};
			};

			struct MaterialSorter
			{
				bool operator() (resource::Material* mat1, resource::Material* mat2) const;
			};
			typedef std::map<object::component::Camera*, std::map<resource::Material*, std::vector<RenderCommand>, MaterialSorter>> CommandQueue;

			class Frame {
				CommandQueue m_queue;
				FrameAllocation m_alloc;
			};
		}
	}
}
