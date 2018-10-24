#pragma once
#include <d3dx11effect.h>
#include <vector>
#include <map>
#include "../utils/Math.h"
#include "Resource.h"
#include "../utils/Buffers.h"
#include <memory>

namespace thomas
{
	namespace resource
	{
		namespace shaderproperty { class ShaderProperty; }
		class Texture2D;
		class Texture2DArray;
		class Shader : public Resource
		{
		public:
			enum class Semantics
			{
				POSITION = 0,
				TEXCOORD = 1,
				NORMAL = 2,
				TANGENT = 3,
				BITANGENT = 4, //Remove?
				BINORMAL = 5,
				BLENDINDICES = 6,
				BLENDWEIGHT = 7,
				COLOR = 8,
				POSITIONT = 9,
				PSIZE = 10,
				BONEINDICES = 11,
				BONEWEIGHTS = 12,
				NORMALTEXTURE = 13,//Temporary hack
				MATERIALSMOOTHNESSFACTOR = 14,//Temporary hack
				UNKNOWN = 15
			};
		private:
			static bool Compile(std::string path, ID3DX11Effect** effect);
		protected:
			Shader(ID3DX11Effect* effect, std::string path);
			~Shader();
		private:
			void SetupReflection();
			DXGI_FORMAT GetDXGIFormat(BYTE mask, D3D_REGISTER_COMPONENT_TYPE componentType);
			
			void Destroy();
			static void RecompileShaders();

			void OnChanged();

			Semantics GetSemanticFromName(std::string semanticName);
			/* Add the effect variable property.
			prop		<<	Effect variable read from shader
			propIndex	<<	Index of the effect variable
			*/
			void AddProperty(ID3DX11EffectVariable* prop, uint32_t propIndex);
		public:
			struct ShaderPass
			{
				std::string name;
				ID3D11InputLayout* inputLayout;
				std::vector<Semantics> inputSemantics;
			};

			static bool Init();
			static Shader* GetStandardShader();

			static Shader* CreateShader(std::string path);
			void BindPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY type);
			void BindVertexBuffer(utils::buffers::VertexBuffer* buffer);
			void BindVertexBuffers(std::vector<utils::buffers::VertexBuffer*> buffers);
			void BindIndexBuffer(utils::buffers::IndexBuffer* indexBuffer);
			void Bind();
			void Draw(UINT vertexCount, UINT startVertexLocation);
			void DrawIndexed(UINT indexCount, UINT startIndexLocation, int baseVertexLocation);
			void DrawIndexedInstance(UINT indexCount, int instanceCount);
			void DrawInstance(UINT vertexCount, int instanceCount);
			std::vector<ShaderPass>* GetPasses();
			void SetPass(int passIndex);
			ShaderPass& GetCurrentPass();
			static void DestroyAllShaders();
			static bool DestroyShader(Shader* shader);

			static void SetGlobalColor(const std::string& name, math::Color value);

			static void SetGlobalFloat(const std::string& name, float value);

			static void SetGlobalInt(const std::string& name, int value);

			static void SetGlobalMatrix(const std::string& name, math::Matrix value);
			
			static void SetGlobalTexture2D(const std::string& name, resource::Texture2D* value);
			static void SetGlobalTexture2DArray(const std::string & name, resource::Texture2DArray* value);
			static void SetGlobalResource(const std::string& name, ID3D11ShaderResourceView* value);
			static void SetGlobalConstantBuffer(const std::string& name, ID3D11Buffer* value);

			static void SetGlobalVector(const std::string& name, math::Vector4 value);

			static Shader* FindByName(const std::string& name);
			static Shader* FindByPath(const std::string& path);

			static void SetGlobalUAV(const std::string & name, ID3D11UnorderedAccessView* value);

			std::vector<std::string> GetMaterialProperties();
			ID3DX11Effect* GetEffect();
			bool HasProperty(const std::string& name);
			/* Get a property from a hash, returns 0 if everything OK, 1 if effect not found!!!!!!!!!!!
			*/
			bool GetPropertyIndex(uint32_t hash, uint32_t &effectIndex);
			std::shared_ptr<shaderproperty::ShaderProperty> GetProperty(const std::string& name);
			std::map<std::string, std::shared_ptr<shaderproperty::ShaderProperty>> GetProperties();
			static void Update();
			void Recompile();
			static void QueueRecompile();
		private:
			ID3DX11Effect* m_effect;
			std::map<std::string, std::shared_ptr<shaderproperty::ShaderProperty>> m_properties;
			std::map<uint32_t, uint32_t> m_property_indices;
			std::vector<ShaderPass> m_passes;
			ShaderPass* m_currentPass;
			std::vector<std::string> m_materialProperties;
			static std::vector<Shader*> s_loadedShaders;
			static Shader* s_standardShader;
			static Shader* s_failedShader;
			static bool s_shouldRecompile;


			class ShaderInclude : public ID3DInclude
			{
				public:
					ShaderInclude(const char* shaderDir, const char* systemDir);
					// Inherited via ID3DInclude
					virtual HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID * ppData, UINT * pBytes) override;
					virtual HRESULT __stdcall Close(LPCVOID pData) override;

			private:
				std::string m_shaderDir;
				std::string m_systemDir;
			};
		};
	}
}
