#include "Shader.h"
#include <AtlBase.h>
#include <atlconv.h>
#include <d3dcompiler.h>
#include "ShaderProperty\shaderProperties.h"
#include "../utils/Utility.h"
#include "../utils/GpuProfiler.h"
#include <fstream>
#include <comdef.h>
#include "..\ThomasCore.h"
#include "..\graphics\Renderer.h"

namespace thomas
{
	namespace resource
	{
		Shader::Shader(ID3DX11Effect* effect, std::string path) : Resource(path)
		{
			m_currentPass = nullptr;
			m_effect = effect;
			SetupReflection();
		}

		Shader::~Shader()
		{
			Destroy();
		}

		void Shader::SetupReflection()
		{
			D3DX11_EFFECT_DESC effectDesc;
			m_effect->GetDesc(&effectDesc);

			if (effectDesc.Techniques == 0)
			{
				LOG("Cannot set up reflection as shader has no techniques");
				return;
			}

			for (uint32_t i = 0; i < effectDesc.GlobalVariables; i++)
			{
				ID3DX11EffectVariable* variable = m_effect->GetVariableByIndex(i);
				if (variable->IsValid())
				{
					AddProperty(variable, i);
				}
				

			}
			
			D3DX11_TECHNIQUE_DESC techniqueDesc;
			ID3DX11EffectTechnique* tech = m_effect->GetTechniqueByIndex(0);
			if (tech->IsValid())
			{
				tech->GetDesc(&techniqueDesc);
				for (uint32_t j = 0; j < techniqueDesc.Passes; j++)
				{
					ShaderPass pass;
					D3DX11_PASS_SHADER_DESC vsPassDesc;
					D3DX11_EFFECT_SHADER_DESC vsDesc;
					D3DX11_PASS_DESC passDesc;
					ID3DX11EffectShaderVariable * vs;

					tech->GetPassByIndex(j)->GetDesc(&passDesc);
					tech->GetPassByIndex(j)->GetVertexShaderDesc(&vsPassDesc);

					//GetComputeShaderDesc

					pass.name = passDesc.Name;
					pass.inputLayout = NULL;

					vs = vsPassDesc.pShaderVariable->AsShader();
					if (vs->IsValid())
					{
						vs->GetShaderDesc(vsPassDesc.ShaderIndex, &vsDesc);

						std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
						std::vector<Semantics> inputSemantics;
						for (uint32_t iInput = 0; iInput < vsDesc.NumInputSignatureEntries; iInput++)
						{
							D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
							vs->GetInputSignatureElementDesc(vsPassDesc.ShaderIndex, iInput, &paramDesc);

							Semantics semantic = GetSemanticFromName(paramDesc.SemanticName);

							D3D11_INPUT_ELEMENT_DESC elementDesc;
							elementDesc.SemanticName = paramDesc.SemanticName;
							elementDesc.SemanticIndex = paramDesc.SemanticIndex;
							elementDesc.InputSlot = iInput;
							elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
							elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
							elementDesc.InstanceDataStepRate = 0;

							// determine DXGI format
							elementDesc.Format = GetDXGIFormat(paramDesc.Mask, paramDesc.ComponentType);
							
							inputLayoutDesc.push_back(elementDesc);
							inputSemantics.push_back(semantic);
						}

						HRESULT result = utils::D3D::Instance()->GetDevice()->CreateInputLayout(&inputLayoutDesc[0], inputLayoutDesc.size(), vsDesc.pBytecode, vsDesc.BytecodeLength, &pass.inputLayout);
						pass.inputSemantics = inputSemantics;
						if (result != S_OK)
						{
							LOG("Failed to create input layout for shader: " << m_path << " Error: " << result);
						}
					}
					m_passes.push_back(pass);
					
				}
			}
			

		}

		DXGI_FORMAT Shader::GetDXGIFormat(BYTE mask, D3D_REGISTER_COMPONENT_TYPE componentType)
		{
			DXGI_FORMAT format;
			if (mask == 1)
			{
				if (componentType == D3D_REGISTER_COMPONENT_UINT32) format = DXGI_FORMAT_R32_UINT;
				else if (componentType == D3D_REGISTER_COMPONENT_SINT32) format = DXGI_FORMAT_R32_SINT;
				else if (componentType == D3D_REGISTER_COMPONENT_FLOAT32) format = DXGI_FORMAT_R32_FLOAT;
			}
			else if (mask <= 3)
			{
				if (componentType == D3D_REGISTER_COMPONENT_UINT32) format = DXGI_FORMAT_R32G32_UINT;
				else if (componentType == D3D_REGISTER_COMPONENT_SINT32) format = DXGI_FORMAT_R32G32_SINT;
				else if (componentType == D3D_REGISTER_COMPONENT_FLOAT32) format = DXGI_FORMAT_R32G32_FLOAT;
			}
			else if (mask <= 7)
			{
				if (componentType == D3D_REGISTER_COMPONENT_UINT32) format = DXGI_FORMAT_R32G32B32_UINT;
				else if (componentType == D3D_REGISTER_COMPONENT_SINT32) format = DXGI_FORMAT_R32G32B32_SINT;
				else if (componentType == D3D_REGISTER_COMPONENT_FLOAT32) format = DXGI_FORMAT_R32G32B32_FLOAT;
			}
			else if (mask <= 15)
			{
				if (componentType == D3D_REGISTER_COMPONENT_UINT32) format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (componentType == D3D_REGISTER_COMPONENT_SINT32) format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (componentType == D3D_REGISTER_COMPONENT_FLOAT32) format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}
			else
			{
				format = DXGI_FORMAT_UNKNOWN;
				LOG("Unable to determine DXGI_FORMAT for shader: " << m_path);
			}
				

			return format;
		}

		void Shader::Destroy()
		{
			SAFE_RELEASE(m_effect);
			for (auto pass : m_passes)
				SAFE_RELEASE(pass.inputLayout);
			m_passes.clear();
			m_properties.clear();
		}

		bool Shader::Compile(std::string filePath, ID3DX11Effect** effect)
		{
			size_t found = filePath.find_last_of("/\\");
			std::string dir = filePath.substr(0, found);
			
			Shader::ShaderInclude include(dir.c_str(), "..\\Data\\FxIncludes");

			ID3DX11Effect* tempEffect = nullptr;
			ID3DBlob* errorBlob = nullptr;
			HRESULT result = D3DX11CompileEffectFromFile(
				CA2W(filePath.c_str()),
				nullptr,
				&include,
				D3DCOMPILE_DEBUG,
				0,
				utils::D3D::Instance()->GetDevice(),
				&tempEffect,
				&errorBlob);

			if (result != S_OK)
			{
				if (errorBlob)
				{
					if (errorBlob->GetBufferSize())
					{
						std::string error((char*)errorBlob->GetBufferPointer());
						std::string test = "asdf" + error;

						std::cout << test;
						LOG("Error compiling shader: " << filePath << ". errorBlob: " << error);
						errorBlob->Release();
					}
				}
				else
				{
					std::string error = HR_TO_STRING(result);
					LOG("Error compiling shader: " << filePath << " error: " << error);
				}
				
				return false;
			}
			else if(errorBlob)
			{
				if (errorBlob->GetBufferSize())
				{
					std::string error = (char*)errorBlob->GetBufferPointer();
					//LOG("Shader Compiler : " << error);
					errorBlob->Release();
				}
			}
			*effect = tempEffect;
			return true;
		}

		std::unique_ptr<resource::Shader> Shader::CreateShader(std::string path)
		{
			ID3DX11Effect* effect = NULL;
			Compile(path, &effect);
			if (!effect)
				return std::unique_ptr<resource::Shader>();

			std::unique_ptr<resource::Shader> shader(new Shader(effect, path));
			if (!shader->hasPasses())
				LOG("Shader: " << path << " contains no techniques or passes");
			return std::move(shader);
		}

		void Shader::BindPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY type)
		{
			utils::D3D::Instance()->GetDeviceContextDeffered()->IASetPrimitiveTopology(type);
		}
		void Shader::BindVertexBuffer(utils::buffers::VertexBuffer* buffer)
		{
			UINT stride = buffer->GetStride();
			ID3D11Buffer* buff = buffer->GetBuffer();
			UINT offset = 0;
			utils::D3D::Instance()->GetDeviceContextDeffered()->IASetVertexBuffers(0, 1, &buff, &stride, &offset);
		}

		void Shader::BindVertexBuffers(std::vector<utils::buffers::VertexBuffer*> buffers)
		{
			std::vector<ID3D11Buffer*> buffs;
			std::vector<UINT> strides;
			std::vector<UINT> offsets;

			for (utils::buffers::VertexBuffer* buffer : buffers)
			{
				buffs.push_back(buffer->GetBuffer());
				strides.push_back(buffer->GetStride());
				offsets.push_back(0);
			}

			utils::D3D::Instance()->GetDeviceContextDeffered()->IASetVertexBuffers(0, buffs.size(), buffs.data(), strides.data(), offsets.data());
		}


		void Shader::BindIndexBuffer(utils::buffers::IndexBuffer* indexBuffer)
		{
			utils::D3D::Instance()->GetDeviceContextDeffered()->IASetIndexBuffer(indexBuffer->GetBuffer(), DXGI_FORMAT_R32_UINT, 0);
		}
		void Shader::Bind()
		{
			for (auto prop : m_properties) {
				prop.second->Apply(this);
			}
		}
		void Shader::Draw(UINT vertexCount, UINT startVertexLocation)
		{
			thomas::utils::D3D::Instance()->GetDeviceContextDeffered()->Draw(vertexCount, startVertexLocation);
			utils::D3D::Instance()->GetProfiler()->AddDrawCall(vertexCount);
		}
		void Shader::DrawIndexed(UINT indexCount, UINT startIndexLocation, int baseVertexLocation)
		{
			thomas::utils::D3D::Instance()->GetDeviceContextDeffered()->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
			utils::D3D::Instance()->GetProfiler()->AddDrawCall(indexCount);
		}
		std::vector<Shader::ShaderPass>* Shader::GetPasses()
		{
			return &m_passes;
		}

		const std::string & Shader::GetPath()
		{
			return m_path;
		}
		
		void Shader::SetPass(int passIndex)
		{
			if (passIndex >= m_passes.size())
			{
				LOG("Shader: " + this->GetName() + " SetPass(), pass-index is out of range");
				return;
			}
			utils::D3D::Instance()->GetDeviceContextDeffered()->IASetInputLayout(m_passes[passIndex].inputLayout);
			ID3DX11EffectPass* pass = m_effect->GetTechniqueByIndex(0)->GetPassByIndex(passIndex);
			pass->Apply(0, utils::D3D::Instance()->GetDeviceContextDeffered());
			m_currentPass = &m_passes[passIndex];
		}
		void Shader::SetProperty(const std::string & name, std::shared_ptr<shaderproperty::ShaderProperty> prop)
		{
			if (HasProperty(name))
				m_properties[name] = prop;
		}
		void Shader::SetPropertyInt(const std::string & name, uint32_t value)
		{
			if (HasProperty(name))
			{
				std::shared_ptr<shaderproperty::ShaderPropertyScalarInt> prop( new shaderproperty::ShaderPropertyScalarInt(value));
				prop->SetName(name);
				m_properties[name] = prop;
			}
		}
		void Shader::SetPropertyUAV(const std::string & name, ID3D11UnorderedAccessView* value)
		{
			if (HasProperty(name))
			{
				std::shared_ptr<shaderproperty::ShaderPropertyUnorderedAccessView> prop(new shaderproperty::ShaderPropertyUnorderedAccessView(value));
				prop->SetName(name);
				m_properties[name] = prop;
			}
		}
		void Shader::SetPropertyResource(const std::string & name, ID3D11ShaderResourceView* value)
		{
			if (HasProperty(name))
			{
				std::shared_ptr<shaderproperty::ShaderPropertyShaderResource> prop(
					new shaderproperty::ShaderPropertyShaderResource(value));
				prop->SetName(name);
				m_properties[name] = prop;
			}
		}
		void Shader::SetPropertyTexture2D(const std::string & name, Texture2D* value)
		{
			if (HasProperty(name))
			{
				std::shared_ptr<shaderproperty::ShaderPropertyTexture2D> prop(
					new shaderproperty::ShaderPropertyTexture2D(value));
				prop->SetName(name);
				m_properties[name] = prop;
			}
		}
		void Shader::SetPropertyTexture2DArray(const std::string & name, Texture2DArray* value)
		{
			if (HasProperty(name))
			{
				std::shared_ptr<shaderproperty::ShaderPropertyTexture2DArray> prop(
					new shaderproperty::ShaderPropertyTexture2DArray(value));
				prop->SetName(name);
				m_properties[name] = prop;
			}
		}

		void Shader::SetPropertyTextureCube(const std::string & name, TextureCube * value)
		{
			if (HasProperty(name))
			{
				std::shared_ptr<shaderproperty::ShaderPropertyTextureCube> prop(
					new shaderproperty::ShaderPropertyTextureCube(value));
				prop->SetName(name);
				m_properties[name] = prop;
			}
		}




		Shader::ShaderPass & Shader::GetCurrentPass()
		{
			return *m_currentPass;
		}
		
		std::vector<std::string> Shader::GetMaterialProperties()
		{
			return m_materialProperties;
		}

		ID3DX11Effect * Shader::GetEffect()
		{
			return m_effect;
		}

		bool Shader::HasProperty(const std::string & name)
		{
			return m_properties.find(name) != m_properties.end();
		}
		bool Shader::GetPropertyIndex(uint32_t hash, uint32_t & effectIndex)
		{
			auto itr = m_property_indices.find(hash);
			if (itr == m_property_indices.end())	return true;
			effectIndex = itr->second;
			return false;
		}
		std::shared_ptr<shaderproperty::ShaderProperty> Shader::GetProperty(const std::string & name)
		{
			for (auto& prop : m_properties)
			{
				if (prop.first == name)
					return prop.second;
			}
			return nullptr;
		}

		std::map<std::string, std::shared_ptr<shaderproperty::ShaderProperty>> Shader::GetProperties()
		{
			return m_properties;
		}


		void Shader::Recompile()
		{
			ID3DX11Effect* tempEffect;
			
			if (Compile(m_path, &tempEffect))
			{
				SAFE_RELEASE(m_effect);
				for (auto pass : m_passes)
					SAFE_RELEASE(pass.inputLayout);
				m_materialProperties.clear();
				m_passes.clear();
				m_effect = tempEffect;
				SetupReflection();
			}
			else
			{
				LOG("Could not recompile shader " << m_path);
			}

		}

		bool Shader::hasPasses()
		{
			return m_passes.size();
		}

		void Shader::OnChanged()
		{
			graphics::Renderer::Instance()->getShaderList().QueueRecompile();
		}
		Shader::Semantics Shader::GetSemanticFromName(std::string semanticName)
		{
			if (semanticName.find("BINORMAL") != std::string::npos)
			{
				return Semantics::BINORMAL;
			}
			else if (semanticName.find("COLOR") != std::string::npos)
			{
				return Semantics::COLOR;
			}
			else if (semanticName.find("BLENDINDICES") != std::string::npos)
			{
				return Semantics::BLENDINDICES;
			}
			else if (semanticName.find("BLENDWEIGHT") != std::string::npos)
			{
				return Semantics::BLENDWEIGHT;
			}
			else if (semanticName.find("NORMAL") != std::string::npos)
			{
				return Semantics::NORMAL;
			}
			else if (semanticName.find("POSITION") != std::string::npos)
			{
				return Semantics::POSITION;
			}
			else if (semanticName.find("POSITIONT") != std::string::npos)
			{
				return Semantics::POSITIONT;
			}
			else if (semanticName.find("PSIZE") != std::string::npos)
			{
				return Semantics::PSIZE;
			}
			else if (semanticName.find("TANGENT") != std::string::npos)
			{
				return Semantics::TANGENT;
			}
			else if (semanticName.find("TEXCOORD") != std::string::npos)
			{
				return Semantics::TEXCOORD;
			}
			else if (semanticName.find("BITANGENT") != std::string::npos)
			{
				return Semantics::BITANGENT;
			}
			else if (semanticName.find("BONEINDICES") != std::string::npos)
			{
				return Semantics::BONEINDICES;
			}
			else if (semanticName.find("BONEWEIGHTS") != std::string::npos)
			{
				return Semantics::BONEWEIGHTS;
			}
			else
			{
				return Semantics::UNKNOWN;
			}
			
		}
		
		void Shader::AddProperty(ID3DX11EffectVariable * prop, uint32_t propIndex)
		{
			D3DX11_EFFECT_TYPE_DESC typeDesc;
			D3DX11_EFFECT_VARIABLE_DESC variableDesc;

			prop->GetType()->GetDesc(&typeDesc);
			prop->GetDesc(&variableDesc);
			ID3DX11EffectConstantBuffer* cBuffer = prop->GetParentConstantBuffer();

			bool isMaterialProperty = false;
			shaderproperty::ShaderProperty* newProperty = nullptr;
			std::string semantic;
			if (variableDesc.Semantic != NULL)
				semantic = variableDesc.Semantic;
			switch (typeDesc.Class)
			{
			case D3D_SVC_SCALAR:
			{
				switch (typeDesc.Type)
				{
				case D3D_SVT_BOOL:
					newProperty = shaderproperty::ShaderPropertyScalarBool::GetDefault();
					break;
				case D3D_SVT_INT:
					newProperty = shaderproperty::ShaderPropertyScalarInt::GetDefault();
				case D3D_SVT_UINT:
					newProperty = shaderproperty::ShaderPropertyScalarInt::GetDefault();
					break;
				case D3D_SVT_FLOAT:
					if (semantic == "MATERIALSMOOTHNESSFACTOR")
					{
						newProperty = new shaderproperty::ShaderPropertyScalarFloat(16);
					}
					else
					{
						newProperty = shaderproperty::ShaderPropertyScalarFloat::GetDefault();
					}
					break;
				default:
					break;
				}
				break;
			}
			case D3D_SVC_VECTOR:
				if(semantic == "COLOR")
					newProperty = shaderproperty::ShaderPropertyColor::GetDefault();
				else if (semantic == "UVTILING")
				{
					newProperty = new shaderproperty::ShaderPropertyVector(math::Vector4(1.0f, 1.0f, 0.0f, 0.0f));
				}
				else
					newProperty = shaderproperty::ShaderPropertyVector::GetDefault();
				break;
			case D3D_SVC_MATRIX_COLUMNS:
			case D3D_SVC_MATRIX_ROWS:
				newProperty = shaderproperty::ShaderPropertyMatrix::GetDefault();
				break;
			case D3D_SVC_OBJECT:
			{
				switch (typeDesc.Type)
				{
				case D3D_SVT_CBUFFER:
					newProperty = shaderproperty::ShaderPropertyConstantBuffer::GetDefault();
					break;

				case D3D_SVT_TEXTURE2DMS:
				case D3D_SVT_RWTEXTURE2D:
				case D3D_SVT_TEXTURECUBE:
					newProperty = shaderproperty::ShaderPropertyTextureCube::GetDefault();
					break;
				case D3D_SVT_TEXTURE2DARRAY:
					newProperty = shaderproperty::ShaderPropertyTexture2DArray::GetDefault();
					break;
				case D3D_SVT_TEXTURE2D:
					isMaterialProperty = true;
					if (semantic == "NORMALTEXTURE")
					{
						newProperty = new shaderproperty::ShaderPropertyTexture2D(Texture2D::GetNormalTexture());
					}
					else if (semantic == "SPECULARTEXTURE")
					{
						newProperty = new shaderproperty::ShaderPropertyTexture2D(Texture2D::GetBlackTexture());
					}
					else
					{
						newProperty = shaderproperty::ShaderPropertyTexture2D::GetDefault();
					}
					break;
				case D3D_SVT_STRUCTURED_BUFFER:
					newProperty = shaderproperty::ShaderPropertyShaderResource::GetDefault();
					break;
				case D3D_SVT_RWSTRUCTURED_BUFFER:
					newProperty = shaderproperty::ShaderPropertyUnorderedAccessView::GetDefault();
					break;
				case D3D_SVT_APPEND_STRUCTURED_BUFFER:
					newProperty = shaderproperty::ShaderPropertyUnorderedAccessView::GetDefault();
					break;
				case D3D_SVT_CONSUME_STRUCTURED_BUFFER:
					newProperty = shaderproperty::ShaderPropertyUnorderedAccessView::GetDefault();
					break;
				case D3D_SVT_BYTEADDRESS_BUFFER:
					newProperty = shaderproperty::ShaderPropertyShaderResource::GetDefault();
					break;
				case D3D_SVT_RWBYTEADDRESS_BUFFER:
					newProperty = shaderproperty::ShaderPropertyUnorderedAccessView::GetDefault();
					break;
				}
				break;
			}
			default:
				break;
			}
			
			std::string name = variableDesc.Name;
			
			if (cBuffer->IsValid())
			{
				D3DX11_EFFECT_VARIABLE_DESC bufferDesc;
				cBuffer->GetDesc(&bufferDesc);
				std::string bufferName = bufferDesc.Name;
				if (bufferName == "MATERIAL_PROPERTIES")
					isMaterialProperty = true;
			}
			if (newProperty != nullptr)
			{
				newProperty->SetName(name);
				uint32_t hash = utility::hash(name);
#ifdef _DEBUG
				// Verify hash doesn't exist
				if (m_property_indices.find(hash) != m_property_indices.end()) {
					std::string err("Warning in ThomasCore::resource::Shader::AddProperty!! Multiple effect properties with identical name hash: " + name);
					//LOG(err);
				}
#endif
				if (!HasProperty(name)) {
					m_property_indices[hash] = propIndex;
					m_properties[name] = std::shared_ptr<shaderproperty::ShaderProperty>(newProperty);
				}
				if(isMaterialProperty)
					m_materialProperties.push_back(name);
			}
			
		}

		Shader::ShaderInclude::ShaderInclude(const char * shaderDir, const char * systemDir) : m_shaderDir(shaderDir), m_systemDir(systemDir)
		{
		}

		HRESULT Shader::ShaderInclude::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID * ppData, UINT * pBytes)
		{
			try
			{
				
				std::string finalPath;
				std::string dir;
				switch (IncludeType)
				{
				case D3D_INCLUDE_LOCAL: // #include "FILE"
					dir = m_shaderDir;
					break;
				case D3D_INCLUDE_SYSTEM: // #include <FILE>
					dir = m_systemDir;
					break;
				default:
					assert(0);
				}

				/*
				If pFileName is absolute: finalPath = pFileName.
				If pFileName is relative: finalPath = dir + "\\" + pFileName
				*/
				if (PathIsRelative(pFileName))
				{
					finalPath = dir + "\\" + pFileName;
				}
				else
				{
					finalPath = pFileName;
				}
				
				std::ifstream fileStream(finalPath, std::ifstream::in | std::ifstream::binary | std::ifstream::ate);
				if (fileStream.fail())
				{
					LOG("Failed to find shader: " << finalPath);
					return E_FAIL;
				}
				uint32_t fileSize = fileStream.tellg();

				if (fileSize)
				{
					char* buffer = new char[fileSize];
					fileStream.seekg(0, std::ifstream::beg);
					fileStream.read(buffer, fileSize);

					*ppData = buffer;
					*pBytes = fileSize;
				}
				else
				{
					*ppData = nullptr;
					*pBytes = 0;
				}
				return S_OK;
			}
			catch (std::exception e)
			{
				LOG("Failed to read shader include: " << pFileName << " error: " << e.what());
				return E_FAIL;
			}
		}
		HRESULT Shader::ShaderInclude::Close(LPCVOID pData)
		{
			char* bufffer = (char*)pData;
			delete[] bufffer;
			return S_OK;
		}
	}
}