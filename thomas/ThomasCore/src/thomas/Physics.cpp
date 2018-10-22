#include "..\..\..\ThomasEngine\src\Physics.h"
#include "..\..\..\ThomasEngine\src\Physics.h"
#include "..\..\..\ThomasEngine\src\Physics.h"
#include "Physics.h"
#include "ThomasTime.h"
#include "object\component\physics\Rigidbody.h"
#include "object\GameObject.h"
#include "object\component\Camera.h"
#include "graphics\BulletDebugDraw.h"
#include "AutoProfile.h"

namespace thomas
{
	std::unique_ptr<btDefaultCollisionConfiguration> Physics::s_collisionConfiguration;
	std::unique_ptr<btCollisionDispatcher> Physics::s_dispatcher;
	std::unique_ptr<btBroadphaseInterface> Physics::s_broadPhase;
	std::unique_ptr<btConstraintSolver> Physics::s_solver;
	std::unique_ptr<graphics::BulletDebugDraw> Physics::s_debugDraw;
	std::unique_ptr<btDiscreteDynamicsWorld> Physics::s_world;
	float Physics::s_timeStep = 1.0f / 60.0f; //Limit physics timestep to 60 FPS
	float Physics::s_timeSinceLastPhysicsStep = 0.0f;
	std::vector<object::component::Rigidbody*> Physics::s_rigidBodies;
	std::vector<Physics::CollisionLayer> Physics::s_collisionLayers;
	float Physics::s_accumulator;
	bool Physics::s_drawDebug = true;

	bool Physics::Init()
	{
		//Init Bullet
		s_collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
		s_dispatcher = std::make_unique<btCollisionDispatcher>(s_collisionConfiguration.get());
		s_broadPhase = std::make_unique<btDbvtBroadphase>();
		s_solver = std::make_unique<btSequentialImpulseConstraintSolver>();
		s_world = std::make_unique<btDiscreteDynamicsWorld>(s_dispatcher.get(), s_broadPhase.get(), s_solver.get(), s_collisionConfiguration.get());
		s_debugDraw = std::make_unique<graphics::BulletDebugDraw>();
		//Set states
		s_world->setGravity(btVector3(0, -9.82f, 0));
		s_debugDraw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
		s_world->setDebugDrawer(s_debugDraw.get());

		gContactStartedCallback = Physics::CollisionStarted;
		gContactProcessedCallback = Physics::CollisionProcessed;
		gContactEndedCallback = Physics::CollisionEnded;
		
		s_debugDraw->setDebugMode(btIDebugDraw::DBG_DrawConstraintLimits );


		s_collisionLayers.push_back(CollisionLayer("Default", 0));

		return true;
	}
	void Physics::AddRigidBody(object::component::Rigidbody * rigidBody, int collisionLayerIndex)
	{

		int size = s_rigidBodies.size();
		s_rigidBodies.push_back(rigidBody);
		s_world->addRigidBody(rigidBody, s_collisionLayers[collisionLayerIndex].groupBit, s_collisionLayers[collisionLayerIndex].collisionMask);
		
	}
	bool Physics::RemoveRigidBody(object::component::Rigidbody * rigidBody)
	{
		int size = s_rigidBodies.size();
		bool found = false;
		for (unsigned i = 0; i < s_rigidBodies.size(); ++i)
		{
			object::component::Rigidbody* rb = s_rigidBodies[i];
			if (rb == rigidBody)
			{
				s_rigidBodies.erase(s_rigidBodies.begin() + i);
				found = true;
				break;
			}
		}
		s_world->removeRigidBody(rigidBody);
		return found;
	}

	void Physics::UpdateRigidbodies()
	{
		PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
		for (object::component::Rigidbody* rb : s_rigidBodies)
		{
			rb->UpdateTransformToRigidBody();
		}
	}

	//Update physics collision
	void Physics::Simulate()
	{
		s_timeSinceLastPhysicsStep += ThomasTime::GetDeltaTime();
		if (s_timeSinceLastPhysicsStep < s_timeStep)
			return;
		s_world->stepSimulation(s_timeSinceLastPhysicsStep, 5, s_timeStep);	
		for (object::component::Rigidbody* rb : s_rigidBodies)
		{
			rb->UpdateRigidbodyToTransform();
		}
		s_timeSinceLastPhysicsStep = 0.f;
	}


	void Physics::DrawDebug(object::component::Camera* camera)
	{
#ifdef _DEBUG
		if (!s_drawDebug)
			return;
		s_world->debugDrawWorld();
#endif
	}

	void Physics::Destroy()
	{
		btCollisionObjectArray arr = s_world->getCollisionObjectArray();
		arr.clear();
		s_collisionConfiguration.reset();
		s_debugDraw.reset();
		s_dispatcher.reset();
		s_solver.reset();
		s_world.reset();		
	}

	bool Physics::Raycast(const math::Vector3& origin, const math::Vector3& direction, RaycastHit& hitInfo, const float maxDistance)
	{
		btVector3 start = ToBullet(origin);
		btVector3 end = ToBullet(origin + direction * maxDistance);
		btCollisionWorld::ClosestRayResultCallback rayCallback(start, end);
		s_world->rayTest(start, end, rayCallback);
		if (rayCallback.hasHit()) 
		{
			hitInfo.collider = reinterpret_cast<object::component::Collider*>(rayCallback.m_collisionObject->getUserPointer());
			hitInfo.point = ToSimple(rayCallback.m_hitPointWorld);
			hitInfo.normal = ToSimple(rayCallback.m_hitNormalWorld);
			hitInfo.distance = maxDistance * rayCallback.m_closestHitFraction;
			return true;
		}
		return false;
		
	}


	void Physics::CollisionStarted(btPersistentManifold * const & manifold)
	{		
		HandleCollision(manifold->getBody0(), manifold->getBody1(), COLLISION_TYPE::STARTED);
	}

	bool Physics::CollisionProcessed(btManifoldPoint & cp, void * body0, void * body1)
	{
		btCollisionObject* obA = reinterpret_cast<btCollisionObject*>(body0);
		btCollisionObject* obB = reinterpret_cast<btCollisionObject*>(body1);

		HandleCollision(obA, obB, COLLISION_TYPE::STAY);
		return false;
	}

	void Physics::CollisionEnded(btPersistentManifold * const & manifold)
	{
		btCollisionObject* obA = (btCollisionObject*)manifold->getBody0();
		btCollisionObject* obB = (btCollisionObject*)manifold->getBody1();

		HandleCollision(manifold->getBody0(), manifold->getBody1(), COLLISION_TYPE::ENDED);
	}

	void Physics::HandleCollision(const btCollisionObject* body0, const btCollisionObject* body1, COLLISION_TYPE collisionType)
	{
		// Avoid self collisions
		if (body0 == body1) return;

		object::component::Collider* colliderA = reinterpret_cast<object::component::Collider*>(body0->getUserPointer());
		object::component::Collider* colliderB = reinterpret_cast<object::component::Collider*>(body1->getUserPointer());

		if(colliderA && colliderB && !colliderA->isDestroyed() && !colliderB->isDestroyed())
		{
			colliderA->OnCollision(colliderB, collisionType);
			colliderB->OnCollision(colliderA, collisionType);
		}
	}

	graphics::BulletDebugDraw * Physics::getDebugDraw()
	{
		if (s_debugDraw)
			return s_debugDraw.get();
		else
			return nullptr;
	}

	btVector3 Physics::ToBullet(const math::Vector3 & vector)
	{
		//TODO: Fix this.
		return btVector3(vector.x, vector.y, vector.z);
		//return *(btVector3*)&vector;
	}

	math::Vector3 Physics::ToSimple(const btVector3 & vector)
	{
		return (math::Vector3)vector;
	}

	btQuaternion Physics::ToBullet(const math::Quaternion& quaternion)
	{
		return *(btQuaternion*)&quaternion;
	}

	math::Quaternion Physics::ToSimple(const btQuaternion & quaternion)
	{
		return (math::Quaternion)quaternion;
	}
	int Physics::GetCollisionLayerCount()
	{
		return s_collisionLayers.size();;
	}
	Physics::CollisionLayer & Physics::GetLayer(int index)
	{
		return s_collisionLayers[index];
	}
}
