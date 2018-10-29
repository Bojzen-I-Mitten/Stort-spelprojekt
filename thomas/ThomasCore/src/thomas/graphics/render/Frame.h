#pragma once

#include <map>
#include <vector>
#include "../../utils/Math.h"
#include "FrameAllocation.h"
#include "../../resource/ShaderProperty/ShaderPropertyStruct.h"
#include "FrameData.h"

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
				uint32_t camera_ID;										// Camera rendered from
				math::Matrix worldMatrix;												// World matrix, make local?
				Mesh* mesh;																// Rendered mesh
				resource::Material* material;											// Material used for rendering
				unsigned int num_local_prop;											// Number of local properties
				const resource::shaderproperty::ShaderPropertyStatic * local_prop;	// Properties local to rendered object

				RenderCommand() {}
				RenderCommand(math::Matrix world, Mesh* m, resource::Material* mat, uint32_t cameraID) :
					camera_ID(cameraID), worldMatrix(world), mesh(m), material(mat), num_local_prop(0), local_prop(NULL)
				{};
				RenderCommand(math::Matrix world, Mesh* m, resource::Material* mat, uint32_t cameraID, unsigned int num_local_prop, const resource::shaderproperty::ShaderPropertyStatic * local_prop) :
					camera_ID(cameraID), worldMatrix(world), mesh(m), material(mat), num_local_prop(num_local_prop), local_prop(local_prop)
				{};
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
				CAMERA_FRAME_DATA m_frameData;
				std::map<resource::Material*, std::vector<RenderCommand>, MaterialSorter> m_commands3D;
				//std::vector<GUIRenderCommand> m_guiCommands;
			};

			/* ID, RenderQueue
			*/
			typedef std::map<uint32_t, CameraRenderQueue> CommandQueue;

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
