#pragma once

#include "Collider.h"
namespace thomas
{
	namespace resource
	{
		class Model;
	}
	namespace object
	{
		namespace component
		{
			class MeshCollider : public Collider
			{
			public:
				MeshCollider();
				resource::Model* GetMesh();
				void SetMesh(resource::Model* value);
				void OnDrawGizmosSelected();
				void SetConcave(bool value);
				bool GetConcave();
				void Update() override;
				void SetTrigger(bool trigger) override;

				void SetMargin(float margin);
				float GetMargin();
			private:
				void RecalcCollider();
				void CalculateConvex();
				void CalculateConcave();



			private:
				bool m_concave = false;
				resource::Model* m_model = nullptr;
				float m_margin = 0.0f;
			};
		}
	}
}