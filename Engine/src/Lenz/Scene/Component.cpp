#include "Component.h"

#include "Physics.h"


namespace PhysicsComponents
{

	/*
	    PhysicsComponent::~PhysicsComponent()
	{
		removeCollider();
	}

	PhysicsComponent::PhysicsComponent(PhysicsComponent&& other) noexcept {
		body = other.body;
		shape = other.shape;
		channel = other.channel;
		type = other.type;

		// Nullify the moved object
		other.body = nullptr;
		other.shape = nullptr;
		other.channel = nullptr;
	}

	PhysicsComponent& PhysicsComponent::operator=(PhysicsComponent&& other) noexcept {
		if (this != &other) {
			body = other.body;
			shape = other.shape;
			channel = other.channel;
			type = other.type;

			// Nullify the moved object
			other.body = nullptr;
			other.shape = nullptr;
			other.channel = nullptr;
		}
		return *this;
	}


	void PhysicsComponent::setColliderBox(const glm::vec3& position,
		const glm::vec3& oriantation,
		const glm::vec3& halfExtents,
		float mass,
		const glm::vec3& centerofmass_translation,
		const glm::vec3& centerofmass_orintation)
	{
		removeCollider();

		type = PhysicsEntityType::Rigid;

		shape = new btBoxShape(btVector3(halfExtents.x, halfExtents.y, halfExtents.z));

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(position.x, position.y, position.z));
		transform.setRotation(btQuaternion(oriantation.x, oriantation.y, oriantation.z));

		btTransform transform_centerofmess;
		transform_centerofmess.setIdentity();
		transform_centerofmess.setOrigin(btVector3(centerofmass_translation.x, centerofmass_translation.y, centerofmass_translation.z));
		transform_centerofmess.setRotation(btQuaternion(centerofmass_orintation.x, centerofmass_orintation.y, centerofmass_orintation.z));

		btDefaultMotionState* motionState = new btDefaultMotionState(transform, transform_centerofmess);

		btVector3 inertia(0, 0, 0);
		if (mass != 0.f)
			shape->calculateLocalInertia(mass, inertia);

		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, inertia);
		body = new btRigidBody(rbInfo);

		channel->dynamicsWorld->addRigidBody((btRigidBody*)body);
	}


	void PhysicsComponent::setColliderSphere(
		const glm::vec3& position,
		const glm::vec3& oriantation,
		float radius,
		float mass,
		const glm::vec3& centerofmass_translation,
		const glm::vec3& centerofmass_orintation)
	{
		removeCollider();

		type = PhysicsEntityType::Rigid;

		shape = new btSphereShape(radius);
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(position.x, position.y, position.z));
		transform.setRotation(btQuaternion(oriantation.x, oriantation.y, oriantation.z));

		btTransform transform_centerofmess;
		transform_centerofmess.setIdentity();
		transform_centerofmess.setOrigin(btVector3(centerofmass_translation.x, centerofmass_translation.y, centerofmass_translation.z));
		transform_centerofmess.setRotation(btQuaternion(centerofmass_orintation.x, centerofmass_orintation.y, centerofmass_orintation.z));

		btDefaultMotionState* motionState = new btDefaultMotionState(transform, transform_centerofmess);

		btVector3 inertia(0, 0, 0);
		if (mass != 0.f)
			shape->calculateLocalInertia(mass, inertia);

		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, inertia);
		body = new btRigidBody(rbInfo);

		channel->dynamicsWorld->addRigidBody((btRigidBody*)body);
	}


	void PhysicsComponent::setColliderCapsule(
		const glm::vec3& position,
		const glm::vec3& oriantation,
		float radius,
		float height,
		float mass,
		const glm::vec3& centerofmass_translation,
		const glm::vec3& centerofmass_orintation
	)
	{
		removeCollider();

		type = PhysicsEntityType::Rigid;

		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(position.x, position.y, position.z));
		transform.setRotation(btQuaternion(oriantation.x, oriantation.y, oriantation.z));

		btTransform transform_centerofmess;
		transform_centerofmess.setIdentity();
		transform_centerofmess.setOrigin(btVector3(centerofmass_translation.x, centerofmass_translation.y, centerofmass_translation.z));
		transform_centerofmess.setRotation(btQuaternion(centerofmass_orintation.x, centerofmass_orintation.y, centerofmass_orintation.z));

		btDefaultMotionState* motionState = new btDefaultMotionState(transform, transform_centerofmess);

		btVector3 inertia(0, 0, 0);
		if (mass != 0.f)
			shape->calculateLocalInertia(mass, inertia);

		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, shape, inertia);
		body = new btRigidBody(rbInfo);

		channel->dynamicsWorld->addRigidBody((btRigidBody*)body);
	}


	void PhysicsComponent::removeCollider()
	{
		if (body && shape) {
			switch (type)
			{
			case PhysicsEntityType::Rigid:
				channel->dynamicsWorld->removeRigidBody((btRigidBody*)body);
				delete ((btRigidBody*)body)->getMotionState();
				delete body;
				delete shape;
				break;
			case PhysicsEntityType::Soft:
				LZ_ERROR("Logic not implemented for this PhysicsEntity type")
					break;
			default:
				LZ_ERROR("Unknown PhyicsEntity type")
					break;
			}

			body = nullptr;
			shape = nullptr;
		}
	}


	glm::mat4 PhysicsComponent::getWorldTransformation() 
	{
		const btTransform& transform = body->getWorldTransform();

		float matrix[16];
		transform.getOpenGLMatrix(matrix);

		return glm::make_mat4(matrix);
	}


	glm::vec3 PhysicsComponent::getTranslation()
	{
		const btTransform& transform = body->getWorldTransform();

		btVector3 origin = transform.getOrigin();

		return glm::vec3(
			static_cast<float>(origin.x()),
			static_cast<float>(origin.y()),
			static_cast<float>(origin.z())
		);
	}


	glm::vec3 PhysicsComponent::getOriantation()
	{
		const btTransform& transform = body->getWorldTransform();
		btQuaternion rotation = transform.getRotation();

		return glm::vec3(
			static_cast<float>(rotation.x()),
			static_cast<float>(rotation.y()),
			static_cast<float>(rotation.z())
		);
	}
	
	*/
}