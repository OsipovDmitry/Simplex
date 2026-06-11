#include "bullet_3_renderer.h"

#include <utils/linesegment.h>
#include <utils/logger.h>
#include <utils/plane.h>
#include <utils/range.h>

#include <core/dynamicbodynode.h>
#include <core/kinematicbodynode.h>
#include <core/staticbodynode.h>

#include <physics_bullet/bulletworld.h>

#include "bulletworldprivate.h"

namespace simplex
{
namespace physics_bullet
{

// DynamicBodyMotionState_3

class DynamicBodyMotionState_3 : public btMotionState
{
public:
    DynamicBodyMotionState_3(DynamicBody_3& body)
        : m_dynamicBody(body)
    {
    }
    ~DynamicBodyMotionState_3() override = default;

    void getWorldTransform(btTransform& worldTransform) const override { worldTransform = m_worldTransform; }

    void setWorldTransform(const btTransform& value) override
    {
        updateWorldTransform(value);
        m_dynamicBody.updateWorldTransform(value);
    }

    void updateWorldTransform(const btTransform& value) { m_worldTransform = value; }

private:
    DynamicBody_3& m_dynamicBody;
    btTransform m_worldTransform = btTransform::getIdentity();
};

class TriangleMeshShape_3 : public btBvhTriangleMeshShape
{
public:
    TriangleMeshShape_3(const std::shared_ptr<btStridingMeshInterface>& stridingMeshInterface)
        : btBvhTriangleMeshShape(stridingMeshInterface.get(), true)
        , m_stridingMeshInterface(stridingMeshInterface)
    {
    }

    ~TriangleMeshShape_3() override = default;

    static std::shared_ptr<TriangleMeshShape_3> create(
        const std::vector<glm::vec3>& vertices,
        const std::vector<uint32_t>& indices)
    {
        auto triangleMesh = std::make_shared<btTriangleMesh>();
        for (const auto& vertex : vertices)
            triangleMesh->findOrAddVertex(Conversions::vec2Bt(vertex), false);
        for (size_t i = 0u; i < indices.size() / 3u; ++i)
            triangleMesh->addTriangleIndices(
                static_cast<int>(indices[3u * i + 0u]), static_cast<int>(indices[3u * i + 1u]),
                static_cast<int>(indices[3u * i + 2u]));

        return std::make_shared<TriangleMeshShape_3>(triangleMesh);
    }

private:
    std::shared_ptr<btStridingMeshInterface> m_stridingMeshInterface;
};

// BodyUserPointer_3

class BodyUserPointer_3
{
public:
    BodyUserPointer_3(BodyBase_3& body)
        : m_bodyBase(body)
    {
    }

    BodyBase_3& bodyBase() { return m_bodyBase; }

private:
    BodyBase_3& m_bodyBase;
};

// Conversions

btScalar Conversions::float2Bt(float value)
{
    return static_cast<btScalar>(value);
}

btVector3 Conversions::vec2Bt(const glm::vec3& value)
{
    return btVector3(float2Bt(value.x), float2Bt(value.y), float2Bt(value.z));
}

btQuaternion Conversions::quat2Bt(const glm::quat& value)
{
    return btQuaternion(float2Bt(value.x), float2Bt(value.y), float2Bt(value.z), float2Bt(value.w));
}

btTransform Conversions::rigidTransform2Bt(const utils::RigidTransform& value)
{
    return btTransform(quat2Bt(value.rotation), vec2Bt(value.translation));
}

std::pair<btScalar, btTransform> Conversions::transform2Bt(const utils::Transform& value)
{
    return {float2Bt(value.scale), btTransform(quat2Bt(value.rotation), vec2Bt(value.translation))};
}

float Conversions::bt2Float(btScalar value)
{
    return static_cast<float>(value);
}

glm::vec3 Conversions::bt2Vec(const btVector3& value)
{
    return glm::vec3(value.x(), value.y(), value.z());
}

glm::quat Conversions::bt2Quat(const btQuaternion& value)
{
    return glm::quat(value.w(), value.x(), value.y(), value.z());
}

utils::RigidTransform Conversions::bt2RigidTRansform(const btTransform& value)
{
    return utils::RigidTransform(bt2Quat(value.getRotation()), bt2Vec(value.getOrigin()));
}

// BodyBase_3

BodyBase_3::~BodyBase_3()
{
    CHECK_CURRENT_CONTEXT;

    removeFromWorld();
}

const utils::Transform& BodyBase_3::transform() const
{
    CHECK_CURRENT_CONTEXT;

    return m_transform;
}

void BodyBase_3::setTransform(const utils::Transform& value)
{
    CHECK_CURRENT_CONTEXT;

    const bool isScaleChanged = m_transform.scale != value.scale;
    m_transform = value;

    removeFromWorld();
    if (isScaleChanged)
    {
        updateCompoundShape();
        updateLocalInertia();
    }
    updateTransform();
    resetVelocitiesAndForces();
    addToWorld();
}

float BodyBase_3::restitution() const
{
    CHECK_CURRENT_CONTEXT;

    return Conversions::bt2Float(m_rigidBody->getRestitution());
}

void BodyBase_3::setRestitution(float value)
{
    CHECK_CURRENT_CONTEXT;

    if (value < 0.f)
    {
        LOG_CRITICAL << "Restitution can't be less than 0";
        return;
    }

    if (value > 1.f)
    {
        LOG_CRITICAL << "Restitution can't be greater than 0";
        return;
    }

    removeFromWorld();
    m_rigidBody->setRestitution(Conversions::float2Bt(value));
    addToWorld();
}

float BodyBase_3::linearDamping() const
{
    CHECK_CURRENT_CONTEXT;

    return Conversions::bt2Float(m_rigidBody->getLinearDamping());
}

void BodyBase_3::setLinearDamping(float value)
{
    CHECK_CURRENT_CONTEXT;

    if (value < 0.f)
    {
        LOG_CRITICAL << "Linear damping can't be less than 0";
        return;
    }

    if (value > 1.f)
    {
        LOG_CRITICAL << "Linear damping can't be greater than 0";
        return;
    }

    removeFromWorld();
    m_rigidBody->setDamping(Conversions::float2Bt(value), m_rigidBody->getAngularDamping());
    addToWorld();
}

float BodyBase_3::angularDamping() const
{
    CHECK_CURRENT_CONTEXT;

    return Conversions::bt2Float(m_rigidBody->getAngularDamping());
}

void BodyBase_3::setAngularDamping(float value)
{
    CHECK_CURRENT_CONTEXT;

    if (value < 0.f)
    {
        LOG_CRITICAL << "Angular damping can't be less than 0";
        return;
    }

    if (value > 1.f)
    {
        LOG_CRITICAL << "Angular damping can't be greater than 0";
        return;
    }

    removeFromWorld();
    m_rigidBody->setDamping(m_rigidBody->getLinearDamping(), Conversions::float2Bt(value));
    addToWorld();
}

float BodyBase_3::linearSleepingThreshold() const
{
    CHECK_CURRENT_CONTEXT;

    return Conversions::bt2Float(m_rigidBody->getLinearSleepingThreshold());
}

void BodyBase_3::setLinearSleepingThreshold(float value)
{
    CHECK_CURRENT_CONTEXT;

    removeFromWorld();
    m_rigidBody->setSleepingThresholds(Conversions::float2Bt(value), m_rigidBody->getAngularSleepingThreshold());
    addToWorld();
}

float BodyBase_3::angularSleepingThreshold() const
{
    CHECK_CURRENT_CONTEXT;

    return Conversions::bt2Float(m_rigidBody->getAngularSleepingThreshold());
}

void BodyBase_3::setAngularSleepingThreshold(float value)
{
    CHECK_CURRENT_CONTEXT;

    removeFromWorld();
    m_rigidBody->setSleepingThresholds(m_rigidBody->getLinearSleepingThreshold(), Conversions::float2Bt(value));
    addToWorld();
}

void BodyBase_3::removeShape(utils::IDsGenerator::value_type ID)
{
    CHECK_CURRENT_CONTEXT;

    if (ID >= m_collisionShapes.size())
    {
        LOG_CRITICAL << "ID is out of range";
        return;
    }

    m_collisionShapeIDsGenerator.clear(ID);
    m_collisionShapes[ID] = {btTransform::getIdentity(), nullptr};

    removeFromWorld();
    updateCompoundShape();
    updateLocalInertia();
    resetVelocitiesAndForces();
    addToWorld();
}

utils::IDsGenerator::value_type BodyBase_3::addSphereShape(float radius, const utils::RigidTransform& localTransform)
{
    CHECK_CURRENT_CONTEXT;

    return addCollisionShape(std::make_shared<btSphereShape>(Conversions::float2Bt(radius)), localTransform);
}

utils::IDsGenerator::value_type BodyBase_3::addBoxShape(const glm::vec3& halfSizes, const utils::RigidTransform& localTransform)
{
    CHECK_CURRENT_CONTEXT;

    return addCollisionShape(std::make_shared<btBoxShape>(Conversions::vec2Bt(halfSizes)), localTransform);
}

utils::IDsGenerator::value_type BodyBase_3::addCapsuleShape(
    float radius,
    float height,
    const utils::RigidTransform& localTransform)
{
    CHECK_CURRENT_CONTEXT;

    return addCollisionShape(
        std::make_shared<btCapsuleShape>(Conversions::float2Bt(radius), Conversions::float2Bt(height)), localTransform);
}

utils::IDsGenerator::value_type BodyBase_3::addCylinderShape(
    const glm::vec3& halfSizes,
    const utils::RigidTransform& localTransform)
{
    CHECK_CURRENT_CONTEXT;

    return addCollisionShape(std::make_shared<btCylinderShape>(Conversions::vec2Bt(halfSizes)), localTransform);
}

utils::IDsGenerator::value_type BodyBase_3::addConeShape(float radius, float height, const utils::RigidTransform& localTransform)
{
    CHECK_CURRENT_CONTEXT;

    return addCollisionShape(
        std::make_shared<btConeShape>(Conversions::float2Bt(radius), Conversions::float2Bt(height)), localTransform);
}

utils::IDsGenerator::value_type BodyBase_3::addConvexHullShape(
    const std::vector<glm::vec3>& vertices,
    const utils::RigidTransform& localTransform)
{
    CHECK_CURRENT_CONTEXT;

    auto convexHullShape = std::make_shared<btConvexHullShape>();
    for (const auto& vertex : vertices)
        convexHullShape->addPoint(Conversions::vec2Bt(vertex), false);
    convexHullShape->recalcLocalAabb();
    convexHullShape->optimizeConvexHull();

    return addCollisionShape(convexHullShape, localTransform);
}

const std::set<std::shared_ptr<core::physics::IConstraint>>& BodyBase_3::constraints() const
{
    CHECK_CURRENT_CONTEXT;

    return m_constraints;
}

std::shared_ptr<core::physics::IFixedConstraint> BodyBase_3::joinFixed(
    const utils::RigidTransform& frame,
    const std::shared_ptr<core::physics::IBody>& anotherBody,
    const utils::RigidTransform& anotherFrame)
{
    CHECK_CURRENT_CONTEXT;

    auto bodyBase = shared_from_this();
    CHECK_EQUAL_CONTEXTS(this, bodyBase);

    auto anotherBodyBase = std::dynamic_pointer_cast<BodyBase_3>(anotherBody);
    if (!anotherBodyBase)
    {
        LOG_CRITICAL << "Body can't be nullptr";
        return nullptr;
    }
    CHECK_EQUAL_CONTEXTS(this, anotherBodyBase);

    auto result = std::make_shared<FixedConstraint_3>(bodyBase, frame, anotherBodyBase, anotherFrame);

    bodyBase->addConstraint(result);
    anotherBodyBase->addConstraint(result);
    addConstraintToWorld(*result);

    return result;
}

std::shared_ptr<core::physics::IPointToPointConstraint> BodyBase_3::joinPointToPoint(
    const glm::vec3& pivot,
    const std::shared_ptr<core::physics::IBody>& anotherBody,
    const glm::vec3& anotherPivot)
{
    CHECK_CURRENT_CONTEXT;

    auto bodyBase = shared_from_this();
    CHECK_EQUAL_CONTEXTS(this, bodyBase);

    auto anotherBodyBase = std::dynamic_pointer_cast<BodyBase_3>(anotherBody);
    if (anotherBodyBase)
    {
        CHECK_EQUAL_CONTEXTS(this, anotherBodyBase);
    }

    std::shared_ptr<PointToPointConstraint_3> result =
        (anotherBodyBase) ? std::make_shared<PointToPointConstraint_3>(bodyBase, pivot, anotherBodyBase, anotherPivot)
                          : std::make_shared<PointToPointConstraint_3>(bodyBase, pivot);

    bodyBase->addConstraint(result);
    if (anotherBodyBase) anotherBodyBase->addConstraint(result);
    addConstraintToWorld(*result);

    return result;
}

std::shared_ptr<core::physics::IHingeConstraint> BodyBase_3::joinHinge(
    const utils::RigidTransform& frame,
    const std::shared_ptr<core::physics::IBody>& anotherBody,
    const utils::RigidTransform& anotherFrame)
{
    CHECK_CURRENT_CONTEXT;

    auto bodyBase = shared_from_this();
    CHECK_EQUAL_CONTEXTS(this, bodyBase);

    auto anotherBodyBase = std::dynamic_pointer_cast<BodyBase_3>(anotherBody);
    if (anotherBodyBase)
    {
        CHECK_EQUAL_CONTEXTS(this, anotherBodyBase);
    }

    std::shared_ptr<HingeConstraint_3> result =
        (anotherBodyBase) ? std::make_shared<HingeConstraint_3>(bodyBase, frame, anotherBodyBase, anotherFrame)
                          : std::make_shared<HingeConstraint_3>(bodyBase, frame);

    bodyBase->addConstraint(result);
    if (anotherBodyBase) anotherBodyBase->addConstraint(result);
    addConstraintToWorld(*result);

    return result;
}

std::shared_ptr<core::physics::ISliderConstraint> BodyBase_3::joinSlider(
    const utils::RigidTransform& frame,
    const std::shared_ptr<core::physics::IBody>& anotherBody,
    const utils::RigidTransform& anotherFrame)
{
    CHECK_CURRENT_CONTEXT;

    auto bodyBase = shared_from_this();
    CHECK_EQUAL_CONTEXTS(this, bodyBase);

    auto anotherBodyBase = std::dynamic_pointer_cast<BodyBase_3>(anotherBody);
    if (anotherBodyBase)
    {
        CHECK_EQUAL_CONTEXTS(this, anotherBodyBase);
    }

    std::shared_ptr<SliderConstraint_3> result =
        (anotherBodyBase) ? std::make_shared<SliderConstraint_3>(bodyBase, frame, anotherBodyBase, anotherFrame)
                          : std::make_shared<SliderConstraint_3>(bodyBase, frame);

    bodyBase->addConstraint(result);
    if (anotherBodyBase) anotherBodyBase->addConstraint(result);
    addConstraintToWorld(*result);

    return result;
}

void BodyBase_3::setDynamicWorld(const std::weak_ptr<btDiscreteDynamicsWorld>& value)
{
    CHECK_CURRENT_CONTEXT;

    removeFromWorld();
    m_dynamicWorld = value;
    addToWorld();
}

std::shared_ptr<btRigidBody>& BodyBase_3::rigidBody()
{
    CHECK_CURRENT_CONTEXT;

    return m_rigidBody;
}

void BodyBase_3::removeConstraint(const std::shared_ptr<ConstraintBase_3>& value)
{
    CHECK_CURRENT_CONTEXT;
    CHECK_EQUAL_CONTEXTS(this, value);

    if (auto it = m_constraints.find(value); it == m_constraints.end())
    {
        LOG_WARNING << "Body doesn't have the constraint to remove";
    }
    else
    {
        m_constraints.erase(it);
    }
}

void BodyBase_3::addConstraint(const std::shared_ptr<ConstraintBase_3>& value)
{
    CHECK_CURRENT_CONTEXT;
    CHECK_EQUAL_CONTEXTS(this, value);

    if (auto it = m_constraints.find(value); it != m_constraints.end())
    {
        LOG_WARNING << "Body already has the constraint";
    }
    else
    {
        m_constraints.insert(value);
    }
}

void BodyBase_3::addConstraintToWorld(ConstraintBase_3& constraintBase)
{
    std::vector<std::shared_ptr<BodyBase_3>> bodyBases;
    for (const auto& bodyBaseWeak : {constraintBase.bodyBase1(), constraintBase.bodyBase2()})
        if (const auto bodyBase = bodyBaseWeak.lock()) bodyBases.push_back(bodyBase);

    if (bodyBases.empty())
    {
        constraintBase.setDynamicWorld(std::weak_ptr<btDiscreteDynamicsWorld>());
        return;
    }

    bool allBodiesInWorld = true;
    for (const auto& bodyBase : bodyBases)
        if (bodyBase->m_dynamicWorld.expired())
        {
            allBodiesInWorld = false;
            break;
        }
    if (!allBodiesInWorld)
    {
        constraintBase.setDynamicWorld(std::weak_ptr<btDiscreteDynamicsWorld>());
        return;
    }

    auto dynamicWorld = bodyBases.front()->m_dynamicWorld.lock();

    auto worldIsUniqueForAllBodies = true;
    for (const auto& bodyBase : bodyBases)
        if (bodyBase->m_dynamicWorld.lock() != dynamicWorld)
        {
            worldIsUniqueForAllBodies = false;
            break;
        }

    if (!worldIsUniqueForAllBodies)
    {
        LOG_CRITICAL << "Constraint's bodies were added in different dynamic worlds";
        return;
    }

    constraintBase.setDynamicWorld(dynamicWorld);
}

BodyBase_3::BodyBase_3(float mass)
{
    SAVE_CURRENT_CONTEXT;

    m_rigidBody = std::make_shared<btRigidBody>(Conversions::float2Bt(mass), nullptr, nullptr);
    m_rigidBody->setCollisionFlags(m_rigidBody->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

    m_bodyUserPointer = std::make_shared<BodyUserPointer_3>(*this);
    m_rigidBody->setUserPointer(m_bodyUserPointer.get());

    updateCompoundShape();
    updateLocalInertia();
    updateTransform();
    resetVelocitiesAndForces();
}

utils::IDsGenerator::value_type BodyBase_3::addCollisionShape(
    const std::shared_ptr<btCollisionShape>& collisionShape,
    const utils::RigidTransform& localTransform)
{
    CHECK_CURRENT_CONTEXT;

    if (!collisionShape)
    {
        LOG_CRITICAL << "Collision shape can't be nullptr";
        return utils::IDsGenerator::last();
    }

    const auto ID = m_collisionShapeIDsGenerator.generate();
    m_collisionShapes.resize(static_cast<size_t>(ID) + 1u);
    m_collisionShapes[ID] = {Conversions::rigidTransform2Bt(localTransform), collisionShape};

    removeFromWorld();
    updateCompoundShape();
    updateLocalInertia();
    resetVelocitiesAndForces();
    addToWorld();

    return ID;
}

void BodyBase_3::removeFromWorld()
{
    CHECK_CURRENT_CONTEXT;

    if (auto dynamicWorld = m_dynamicWorld.lock()) dynamicWorld->removeRigidBody(m_rigidBody.get());
}

void BodyBase_3::addToWorld()
{
    CHECK_CURRENT_CONTEXT;

    if (auto dynamicWorld = m_dynamicWorld.lock())
    {
        dynamicWorld->addRigidBody(m_rigidBody.get());
        m_rigidBody->activate(true);
    }
}

void BodyBase_3::resetVelocitiesAndForces()
{
    CHECK_CURRENT_CONTEXT;

    m_rigidBody->setLinearVelocity(Conversions::vec2Bt(glm::vec3(0.f)));
    m_rigidBody->setAngularVelocity(Conversions::vec2Bt(glm::vec3(0.f)));
    m_rigidBody->clearForces();
}

void BodyBase_3::updateCompoundShape()
{
    CHECK_CURRENT_CONTEXT;

    const auto scale = Conversions::float2Bt(m_transform.scale);

    m_compoundShape = std::make_shared<btCompoundShape>();
    for (auto& [localTransform, collisionShape] : m_collisionShapes)
    {
        if (!collisionShape) continue;

        collisionShape->setLocalScaling(btVector3(scale, scale, scale));
        m_compoundShape->addChildShape(
            btTransform(localTransform.getBasis(), localTransform.getOrigin() * scale), collisionShape.get());
    }
    m_rigidBody->setCollisionShape(m_compoundShape.get());
}

void BodyBase_3::updateLocalInertia()
{
    CHECK_CURRENT_CONTEXT;

    const auto mass = m_rigidBody->getMass();

    auto localInertia = Conversions::vec2Bt(glm::vec3(0.f));
    if (mass > Conversions::float2Bt(0.f)) m_compoundShape->calculateLocalInertia(mass, localInertia);

    m_rigidBody->setMassProps(mass, localInertia);
    m_rigidBody->updateInertiaTensor();
}

void BodyBase_3::updateTransform()
{
    CHECK_CURRENT_CONTEXT;

    const auto [_, worldTransform] = Conversions::transform2Bt(m_transform);

    m_rigidBody->setWorldTransform(worldTransform);
    if (auto* dynamicBodyMotionsState_3 = dynamic_cast<DynamicBodyMotionState_3*>(m_rigidBody->getMotionState()))
        dynamicBodyMotionsState_3->updateWorldTransform(worldTransform);
}

// StaticBody_3

StaticBody_3::StaticBody_3()
    : BodyBase_3(0.f)
{
    // no need to SAVE_CURRENT_CONTEXT here. It was done inside BodyBase_3 constructor
}

StaticBody_3::~StaticBody_3() = default;

utils::IDsGenerator::value_type StaticBody_3::addTriangleMeshShape(
    const std::vector<glm::vec3>& vertices,
    const std::vector<uint32_t>& indices,
    const utils::RigidTransform& localTransform)
{
    CHECK_CURRENT_CONTEXT;

    return addCollisionShape(TriangleMeshShape_3::create(vertices, indices), localTransform);
}

utils::IDsGenerator::value_type StaticBody_3::addPlaneShape(const utils::Plane& plane, const utils::RigidTransform& localTransform)
{
    CHECK_CURRENT_CONTEXT;

    return addCollisionShape(
        std::make_shared<btStaticPlaneShape>(Conversions::vec2Bt(plane.normal()), Conversions::float2Bt(plane.dist())),
        localTransform);
}

utils::IDsGenerator::value_type StaticBody_3::addHeightfieldShape(const utils::RigidTransform& localTransform)
{
    CHECK_CURRENT_CONTEXT;

    // TODO: add height field implementation
    return addCollisionShape(std::make_shared<btSphereShape>(Conversions::float2Bt(1.f)), localTransform);
}

// KinematicBody_3

KinematicBody_3::KinematicBody_3()
    : BodyBase_3(0.f)
{
    // no need to SAVE_CURRENT_CONTEXT here. It was done inside BodyBase_3 constructor

    m_rigidBody->setCollisionFlags(m_rigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    m_rigidBody->setActivationState(DISABLE_DEACTIVATION);
}

KinematicBody_3::~KinematicBody_3() = default;

// DynamicBody_3

DynamicBody_3::DynamicBody_3()
    : BodyBase_3(1.f)
{
    // no need to SAVE_CURRENT_CONTEXT here. It was done inside BodyBase_3 constructor

    m_motionState = std::make_shared<DynamicBodyMotionState_3>(*this);
    m_rigidBody->setMotionState(m_motionState.get());

    updateTransform();
}

DynamicBody_3::~DynamicBody_3() = default;

float DynamicBody_3::mass() const
{
    CHECK_CURRENT_CONTEXT;

    return Conversions::bt2Float(m_rigidBody->getMass());
}

void DynamicBody_3::setMass(float value)
{
    CHECK_CURRENT_CONTEXT;

    if (value <= 0.f)
    {
        LOG_CRITICAL << "Mass can't be less or equal than 0";
        return;
    }

    removeFromWorld();
    m_rigidBody->setMassProps(Conversions::float2Bt(value), Conversions::vec2Bt(glm::vec3(0.f)));
    updateLocalInertia();
    addToWorld();
}

void DynamicBody_3::applyForce(const glm::vec3& force, const glm::vec3& localPosition)
{
    CHECK_CURRENT_CONTEXT;

    m_rigidBody->applyForce(Conversions::vec2Bt(force), Conversions::vec2Bt(localPosition));
    m_rigidBody->activate(true);
}

void DynamicBody_3::applyImpulse(const glm::vec3& impulse, const glm::vec3& localPosition)
{
    CHECK_CURRENT_CONTEXT;

    m_rigidBody->applyImpulse(Conversions::vec2Bt(impulse), Conversions::vec2Bt(localPosition));
    m_rigidBody->activate(true);
}

std::pair<bool, utils::Transform> DynamicBody_3::resetTransformChangedAfterSimulation()
{
    CHECK_CURRENT_CONTEXT;

    const std::pair<bool, utils::Transform> result{m_isTransformChanged, m_transform};
    m_isTransformChanged = false;
    return result;
}

void DynamicBody_3::updateWorldTransform(const btTransform& value)
{
    CHECK_CURRENT_CONTEXT;

    m_isTransformChanged = true;

    const auto scale = m_transform.scale;
    m_transform = utils::Transform(scale, Conversions::bt2Quat(value.getRotation()), Conversions::bt2Vec(value.getOrigin()));
}

// ConstraintBase_3

ConstraintBase_3::~ConstraintBase_3()
{
    CHECK_CURRENT_CONTEXT;

    removeFromWorld();
}

std::weak_ptr<core::physics::IBody> ConstraintBase_3::body1()
{
    CHECK_CURRENT_CONTEXT;

    return bodyBase1();
}

std::weak_ptr<const core::physics::IBody> ConstraintBase_3::body1() const
{
    CHECK_CURRENT_CONTEXT;

    return const_cast<ConstraintBase_3*>(this)->body1();
}

std::weak_ptr<core::physics::IBody> ConstraintBase_3::body2()
{
    CHECK_CURRENT_CONTEXT;

    return bodyBase2();
}

std::weak_ptr<const core::physics::IBody> ConstraintBase_3::body2() const
{
    CHECK_CURRENT_CONTEXT;

    return const_cast<ConstraintBase_3*>(this)->body2();
}

void ConstraintBase_3::detachFromBodies()
{
    CHECK_CURRENT_CONTEXT;

    removeFromWorld();
    removeFromBodies();
}

bool ConstraintBase_3::isCollisionBetweenLinkedBodiesEnabled() const
{
    CHECK_CURRENT_CONTEXT;

    return m_isCollisionBetweenLinkedBodiesEnabled;
}

void ConstraintBase_3::enableCollisionBetweenLinkedBodies(bool value)
{
    CHECK_CURRENT_CONTEXT;

    removeFromWorld();
    m_isCollisionBetweenLinkedBodiesEnabled = value;
    addToWorld();
}

void ConstraintBase_3::setDynamicWorld(const std::weak_ptr<btDiscreteDynamicsWorld>& value)
{
    CHECK_CURRENT_CONTEXT;

    removeFromWorld();
    m_dynamicWorld = value;
    addToWorld();
}

std::weak_ptr<BodyBase_3>& ConstraintBase_3::bodyBase1()
{
    return m_bodyBase1;
}

std::weak_ptr<BodyBase_3>& ConstraintBase_3::bodyBase2()
{
    return m_bodyBase2;
}

ConstraintBase_3::ConstraintBase_3(
    const std::shared_ptr<btTypedConstraint>& typedConstraint,
    const std::weak_ptr<BodyBase_3>& bodyBase1,
    const std::weak_ptr<BodyBase_3>& bodyBase2)
    : m_typedConstraint(typedConstraint)
    , m_bodyBase1(bodyBase1)
    , m_bodyBase2(bodyBase2)
{
    SAVE_CURRENT_CONTEXT;

    if (!m_typedConstraint)
    {
        LOG_CRITICAL << "Constraint can't be nullptr";
        return;
    }
}

void ConstraintBase_3::removeFromBodies()
{
    CHECK_CURRENT_CONTEXT;

    auto sharedThis = shared_from_this();
    if (auto bodyBase1 = m_bodyBase1.lock()) bodyBase1->removeConstraint(sharedThis);
    if (auto bodyBase2 = m_bodyBase2.lock()) bodyBase2->removeConstraint(sharedThis);
}

void ConstraintBase_3::addToBodies()
{
    CHECK_CURRENT_CONTEXT;

    auto sharedThis = shared_from_this();
    if (auto bodyBase1 = m_bodyBase1.lock()) bodyBase1->addConstraint(sharedThis);
    if (auto bodyBase2 = m_bodyBase2.lock()) bodyBase2->addConstraint(sharedThis);
}

void ConstraintBase_3::removeFromWorld()
{
    CHECK_CURRENT_CONTEXT;

    if (auto dynamicWorld = m_dynamicWorld.lock()) dynamicWorld->removeConstraint(m_typedConstraint.get());
}

void ConstraintBase_3::addToWorld()
{
    CHECK_CURRENT_CONTEXT;

    if (auto dynamicWorld = m_dynamicWorld.lock())
        dynamicWorld->addConstraint(m_typedConstraint.get(), !m_isCollisionBetweenLinkedBodiesEnabled);
}

// FixedConstraint_3

FixedConstraint_3::FixedConstraint_3(
    const std::shared_ptr<BodyBase_3>& bodyBase1,
    const utils::RigidTransform& frame1,
    const std::shared_ptr<BodyBase_3>& bodyBase2,
    const utils::RigidTransform& frame2)
    : ConstraintBase_3(
          std::make_shared<btFixedConstraint>(
              *bodyBase1->rigidBody(),
              *bodyBase2->rigidBody(),
              Conversions::rigidTransform2Bt(frame1),
              Conversions::rigidTransform2Bt(frame2)),
          bodyBase1,
          bodyBase2)
{
    // no need to SAVE_CURRENT_CONTEXT here. It was done inside ConstraintBase_3 constructor

    CHECK_EQUAL_CONTEXTS(this, bodyBase1);
    CHECK_EQUAL_CONTEXTS(this, bodyBase2);
}

FixedConstraint_3::~FixedConstraint_3() = default;

utils::RigidTransform FixedConstraint_3::frame1() const
{
    CHECK_CURRENT_CONTEXT;

    auto fixedConstraint = std::static_pointer_cast<btFixedConstraint>(m_typedConstraint);
    if (!fixedConstraint)
    {
        LOG_CRITICAL << "Constraint can't be nullptr";
        return utils::RigidTransform();
    }

    return Conversions::bt2RigidTRansform(fixedConstraint->getFrameOffsetA());
}

utils::RigidTransform FixedConstraint_3::frame2() const
{
    CHECK_CURRENT_CONTEXT;

    auto fixedConstraint = std::static_pointer_cast<btFixedConstraint>(m_typedConstraint);
    if (!fixedConstraint)
    {
        LOG_CRITICAL << "Constraint can't be nullptr";
        return utils::RigidTransform();
    }

    return Conversions::bt2RigidTRansform(fixedConstraint->getFrameOffsetB());
}

// PointToPointConstraint_3

PointToPointConstraint_3::PointToPointConstraint_3(
    const std::shared_ptr<BodyBase_3>& bodyBase1,
    const glm::vec3& pivot1,
    const std::shared_ptr<BodyBase_3>& bodyBase2,
    const glm::vec3& pivot2)
    : ConstraintBase_3(
          std::make_shared<btPoint2PointConstraint>(
              *bodyBase1->rigidBody(),
              *bodyBase2->rigidBody(),
              Conversions::vec2Bt(pivot1),
              Conversions::vec2Bt(pivot2)),
          bodyBase1,
          bodyBase2)
{
    // no need to SAVE_CURRENT_CONTEXT here. It was done inside ConstraintBase_3 constructor

    CHECK_EQUAL_CONTEXTS(this, bodyBase1);
    CHECK_EQUAL_CONTEXTS(this, bodyBase2);
}

PointToPointConstraint_3::PointToPointConstraint_3(const std::shared_ptr<BodyBase_3>& bodyBase, const glm::vec3& pivot)
    : ConstraintBase_3(
          std::make_shared<btPoint2PointConstraint>(*bodyBase->rigidBody(), Conversions::vec2Bt(pivot)),
          bodyBase,
          std::weak_ptr<BodyBase_3>())
{
    // no need to SAVE_CURRENT_CONTEXT here. It was done inside ConstraintBase_3 constructor

    CHECK_EQUAL_CONTEXTS(this, bodyBase);
}

PointToPointConstraint_3::~PointToPointConstraint_3() = default;

glm::vec3 PointToPointConstraint_3::pivot1() const
{
    CHECK_CURRENT_CONTEXT;

    auto pointToPointConstraint = std::static_pointer_cast<btPoint2PointConstraint>(m_typedConstraint);
    if (!pointToPointConstraint)
    {
        LOG_CRITICAL << "Constraint can't be nullptr";
        return glm::vec3(0.f);
    }

    return Conversions::bt2Vec(pointToPointConstraint->getPivotInA());
}

glm::vec3 PointToPointConstraint_3::pivot2() const
{
    CHECK_CURRENT_CONTEXT;

    auto pointToPointConstraint = std::static_pointer_cast<btPoint2PointConstraint>(m_typedConstraint);
    if (!pointToPointConstraint)
    {
        LOG_CRITICAL << "Constraint can't be nullptr";
        return glm::vec3(0.f);
    }

    return Conversions::bt2Vec(pointToPointConstraint->getPivotInB());
}

// HingeConstraint_3

HingeConstraint_3::HingeConstraint_3(
    const std::shared_ptr<BodyBase_3>& bodyBase1,
    const utils::RigidTransform& frame1,
    const std::shared_ptr<BodyBase_3>& bodyBase2,
    const utils::RigidTransform& frame2)
    : ConstraintBase_3(
          std::make_shared<btHingeConstraint>(
              *bodyBase1->rigidBody(),
              *bodyBase2->rigidBody(),
              Conversions::rigidTransform2Bt(frame1),
              Conversions::rigidTransform2Bt(frame2)),
          bodyBase1,
          bodyBase2)
{
    // no need to SAVE_CURRENT_CONTEXT here. It was done inside ConstraintBase_3 constructor

    CHECK_EQUAL_CONTEXTS(this, bodyBase1);
    CHECK_EQUAL_CONTEXTS(this, bodyBase2);
}

HingeConstraint_3::HingeConstraint_3(const std::shared_ptr<BodyBase_3>& bodyBase, const utils::RigidTransform& frame)
    : ConstraintBase_3(
          std::make_shared<btHingeConstraint>(*bodyBase->rigidBody(), Conversions::rigidTransform2Bt(frame)),
          bodyBase,
          std::weak_ptr<BodyBase_3>())
{
    // no need to SAVE_CURRENT_CONTEXT here. It was done inside ConstraintBase_3 constructor

    CHECK_EQUAL_CONTEXTS(this, bodyBase);
}

HingeConstraint_3::~HingeConstraint_3() = default;

utils::RigidTransform HingeConstraint_3::frame1() const
{
    CHECK_CURRENT_CONTEXT;

    auto hingeConstraint = std::static_pointer_cast<btHingeConstraint>(m_typedConstraint);
    if (!hingeConstraint)
    {
        LOG_CRITICAL << "Constraint can't be nullptr";
        return utils::RigidTransform();
    }

    return Conversions::bt2RigidTRansform(hingeConstraint->getFrameOffsetA());
}

utils::RigidTransform HingeConstraint_3::frame2() const
{
    CHECK_CURRENT_CONTEXT;

    auto hingeConstraint = std::static_pointer_cast<btHingeConstraint>(m_typedConstraint);
    if (!hingeConstraint)
    {
        LOG_CRITICAL << "Constraint can't be nullptr";
        return utils::RigidTransform();
    }

    return Conversions::bt2RigidTRansform(hingeConstraint->getFrameOffsetB());
}

utils::Range HingeConstraint_3::angularLimits() const
{
    CHECK_CURRENT_CONTEXT;

    auto hingeConstraint = std::static_pointer_cast<btHingeConstraint>(m_typedConstraint);
    if (!hingeConstraint)
    {
        LOG_CRITICAL << "Constraint can't be nullptr";
        return utils::Range();
    }

    return utils::Range(
        Conversions::bt2Float(hingeConstraint->getLowerLimit()), Conversions::bt2Float(hingeConstraint->getUpperLimit()));
}

void HingeConstraint_3::setAngularLimits(const utils::Range& value)
{
    CHECK_CURRENT_CONTEXT;

    auto hingeConstraint = std::static_pointer_cast<btHingeConstraint>(m_typedConstraint);
    if (!hingeConstraint)
    {
        LOG_CRITICAL << "Constraint can't be nullptr";
        return;
    }

    hingeConstraint->setLimit(Conversions::float2Bt(value.nearValue()), Conversions::float2Bt(value.farValue()));
}

// SliderConstraint_3

SliderConstraint_3::SliderConstraint_3(
    const std::shared_ptr<BodyBase_3>& bodyBase1,
    const utils::RigidTransform& frame1,
    const std::shared_ptr<BodyBase_3>& bodyBase2,
    const utils::RigidTransform& frame2)
    : ConstraintBase_3(
          std::make_shared<btSliderConstraint>(
              *bodyBase1->rigidBody(),
              *bodyBase2->rigidBody(),
              Conversions::rigidTransform2Bt(frame1),
              Conversions::rigidTransform2Bt(frame2),
              false),
          bodyBase1,
          bodyBase2)
{
    // no need to SAVE_CURRENT_CONTEXT here. It was done inside ConstraintBase_3 constructor

    CHECK_EQUAL_CONTEXTS(this, bodyBase1);
    CHECK_EQUAL_CONTEXTS(this, bodyBase2);
}

SliderConstraint_3::SliderConstraint_3(const std::shared_ptr<BodyBase_3>& bodyBase, const utils::RigidTransform& frame)
    : ConstraintBase_3(
          std::make_shared<btSliderConstraint>(*bodyBase->rigidBody(), Conversions::rigidTransform2Bt(frame), false),
          bodyBase,
          std::weak_ptr<BodyBase_3>())
{
    // no need to SAVE_CURRENT_CONTEXT here. It was done inside ConstraintBase_3 constructor

    CHECK_EQUAL_CONTEXTS(this, bodyBase);
}

SliderConstraint_3::~SliderConstraint_3() = default;

utils::RigidTransform SliderConstraint_3::frame1() const
{
    CHECK_CURRENT_CONTEXT;

    auto sliderConstraint = std::static_pointer_cast<btSliderConstraint>(m_typedConstraint);
    if (!sliderConstraint)
    {
        LOG_CRITICAL << "Constraint can't be nullptr";
        return utils::RigidTransform();
    }

    return Conversions::bt2RigidTRansform(sliderConstraint->getFrameOffsetA());
}

utils::RigidTransform SliderConstraint_3::frame2() const
{
    CHECK_CURRENT_CONTEXT;

    auto sliderConstraint = std::static_pointer_cast<btSliderConstraint>(m_typedConstraint);
    if (!sliderConstraint)
    {
        LOG_CRITICAL << "Constraint can't be nullptr";
        return utils::RigidTransform();
    }

    return Conversions::bt2RigidTRansform(sliderConstraint->getFrameOffsetB());
}

utils::Range SliderConstraint_3::linearLimits() const
{
    CHECK_CURRENT_CONTEXT;

    auto sliderConstraint = std::static_pointer_cast<btSliderConstraint>(m_typedConstraint);
    if (!sliderConstraint)
    {
        LOG_CRITICAL << "Constraint can't be nullptr";
        return utils::Range();
    }

    return utils::Range(
        Conversions::bt2Float(sliderConstraint->getLowerLinLimit()), Conversions::bt2Float(sliderConstraint->getUpperLinLimit()));
}

void SliderConstraint_3::setLinearLimits(const utils::Range& value)
{
    CHECK_CURRENT_CONTEXT;

    auto sliderConstraint = std::static_pointer_cast<btSliderConstraint>(m_typedConstraint);
    if (!sliderConstraint)
    {
        LOG_CRITICAL << "Constraint can't be nullptr";
        return;
    }

    sliderConstraint->setLowerLinLimit(Conversions::float2Bt(value.nearValue()));
    sliderConstraint->setUpperLinLimit(Conversions::float2Bt(value.farValue()));
}

utils::Range SliderConstraint_3::angularLimits() const
{
    CHECK_CURRENT_CONTEXT;

    auto sliderConstraint = std::static_pointer_cast<btSliderConstraint>(m_typedConstraint);
    if (!sliderConstraint)
    {
        LOG_CRITICAL << "Constraint can't be nullptr";
        return utils::Range();
    }

    return utils::Range(
        Conversions::bt2Float(sliderConstraint->getLowerAngLimit()), Conversions::bt2Float(sliderConstraint->getUpperAngLimit()));
}

void SliderConstraint_3::setAngularLimits(const utils::Range& value)
{
    CHECK_CURRENT_CONTEXT;

    auto sliderConstraint = std::static_pointer_cast<btSliderConstraint>(m_typedConstraint);
    if (!sliderConstraint)
    {
        LOG_CRITICAL << "Constraint can't be nullptr";
        return;
    }

    sliderConstraint->setLowerAngLimit(Conversions::float2Bt(value.nearValue()));
    sliderConstraint->setUpperAngLimit(Conversions::float2Bt(value.farValue()));
}

// Bullet_3_Renderer

Bullet_3_Renderer::Bullet_3_Renderer(const std::string& name, const std::weak_ptr<BulletWorld>& world)
    : core::physics::RendererBase(name)
    , m_world(world)
{
    if (m_world.expired()) LOG_CRITICAL << "BulletWorld can't be nullptr";

    m_collisionConfiguration = std::make_shared<btDefaultCollisionConfiguration>();
    m_dispatcher = std::make_shared<btCollisionDispatcher>(m_collisionConfiguration.get());
    m_overlappingPairCache = std::make_shared<btDbvtBroadphase>();
    m_solver = std::make_shared<btSequentialImpulseConstraintSolver>();
    m_dynamicWorld = std::make_shared<btDiscreteDynamicsWorld>(
        m_dispatcher.get(), m_overlappingPairCache.get(), m_solver.get(), m_collisionConfiguration.get());

    gContactAddedCallback = collisionCallback;

    LOG_INFO << "Physics renderer \"" << Bullet_3_Renderer::name() << "\" has been created";
}

Bullet_3_Renderer::~Bullet_3_Renderer()
{
    LOG_INFO << "Physics renderer \"" << Bullet_3_Renderer::name() << "\" has been destroyed";
}

std::shared_ptr<core::physics::IPhysicsWorld> Bullet_3_Renderer::world()
{
    // No CHECK_THIS_CONTEXT because of recursion calls
    return m_world.expired() ? nullptr : m_world.lock();
}

std::shared_ptr<const core::physics::IPhysicsWorld> Bullet_3_Renderer::world() const
{
    // No CHECK_THIS_CONTEXT because of recursion calls
    return const_cast<Bullet_3_Renderer*>(this)->world();
}

const glm::vec3& Bullet_3_Renderer::gravity() const
{
    CHECK_THIS_CONTEXT;
    return m_gravity;
}

void Bullet_3_Renderer::setGravity(const glm::vec3& value)
{
    CHECK_THIS_CONTEXT;
    m_gravity = value;
    m_dynamicWorld->setGravity(Conversions::vec2Bt(m_gravity));
}

uint32_t Bullet_3_Renderer::simulationTimeStep() const
{
    CHECK_THIS_CONTEXT;
    return m_simulationTimeStep;
}

void Bullet_3_Renderer::setSimulationTimeStep(uint32_t value)
{
    CHECK_THIS_CONTEXT;
    if (!value)
    {
        LOG_CRITICAL << "Simulation time step must be greater than 0";
        return;
    }
    m_simulationTimeStep = value;
}

void Bullet_3_Renderer::stepSimulation(uint32_t dt)
{
    CHECK_THIS_CONTEXT;

    const auto timeStep = static_cast<float>(dt) * .001f;
    const auto fixedTimeStep = static_cast<float>(m_simulationTimeStep) * .001f;
    const auto maxSubSteps = static_cast<int>(glm::ceil(timeStep / fixedTimeStep) + .5f);
    m_dynamicWorld->stepSimulation(Conversions::float2Bt(timeStep), 10 /*maxSubSteps*/, Conversions::float2Bt(fixedTimeStep));
}

void Bullet_3_Renderer::addBodyToSimulation(const std::shared_ptr<core::physics::IBody>& body)
{
    CHECK_THIS_CONTEXT;

    auto bodyBase_3 = std::dynamic_pointer_cast<BodyBase_3>(body);
    if (!bodyBase_3)
    {
        LOG_CRITICAL << "Body can't be nullptr";
        return;
    }
    CHECK_RESOURCE_CONTEXT(bodyBase_3);

    bodyBase_3->setDynamicWorld(m_dynamicWorld);

    for (auto& constraint : bodyBase_3->constraints())
    {
        auto constraintBase_3 = std::dynamic_pointer_cast<ConstraintBase_3>(constraint);
        CHECK_RESOURCE_CONTEXT(constraintBase_3);

        if (!constraintBase_3)
        {
            LOG_CRITICAL << "Constraint can't be nullptr";
            return;
        }

        BodyBase_3::addConstraintToWorld(*constraintBase_3);
    }
}

void Bullet_3_Renderer::removeBodyFromSimulation(const std::shared_ptr<core::physics::IBody>& body)
{
    CHECK_THIS_CONTEXT;

    auto bodyBase_3 = std::dynamic_pointer_cast<BodyBase_3>(body);
    if (!bodyBase_3)
    {
        LOG_CRITICAL << "Body can't be nullptr";
        return;
    }
    CHECK_RESOURCE_CONTEXT(bodyBase_3);

    bodyBase_3->setDynamicWorld(std::weak_ptr<btDiscreteDynamicsWorld>());

    for (auto& constraint : bodyBase_3->constraints())
    {
        auto constraintBase_3 = std::dynamic_pointer_cast<ConstraintBase_3>(constraint);
        CHECK_RESOURCE_CONTEXT(constraintBase_3);

        if (!constraintBase_3)
        {
            LOG_CRITICAL << "Constraint can't be nullptr";
            return;
        }

        BodyBase_3::addConstraintToWorld(*constraintBase_3);
    }
}

std::shared_ptr<core::physics::IStaticBody> Bullet_3_Renderer::createStaticBody() const
{
    CHECK_THIS_CONTEXT;

    return std::make_shared<StaticBody_3>();
}

std::shared_ptr<core::physics::IKinematicBody> Bullet_3_Renderer::createKinematicBody() const
{
    CHECK_THIS_CONTEXT;

    return std::make_shared<KinematicBody_3>();
}

std::shared_ptr<core::physics::IDynamicBody> Bullet_3_Renderer::createDynamicBody() const
{
    CHECK_THIS_CONTEXT;

    return std::make_shared<DynamicBody_3>();
}

std::optional<core::physics::RendererBase::LineSegmentIntersectionResult> Bullet_3_Renderer::singleIntersection(
    const utils::LineSegment& lineSegment) const
{
    CHECK_THIS_CONTEXT;

    const auto rayFromWorld = Conversions::vec2Bt(lineSegment.startPoint());
    const auto rayToWorld = Conversions::vec2Bt(lineSegment.endPoint());

    btCollisionWorld::ClosestRayResultCallback rayCallback(rayFromWorld, rayToWorld);
    m_dynamicWorld->rayTest(rayFromWorld, rayToWorld, rayCallback);

    if (!rayCallback.hasHit()) return std::nullopt;

    auto bodyUserPointer = reinterpret_cast<BodyUserPointer_3*>(rayCallback.m_collisionObject->getUserPointer());
    if (!bodyUserPointer) return std::nullopt;

    auto body = bodyUserPointer->bodyBase().shared_from_this();
    const auto bodyTransformInverted = body->transform().inverted();

    return LineSegmentIntersectionResult{
        body, bodyTransformInverted.transformPoint(Conversions::bt2Vec(rayCallback.m_hitPointWorld)),
        bodyTransformInverted.transformVector(Conversions::bt2Vec(rayCallback.m_hitNormalWorld)),
        Conversions::bt2Float(rayCallback.m_closestHitFraction)};
}

std::vector<core::physics::RendererBase::LineSegmentIntersectionResult> Bullet_3_Renderer::allIntersections(
    const utils::LineSegment& lineSegment) const
{
    CHECK_THIS_CONTEXT;

    const auto rayFromWorld = Conversions::vec2Bt(lineSegment.startPoint());
    const auto rayToWorld = Conversions::vec2Bt(lineSegment.endPoint());

    btCollisionWorld::AllHitsRayResultCallback rayCallback(rayFromWorld, rayToWorld);
    m_dynamicWorld->rayTest(rayFromWorld, rayToWorld, rayCallback);

    const auto intersectionsCount = rayCallback.m_collisionObjects.size();

    std::vector<core::physics::RendererBase::LineSegmentIntersectionResult> result;
    result.reserve(intersectionsCount);
    for (size_t i = 0u; i < intersectionsCount; ++i)
    {
        auto bodyUserPointer = reinterpret_cast<BodyUserPointer_3*>(rayCallback.m_collisionObjects[i]->getUserPointer());
        if (!bodyUserPointer) continue;

        auto body = bodyUserPointer->bodyBase().shared_from_this();
        const auto bodyTransformInverted = body->transform().inverted();

        result.push_back(
            {body, bodyTransformInverted.transformPoint(Conversions::bt2Vec(rayCallback.m_hitPointWorld[i])),
             bodyTransformInverted.transformVector(Conversions::bt2Vec(rayCallback.m_hitNormalWorld[i])),
             Conversions::bt2Float(rayCallback.m_hitFractions[i])});
    }

    std::sort(result.begin(), result.end());

    return result;
}

bool Bullet_3_Renderer::doMakeCurrent()
{
    return true;
}

bool Bullet_3_Renderer::doDoneCurrent()
{
    return true;
}

bool Bullet_3_Renderer::collisionCallback(
    btManifoldPoint& cp,
    const btCollisionObjectWrapper* colObj0Wrap,
    int partId0,
    int index0,
    const btCollisionObjectWrapper* colObj1Wrap,
    int partId1,
    int index1)
{
    return true;
}

} // namespace physics_bullet
} // namespace simplex
