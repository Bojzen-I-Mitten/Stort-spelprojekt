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
		class TextureCube;
		class Shader : public Resource
		{
		public:
			enum class Semantics
			{
				POSITION = 0,
				TEXCOORD = 1, // TEXCOORD0
				TEXCOORD1 = 2,
				NORMAL = 3,
				TANGENT = 4,
				BITANGENT = 5, // Replace with Tangent + cross mult?
				BINORMAL = 6,
				BLENDINDICES = 7,
				BLENDWEIGHT = 8,
				COLOR = 9,
				POSITIONT = 10,
				PSIZE = 11,
				BONEINDICES = 12,
				BONEWEIGHTS = 13,
				NORMALTEXTURE = 14,
				MATERIALSMOOTHNESSFACTOR = 15,
				UVTILING = 16,
				UNKNOWN = 17
			};

		protected:
			Shader(ID3DX11Effect* effect, std::string path);
			static bool Compile(std::string path, ID3DX11Effect** effect);
		public:
			virtual ~Shader();
			/* Compile the shader file
			*/
			static std::unique_ptr<resource::Shader> CreateShader(std::string path);
		private:
			void SetupReflection();
			DXGI_FORMAT GetDXGIFormat(BYTE mask, D3D_REGISTER_COMPONENT_TYPE componentType);
			
			void Destroy();
			void OnChanged();

			Semantics GetSemanticFromName(std::string semanticName, uint32_t semanticIndex);
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
					   			 
			void BindPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY type);
			void BindVertexBuffer(utils::buffers::VertexBuffer* buffer);
			void BindVertexBuffers(std::vector<utils::buffers::VertexBuffer*> buffers);
			void BindIndexBuffer(utils::buffers::IndexBuffer* indexBuffer);
			void Bind();
			void Draw(UINT vertexCount, UINT startVertexLocation);
			void DrawIndexed(UINT indexCount, UINT startIndexLocation, int baseVertexLocation);
			ShaderPass& GetCurrentPass();

			std::vector<std::string> GetMaterialProperties();
			ID3DX11Effect* GetEffect();
			bool HasProperty(const std::string& name);
			/* Recompile the shader
			*/
			void Recompile();
		public:	// Has

			bool hasPasses();

		public:	// Get


			/* Get a property from a hash, returns 0 if everything OK, 1 if effect not found!!!!!!!!!!!
			*/
			bool GetPropertyIndex(uint32_t hash, uint32_t &effectIndex);
			std::shared_ptr<shaderproperty::ShaderProperty> GetProperty(const std::string& name);
			std::map<std::string, std::shared_ptr<shaderproperty::ShaderProperty>> GetProperties();
			std::vector<ShaderPass>* GetPasses();

			const std::string& GetPath();

		public:	// Set

			void SetPass(int passIndex);

			/* Set a property of ShaderProperty type
			*/
			void SetProperty(const std::string &name, std::shared_ptr<shaderproperty::ShaderProperty> prop);
			/*	Set an uint property
			*/
			void SetPropertyInt(const std::string & name, uint32_t value);

			/*	Set an UAV property
			*/
			void SetPropertyUAV(const std::string & name, ID3D11UnorderedAccessView * value);

			void SetPropertyResource(const std::string & name, ID3D11ShaderResourceView * value);

			void SetPropertyTexture2D(const std::string & name, Texture2D * value);

			void SetPropertyTexture2DArray(const std::string & name, Texture2DArray * value);

			void SetPropertyTextureCube(const std::string & name, TextureCube * value);

		private:
			ID3DX11Effect* m_effect;
			std::map<std::string, std::shared_ptr<shaderproperty::ShaderProperty>> m_properties;
			std::map<uint32_t, uint32_t> m_property_indices;
			std::vector<ShaderPass> m_passes;
			ShaderPass* m_currentPass;
			std::vector<std::string> m_materialProperties;


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
