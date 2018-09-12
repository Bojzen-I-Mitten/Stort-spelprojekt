#pragma once
#include "../Common.h"
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
		
		class THOMAS_API Model : public Resource {
		private:
			math::BoundingBox GenerateBounds();
			void OnChanged();
		public:
			struct BoneInfo
			{
				std::string name;
				math::Matrix offsetMatrix;
				unsigned int parentBone;
			};
			struct ModelData
			{
			public:
				ModelData();
				~ModelData();
				std::map<std::string, unsigned int> boneMapping;
				std::vector<BoneInfo> boneInfo;
				std::vector<std::shared_ptr<graphics::Mesh>> meshes;
				std::shared_ptr<graphics::animation::Skeleton> m_skeleton;
			};
			Model(std::string path);

			std::vector<std::shared_ptr<graphics::Mesh>> GetMeshes();
			std::vector<BoneInfo> GetBones();
			graphics::animation::Skeleton* GetSkeleton();

			~Model();
			
		public:
			math::BoundingBox m_bounds;
		private:
			ModelData m_data;
		};
	}
}