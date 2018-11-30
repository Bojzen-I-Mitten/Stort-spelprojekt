#pragma once
#include "../utils/Math.h"
#include "Resource.h"
#include <vector>
#include <memory>
#include <map>
namespace thomas {
	namespace graphics
	{
		class Mesh;
		namespace animation {
			class Skeleton;
		}
	}
	namespace resource {
		
		class Model : public Resource {
		private:
			math::BoundingBox GenerateBounds();
			void OnChanged();
		public:
			struct ModelData
			{
			public:
				ModelData();
				~ModelData();
				std::vector<std::shared_ptr<graphics::Mesh>> m_meshes;
				std::shared_ptr<graphics::animation::Skeleton> m_skeleton;

				void clear();
				void PreSkeletonParse();
				void PostLoad();
			};
			Model(std::string path);

			std::vector<std::shared_ptr<graphics::Mesh>> GetMeshes();
			graphics::animation::Skeleton* GetSkeleton();

			~Model();
			
		public:
			math::BoundingBox m_bounds;
		private:
			ModelData m_data;
		};
	}
}