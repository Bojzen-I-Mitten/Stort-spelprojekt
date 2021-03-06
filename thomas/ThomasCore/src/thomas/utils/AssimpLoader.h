#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "Math.h"
#include <memory>
#include "../resource/Model.h"
#include <assimp\matrix4x4.h>
struct aiNode;
struct aiMesh;
struct aiScene;
struct aiMaterial;
namespace thomas
{
	namespace graphics 
	{
		class Mesh;
		class Texture;
		namespace animation {
			class Skeleton;
			class AnimationData;
		}
	}
	namespace utils
	{

		class AssimpLoader
		{
		public:

			static void LoadModel(std::string path, resource::Model::ModelData &modelData);
			static std::vector<std::shared_ptr<graphics::animation::AnimationData>> LoadAnimation(std::string path);
			
			static std::string GetMaterialName(aiMaterial* material);
			static int GetMaterialShadingModel(aiMaterial* material);
			static math::Color GetMaterialColor(aiMaterial* material, const char* pKey, unsigned int type, unsigned int idx);
			static float GetMaterialShininess(aiMaterial* material);
			static float GetMaterialShininessStrength(aiMaterial* material);
			static int GetMaterialBlendMode(aiMaterial* material);
			static float GetMaterialOpacity(aiMaterial* material);

			static std::vector<graphics::Texture*> GetMaterialTextures(aiMaterial* material);

		private:
			
		};

	}
}

