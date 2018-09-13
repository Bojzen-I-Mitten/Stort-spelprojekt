#include "Model.h"

#include "../graphics/Mesh.h"
#include "../utils/AssimpLoader.h"
#include "../graphics/animation/data/Skeleton.h"
namespace thomas {
	namespace resource {

		math::BoundingBox Model::GenerateBounds()
		{
			if (m_data.meshes.size() == 0)
				return math::BoundingBox();
			math::BoundingBox bounds = m_data.meshes[0]->m_bounds;
			
			for (int i = 1; i < m_data.meshes.size(); i++)
			{
				math::BoundingBox::CreateMerged(bounds, bounds, m_data.meshes[i]->m_bounds);
			}
			return bounds;
		}

		void Model::OnChanged()
		{
			m_data.meshes.clear();
			//m_data.meshes = utils::AssimpLoader::LoadModel(m_path);
			m_bounds = GenerateBounds();
		}

		Model::Model(std::string path) : Resource(path)
		{
			utils::AssimpLoader::LoadModel(path, m_data);
			m_bounds = GenerateBounds();
		}
		std::vector<std::shared_ptr<graphics::Mesh>> Model::GetMeshes()
		{
			return m_data.meshes;
		}
		
		graphics::animation::Skeleton * Model::GetSkeleton()
		{
			return m_data.m_skeleton.get();
		}


		Model::~Model()
		{
			m_data.meshes.clear();
		}


		Model::ModelData::ModelData() { }
		Model::ModelData::~ModelData() { }

	}
}