/*
	Helper file for loading models with the assimp library. This class prepares the mesh
	with vertex and index buffers and renders the model.
*/

#pragma once
#include "..\utils\Math.h"
#include "..\resource\Shader.h"
#include <vector>
#include <map>
#include <memory>

namespace thomas
{
	namespace object
	{
		namespace component
		{
			class Camera;
			class Transform;
		}
	}
	namespace resource { class Material; }


	namespace graphics 
	{
		struct BoneIndex {
			int boneIndex0 = 0;
			int boneIndex1 = 0;
			int boneIndex2 = 0;
			int boneIndex3 = 0;
		};
		struct BoneWeight
		{
			float weight0 = 0.0f;
			float weight1 = 0.0f;
			float weight2 = 0.0f;
			float weight3 = 0.0f;

			void Normalize();
		};
		struct Vertices 
		{
			std::vector<math::Vector4>	positions;
			std::vector<math::Vector4>	colors;
			std::vector<math::Vector2>	texCoord0;
			std::vector<math::Vector3>	normals;
			std::vector<math::Vector3>	tangents;
			std::vector<math::Vector3>	bitangents;
			std::vector<BoneIndex>		boneIndices;
			std::vector<BoneWeight>		boneWeights;

			/* Append a vertex bone weight for a bone at index
			*/
			void AddBoneData(int vertIndex, int boneIndex, float weight);

			/* Post process vertices after all vertices are inserted
			*/
			void PostProcess();

		};

		struct MeshData
		{
			Vertices vertices;
			std::vector<unsigned int> indices;
			
			std::map<resource::Shader::Semantics, std::unique_ptr<utils::buffers::VertexBuffer>> vertexBuffers;
			std::unique_ptr<utils::buffers::IndexBuffer> indexBuffer = nullptr;
		};

		class Mesh
		{
		public:
			Mesh(const Vertices & vertices, std::vector<unsigned int> indices, const std::string & name);
			~Mesh();
			void Draw(resource::Shader* shader);

		public:
			void SetName(const std::string & name);

		public:
			std::string GetName() const;
		    unsigned int GetIndexCount() const;
			unsigned int GetVertexCount() const;
			Vertices & GetVertices();
			std::vector<unsigned int> & GetIndices();
			MeshData & GetData();

		private:
			void SetupMesh();
			math::BoundingBox GenerateBounds();

		public:
			math::BoundingBox m_bounds;

		private:
			std::string m_name;
			MeshData m_data;

			
		};
	}
}

