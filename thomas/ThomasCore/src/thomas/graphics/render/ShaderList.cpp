#include "ShaderList.h"
#include<algorithm>
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
				m_waitingList_Rmv(),
				m_addLock(),
				m_rmvLock()
			{
				m_standard = CreateShader("../Data/FXIncludes/StandardShader.fx");
				m_failed = CreateShader("../Data/FXIncludes/FailedShader.fx");
			}
			ShaderList::~ShaderList()
			{
				Destroy();
			}
			void ShaderList::Destroy()
			{
				// Cleanup
				for (int i = 0; i < m_renderableShaders.size(); i++)
					delete m_renderableShaders[i];
				m_renderableShaders.clear();

				m_addLock.lock();
				for (size_t i = 0; i < m_waitingList_Add.size(); i++)
					delete m_waitingList_Add[i];
				m_waitingList_Add.clear();
				m_addLock.unlock();

				m_rmvLock.lock();
				m_waitingList_Rmv.clear();
				m_rmvLock.unlock();
			}
			void ShaderList::SyncList()
			{
				// Sync. add list
				m_addLock.lock();
				for (size_t i = 0; i < m_waitingList_Add.size(); i++)
					m_renderableShaders.push_back(m_waitingList_Add[i]);
				m_waitingList_Add.clear();
				m_addLock.unlock();
				// Sync. rmv list
				m_rmvLock.lock();
				for (size_t i = 0; i < m_waitingList_Rmv.size(); i++)
				{
					for (auto it = m_renderableShaders.begin(); it < m_renderableShaders.end(); it++)
					{
						resource::Shader * s = *it._Ptr;
						if (m_waitingList_Rmv[i] == s)
						{
							m_renderableShaders.erase(it);
							delete s;
						}
					}
				}
				m_waitingList_Rmv.clear();
				m_rmvLock.unlock();
				// Recompile if necessary
				RecompileShaders();
			}
			void ShaderList::add(resource::Shader * s) const
			{
				m_addLock.lock();
				m_waitingList_Add.push_back(s);
				m_addLock.unlock();
			}
			void ShaderList::rmv(resource::Shader * s) const
			{
				m_rmvLock.lock();
				m_waitingList_Rmv.push_back(s);
				m_rmvLock.unlock();
			}
			

			resource::Shader * ShaderList::CreateShader(std::string path) const
			{
				resource::Shader* foundShader = FindByName(resource::Resource::PathToName(path));
				if (foundShader)
					return foundShader;
				
				std::unique_ptr<resource::Shader> shader = resource::Shader::CreateShader(path);
				if (!shader || !shader->hasPasses())
				{
					LOG("Global Shader Load at path: " << path << " failed.");
					return nullptr;
				}
				else
				{
					resource::Shader* ptr = shader.release();
					m_waitingList_Add.push_back(ptr);
					return ptr;
				}
			}

			resource::ComputeShader * ShaderList::CreateComputeShader(std::string path) const
			{
				resource::Shader* foundShader = FindByName(resource::Resource::PathToName(path));
				if (foundShader)
				{
					resource::ComputeShader* cs = dynamic_cast<resource::ComputeShader*>(foundShader);
					if (cs == nullptr)
						throw std::invalid_argument("Compute shader with path: " + path + " already exists as a normal Shader object!!!");
					return cs;	// Found compatible
				}
				std::unique_ptr<resource::ComputeShader> shader = resource::ComputeShader::CreateComputeShader(path);
				if (!shader || !shader->hasPasses())
				{
					LOG("Global ComputeShader Load at path: " << path << " failed.");
					return nullptr;
				}
				else
				{
					resource::ComputeShader* ptr = shader.release();
					m_waitingList_Add.push_back(ptr);
					return ptr;
				}
			}

			void ShaderList::RecompileShaders()
			{
#ifdef _EDITOR
				if (m_syncShaders)
				{
					LOG("Recompiling Shaders...");
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