#include "Physics.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Component.h"

#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>


namespace lenz {

    PhysicsSystem::PhysicsSystem() {
        auto broadphase = new btDbvtBroadphase();
        collisionConfiguration = new btDefaultCollisionConfiguration();
        dispatcher = new btCollisionDispatcher(collisionConfiguration);
        solver = new btSequentialImpulseConstraintSolver();
        dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

		SetGravity({ 0, DEFAULT_GRAVITY, 0 });
		SetGroundPlane(DEFAULT_GROUND_Y);
    }

    
	PhysicsSystem::~PhysicsSystem() {
        delete dynamicsWorld;
        delete solver;
        delete dispatcher;
        delete collisionConfiguration;
        delete broadphase;
		delete groundPlane;
    }

    
	void PhysicsSystem::Update(float deltaTime) {
        dynamicsWorld->stepSimulation(deltaTime, 10);
    }

    
	void PhysicsSystem::AddEntity(PhysicsComponent& comp) {
		comp.dynamicsWorld = this->dynamicsWorld;
    }


	void PhysicsSystem::RemoveEntity(const PhysicsComponent& comp) {
		dynamicsWorld->removeCollisionObject(comp.body);
	}


	void PhysicsSystem::SetGroundPlane(float Y)
	{
		if (groundPlane)
		{
			dynamicsWorld->removeRigidBody(groundRigidBody);
			delete groundRigidBody;
			delete groundPlane;
		}
		groundPlane = new btStaticPlaneShape(btVector3(0, 1, 0), Y);
		btDefaultMotionState* groundMotionState = new btDefaultMotionState(
			btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, -10, 0)));
		btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundPlane, btVector3(0, 0, 0));
		groundRigidBody = new btRigidBody(groundRigidBodyCI);
		dynamicsWorld->addRigidBody(groundRigidBody);
	}


	void PhysicsSystem::SetGravity(glm::vec3 vec)
	{
		dynamicsWorld->setGravity(btVector3(vec.x, vec.y, vec.z));
	}



	PhysicsComponent::PhysicsComponent(void* gpuPtr, const btTransform& startTrans, const btTransform& centerOfMassOffset)
		: motionState(new MappedMotionState(gpuPtr, startTrans, centerOfMassOffset))
	{
	}

	PhysicsComponent::~PhysicsComponent()
	{
		if (body != nullptr)
			dynamicsWorld->removeCollisionObject(body);
		removeCollider();

		delete(motionState);
	}



	PhysicsComponent::PhysicsComponent(PhysicsComponent&& other) noexcept 
	{
		body = std::move(other.body);
		shape = std::move(other.shape);
		type = std::move(other.type);
		motionState = std::move(other.motionState);

		other.body = nullptr;
		other.shape = nullptr;
		other.motionState = nullptr;
	}

	
	PhysicsComponent& PhysicsComponent::operator=(PhysicsComponent&& other) noexcept {
		if (this != &other) {
			body = other.body;
			shape = other.shape;
			dynamicsWorld = other.dynamicsWorld;
			type = other.type;

			other.body = nullptr;
			other.shape = nullptr;
			other.dynamicsWorld = nullptr;
		}
		return *this;
	}

	void PhysicsComponent::invalidateCollider(
		const glm::vec3& position,
		const glm::vec3& orientation,
		float mass,
		const glm::vec3& centerofmass_translation,
		const glm::vec3& centerofmass_orientation
	)
	{
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(position.x, position.y, position.z));

		btQuaternion q;
		q.setEulerZYX(orientation.z, orientation.y, orientation.x);
		transform.setRotation(q);

		btTransform transform_centerofmass;
		transform_centerofmass.setIdentity();
		transform_centerofmass.setOrigin(btVector3(centerofmass_translation.x, centerofmass_translation.y, centerofmass_translation.z));

		btQuaternion q2;
		q2.setEulerZYX(centerofmass_orientation.z, centerofmass_orientation.y, centerofmass_orientation.x);
		transform_centerofmass.setRotation(q2);

		motionState->Invalidate(transform, transform_centerofmass);

		//btMotionState* defaultMotionState = new btDefaultMotionState(transform, transform_centerofmass);

		btVector3 inertia(0, 0, 0);
		if (mass != 0.f)
			shape->calculateLocalInertia(mass, inertia);

		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, inertia);
		body = new btRigidBody(rbInfo);

		dynamicsWorld->addRigidBody((btRigidBody*)body);
	}



	void PhysicsComponent::setColliderBox(const glm::vec3& position,
		const glm::vec3& orientation,
		const glm::vec3& halfExtents,
		float mass,
		const glm::vec3& centerofmass_translation,
		const glm::vec3& centerofmass_orientation)
	{
		removeCollider();

		type = PhysicsEntityType::Rigid;
		shape = new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z));
		invalidateCollider(position, orientation, mass, centerofmass_translation, centerofmass_orientation);
	}


	void PhysicsComponent::setColliderSphere(
		const glm::vec3& position,
		const glm::vec3& orientation,
		float radius,
		float mass,
		const glm::vec3& centerofmass_translation,
		const glm::vec3& centerofmass_orientation)
	{
		removeCollider();

		type = PhysicsEntityType::Rigid;
		shape = new btSphereShape(radius);
		invalidateCollider(position, orientation, mass, centerofmass_translation, centerofmass_orientation);
	}


	void PhysicsComponent::setColliderCapsule(
		const glm::vec3& position,
		const glm::vec3& orientation,
		float radius,
		float height,
		float mass,
		const glm::vec3& centerofmass_translation,
		const glm::vec3& centerofmass_orientation
	)
	{
		removeCollider();

		type = PhysicsEntityType::Rigid;
		shape = new btCapsuleShape(radius, height);
		invalidateCollider(position, orientation, mass, centerofmass_translation, centerofmass_orientation);
	}

	float findMin(const std::vector<float>& h) {
		return *std::min_element(h.begin(), h.end());
	}

	float findMax(const std::vector<float>& h) {
		return *std::max_element(h.begin(), h.end());
	}


	void PhysicsComponent::setHeightMapCollider(
		const glm::vec3& position,
		const glm::vec3& orientation,
		const glm::vec2& size,
		const std::vector<float>& heightData,
		float mass,
		const glm::vec3& centerofmass_translation,
		const glm::vec3& centerofmass_orientation
	)
	{
		assert(heightData.size() == size.x * size.y);
		removeCollider();

		shape = new btHeightfieldTerrainShape(
			size.x,           
			size.y,          
			heightData.data(),
			1.0f,            
			0.0f,            
			0.0f,            
			1,               
			PHY_FLOAT,       
			false            
		);

		invalidateCollider(position, orientation, mass, centerofmass_translation, centerofmass_orientation);
	}


	void PhysicsComponent::setConvexHullCollider(
		const glm::vec3& position,
		const glm::vec3& orientation,
		const std::vector<float>& points,
		float mass,
		bool optimize, 
		const glm::vec3& centerofmass_translation,
		const glm::vec3& centerofmass_orientation
	)
	{
		removeCollider();

		auto* shapeptr = new btConvexHullShape();
		for (size_t i = 0; i < points.size(); i += 3) {
			shapeptr->addPoint(btVector3(points[i], points[i + 1], points[i + 2]), false);
		}
		shapeptr->recalcLocalAabb();
		if (optimize) shapeptr->optimizeConvexHull();
		shape = shapeptr;
		invalidateCollider(position, orientation, mass, centerofmass_translation, centerofmass_orientation);
	}


	void PhysicsComponent::setVHACDCollider(
		const glm::vec3& position,
		const glm::vec3& orientation,
		const std::vector<float>& points,
		const std::vector<uint32_t>& indices,
		float mass,
		VHACDparams params,
		const glm::vec3& centerofmass_translation,
		const glm::vec3& centerofmass_orientation
	) {
		using namespace VHACD;

		IVHACD* interfaceVHACD = CreateVHACD();

		if (!interfaceVHACD->Compute(
			points.data(), points.size() / 3,
			indices.data(), indices.size() / 3,
			params)) {
			std::cerr << "VHACD computation failed.\n";
		}

		btCompoundShape* compoundShape = new btCompoundShape();

		uint32_t hullCount = interfaceVHACD->GetNConvexHulls();
		for (uint32_t i = 0; i < hullCount; ++i) {
			VHACD::IVHACD::ConvexHull hull;
			interfaceVHACD->GetConvexHull(i, hull);

			btConvexHullShape* part = new btConvexHullShape();

			for (const auto& vertex : hull.m_points) {
				part->addPoint(btVector3(vertex.mX, vertex.mY, vertex.mZ), false);
			}

			part->recalcLocalAabb();

			btTransform localTransform;
			localTransform.setIdentity();
			compoundShape->addChildShape(localTransform, part);
		}

		shape = compoundShape;
		invalidateCollider(position, orientation, mass, centerofmass_translation, centerofmass_orientation);

		interfaceVHACD->Clean();
		interfaceVHACD->Release();

	}


    void PhysicsComponent::setKinematic(bool enable) {
        if (btRigidBody* rb = btRigidBody::upcast(body)) {
            if (enable) {
                rb->setCollisionFlags(rb->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
                rb->setActivationState(DISABLE_DEACTIVATION);
                type = PhysicsEntityType::Kinematic;
            }
            else {
                rb->setCollisionFlags(rb->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
                rb->setActivationState(ACTIVE_TAG);
                type = PhysicsEntityType::Rigid;
            }
        }
    }


    void PhysicsComponent::updateKinematicTransform(const glm::vec3& position, const glm::quat& orientation) {
        if (type == PhysicsEntityType::Kinematic && body) {
            btTransform transform;
            transform.setOrigin(btVector3(position.x, position.y, position.z));
            transform.setRotation(btQuaternion(orientation.x, orientation.y, orientation.z, orientation.w));
            body->setWorldTransform(transform);
            if (btRigidBody::upcast(body)->getMotionState()) {
				btRigidBody::upcast(body)->getMotionState()->setWorldTransform(transform);
            }
        }
    }


    void PhysicsComponent::removeCollider() {
        if (dynamicsWorld && body) {
            if (btRigidBody* rb = btRigidBody::upcast(body)) {
                dynamicsWorld->removeRigidBody(rb);
            }
            else {
                dynamicsWorld->removeCollisionObject(body);
            }
            delete body;
            body = nullptr;

            delete shape;
            shape = nullptr;
        }
    }


    void PhysicsComponent::applyForce(const glm::vec3& force) {
        if (btRigidBody* rb = btRigidBody::upcast(body)) {
            rb->applyCentralForce(btVector3(force.x, force.y, force.z));
        }
    }


    void PhysicsComponent::applyImpulse(const glm::vec3& impulse) {
        if (btRigidBody* rb = btRigidBody::upcast(body)) {
            rb->applyCentralImpulse(btVector3(impulse.x, impulse.y, impulse.z));
        }
    }


    void PhysicsComponent::setLinearVelocity(const glm::vec3& vel) {
        if (btRigidBody* rb = btRigidBody::upcast(body)) {
            rb->setLinearVelocity(btVector3(vel.x, vel.y, vel.z));
        }
    }

    
	glm::vec3 PhysicsComponent::getLinearVelocity() const {
        if (btRigidBody* rb = btRigidBody::upcast(body)) {
            btVector3 v = rb->getLinearVelocity();
            return glm::vec3(v.getX(), v.getY(), v.getZ());
        }
        return glm::vec3(0.0f);
    }

    
	void PhysicsComponent::setAngularVelocity(const glm::vec3& vel) {
        if (btRigidBody* rb = btRigidBody::upcast(body)) {
            rb->setAngularVelocity(btVector3(vel.x, vel.y, vel.z));
        }
    }

	void PhysicsComponent::setPosition(const glm::vec3& position)
	{
		if (!body) return;

		btTransform transform = body->getWorldTransform();
		transform.setOrigin(btVector3(position.x, position.y, position.z));
		body->setWorldTransform(transform);

		if (auto* motionState = dynamic_cast<btRigidBody*>(body)->getMotionState())
		{
			motionState->setWorldTransform(transform);
		}
	}

	void PhysicsComponent::setOrientation(const glm::quat& orientation)
	{
		if (!body) return;

		btTransform transform = body->getWorldTransform();
		btQuaternion rot(orientation.x, orientation.y, orientation.z, orientation.w);
		transform.setRotation(rot);
		body->setWorldTransform(transform);

		if (auto* motionState = dynamic_cast<btRigidBody*>(body)->getMotionState())
		{
			motionState->setWorldTransform(transform);
		}
	}

	void PhysicsComponent::setScale(const glm::vec3& scale)
	{
		if (!shape || !body) return;

		shape->setLocalScaling(btVector3(scale.x, scale.y, scale.z));
		body->activate(); // Reactivate body if sleeping

		// If needed, re-initialize inertia and mass if dynamic body
		if (auto* rigid = dynamic_cast<btRigidBody*>(body))
		{
			btVector3 inertia(0, 0, 0);
			if (rigid->getInvMass() > 0.0f)
			{
				shape->calculateLocalInertia(1.0f / rigid->getInvMass(), inertia);
				rigid->setMassProps(1.0f / rigid->getInvMass(), inertia);
			}
		}
	}


    
	glm::vec3 PhysicsComponent::getAngularVelocity() const {
        if (btRigidBody* rb = btRigidBody::upcast(body)) {
            btVector3 v = rb->getAngularVelocity();
            return glm::vec3(v.getX(), v.getY(), v.getZ());
        }
        return glm::vec3(0.0f);
    }


	glm::mat4 PhysicsComponent::getWorldTransformation() const {

		const btTransform& transform = body->getWorldTransform();

		float matrix[16];
		transform.getOpenGLMatrix(matrix);

		return glm::make_mat4(matrix);
	}


	glm::vec3 PhysicsComponent::getTranslation() const {

		const btTransform& transform = body->getWorldTransform();

		btVector3 origin = transform.getOrigin();

		return glm::vec3(
			static_cast<float>(origin.x()),
			static_cast<float>(origin.y()),
			static_cast<float>(origin.z())
		);
	}


	glm::vec3 PhysicsComponent::getOrientation() const {

		const btTransform& transform = body->getWorldTransform();
		btQuaternion rotation = transform.getRotation();

		return glm::vec3(
			static_cast<float>(rotation.x()),
			static_cast<float>(rotation.y()),
			static_cast<float>(rotation.z())
		);
	}

    void PhysicsComponent::setDamping(float linearDamping, float angularDamping) {
        if (btRigidBody* rb = btRigidBody::upcast(body)) {
            rb->setDamping(linearDamping, angularDamping);
        }
    }

	void PhysicsComponent::setStatus(bool isactive) {
		btRigidBody::upcast(body)->activate(isactive);
	}

}