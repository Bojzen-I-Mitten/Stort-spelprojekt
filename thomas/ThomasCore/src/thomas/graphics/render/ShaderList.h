#pragma once

#include <vector>
#include "../../utils/atomic/Synchronization.h"
#include "../../utils/Math.h"

struct ID3DX11Effect;

namespace thomas {


	namespace resource
	{
		class Shader;
		class ComputeShader;
		class Texture2D;
		class Texture2DArray;
	}


	namespace graphics {

		namespace render {

			class ShaderList
			{
			public:
				ShaderList();
				~ShaderList();

				void SyncList();
				void add(resource::Shader* s) const;

				resource::Shader* CreateShader(std::string path) const;
				resource::ComputeShader* CreateComputeShader(std::string path) const;

				resource::Shader* FindByName(const std::string& name) const;
				resource::Shader* FindByPath(const std::string& path) const;

				void QueueRecompile() const;

			private:

				void RecompileShaders();


				ID3DX11Effect* CompileShader(const std::string& path) const;
			public:

				resource::Shader* GetStandardShader();

				void SetGlobalColor(const std::string& name, math::Color value);

				void SetGlobalFloat(const std::string& name, float value);

				void SetGlobalInt(const std::string& name, int value);

				void SetGlobalMatrix(const std::string& name, math::Matrix value);

				void SetGlobalTexture2D(const std::string& name, resource::Texture2D* value);
				void SetGlobalTexture2DArray(const std::string & name, resource::Texture2DArray* value);
				void SetGlobalResource(const std::string& name, ID3D11ShaderResourceView* value);
				void SetGlobalConstantBuffer(const std::string& name, ID3D11Buffer* value);

				void SetGlobalVector(const std::string& name, math::Vector4 value);
				void SetGlobalUAV(const std::string & name, ID3D11UnorderedAccessView* value);




			private:
				// Containers
				std::vector<resource::Shader*> m_renderableShaders;
				mutable  std::vector<resource::Shader*> m_waitingList_Add;
				// Sync.
				mutable  util::atomics::SpinLock m_addLock;	// Should be changed to another lock
#ifdef _EDITOR
				mutable  std::atomic<bool> m_syncShaders;
#endif

				// Standards
				resource::Shader* m_standard, *m_failed;
			};
		}
	}
}