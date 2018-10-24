#include "ShaderList.h"
#include "../../ThomasCore.h"
#include "../../utils/Utility.h"
#include "../../resource/Shader.h"
#include "../../resource/ComputeShader.h"
#include "../../resource/texture/Texture2D.h"
#include "../../resource/texture/Texture2DArray.h"
#include "../../resource/ShaderProperty/ShaderProperty.h"
#include "../../resource/ShaderProperty/shaderProperties.h"

namespace thomas {


	namespace resource
	{
		class Shader;
	}


	namespace graphics {

		namespace render {
			ShaderList::ShaderList()	:
				m_renderableShaders(),
				m_waitingList_Add(),
				m_addLock()
			{
				m_standard = CreateShader("../Data/FXIncludes/StandardShader.fx");
				m_failed = CreateShader("../Data/FXIncludes/FailedShader.fx");
			}
			ShaderList::~ShaderList()
			{
				for (int i = 0; i < m_renderableShaders.size(); i++)
					delete m_renderableShaders[i];
				m_renderableShaders.clear();
			}
			void ShaderList::SyncList()
			{
				m_addLock.lock();
				for (size_t i = 0; i < m_waitingList_Add.size(); i++)
					m_renderableShaders.push_back(m_waitingList_Add[i]);
				// Recompile if necessary
				RecompileShaders();
				m_addLock.unlock();
			}
			void ShaderList::add(resource::Shader * s) const
			{
				m_addLock.lock();
				m_waitingList_Add.push_back(s);
				m_addLock.unlock();
			}
			

			ID3DX11Effect * ShaderList::CompileShader(const std::string& path) const
			{
				ID3DX11Effect* effect = NULL;

				if (!resource::Shader::Compile(path, &effect)) {
					if (!resource::Shader::Compile(m_failed->GetPath(), &effect))
						throw std::exception("Fallback shader failed to compile...!");
				}
				return effect;
			}

			resource::Shader * ShaderList::CreateShader(std::string path) const
			{
				resource::Shader* foundShader = FindByName(resource::Resource::PathToName(path));
				if (foundShader)
					return foundShader;

				ID3DX11Effect* effect = CompileShader(path);

				resource::Shader* shader = new resource::Shader(effect, path);
				m_waitingList_Add.push_back(shader);
				if (!shader->hasPasses())
				{
					LOG("Shader: " << path << " contains no techniques or passes");
					return nullptr;
				}
				else
				{
					m_waitingList_Add.push_back(shader);
					return shader;
				}
			}

			resource::ComputeShader * ShaderList::CreateComputeShader(std::string path) const
			{
				resource::Shader* foundShader = FindByName(resource::Resource::PathToName(path));
				if (foundShader)
				{
					resource::ComputeShader* shader = dynamic_cast<resource::ComputeShader*>(foundShader);
					if (shader == nullptr)
						throw std::invalid_argument("Conpute shader with path: " + path + " already exists as a normal Shader object!!!");
					return shader;	// Found compatible
				}
				std::unique_ptr<resource::ComputeShader> cs = resource::ComputeShader::CreateComputeShader(path);
				if (!cs->hasPasses())
				{
					LOG("Shader: " << path << " contains no techniques or passes");
					return nullptr;
				}
				else
				{
					resource::ComputeShader* ptr = cs.release();
					m_waitingList_Add.push_back(ptr);
					return ptr;
				}
			}

			void ShaderList::RecompileShaders()
			{
#ifdef _EDITOR
				LOG("Recompiling Shaders...");
				if (m_syncShaders)
				{
					for (resource::Shader* shader : m_renderableShaders)
						shader->Recompile();
				}
#endif
			}

			resource::Shader * ShaderList::FindByName(const std::string & name) const
			{
				for (resource::Shader* shader : m_renderableShaders)
				{
					if (shader->GetName() == name)
						return shader;
				}
				return nullptr;
			}

			resource::Shader * ShaderList::FindByPath(const std::string& path) const
			{
				for (resource::Shader* shader : m_renderableShaders)
				{
					if (m_failed->GetPath() == path)
						return shader;
				}
				return nullptr;
			}
			void ShaderList::QueueRecompile() const
			{
#ifdef _EDITOR
				m_syncShaders = true;
#endif
			}
			/*

			bool Shader::DestroyShader(Shader * shader)
			{
				auto it = m_renderableShaders.begin();

				while (it != m_renderableShaders.end())
				{
					if (*it._Ptr == shader)
					{

						m_renderableShaders.erase(it);
						delete shader;
						return true;
					}
					it++;
				}

				LOG("FAILED TO DESTROY SHADER, SHADER NOT LOADED?");

				return false;
			}
			*/

#pragma region SetGlobal

			void ShaderList::SetGlobalColor(const std::string & name, math::Color value)
			{
				using namespace resource::shaderproperty;
				std::shared_ptr<ShaderProperty> prop(new ShaderPropertyColor(value));
				prop->SetName(name);
				for (auto shader : m_renderableShaders)
					shader->SetProperty(name, prop);
			}
			void ShaderList::SetGlobalFloat(const std::string & name, float value)
			{
				using namespace resource::shaderproperty;
				std::shared_ptr<ShaderProperty> prop(new ShaderPropertyScalarFloat(value));
				prop->SetName(name);
				for (auto shader : m_renderableShaders)
					shader->SetProperty(name, prop);
			}
			void ShaderList::SetGlobalInt(const std::string & name, int value)
			{
				using namespace resource::shaderproperty;
				std::shared_ptr<ShaderProperty> prop(new ShaderPropertyScalarInt(value));
				prop->SetName(name);
				for (auto shader : m_renderableShaders)
					shader->SetProperty(name, prop);
			}
			void ShaderList::SetGlobalMatrix(const std::string & name, math::Matrix value)
			{
				using namespace resource::shaderproperty;
				std::shared_ptr<ShaderProperty> prop(new ShaderPropertyMatrix(value));
				prop->SetName(name);
				for (auto shader : m_renderableShaders)
					shader->SetProperty(name, prop);
			}
			void ShaderList::SetGlobalTexture2D(const std::string & name, resource::Texture2D* value)
			{
				using namespace resource::shaderproperty;
				std::shared_ptr<ShaderProperty> prop(new ShaderPropertyTexture2D(value));
				prop->SetName(name);
				for (auto shader : m_renderableShaders)
					shader->SetProperty(name, prop);
			}
			void ShaderList::SetGlobalTexture2DArray(const std::string & name, resource::Texture2DArray* value)
			{
				using namespace resource::shaderproperty;
				std::shared_ptr<ShaderProperty> prop(new ShaderPropertyTexture2DArray(value));
				prop->SetName(name);
				for (auto shader : m_renderableShaders)
					shader->SetProperty(name, prop);
			}
			void ShaderList::SetGlobalResource(const std::string & name, ID3D11ShaderResourceView * value)
			{
				using namespace resource::shaderproperty;
				std::shared_ptr<ShaderProperty> prop(new ShaderPropertyShaderResource(value));
				prop->SetName(name);
				for (auto shader : m_renderableShaders)
					shader->SetProperty(name, prop);
			}
			void ShaderList::SetGlobalConstantBuffer(const std::string & name, ID3D11Buffer * value)
			{
				using namespace resource::shaderproperty;
				std::shared_ptr<ShaderProperty> prop(new ShaderPropertyConstantBuffer(value));
				prop->SetName(name);
				for (auto shader : m_renderableShaders)
					shader->SetProperty(name, prop);
			}

			void ShaderList::SetGlobalVector(const std::string & name, math::Vector4 value)
			{
				using namespace resource::shaderproperty;
				std::shared_ptr<ShaderProperty> prop(new ShaderPropertyVector(value));
				prop->SetName(name);
				for (auto shader : m_renderableShaders)
					shader->SetProperty(name, prop);
			}

			void ShaderList::SetGlobalUAV(const std::string & name, ID3D11UnorderedAccessView * value)
			{
				using namespace resource::shaderproperty;
				std::shared_ptr<ShaderProperty> prop(new ShaderPropertyUnorderedAccessView(value));
				prop->SetName(name);
				for (auto shader : m_renderableShaders)
					shader->SetProperty(name, prop);
			}
			resource::Shader * ShaderList::GetStandardShader()
			{
				return m_standard;
			}
#pragma endregion
		}
	}
}