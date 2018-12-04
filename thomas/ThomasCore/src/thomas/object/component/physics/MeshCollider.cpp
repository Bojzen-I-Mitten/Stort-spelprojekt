#include "MeshCollider.h"
#include "../../../resource/Model.h"
#include <BulletCollision\CollisionShapes\btShapeHull.h>
#include "../../../graphics/Mesh.h"
#include "../../../ThomasCore.h"
namespace thomas
{

	namespace object
	{
		namespace component
		{
			MeshCollider::MeshCollider() : Collider()
			{
				m_collisionObject = nullptr;
			}
			resource::Model * MeshCollider::GetMesh()
			{
				return m_model;
			}
			void MeshCollider::SetMesh(resource::Model * value)
			{
				m_model = value;
				RecalcCollider();
			}
			void MeshCollider::OnDrawGizmosSelected()
			{
				//((btConvexHullShape*)m_collisionShape)->get
			}
			void MeshCollider::SetConcave(bool value)
			{
				if (m_concave != value)
				{
					m_concave = value;
					RecalcCollider();
				}
				
			}
			bool MeshCollider::GetConcave()
			{
				return m_concave;
			}
			void MeshCollider::Update()
			{
				if (!m_concave)
				{
					//Collider::Update();
				}

			}
			void MeshCollider::SetTrigger(bool trigger)
			{
				// Not concave?
				if (!m_concave)
				{
					if(m_enabled)	// Check enabled
						Collider::Update();
					return;
				}

				if (trigger) {
					if (m_collisionObject) {
						m_collisionObject->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
					}
				}
				else
				{
					if (m_collisionObject) {
						m_collisionObject->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
					}
				}
				m_trigger = trigger;
			}
			void MeshCollider::RecalcCollider()
			{
				if (m_collisionObject)
				{
					delete m_collisionShape;
					m_collisionShape = nullptr;
				}
				if(m_model)
				{
					if (m_concave)
						CalculateConcave();
					else
						CalculateConvex();
				}
			}
			void MeshCollider::CalculateConvex()
			{
				m_collisionShape = new btConvexHullShape();
				btConvexHullShape* temp = new btConvexHullShape();
				for (auto mesh : m_model->GetMeshes())
				{
					thomas::graphics::Vertices & verts = mesh->GetVertices();
					for (uint32_t i = 0; i < mesh->GetVertexCount(); i++)
					{
						math::Vector4 v = verts.positions[i];
						temp->addPoint(btVector3(v.x, v.y, v.z));
					}
				}
				btShapeHull* hull = new btShapeHull(temp);
				hull->buildHull(1.0f);


				const btVector3* hullPtr = hull->getVertexPointer();
				for (int i = 0; i < hull->numVertices(); i++)
					((btConvexHullShape*)m_collisionShape)->addPoint(hullPtr[i], false);


				((btConvexHullShape*)m_collisionShape)->recalcLocalAabb();

				delete temp;
				delete hull;

			}
			void MeshCollider::CalculateConcave()
			{

				btTriangleIndexVertexArray* btModel = new btTriangleIndexVertexArray();
				for (auto mesh : m_model->GetMeshes())
				{

					btVector3* vertices = new btVector3[mesh->GetVertexCount()];
					for (uint32_t i = 0; i < mesh->GetVertexCount(); i++)
					{
						vertices[i] = (btVector3&)mesh->GetVertices().positions[i];
					}

					int triangleCount = mesh->GetIndexCount() > 0 ? mesh->GetIndexCount() : mesh->GetVertexCount();
					triangleCount /= 3;

					btIndexedMesh indexedMesh;
					indexedMesh.m_numTriangles = triangleCount;
					indexedMesh.m_numVertices = mesh->GetVertexCount();
					indexedMesh.m_triangleIndexStride = 3 * sizeof(unsigned int);
					indexedMesh.m_triangleIndexBase = (unsigned char*)mesh->GetIndices().data();
					indexedMesh.m_vertexStride = sizeof(btVector3);
					indexedMesh.m_vertexBase = (unsigned char*)vertices;
					
					
					btModel->addIndexedMesh(indexedMesh);
				}

				m_collisionShape = new btBvhTriangleMeshShape(btModel, true, true);
				
			}
		}
	}
}

