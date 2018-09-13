#include "AssimpLoader.h"

#include "../resource/texture/Texture.h"
#include "../graphics/Mesh.h"
#include "../resource/Material.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include "../graphics/animation/data/Skeleton.h"
#include "Math.h"

namespace thomas
{
	namespace utils
	{
#pragma region Declares

		struct SkeletonConstruct {

			std::map<std::string, unsigned int> mapping;
			std::vector<graphics::animation::Bone> boneInfo;
			/* Generate the skeleton from the gathered data. Null if no skeleton data is avaiable */
			graphics::animation::Skeleton* generateSkeleton() {
				if (boneInfo.size() == 0)
					return nullptr;
				graphics::animation::Skeleton* skel = new graphics::animation::Skeleton(boneInfo);
				//for (unsigned int i = 0; i < _animations.size(); i++)
				//	skel->addAnimation(_animations[i]);
				return skel;
			}
			bool hasSkeleton() {
				return boneInfo.size() != 0;
			}
		};
		void ProcessSkeleton(aiNode * node, resource::Model::ModelData & modelData, SkeletonConstruct &boneMap, int parentBone, math::Matrix globalInverseTransform, math::Matrix parentTransform);
		void ProcessNode(aiNode* node, const aiScene* scene, resource::Model::ModelData& modelData, SkeletonConstruct &boneMap);

#pragma endregion


		/* Converts assimp 4x4 matrix to glm::mat4x4
		*/
		math::Matrix convertAssimpMatrix(aiMatrix4x4 matrix) {
			math::Matrix m;
			m[0][0] = matrix.a1; m[0][1] = matrix.b1;  m[0][2] = matrix.c1;  m[0][3] = matrix.d1;
			m[1][0] = matrix.a2; m[1][1] = matrix.b2;  m[1][2] = matrix.c2;  m[1][3] = matrix.d2;
			m[2][0] = matrix.a3; m[2][1] = matrix.b3;  m[2][2] = matrix.c3;  m[2][3] = matrix.d3;
			m[3][0] = matrix.a4; m[3][1] = matrix.b4;  m[3][2] = matrix.c4;  m[3][3] = matrix.d4;
			return m;
		}
		void AssimpLoader::LoadModel(std::string path, resource::Model::ModelData &modelData)
		{
			modelData = resource::Model::ModelData();
			std::vector<std::shared_ptr<graphics::Mesh>> meshes;
			std::string dir = path.substr(0, path.find_last_of("\\/"));
			// Read file via ASSIMP
			Assimp::Importer importer;
			const aiScene* scene = importer.ReadFile
			(
				path,
				aiProcess_FindDegenerates |
				aiProcess_FindInvalidData |
				aiProcess_ImproveCacheLocality |
				aiProcess_JoinIdenticalVertices |
				aiProcess_OptimizeGraph |
				aiProcess_OptimizeMeshes |
				aiProcess_RemoveRedundantMaterials |
				aiProcess_SortByPType |
				aiProcess_Triangulate |
				aiProcess_ValidateDataStructure |
				aiProcess_GenSmoothNormals |
				aiProcess_CalcTangentSpace |
				aiProcess_FlipUVs
			);

			if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
			{
				LOG("ERROR::ASSIMP " << importer.GetErrorString());
				return;
			}

			SkeletonConstruct skelConstruct;

			// Process ASSIMP's root node recursively
			ProcessNode(scene->mRootNode, scene, modelData, skelConstruct);
			
			math::Matrix globalInverseTransform = math::Matrix((float*)&scene->mRootNode->mTransformation.Inverse());
			ProcessSkeleton(scene->mRootNode, modelData, skelConstruct, -1, globalInverseTransform, math::Matrix::Identity);

			if (skelConstruct.hasSkeleton())
				modelData.m_skeleton = std::shared_ptr<graphics::animation::Skeleton>(skelConstruct.generateSkeleton());
			return;
		}

		std::string AssimpLoader::GetMaterialName(aiMaterial * material)
		{
			aiString name;
			material->Get(AI_MATKEY_NAME, name);
			return std::string(name.C_Str());
		}

		int AssimpLoader::GetMaterialShadingModel(aiMaterial * material)
		{
			int shadingModel;
			material->Get(AI_MATKEY_SHADING_MODEL, shadingModel);
			return shadingModel;
		}

		math::Color AssimpLoader::GetMaterialColor(aiMaterial * material, const char * pKey, unsigned int type, unsigned int idx)
		{
			aiColor3D color;
			material->Get(pKey, type, idx, color);
			return math::Color(color.r, color.g, color.b);
		}

		float AssimpLoader::GetMaterialShininess(aiMaterial * material)
		{
			float shininess;
			material->Get(AI_MATKEY_SHININESS, shininess);
			return shininess;
		}

		float AssimpLoader::GetMaterialShininessStrength(aiMaterial * material)
		{
			float shininessStrength;
			material->Get(AI_MATKEY_SHININESS_STRENGTH, shininessStrength);
			return shininessStrength;
		}

		int AssimpLoader::GetMaterialBlendMode(aiMaterial * material)
		{
			int blendMode;
			material->Get(AI_MATKEY_BLEND_FUNC, blendMode);
			return blendMode;
		}

		std::vector<graphics::Texture*> AssimpLoader::GetMaterialTextures(aiMaterial * material)
		{
			/*struct TextureInfo {
				aiString textureNameAiString;
				graphics::Texture::TextureType textureType;
				int mappingMode;
			};
			std::vector<TextureInfo> textureInfos;
*/

			//Get all textures information


			//TextureInfo texInfo;
			//if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texInfo.textureNameAiString) == AI_SUCCESS)
			//{
			//	texInfo.textureType = graphics::Texture::TextureType::DIFFUSE;
			//	material->Get(_AI_MATKEY_MAPPING_BASE, aiTextureType_DIFFUSE, 0, texInfo.mappingMode);
			//	textureInfos.push_back(texInfo);
			//}
			//	
			//if (material->GetTexture(aiTextureType_SPECULAR, 0, &texInfo.textureNameAiString) == AI_SUCCESS)
			//{
			//	texInfo.textureType = graphics::Texture::TextureType::SPECULAR;
			//	material->Get(_AI_MATKEY_MAPPING_BASE, aiTextureType_SPECULAR, 0, texInfo.mappingMode);
			//	textureInfos.push_back(texInfo);
			//}
			//	
			//if (material->GetTexture(aiTextureType_NORMALS, 0, &texInfo.textureNameAiString) == AI_SUCCESS)
			//{
			//	texInfo.textureType = graphics::Texture::TextureType::NORMAL;
			//	material->Get(_AI_MATKEY_MAPPING_BASE, aiTextureType_NORMALS, 0, texInfo.mappingMode);
			//	textureInfos.push_back(texInfo);
			//}
			//else if (material->GetTexture(aiTextureType_HEIGHT, 0, &texInfo.textureNameAiString) == AI_SUCCESS)
			//{
			//	texInfo.textureType = graphics::Texture::TextureType::NORMAL;
			//	material->Get(_AI_MATKEY_MAPPING_BASE, aiTextureType_NORMALS, 0, texInfo.mappingMode);
			//	textureInfos.push_back(texInfo);
			//}

				

			//Create a texture object for every texture found.
			std::vector<graphics::Texture*> textures;
			//for (unsigned int i = 0; i < textureInfos.size(); i++)
			//{
			//	std::string textureName(textureInfos[i].textureNameAiString.C_Str());
			//	graphics::Texture* texture = graphics::Texture::CreateTexture
			//	(
			//		textureInfos[i].mappingMode,
			//		textureInfos[i].textureType,
			//		dir + "/" + textureName
			//	);
			//	if(texture->Initialized())
			//		textures.push_back(texture);
			//}
			return textures;
			
		}

		float AssimpLoader::GetMaterialOpacity(aiMaterial * material)
		{
			float opacity;
			material->Get(AI_MATKEY_OPACITY, opacity);
			return opacity;
		}

		void ProcessMesh(aiMesh * mesh, const aiScene* scene,
			std::string meshName, resource::Model::ModelData& modelData, SkeletonConstruct &boneMap, aiMatrix4x4& transform)
		{
			graphics::Vertices vertices;
			std::vector <unsigned int> indices;
			std::string name = meshName + "-" + std::string(mesh->mName.C_Str());
			resource::Material* material;

			//vector<Texture> textures;
			vertices.positions.resize(mesh->mNumVertices);
			vertices.normals.resize(mesh->mNumVertices);

			if (mesh->mTextureCoords[0])
				vertices.texCoord0.resize(mesh->mNumVertices);
			
			if (mesh->HasTangentsAndBitangents())
			{
				vertices.tangents.resize(mesh->mNumVertices);
				vertices.bitangents.resize(mesh->mNumVertices);
			}
				

			if (mesh->HasBones())
				vertices.boneWeights.resize(mesh->mNumVertices);

			// Walk through each of the mesh's vertices
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				

				// Positions
				math::Vector3 v3 = math::Vector3((float*)&(transform * mesh->mVertices[i]));
				vertices.positions[i] = math::Vector4(v3.x, v3.y, v3.z, 1.0f);

				// Normals
				vertices.normals[i] = math::Vector3((float*)&(transform *mesh->mNormals[i]));

				// Tangents
				if (mesh->HasTangentsAndBitangents())
				{
					
					

					vertices.tangents[i] = math::Vector3((float*)&mesh->mTangents[i]);

					// Bitangents
					vertices.bitangents[i] = math::Vector3((float*)&mesh->mBitangents[i]);

				}
				

				// Texture Coordinates
				if (mesh->mTextureCoords[0])
				{
					
					math::Vector2 vec;

					// A vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
					// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
					vertices.texCoord0[i] = math::Vector2((float*)&mesh->mTextureCoords[0][i]);
				}
			}

			// Now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
			
				for (unsigned int j = 0; j < face.mNumIndices; j++)
					indices.push_back(face.mIndices[j]);
			}

			if (mesh->HasBones())
			{
				for (unsigned i = 0; i < mesh->mNumBones; i++)
				{
					
					unsigned int boneIndex = 0;
					aiBone* meshBone = mesh->mBones[i];
					aiNode *boneNode = scene->mRootNode->FindNode(meshBone->mName);
					std::string boneName = meshBone->mName.C_Str();
					if (boneMap.mapping.find(boneName) == boneMap.mapping.end()) //bone does not exist
					{
						boneIndex = boneMap.boneInfo.size();
						graphics::animation::Bone bi;
						boneMap.boneInfo.push_back(bi);
						
					}
					else //Bone already exists
						boneIndex = boneMap.mapping[boneName];

					boneMap.mapping[boneName] = boneIndex;
					boneMap.boneInfo[boneIndex]._boneIndex = boneIndex;
					boneMap.boneInfo[boneIndex]._boneName = boneName;
					boneMap.boneInfo[boneIndex]._invBindPose = convertAssimpMatrix(meshBone->mOffsetMatrix);
					boneMap.boneInfo[boneIndex]._bindPose = convertAssimpMatrix(boneNode->mTransformation);

					for (int j = 0; j < meshBone->mNumWeights; j++)
					{
						vertices.boneWeights[meshBone->mWeights[j].mVertexId].AddBoneData(boneIndex, meshBone->mWeights[j].mWeight);
					}
				}
			}

			//Process materials

			aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

			//TODO: material import
			//material = graphics::Material::CreateMaterial(dir, materialType, mat);


			std::shared_ptr<graphics::Mesh> m(new graphics::Mesh(vertices, indices, name));
			modelData.meshes.push_back(m);

		}

		void ProcessSkeleton(aiNode * node, resource::Model::ModelData & modelData, SkeletonConstruct &boneMap, int parentBone, math::Matrix globalInverseTransform, math::Matrix parentTransform)
		{
			std::string boneName = node->mName.C_Str();

			math::Matrix nodeTransform = math::Matrix((float*)&node->mTransformation);
			math::Matrix globalTransform = nodeTransform * parentTransform;

			if (boneMap.mapping.find(node->mName.C_Str()) != boneMap.mapping.end())
			{
				unsigned int BoneIndex = boneMap.mapping[boneName];
				if (parentBone != -1)
					boneMap.boneInfo[BoneIndex]._parentIndex = parentBone;
				else
					boneMap.boneInfo[BoneIndex]._parentIndex = BoneIndex;
				parentBone = BoneIndex;
				
				//boneMap.bones[BoneIndex].offsetMatrix =
				//	(globalInverseTransform * globalTransform * boneMap.bones[BoneIndex].offsetMatrix).Transpose();
			}
			else
			{
				int x = 5;
			}
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				ProcessSkeleton(node->mChildren[i], modelData, boneMap, parentBone, globalInverseTransform, globalTransform);
			}
		}

		void ProcessNode(aiNode * node, const aiScene * scene, resource::Model::ModelData& modelData, SkeletonConstruct &boneMap)
		{
			std::string modelName(scene->mRootNode->mName.C_Str());
			std::string nodeName(node->mName.C_Str());
			if (nodeName == modelName)
				nodeName = "root";
			// Process each mesh located at the current node
			for (unsigned int i = 0; i < node->mNumMeshes; i++)
			{
				// The node object only contains indices to index the actual objects in the scene. 
				// The scene contains all the data, node is just to keep stuff organized (like relations between nodes).
				aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
				
				ProcessMesh(mesh, scene, modelName + "-" + nodeName + "-" + std::to_string(i), modelData, boneMap, node->mTransformation);
			}
			
			// After we've processed all of the meshes (if any) we then recursively process each of the children nodes
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				ProcessNode(node->mChildren[i], scene, modelData, boneMap);
			}
		}
	}
}
