#pragma once
#include "utils\Math.h"
#include <btBulletDynamicsCommon.h>
#include <memory>
#include <vector>

namespace thomas
{

	namespace graphics { class BulletDebugDraw; };
	namespace object { namespace component { class Camera; class Rigidbody; class Collider; } }
	class Physics
	{
	public:

		struct CollisionLayer
		{
			std::string name = "None";
			unsigned int mask = ~0u;
		};
		
		struct RaycastHit
		{
			thomas::object::component::Collider* collider;
			math::Vector3 normal;
			math::Vector3 point;
			float distance;
		};

		enum class COLLISION_TYPE {
			STARTED,
			STAY,
			ENDED
		};
	public:
		static bool Init();
		static void AddRigidBody(object::component::Rigidbody* rigidBody);
		static bool RemoveRigidBody(object::component::Rigidbody* rigidBody);
		static bool IsRigidbodyInWorld(object::component::Rigidbody* rigidBody);
		static void UpdateRigidbodies();
		static void Simulate();
		static void DrawDebug(object::component::Camera* camera);
		static void Destroy();
		static bool Raycast(const math::Vector3& origin, const math::Vector3& direction, RaycastHit& hitInfo, const float maxDistance = FLT_MAX);
	private:
		static void CollisionStarted(btPersistentManifold* const& manifold);
		static bool CollisionProcessed(btManifoldPoint& cp, void* body0, void* body1);
		static void CollisionEnded(btPersistentManifold* const& manifold);
		static void HandleCollision(const btCollisionObject* body0, const btCollisionObject* body1, COLLISION_TYPE collisionType);
	public:
		static graphics::BulletDebugDraw* getDebugDraw();

	public:
		static btVector3 ToBullet(const math::Vector3& vector);
		static btQuaternion ToBullet(const math::Quaternion& quaternion);
		static math::Vector3 ToSimple(const btVector3& vector);
		static math::Quaternion ToSimple(const btQuaternion& quaternion);

	public:
		static bool s_drawDebug;
		static std::unique_ptr<btDiscreteDynamicsWorld> s_world;

	public:
		static void SetCollisionLayer(std::string name, unsigned int group, unsigned int mask);
		static void SetGroupCollisionFlag(unsigned int group1, unsigned int group2, bool collide);
		static int GetCollisionGroup(std::string name);
		static std::string GetCollisionGroup(unsigned int group);
		static unsigned int GetCollisionGroupBit(unsigned int group);
		static unsigned int GetCollisionMask(unsigned int group);
		static unsigned int GetCollisionMask(std::string name);
		static unsigned int GetCollisionLayerCount();

	private:
		static std::unique_ptr<btDefaultCollisionConfiguration> s_collisionConfiguration;
		static std::unique_ptr<btCollisionDispatcher> s_dispatcher;
		static std::unique_ptr<btBroadphaseInterface> s_broadPhase;
		static std::unique_ptr<btConstraintSolver> s_solver;
		static std::unique_ptr<graphics::BulletDebugDraw> s_debugDraw;

	private:
		static float s_timeSinceLastPhysicsStep;
		static float s_accumulator;
		static float s_timeStep;

		static std::vector<object::component::Rigidbody*> s_rigidBodies;
		static CollisionLayer s_collisionLayers[32];
	};
}