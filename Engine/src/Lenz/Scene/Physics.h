#pragma once
#define BUILD_VHACD
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <btBulletDynamicsCommon.h>
#include <unordered_map>

#include "Entity.h"
#include "Scene.h"

#include <vhacd.h>


namespace lenz
{
    typedef VHACD::IVHACD::Parameters VHACDparams;

    class PhysicsSystem {
    public:
        static constexpr int MAX_SUBSTEPS = 10;
        static constexpr float DEFAULT_GROUND_Y = -5.0f;
        static constexpr float DEFAULT_GRAVITY = -9.81f;

        PhysicsSystem();
        ~PhysicsSystem();

        void Update(float deltaTime);

        void AddEntity(Entity entity);

        void RemoveEntity(Entity entity);

        void SetGroundPlane(float Y);

        void SetGravity(glm::vec3 vec);

        inline glm::vec3 GetGroundPlaneNormal() const { btVector3 btvec = groundPlane->getPlaneNormal(); return glm::vec3{ btvec.getX(), btvec.getY(), btvec.getZ()}; }

        inline float GetGroundPlaneConstant() const { return groundPlane->getPlaneConstant(); }


        btBroadphaseInterface* broadphase{ nullptr };
        btDefaultCollisionConfiguration* collisionConfiguration{ nullptr };
        btCollisionDispatcher* dispatcher{ nullptr };
        btSequentialImpulseConstraintSolver* solver{ nullptr };
        btDiscreteDynamicsWorld* dynamicsWorld{ nullptr };

        btStaticPlaneShape* groundPlane{ nullptr };
        btRigidBody* groundRigidBody{ nullptr };

        Scene* scene;
    };


	enum struct PhysicsEntityType {
		Rigid, Soft, Kinematic
	};


    struct PhysicsSpecification {
        PhysicsSpecification() = default;
        ~PhysicsSpecification();

        PhysicsSpecification(PhysicsSpecification&& other) noexcept;
        PhysicsSpecification& operator=(PhysicsSpecification&& other) noexcept;

        void setColliderBox(
            const glm::vec3& position,
            const glm::vec3& orientation,
            const glm::vec3& halfExtents,
            float mass,
            const glm::vec3& centerofmass_translation = glm::vec3(0),
            const glm::vec3& centerofmass_orientation = glm::vec3(0)
        );

        void setColliderSphere(
            const glm::vec3& position,
            const glm::vec3& orientation,
            float radius,
            float mass,
            const glm::vec3& centerofmass_translation = glm::vec3(0),
            const glm::vec3& centerofmass_orientation = glm::vec3(0)
        );

        void setColliderCapsule(
            const glm::vec3& position,
            const glm::vec3& orientation,
            float radius,
            float height,
            float mass,
            const glm::vec3& centerofmass_translation = glm::vec3(0),
            const glm::vec3& centerofmass_orientation = glm::vec3(0)
        );

        void setConvexHullCollider(
            const glm::vec3& position,
            const glm::vec3& orientation,
            const std::vector<float>& points,
            float mass,
            bool optimize = true,
            const glm::vec3& centerofmass_translation = glm::vec3(0),
            const glm::vec3& centerofmass_orientation = glm::vec3(0)
        );

        void setHeightMapCollider(
            const glm::vec3& position,
            const glm::vec3& orientation,
            const glm::vec2& size,
            const std::vector<float>& heightData,
            float mass,
            const glm::vec3& centerofmass_translation = glm::vec3(0),
            const glm::vec3& centerofmass_orientation = glm::vec3(0)
        );

        void setVHACDCollider(
            const glm::vec3& position,
            const glm::vec3& orientation,
            const std::vector<float>& points,
            const std::vector<uint32_t>& indices,
            float mass,
            VHACDparams specs,
            const glm::vec3& centerofmass_translation = glm::vec3(0),
            const glm::vec3& centerofmass_orientation = glm::vec3(0)
        );

        void setKinematic(bool enable);
        void updateKinematicTransform(const glm::vec3& position, const glm::quat& orientation);

        void invalidateCollider(
            const glm::vec3& position,
            const glm::vec3& orientation,
            float mass,
            const glm::vec3& centerofmass_translation = glm::vec3(0),
            const glm::vec3& centerofmass_orientation = glm::vec3(0)
        );
        void removeCollider();

        void applyForce(const glm::vec3& force);
        void applyImpulse(const glm::vec3& impulse);
        void setLinearVelocity(const glm::vec3& vel);
        void setAngularVelocity(const glm::vec3& vel);
        void setPosition(const glm::vec3& position);
        void setOrientation(const glm::quat& orientation);
        void setScale(const glm::vec3& scale);

        void setDamping(float linearDamping, float angularDamping);
        void setStatus(bool active);

        template<typename T>
        void setUserPointer(T* _t)
        {
            if (body != nullptr) body->setUserPointer(_t);
        }

        glm::vec3 getLinearVelocity() const;
        glm::vec3 getAngularVelocity() const;
        glm::mat4 getWorldTransformation() const;
        glm::vec3 getTranslation() const;
        glm::vec3 getOrientation() const;

        btCollisionObject* body = nullptr;
        btCollisionShape* shape = nullptr;
        PhysicsSystem* channel = nullptr;
        PhysicsEntityType type = PhysicsEntityType::Rigid;
    };

}

