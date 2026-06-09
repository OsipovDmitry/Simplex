#ifndef PHYSICS_BULLET_3_RENDERER_H
#define PHYSICS_BULLET_3_RENDERER_H

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <utils/noncopyble.h>

#include <core/physicsrendererbase.h>

#include <btBulletDynamicsCommon.h>
#include <physics_bullet/forwarddecl.h>

#define CURRENT_CONTEXT_INFO                                                                                                     \
protected:                                                                                                                       \
    std::shared_ptr<core::physics::RendererBase> m_renderer;                                                                     \
                                                                                                                                 \
public:                                                                                                                          \
    std::shared_ptr<core::physics::RendererBase> renderer() const                                                                \
    {                                                                                                                            \
        return m_renderer;                                                                                                       \
    };

#define SAVE_CURRENT_CONTEXT                                                                                                     \
    m_renderer = core::physics::RendererBase::current();                                                                         \
    if (!m_renderer) LOG_CRITICAL << "No current context";

#define CHECK_CURRENT_CONTEXT                                                                                                    \
    if (m_renderer != core::physics::RendererBase::current()) LOG_CRITICAL << "Resource was created in anotrher context";

#define CHECK_THIS_CONTEXT                                                                                                       \
    if (shared_from_this() != core::physics::RendererBase::current()) LOG_CRITICAL << "This context is not current";

#define CHECK_RESOURCE_CONTEXT(resource)                                                                                         \
    if (shared_from_this() != resource->renderer()) LOG_CRITICAL << "Resource was created in anotrher context";

#define CHECK_EQUAL_CONTEXTS(resource1, resource2)                                                                               \
    if (resource1->renderer() != resource2->renderer()) LOG_CRITICAL << "Resources were created in different contexts";

namespace simplex
{
namespace physics_bullet
{

class Bullet_3_Renderer;
class DynamicBodyMotionState_3;
class BodyUserPointer_3;
class ConstraintBase_3;

class Conversions
{
public:
    static btScalar float2Bt(float);
    static btVector3 vec2Bt(const glm::vec3&);
    static btQuaternion quat2Bt(const glm::quat&);
    static btTransform rigidTransform2Bt(const utils::RigidTransform&);
    static std::pair<btScalar, btTransform> transform2Bt(const utils::Transform&);

    static float bt2Float(btScalar);
    static glm::vec3 bt2Vec(const btVector3&);
    static glm::quat bt2Quat(const btQuaternion&);
    static utils::RigidTransform bt2RigidTRansform(const btTransform&);
};

class BodyBase_3 : public std::enable_shared_from_this<BodyBase_3>, public virtual core::physics::IBody
{
    NONCOPYBLE(BodyBase_3)
    CURRENT_CONTEXT_INFO
public:
    ~BodyBase_3() override;

    const utils::Transform& transform() const override;
    void setTransform(const utils::Transform&) override;

    float restitution() const override;
    void setRestitution(float) override;

    float linearDamping() const override;
    void setLinearDamping(float) override;

    float angularDamping() const override;
    void setAngularDamping(float) override;

    float linearSleepingThreshold() const override;
    void setLinearSleepingThreshold(float) override;

    float angularSleepingThreshold() const override;
    void setAngularSleepingThreshold(float) override;

    void removeShape(utils::IDsGenerator::value_type) override;
    utils::IDsGenerator::value_type addSphereShape(
        float radius,
        const utils::RigidTransform& localTransform = utils::RigidTransform()) override;
    utils::IDsGenerator::value_type addBoxShape(
        const glm::vec3& halfSizes,
        const utils::RigidTransform& localTransform = utils::RigidTransform()) override;
    utils::IDsGenerator::value_type addCapsuleShape(
        float radius,
        float height,
        const utils::RigidTransform& localTransform = utils::RigidTransform()) override;
    utils::IDsGenerator::value_type addCylinderShape(
        const glm::vec3& halfSizes,
        const utils::RigidTransform& localTransform = utils::RigidTransform()) override;
    utils::IDsGenerator::value_type addConeShape(
        float radius,
        float height,
        const utils::RigidTransform& localTransform = utils::RigidTransform()) override;
    utils::IDsGenerator::value_type addConvexHullShape(
        const std::vector<glm::vec3>& vertices,
        const utils::RigidTransform& localTransform = utils::RigidTransform()) override;

    const std::set<std::shared_ptr<core::physics::IConstraint>>& constraints() const override;
    std::shared_ptr<core::physics::IFixedConstraint> joinFixed(
        const utils::RigidTransform& frame,
        const std::shared_ptr<core::physics::IBody>& anotherBody,
        const utils::RigidTransform& anotherFrame) override;
    std::shared_ptr<core::physics::IPointToPointConstraint> joinPointToPoint(
        const glm::vec3& pivot,
        const std::shared_ptr<core::physics::IBody>& anotherBody = nullptr,
        const glm::vec3& anotherPivot = glm::vec3(0.f)) override;
    std::shared_ptr<core::physics::IHingeConstraint> joinHinge(
        const utils::RigidTransform& frame,
        const std::shared_ptr<core::physics::IBody>& anotherBody = nullptr,
        const utils::RigidTransform& anotherFrame = utils::RigidTransform()) override;
    std::shared_ptr<core::physics::ISliderConstraint> joinSlider(
        const utils::RigidTransform& frame,
        const std::shared_ptr<core::physics::IBody>& anotherBody = nullptr,
        const utils::RigidTransform& anotherFrame = utils::RigidTransform()) override;

    void setDynamicWorld(const std::weak_ptr<btDiscreteDynamicsWorld>&);

    std::shared_ptr<btRigidBody>& rigidBody();

    void removeConstraint(const std::shared_ptr<ConstraintBase_3>&);
    void addConstraint(const std::shared_ptr<ConstraintBase_3>&);

    static void addConstraintToWorld(ConstraintBase_3&);

protected:
    explicit BodyBase_3(float mass);

    utils::IDsGenerator::value_type addCollisionShape(
        const std::shared_ptr<btCollisionShape>& collisionShape,
        const utils::RigidTransform& localTransform);

    void removeFromWorld();
    void addToWorld();
    void resetVelocitiesAndForces();
    void updateCompoundShape();
    void updateLocalInertia();
    void updateTransform();

    std::weak_ptr<btDiscreteDynamicsWorld> m_dynamicWorld;

    utils::Transform m_transform;
    bool m_isTransformChanged = false;

    std::shared_ptr<btCompoundShape> m_compoundShape;
    std::shared_ptr<btRigidBody> m_rigidBody;
    std::shared_ptr<BodyUserPointer_3> m_bodyUserPointer;

    std::vector<std::pair<btTransform, std::shared_ptr<btCollisionShape>>> m_collisionShapes;
    utils::IDsGenerator m_collisionShapeIDsGenerator;

    std::set<std::shared_ptr<core::physics::IConstraint>> m_constraints;
};

class StaticBody_3 : public BodyBase_3, public core::physics::IStaticBody
{
    NONCOPYBLE(StaticBody_3)
public:
    StaticBody_3();
    ~StaticBody_3() override;

    utils::IDsGenerator::value_type addTriangleMeshShape(
        const std::vector<glm::vec3>& vertices,
        const std::vector<uint32_t>& indices,
        const utils::RigidTransform& localTransform = utils::RigidTransform()) override;
    utils::IDsGenerator::value_type addPlaneShape(
        const utils::Plane& plane,
        const utils::RigidTransform& localTransform = utils::RigidTransform()) override;
    utils::IDsGenerator::value_type addHeightfieldShape(
        const utils::RigidTransform& localTransform = utils::RigidTransform()) override;
};

class KinematicBody_3 : public BodyBase_3, public core::physics::IKinematicBody
{
    NONCOPYBLE(KinematicBody_3)
public:
    KinematicBody_3();
    ~KinematicBody_3() override;
};

class DynamicBody_3 : public BodyBase_3, public core::physics::IDynamicBody
{
    NONCOPYBLE(DynamicBody_3)
public:
    DynamicBody_3();
    ~DynamicBody_3() override;

    float mass() const override;
    void setMass(float) override;

    void applyForce(const glm::vec3& force, const glm::vec3& localPosition) override;
    void applyImpulse(const glm::vec3& impulse, const glm::vec3& localPosition) override;

    std::pair<bool, utils::Transform> resetTransformChangedAfterSimulation() override;

    void updateWorldTransform(const btTransform&);

private:
    std::shared_ptr<DynamicBodyMotionState_3> m_motionState;
};

class ConstraintBase_3 : public std::enable_shared_from_this<ConstraintBase_3>, public virtual core::physics::IConstraint
{
    NONCOPYBLE(ConstraintBase_3)
    CURRENT_CONTEXT_INFO
public:
    virtual ~ConstraintBase_3();

    std::weak_ptr<core::physics::IBody> body1() override;
    std::weak_ptr<const core::physics::IBody> body1() const override;

    std::weak_ptr<core::physics::IBody> body2() override;
    std::weak_ptr<const core::physics::IBody> body2() const override;

    void detachFromBodies() override;

    bool isCollisionBetweenLinkedBodiesEnabled() const override;
    void enableCollisionBetweenLinkedBodies(bool = true) override;

    void setDynamicWorld(const std::weak_ptr<btDiscreteDynamicsWorld>&);

    std::weak_ptr<BodyBase_3>& bodyBase1();
    std::weak_ptr<BodyBase_3>& bodyBase2();

protected:
    ConstraintBase_3(
        const std::shared_ptr<btTypedConstraint>&,
        const std::weak_ptr<BodyBase_3>&,
        const std::weak_ptr<BodyBase_3>&);

    void removeFromBodies();
    void addToBodies();
    void removeFromWorld();
    void addToWorld();

    std::weak_ptr<btDiscreteDynamicsWorld> m_dynamicWorld;
    std::shared_ptr<btTypedConstraint> m_typedConstraint;

    std::weak_ptr<BodyBase_3> m_bodyBase1;
    std::weak_ptr<BodyBase_3> m_bodyBase2;

    bool m_isCollisionBetweenLinkedBodiesEnabled = true;
};

class FixedConstraint_3 : public ConstraintBase_3, public core::physics::IFixedConstraint
{
public:
    FixedConstraint_3(
        const std::shared_ptr<BodyBase_3>&,
        const utils::RigidTransform&,
        const std::shared_ptr<BodyBase_3>&,
        const utils::RigidTransform&);
    ~FixedConstraint_3() override;

    utils::RigidTransform frame1() const override;
    utils::RigidTransform frame2() const override;
};

class PointToPointConstraint_3 : public ConstraintBase_3, public core::physics::IPointToPointConstraint
{
public:
    PointToPointConstraint_3(
        const std::shared_ptr<BodyBase_3>&,
        const glm::vec3&,
        const std::shared_ptr<BodyBase_3>&,
        const glm::vec3&);
    PointToPointConstraint_3(const std::shared_ptr<BodyBase_3>&, const glm::vec3&);
    ~PointToPointConstraint_3() override;

    glm::vec3 pivot1() const override;
    glm::vec3 pivot2() const override;
};

class HingeConstraint_3 : public ConstraintBase_3, public core::physics::IHingeConstraint
{
public:
    HingeConstraint_3(
        const std::shared_ptr<BodyBase_3>&,
        const utils::RigidTransform&,
        const std::shared_ptr<BodyBase_3>&,
        const utils::RigidTransform&);
    HingeConstraint_3(const std::shared_ptr<BodyBase_3>&, const utils::RigidTransform&);
    ~HingeConstraint_3() override;

    utils::RigidTransform frame1() const override;
    utils::RigidTransform frame2() const override;

    utils::Range angularLimits() const override;
    void setAngularLimits(const utils::Range&) override;
};

class SliderConstraint_3 : public ConstraintBase_3, public core::physics::ISliderConstraint
{
public:
    SliderConstraint_3(
        const std::shared_ptr<BodyBase_3>&,
        const utils::RigidTransform&,
        const std::shared_ptr<BodyBase_3>&,
        const utils::RigidTransform&);
    SliderConstraint_3(const std::shared_ptr<BodyBase_3>&, const utils::RigidTransform&);
    ~SliderConstraint_3() override;

    utils::RigidTransform frame1() const override;
    utils::RigidTransform frame2() const override;

    utils::Range linearLimits() const override;
    void setLinearLimits(const utils::Range&) override;

    utils::Range angularLimits() const override;
    void setAngularLimits(const utils::Range&) override;
};

class Bullet_3_Renderer : public core::physics::RendererBase
{
    NONCOPYBLE(Bullet_3_Renderer)
public:
    Bullet_3_Renderer(const std::string&, const std::weak_ptr<BulletWorld>&);
    ~Bullet_3_Renderer() override;

    std::shared_ptr<core::physics::IPhysicsWorld> world() override;
    std::shared_ptr<const core::physics::IPhysicsWorld> world() const override;

    const glm::vec3& gravity() const override;
    void setGravity(const glm::vec3&) override;

    uint32_t simulationTimeStep() const override;
    void setSimulationTimeStep(uint32_t) override;

    void stepSimulation(uint32_t) override;
    void addBodyToSimulation(const std::shared_ptr<core::physics::IBody>&) override;
    void removeBodyFromSimulation(const std::shared_ptr<core::physics::IBody>&) override;

    std::shared_ptr<core::physics::IStaticBody> createStaticBody() const override;
    std::shared_ptr<core::physics::IKinematicBody> createKinematicBody() const override;
    std::shared_ptr<core::physics::IDynamicBody> createDynamicBody() const override;

    std::optional<LineSegmentIntersectionResult> singleIntersection(const utils::LineSegment&) const override;
    std::vector<LineSegmentIntersectionResult> allIntersections(const utils::LineSegment&) const override;

protected:
    bool doMakeCurrent() override;
    bool doDoneCurrent() override;

private:
    std::weak_ptr<BulletWorld> m_world;

    std::shared_ptr<btDefaultCollisionConfiguration> m_collisionConfiguration;
    std::shared_ptr<btCollisionDispatcher> m_dispatcher;
    std::shared_ptr<btBroadphaseInterface> m_overlappingPairCache;
    std::shared_ptr<btSequentialImpulseConstraintSolver> m_solver;
    std::shared_ptr<btDiscreteDynamicsWorld> m_dynamicWorld;

    glm::vec3 m_gravity = glm::vec3(0.f);
    uint32_t m_simulationTimeStep = 17u;
};

} // namespace physics_bullet
} // namespace simplex

#endif // PHYSICS_BULLET_3_RENDERER_H
