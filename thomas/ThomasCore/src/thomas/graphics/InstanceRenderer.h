#pragma once

#include <memory>
namespace thomas
{
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

		class InstanceRenderer
		{
		private:
			struct RenderData
			{
				Model* mesh;
				Material* material;

				RenderData(Model* m, Material* mat) : mesh(m), material(mat) {};
			};

		public:
			InstanceRenderer()
				: m_list()
			{
			}

			void clearList()
			{
				m_list.clear();
			}

			void add(unsigned int ID, Model* mesh, Material* mat)
			{
				m_list.insert(std::make_pair(ID, RenderData(mesh, mat)));
			}


		private:
			std::map<unsigned int, RenderData> m_list;
		};
	}
}
