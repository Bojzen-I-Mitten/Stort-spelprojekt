#include "Mesh.h"
#include "../utils/GpuProfiler.h"
#include "../resource/ComputeShader.h"

namespace thomas 
{
	namespace graphics 
	{
		Mesh::Mesh(const Vertices & vertices, std::vector<unsigned int> indices, const std::string & name) : m_name(name)
		{
			m_data.vertices = vertices;
			m_data.indices = indices;
			SetupMesh();
			m_bounds = GenerateBounds();
		}

		Mesh::~Mesh()
		{
		}

		void Mesh::Draw(resource::Shader * shader)
		{
			if ((m_data.vertices.boneIndices.size() > 0))
			{
				std::vector<utils::buffers::ByteAddressBuffer*> vertexBuffer;

				//Add vertex buffers?
				for (auto semantic : shader->GetCurrentPass().inputSemantics)
				{
					if (m_data.skinVertexBuffers.find(semantic) != m_data.skinVertexBuffers.end())
						vertexBuffer.push_back(m_data.skinVertexBuffers[semantic].get());
				}

				//Set buffers and draw mesh
				shader->BindVertexBuffers(vertexBuffer);
			}
			else
			{
				std::vector<utils::buffers::VertexBuffer*> vertexBuffers;

				//Add vertex buffers?
				for (auto semantic : shader->GetCurrentPass().inputSemantics)
				{
					if (m_data.vertexBuffers.find(semantic) != m_data.vertexBuffers.end())
						vertexBuffers.push_back(m_data.vertexBuffers[semantic].get());
				}

				//Set buffers and draw mesh
				shader->BindVertexBuffers(vertexBuffers);
			}
			if (!m_data.indexBuffer)
			{
				shader->Draw(GetVertexCount(), 0);
				utils::D3D::Instance()->GetProfiler()->AddDrawCall(GetVertexCount() / 3, GetVertexCount());
			}
			else
			{
				// Draw indexed
				shader->BindIndexBuffer(m_data.indexBuffer.get());
				shader->DrawIndexed(GetIndexCount(), 0, 0);
				utils::D3D::Instance()->GetProfiler()->AddDrawCall(GetIndexCount() / 3, GetVertexCount());
			}
		}

		void Mesh::Skin(resource::ComputeShader * computeShader)
		{
			
			int test = m_data.skinOrigVertexBuffers[resource::Shader::Semantics::POSITION].get()->GetStride(); 
			int test2 = m_data.skinOrigVertexBuffers[resource::Shader::Semantics::NORMAL].get()->GetStride(); 
			//set srvs
			if (m_data.vertices.positions.size() > 0)
				computeShader->SetPropertyResource("vertexOrigData", m_data.skinOrigVertexBuffers[resource::Shader::Semantics::POSITION].get()->GetSRV());
			if (m_data.vertices.normals.size() > 0)
				computeShader->SetPropertyResource("normalOrigData", m_data.skinOrigVertexBuffers[resource::Shader::Semantics::NORMAL].get()->GetSRV());
			if (m_data.vertices.boneIndices.size() > 0)
				computeShader->SetPropertyResource("boneIndexOrigData", m_data.skinOrigVertexBuffers[resource::Shader::Semantics::BONEINDICES].get()->GetSRV());
			if (m_data.vertices.boneWeights.size() > 0)
				computeShader->SetPropertyResource("boneWeightOrigData", m_data.skinOrigVertexBuffers[resource::Shader::Semantics::BONEWEIGHTS].get()->GetSRV());

			if (m_data.vertices.positions.size() > 0)
				computeShader->SetPropertyUAV("vertexData", m_data.skinVertexBuffers[resource::Shader::Semantics::POSITION].get()->GetUAV());
			if (m_data.vertices.normals.size() > 0)
				computeShader->SetPropertyUAV("normalData", m_data.skinVertexBuffers[resource::Shader::Semantics::NORMAL].get()->GetUAV());
			if (m_data.vertices.boneIndices.size() > 0)
				computeShader->SetPropertyUAV("boneIndexData", m_data.skinVertexBuffers[resource::Shader::Semantics::BONEINDICES].get()->GetUAV());
			if (m_data.vertices.boneWeights.size() > 0)
				computeShader->SetPropertyUAV("boneWeightData", m_data.skinVertexBuffers[resource::Shader::Semantics::BONEWEIGHTS].get()->GetUAV());

			computeShader->SetPropertyInt("maxNrOfVerts", GetVertexCount());
			
			computeShader->Bind();
			computeShader->SetPass(0);

			int gridDimX = (GetVertexCount() + 255) / 256;
			computeShader->Dispatch(gridDimX);

			computeShader->UnbindAllSRVs();
			computeShader->UnbindAllUAVs();
		}

		void Mesh::SetName(const std::string & name)
		{
			m_name = name;
		}

		MeshData & Mesh::GetData()
		{
			return m_data;
		}

		std::string Mesh::GetName() const
		{
			return m_name;
		}

		unsigned int Mesh::GetIndexCount() const
		{
			return m_data.indexBuffer->IndexCount();
		}

		unsigned int Mesh::GetVertexCount() const
		{
			return (uint32_t)m_data.vertices.positions.size();
		}

		Vertices & Mesh::GetVertices()
		{
			return m_data.vertices;
		}

		std::vector<unsigned int> & Mesh::GetIndices()
		{
			return m_data.indices;
		}

		void Mesh::SetupMesh()
		{
			

			if (m_data.vertices.boneIndices.size() > 0)
			{

				if (m_data.vertices.positions.size() > 0)
					m_data.skinOrigVertexBuffers.insert(std::make_pair(resource::Shader::Semantics::POSITION, std::make_unique<utils::buffers::ByteAddressBuffer>(m_data.vertices.positions,D3D11_BIND_FLAG(D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE))));
				if (m_data.vertices.normals.size() > 0)
					m_data.skinOrigVertexBuffers.insert(std::make_pair(resource::Shader::Semantics::NORMAL, std::make_unique<utils::buffers::ByteAddressBuffer>(m_data.vertices.normals, D3D11_BIND_FLAG(D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE))));

				m_data.skinOrigVertexBuffers.insert(std::make_pair(resource::Shader::Semantics::BONEINDICES, std::make_unique<utils::buffers::ByteAddressBuffer>(m_data.vertices.boneIndices, D3D11_BIND_FLAG(D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE))));
				if (m_data.vertices.boneWeights.size() > 0)
					m_data.skinOrigVertexBuffers.insert(std::make_pair(resource::Shader::Semantics::BONEWEIGHTS, std::make_unique<utils::buffers::ByteAddressBuffer>(m_data.vertices.boneWeights, D3D11_BIND_FLAG(D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE))));

				//Insert data if the property exist
				if (m_data.vertices.positions.size() > 0)
				{
					m_data.skinVertexBuffers.insert(std::make_pair(resource::Shader::Semantics::POSITION, std::make_unique<utils::buffers::ByteAddressBuffer>(m_data.vertices.positions, D3D11_BIND_FLAG(D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS))));
				}
				if (m_data.vertices.colors.size() > 0)
				{		
					m_data.skinVertexBuffers.insert(std::make_pair(resource::Shader::Semantics::COLOR, std::make_unique<utils::buffers::ByteAddressBuffer>(m_data.vertices.colors, D3D11_BIND_FLAG(D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS))));
				}
				if (m_data.vertices.texCoord0.size() > 0)
				{		
					m_data.skinVertexBuffers.insert(std::make_pair(resource::Shader::Semantics::TEXCOORD, std::make_unique<utils::buffers::ByteAddressBuffer>(m_data.vertices.texCoord0, D3D11_BIND_FLAG(D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS))));
				}
				if (m_data.vertices.normals.size() > 0)
				{		
					m_data.skinVertexBuffers.insert(std::make_pair(resource::Shader::Semantics::NORMAL, std::make_unique<utils::buffers::ByteAddressBuffer>(m_data.vertices.normals, D3D11_BIND_FLAG(D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS))));
				}
				if (m_data.vertices.tangents.size() > 0)
				{
					m_data.skinVertexBuffers.insert(std::make_pair(resource::Shader::Semantics::TANGENT, std::make_unique<utils::buffers::ByteAddressBuffer>(m_data.vertices.tangents, D3D11_BIND_FLAG(D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS))));
				}
				if (m_data.vertices.bitangents.size() > 0)
				{
					m_data.skinVertexBuffers.insert(std::make_pair(resource::Shader::Semantics::BITANGENT, std::make_unique<utils::buffers::ByteAddressBuffer>(m_data.vertices.bitangents, D3D11_BIND_FLAG(D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS))));
				}
				if (m_data.vertices.boneIndices.size() > 0)
				{
					m_data.skinVertexBuffers.insert(std::make_pair(resource::Shader::Semantics::BONEINDICES, std::make_unique<utils::buffers::ByteAddressBuffer>(m_data.vertices.boneIndices, D3D11_BIND_FLAG(D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS))));
				}
				if (m_data.vertices.boneWeights.size() > 0)
				{
					m_data.skinVertexBuffers.insert(std::make_pair(resource::Shader::Semantics::BONEWEIGHTS, std::make_unique<utils::buffers::ByteAddressBuffer>(m_data.vertices.boneWeights, D3D11_BIND_FLAG(D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS))));
				}
			}
			else
			{
				//Insert data if the property exist
				if (m_data.vertices.positions.size() > 0)
					m_data.vertexBuffers.insert(std::make_pair(resource::Shader::Semantics::POSITION, std::make_unique<utils::buffers::VertexBuffer>(m_data.vertices.positions)));
				if (m_data.vertices.colors.size() > 0)
					m_data.vertexBuffers.insert(std::make_pair(resource::Shader::Semantics::COLOR, std::make_unique<utils::buffers::VertexBuffer>(m_data.vertices.colors)));
				if (m_data.vertices.texCoord0.size() > 0)
					m_data.vertexBuffers.insert(std::make_pair(resource::Shader::Semantics::TEXCOORD, std::make_unique<utils::buffers::VertexBuffer>(m_data.vertices.texCoord0)));
				if (m_data.vertices.normals.size() > 0)
					m_data.vertexBuffers.insert(std::make_pair(resource::Shader::Semantics::NORMAL, std::make_unique<utils::buffers::VertexBuffer>(m_data.vertices.normals)));
				if (m_data.vertices.tangents.size() > 0)
					m_data.vertexBuffers.insert(std::make_pair(resource::Shader::Semantics::TANGENT, std::make_unique<utils::buffers::VertexBuffer>(m_data.vertices.tangents)));
				if (m_data.vertices.bitangents.size() > 0)
					m_data.vertexBuffers.insert(std::make_pair(resource::Shader::Semantics::BITANGENT, std::make_unique<utils::buffers::VertexBuffer>(m_data.vertices.bitangents)));
				if (m_data.vertices.boneIndices.size() > 0)
					m_data.vertexBuffers.insert(std::make_pair(resource::Shader::Semantics::BONEINDICES, std::make_unique<utils::buffers::VertexBuffer>(m_data.vertices.boneIndices)));
				if (m_data.vertices.boneWeights.size() > 0)
					m_data.vertexBuffers.insert(std::make_pair(resource::Shader::Semantics::BONEWEIGHTS, std::make_unique<utils::buffers::VertexBuffer>(m_data.vertices.boneWeights)));
			}

			if (!m_data.indices.empty())
				m_data.indexBuffer = std::make_unique<utils::buffers::IndexBuffer>(m_data.indices);
		}

		math::BoundingBox Mesh::GenerateBounds()
		{
			math::BoundingBox bounds;
			std::vector<math::Vector3> points(m_data.vertices.positions.size());

			for (auto point : m_data.vertices.positions)
				points.push_back(math::Vector3(point));

			math::BoundingBox::CreateFromPoints(bounds, points.size(), points.data(), sizeof(math::Vector3));
			return bounds;
		}

#pragma region Vertex Structs

		float square(float x) { return x * x; }
		void BoneWeight::Normalize() {
			float len_inv = 1.f / std::sqrtf(square(weight0) + square(weight1) + square(weight2) + square(weight3));
			weight0 *= len_inv;
			weight1 *= len_inv;
			weight2 *= len_inv;
			weight3 *= len_inv;
		}
		void Vertices::AddBoneData(int vertIndex, int boneIndex, float weight)
		{
			BoneWeight &w = boneWeights[vertIndex];
			BoneIndex &i = boneIndices[vertIndex];
			if (w.weight0 == 0.0f)
			{
				i.boneIndex0 = boneIndex;
				w.weight0 = weight;
			}
			else if (w.weight1 == 0.0f)
			{
				i.boneIndex1 = boneIndex;
				w.weight1 = weight;
			}
			else if (w.weight2 == 0.0f)
			{
				i.boneIndex2 = boneIndex;
				w.weight2 = weight;
			}
			else if (w.weight3 == 0.0f)
			{
				i.boneIndex3 = boneIndex;
				w.weight3 = weight;
			}
		}
			/* Post process vertices after all vertices are inserted
			*/
		void Vertices::PostProcess() {
		for (unsigned int i = 0; i < boneWeights.size(); i++)
			boneWeights[i].Normalize();
		}
#pragma endregion
	}
}
