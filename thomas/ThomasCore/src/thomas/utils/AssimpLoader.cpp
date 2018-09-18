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

			std::map<std::string, unsigned int> m_mapping;
			std::vector<graphics::animation::Bone> m_boneInfo;
			/* Absolute node transform
			 * Transform relative from Node -> Parent (P*R*N : Parent*Relative*Node = Absolute)
			 * Parent inverse
			*/
			std::vector<aiMatrix4x4> m_absoluteBind, m_relativeParent, m_absoluteInv;
			std::vector < std::shared_ptr< graphics::animation::AnimationData> > m_animList;
			/* Generate the skeleton from the gathered data. Null if no skeleton data is avaiable */
			graphics::animation::Skeleton* generateSkeleton() {
				if (m_boneInfo.size() == 0)
					return nullptr;
				graphics::animation::Skeleton* skel = new graphics::animation::Skeleton(m_boneInfo);
				//for (unsigned int i = 0; i < _animations.size(); i++)
				//	skel->addAnimation(_animations[i]);
				return skel;
			}
			bool hasSkeleton() {
				return m_boneInfo.size() != 0;
			}

			/*	Get a bone index from string. -1 if nothing found */
			int getBoneIndex(const char* name) {
				auto itr = m_mapping.find(name);
				if (itr != m_mapping.end())
					return itr->second;
				return -1;
			}
			void postFetch() {
				m_absoluteBind.resize(m_boneInfo.size());
				m_relativeParent.resize(m_boneInfo.size());
				m_absoluteInv.resize(m_boneInfo.size());
			}
		};
		void ProcessSkeleton(aiNode * node, resource::Model::ModelData & modelData, SkeletonConstruct &boneMap, int parentBone, aiMatrix4x4 globalInverseTransform, aiMatrix4x4 parentTransform);
		void ProcessNode(aiNode* node, const aiScene* scene, resource::Model::ModelData& modelData, SkeletonConstruct &boneMap, aiMatrix4x4 parentTransform);
		void ProcessAnimations(const aiScene* scene, SkeletonConstruct& construct);

#pragma endregion


		/* Converts assimp 4x4 matrix to glm::mat4x4
		*/
		math::Matrix convertAssimpMatrix(aiMatrix4x4 matrix) {
			math::Matrix m(&matrix.a1);
			m = m.Transpose();
			//m[0][0] = matrix.a1; m[0][1] = matrix.a2;  m[0][2] = matrix.c1;  m[0][3] = matrix.d1;
			//m[1][0] = matrix.a2; m[1][1] = matrix.b2;  m[1][2] = matrix.c2;  m[1][3] = matrix.d2;
			//m[2][0] = matrix.a3; m[2][1] = matrix.b3;  m[2][2] = matrix.c3;  m[2][3] = matrix.d3;
			//m[3][0] = matrix.a4; m[3][1] = matrix.b4;  m[3][2] = matrix.c4;  m[3][3] = matrix.d4;
			return m;
		}

		const aiScene* LoadScene(Assimp::Importer &importer, const std::string &path)
		{
			std::string dir = path.substr(0, path.find_last_of("\\/"));
			// Read file via ASSIMP
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
				| aiProcess_ConvertToLeftHanded
			);

			if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
			{
				LOG("ERROR::ASSIMP " << importer.GetErrorString());
				return NULL;
			}
			return scene;
		}

		void verifySkeleton(resource::Model::ModelData &modelData, SkeletonConstruct &skelConstruct);
		void Process(const aiScene* scene, resource::Model::ModelData &modelData, SkeletonConstruct &skelConstruct)
		{
			std::vector<std::shared_ptr<graphics::Mesh>> m_meshes;
			
			aiMatrix4x4 globalInverseTransform = scene->mRootNode->mTransformation.Inverse();

			// Process ASSIMP's root node recursively
			ProcessNode(scene->mRootNode, scene, modelData, skelConstruct, globalInverseTransform);
			if (skelConstruct.hasSkeleton()) {
				skelConstruct.postFetch();
				// Process bone parenting
				ProcessSkeleton(scene->mRootNode, modelData, skelConstruct, -1, globalInverseTransform, aiMatrix4x4());

				verifySkeleton(modelData, skelConstruct);
			}
		}

		void verifySkeleton(resource::Model::ModelData &modelData, SkeletonConstruct &skelConstruct) {
			if (skelConstruct.hasSkeleton()) {
				std::vector<math::Matrix> m_list(skelConstruct.m_boneInfo.size());
				m_list[0] = skelConstruct.m_boneInfo[0]._bindPose;
				for (int i = 1; i < skelConstruct.m_boneInfo.size(); i++) {
					m_list[i] = skelConstruct.m_boneInfo[i]._bindPose * m_list[skelConstruct.m_boneInfo[i]._parentIndex];

					math::Matrix p = skelConstruct.m_boneInfo[i]._invBindPose * m_list[i];
					int a = 0;
				}

				thomas::graphics::Vertices & vert = modelData.m_meshes[0]->GetVertices();
				for (int i = 0; i < vert.positions.size(); i++) {
					math::Vector4 v = math::Vector4::Transform(vert.positions[i], skelConstruct.m_boneInfo[0]._invBindPose);
					v = math::Vector4::Transform(vert.positions[i], skelConstruct.m_boneInfo[0]._bindPose);
					int a = 0;
				}
			}
		}

		void AssimpLoader::LoadModel(std::string path, resource::Model::ModelData &modelData)
		{
			Assimp::Importer importer;
			SkeletonConstruct skelConstruct;
			const aiScene* scene = LoadScene(importer, path);
			if (!scene) return;
			Process(scene, modelData, skelConstruct);
			if (skelConstruct.hasSkeleton())
				modelData.m_skeleton = std::shared_ptr<graphics::animation::Skeleton>(skelConstruct.generateSkeleton());
			return;
		}
		std::vector<std::shared_ptr<graphics::animation::AnimationData>> AssimpLoader::LoadAnimation(std::string path)
		{
			Assimp::Importer importer;
			SkeletonConstruct skelConstruct;
			resource::Model::ModelData tmpData;
			const aiScene* scene = LoadScene(importer, path);
			if (!scene) return std::vector<std::shared_ptr<graphics::animation::AnimationData>>();
			Process(scene, tmpData, skelConstruct);
			if (skelConstruct.hasSkeleton())
				ProcessAnimations(scene, skelConstruct);
			return skelConstruct.m_animList;
		}

#pragma region Material

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
#pragma endregion

		void ProcessMesh(aiMesh * mesh, const aiScene* scene,
			std::string meshName, resource::Model::ModelData& modelData, SkeletonConstruct &boneMap, aiMatrix4x4& node_transform)
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
			aiMatrix4x4 bakeMatrix = node_transform;

			if (mesh->HasBones()) {
				vertices.boneIndices.resize(mesh->mNumVertices);
				vertices.boneWeights.resize(mesh->mNumVertices);
			}
			aiVector3D t, s;
			aiQuaternion r;
			bakeMatrix.Decompose(s, r, t);
			aiMatrix3x3 normalTrans = r.GetMatrix();
			// Walk through each of the mesh's vertices
			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{


				// Positions
				math::Vector3 v3 = math::Vector3((float*)&(bakeMatrix * mesh->mVertices[i]));
				vertices.positions[i] = math::Vector4(v3.x, v3.y, v3.z, 1.0f);

				// Normals
				vertices.normals[i] = math::Vector3((float*)&(normalTrans *mesh->mNormals[i]));
				vertices.normals[i].Normalize();

				// Tangents
				if (mesh->HasTangentsAndBitangents())
				{

					vertices.tangents[i] = math::Vector3((float*)&(normalTrans * mesh->mTangents[i]));
					vertices.tangents[i].Normalize();

					// Bitangents
					vertices.bitangents[i] = math::Vector3((float*)&(normalTrans * mesh->mBitangents[i]));
					vertices.bitangents[i].Normalize();
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
				aiMatrix4x4 bakeInv = node_transform;
				bakeInv.Inverse();
				for (unsigned i = 0; i < mesh->mNumBones; i++)
				{

					unsigned int boneIndex = 0;
					aiBone* meshBone = mesh->mBones[i];
					aiNode *boneNode = scene->mRootNode->FindNode(meshBone->mName);
					std::string boneName = meshBone->mName.C_Str();
					if (boneMap.m_mapping.find(boneName) == boneMap.m_mapping.end()) //bone does not exist
					{
						boneIndex = boneMap.m_boneInfo.size();
						graphics::animation::Bone bi;
						boneMap.m_boneInfo.push_back(bi);

					}
					else //Bone already exists
						boneIndex = boneMap.m_mapping[boneName];

					boneMap.m_mapping[boneName] = boneIndex;
					boneMap.m_boneInfo[boneIndex]._boneIndex = boneIndex;
					boneMap.m_boneInfo[boneIndex]._boneName = boneName;
					boneMap.m_boneInfo[boneIndex]._invBindPose = convertAssimpMatrix(meshBone->mOffsetMatrix * bakeInv);
					//boneMap.m_boneInfo[boneIndex]._bindPose = boneMap.m_boneInfo[boneIndex]._invBindPose.Invert();

					for (int j = 0; j < meshBone->mNumWeights; j++)
					{
						vertices.AddBoneData(meshBone->mWeights[j].mVertexId, boneIndex, meshBone->mWeights[j].mWeight);
					}
				}

			}

			//Process materials

			aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

			//TODO: material import
			//material = graphics::Material::CreateMaterial(dir, materialType, mat);

			vertices.PostProcess();

			std::shared_ptr<graphics::Mesh> m(new graphics::Mesh(vertices, indices, name));
			modelData.m_meshes.push_back(m);

		}

		void ProcessSkeleton(aiNode * node, resource::Model::ModelData & modelData, SkeletonConstruct &boneMap, int parentBone, aiMatrix4x4 globalInverseTransform, aiMatrix4x4 parentTransform)
		{
			std::string boneName = node->mName.C_Str();

			aiMatrix4x4 nodeTransform = node->mTransformation;
			aiMatrix4x4 object_space = parentTransform * nodeTransform;

			if (boneMap.m_mapping.find(node->mName.C_Str()) != boneMap.m_mapping.end())
			{
				unsigned int BoneIndex = boneMap.m_mapping[boneName];
				if (parentBone != -1) {	// Parented bone
					boneMap.m_boneInfo[BoneIndex]._parentIndex = parentBone;
					boneMap.m_relativeParent[BoneIndex] = boneMap.m_absoluteInv[parentBone] * parentTransform;
					boneMap.m_boneInfo[BoneIndex]._bindPose = convertAssimpMatrix(boneMap.m_relativeParent[BoneIndex] * nodeTransform);
				}
				else {					// Root bone (no parent)
					boneMap.m_boneInfo[BoneIndex]._parentIndex = BoneIndex;
					boneMap.m_boneInfo[BoneIndex]._bindPose = convertAssimpMatrix(object_space);
					boneMap.m_relativeParent[BoneIndex] = parentTransform;
				}
				parentBone = BoneIndex;
				// Store absolute transform
				boneMap.m_absoluteBind[BoneIndex] = object_space;
				boneMap.m_absoluteInv[BoneIndex] = object_space;
				boneMap.m_absoluteInv[BoneIndex].Inverse();

#ifdef DEBUG
				// Transform test (should be identity if not transformed)
				math::Matrix object_m = convertAssimpMatrix(object_space);
				math::Matrix object_inv = object_m.Invert();
				math::Matrix m = boneMap.m_boneInfo[BoneIndex]._invBindPose * object_m;
				int a = 0;
#endif
			}

			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				ProcessSkeleton(node->mChildren[i], modelData, boneMap, parentBone, globalInverseTransform, object_space);
			}
		}

		void ProcessNode(aiNode * node, const aiScene * scene, resource::Model::ModelData& modelData, SkeletonConstruct &boneMap, aiMatrix4x4 parentTransform)
		{
			aiMatrix4x4 nodeTransform = node->mTransformation;
			parentTransform = parentTransform * nodeTransform;

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
				ProcessNode(node->mChildren[i], scene, modelData, boneMap, parentTransform);
			}
		}


#pragma region AnimationData

		using namespace graphics::animation;
		struct AnimSize {
			size_t _numFloats;
			size_t _numChannels;
			size_t _numNodeChannels;
			size_t _numBones;
		};
		struct AnimationConstruct {
			size_t _dataInd;
			float* m_data;
			std::vector<std::vector<graphics::animation::Channel>> _keys;
		public:

			AnimationConstruct(AnimSize size)
				: _dataInd(0), m_data(new float[size._numFloats]), _keys(size._numBones)
			{
				for (unsigned int i = 0; i < size._numBones; i++)
				{
					_keys[i] = std::vector<Channel>(size._numNodeChannels);
				}
			}
			~AnimationConstruct()
			{
				delete[] m_data;
			}

			std::shared_ptr<graphics::animation::AnimationData> generateAnim(const char* name, float duration) {
				using namespace graphics::animation;
				std::unique_ptr<float> ptr(m_data);
				m_data = nullptr;
				std::vector<ObjectChannel> channels(_keys.size());
				for (unsigned int i = 0; i < _keys.size(); i++)
					channels[i] = ObjectChannel(_keys[i]);
				return std::shared_ptr<AnimationData>(new AnimationData(name, duration, channels, ptr));
			}

			void insert4(int ch, int nodeCh, float time, const float value[4]) {
				_keys[ch][nodeCh].m_keys.push_back(ChannelKey(time, m_data + _dataInd));
				m_data[_dataInd] = value[0];
				m_data[_dataInd + 1] = value[1];
				m_data[_dataInd + 2] = value[2];
				m_data[_dataInd + 3] = value[3];
				_dataInd += 4;
			}
			void insert3(int ch, int nodeCh, float time, const float value[3]) {
				_keys[ch][nodeCh].m_keys.push_back(ChannelKey(time, m_data + _dataInd));
				m_data[_dataInd] = value[0];
				m_data[_dataInd + 1] = value[1];
				m_data[_dataInd + 2] = value[2];
				_dataInd += 3;
			}
		};

		AnimSize ReadAnimData(aiAnimation *anim, SkeletonConstruct& construct) {
			AnimSize size;
			size._numChannels = 0;
			size._numFloats = 0;
			size._numNodeChannels = 3;
			size._numBones = construct.m_boneInfo.size();
			for (unsigned int ch = 0; ch < anim->mNumChannels; ch++) {
				aiNodeAnim *channel = anim->mChannels[ch];
				int bone = construct.getBoneIndex(channel->mNodeName.C_Str());
				if (bone < 0)
					continue; //This channel does not animate a bone.
				size._numChannels++;
				size._numFloats += 3 * channel->mNumPositionKeys;
				size._numFloats += 3 * channel->mNumScalingKeys;
				size._numFloats += 4 * channel->mNumRotationKeys;
			}
			return size;
		}

		aiVector3D mult_comp(aiVector3D v, aiVector3D v2) {
			return aiVector3D(v.x*v2.x, v.y*v2.y, v.z*v2.z);
		}
		void ProcessChannel(aiNodeAnim *channel, double ticksPerSecond, SkeletonConstruct& construct, AnimationConstruct& anim) {

			int bone = construct.getBoneIndex(channel->mNodeName.C_Str());
			if (bone < 0)
				return; //This channel does not animate a bone.

			// Decompose parent relative bone
			aiVector3D t, s;
			aiQuaternion r;
			construct.m_relativeParent[bone].Decompose(s, r, t);
			for (unsigned int i = 0; i < channel->mNumPositionKeys; i++) {
				aiVectorKey key = channel->mPositionKeys[i];
				aiVector3D trans = construct.m_relativeParent[bone] * key.mValue;

				anim.insert3(bone, 0, (float)(key.mTime / ticksPerSecond), &trans.x);
			}
			for (unsigned int i = 0; i < channel->mNumScalingKeys; i++) {
				aiVectorKey key = channel->mScalingKeys[i];
				aiVector3D scale = r.Rotate(mult_comp(s, key.mValue));
				anim.insert3(bone, 1, (float)(key.mTime / ticksPerSecond), &scale.x);
			}
			for (unsigned int i = 0; i < channel->mNumRotationKeys; i++) {
				aiQuatKey key = channel->mRotationKeys[i];
				aiQuaternion quat = r * key.mValue;
				
				math::Quaternion dxQ(quat.x, quat.y, quat.z, quat.w);
				anim.insert4(bone, 2, (float)(key.mTime / ticksPerSecond), &dxQ.x);
			}
		}

		void ProcessAnimations(const aiScene* scene, SkeletonConstruct& construct) {

			for (unsigned int i = 0; i < scene->mNumAnimations; i++) {
				aiAnimation *anim = scene->mAnimations[i];
				AnimSize size = ReadAnimData(anim, construct);
				if (size._numFloats > 0) {
					AnimationConstruct animConst(size);

					float duration = (float)(anim->mDuration / anim->mTicksPerSecond);
					for (unsigned int ch = 0; ch < anim->mNumChannels; ch++) {
						aiNodeAnim *channel = anim->mChannels[ch];
						ProcessChannel(channel, anim->mTicksPerSecond, construct, animConst);
					}
					construct.m_animList.push_back(animConst.generateAnim(anim->mName.C_Str(), duration));
				}
			}
		}

#pragma endregion
	}
}
