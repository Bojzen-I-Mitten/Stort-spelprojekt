#include "Model.h"

#include "../graphics/Mesh.h"
#include "../utils/AssimpLoader.h"
#include "../graphics/animation/data/Skeleton.h"
namespace thomas {
	namespace resource {

		math::BoundingBox Model::GenerateBounds()
		{
			if (m_data.m_meshes.size() == 0)
				return math::BoundingBox();
			math::BoundingBox bounds = m_data.m_meshes[0]->m_bounds;
			
			for (int i = 1; i < m_data.m_meshes.size(); i++)
			{
				math::BoundingBox::CreateMerged(bounds, bounds, m_data.m_meshes[i]->m_bounds);
			}
			return bounds;
		}

		void Model::OnChanged()
		{
			m_data.clear();
			utils::AssimpLoader::LoadModel(m_path, m_data);
			m_bounds = GenerateBounds();
		}

		Model::Model(std::string path) : Resource(path)
		{
			utils::AssimpLoader::LoadModel(path, m_data);
			m_bounds = GenerateBounds();
		}
		std::vector<std::shared_ptr<graphics::Mesh>> Model::GetMeshes()
		{
			return m_data.m_meshes;
		}
		
		graphics::animation::Skeleton * Model::GetSkeleton()
		{
			return m_data.m_skeleton.get();
		}


		Model::~Model()
		{
			m_data.clear();
		}


		Model::ModelData::ModelData() { }
		Model::ModelData::~ModelData() { }

		void Model::ModelData::clear()
		{
			m_meshes.clear();
			m_skeleton = NULL;
		}

	}
}