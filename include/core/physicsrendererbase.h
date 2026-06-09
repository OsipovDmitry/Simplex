#ifndef CORE_PHYSICSRENDERERBASE_H
#define CORE_PHYSICSRENDERERBASE_H

#include <optional>
#include <vector>

#include <utils/customdata.h>
#include <utils/idgenerator.h>
#include <utils/transform.h>

#include <core/coreglobal.h>
#include <core/forwarddecl.h>
#include <core/irenderer.h>

namespace simplex
{
namespace core
{
namespace physics
{

class IBody
{
public:
    class CustomData
    {
    public:
        virtual ~CustomData() = default;
    };

    CUSTOMDATA(CustomData)
public:
    virtual ~IBody() = default;

    virtual const utils::Transform& transform() const = 0;
    virtual void setTransform(const utils::Transform&) = 0;

    virtual float restitution() const = 0;
    virtual void setRestitution(float) = 0;

    virtual float linearDamping() const = 0;
    virtual void setLinearDamping(float) = 0;

    virtual float angularDamping() const = 0;
    virtual void setAngularDamping(float) = 0;

    virtual float linearSleepingThreshold() const = 0;
    virtual void setLinearSleepingThreshold(float) = 0;

    virtual float angularSleepingThreshold() const = 0;
    virtual void setAngularSleepingThreshold(float) = 0;

    virtual void removeShape(utils::IDsGenerator::value_type) = 0;
    virtual utils::IDsGenerator::value_type addSphereShape(
        float radius,
        const utils::RigidTransform& localTransform = utils::RigidTransform()) = 0;
    virtual utils::IDsGenerator::value_type addBoxShape(
        const glm::vec3& halfSizes,
        const utils::RigidTransform& localTransform = utils::RigidTransform()) = 0;
    virtual utils::IDsGenerator::value_type addCapsuleShape(
        float radius,
        float height,
        const utils::RigidTransform& localTransform = utils::RigidTransform()) = 0;
    virtual utils::IDsGenerator::value_type addCylinderShape(
        const glm::vec3& halfSizes,
        const utils::RigidTransform& localTransform = utils::RigidTransform()) = 0;
    virtual utils::IDsGenerator::value_type addConeShape(
        float radius,
        float height,
        const utils::RigidTransform& localTransform = utils::RigidTransform()) = 0;
    virtual utils::IDsGenerator::value_type addConvexHullShape(
        const std::vector<glm::vec3>& vertices,
        const utils::RigidTransform& localTransform = utils::RigidTransform()) = 0;

    virtual const std::set<std::shared_ptr<IConstraint>>& constraints() const = 0;
    virtual std::shared_ptr<IFixedConstraint> joinFixed(
        const utils::RigidTransform& frame,
        const std::shared_ptr<IBody>& anotherBody,
        const utils::RigidTransform& anotherFrame) = 0;
    virtual std::shared_ptr<IPointToPointConstraint> joinPointToPoint(
        const glm::vec3& pivot,
        const std::shared_ptr<IBody>& anotherBody = nullptr,
        const glm::vec3& anotherPivot = glm::vec3(0.f)) = 0;
    virtual std::shared_ptr<IHingeConstraint> joinHinge(
        const utils::RigidTransform& frame,
        const std::shared_ptr<IBody>& anotherBody = nullptr,
        const utils::RigidTransform& anotherFrame = utils::RigidTransform()) = 0;
    virtual std::shared_ptr<ISliderConstraint> joinSlider(
        const utils::RigidTransform& frame,
        const std::shared_ptr<IBody>& anotherBody = nullptr,
        const utils::RigidTransform& anotherFrame = utils::RigidTransform()) = 0;
};

class IStaticBody : public virtual IBody
{
public:
    virtual utils::IDsGenerator::value_type addTriangleMeshShape(
        const std::vector<glm::vec3>& vertices,
        const std::vector<uint32_t>& indices,
        const utils::RigidTransform& localTransform = utils::RigidTransform()) = 0;
    virtual utils::IDsGenerator::value_type addPlaneShape(
        const utils::Plane& plane,
        const utils::RigidTransform& localTransform = utils::RigidTransform()) = 0;
    virtual utils::IDsGenerator::value_type addHeightfieldShape(
        const utils::RigidTransform& localTransform = utils::RigidTransform()) = 0;
};

class IKinematicBody : public virtual IBody
{
public:
};

class IDynamicBody : public virtual IBody
{
public:
    virtual float mass() const = 0;
    virtual void setMass(float) = 0;

    virtual void applyForce(const glm::vec3& force, const glm::vec3& localPosition) = 0;
    virtual void applyImpulse(const glm::vec3& impulse, const glm::vec3& localPosition) = 0;

    virtual std::pair<bool, utils::Transform> resetTransformChangedAfterSimulation() = 0;
};

class IConstraint
{
public:
    virtual ~IConstraint() = default;

    virtual std::weak_ptr<IBody> body1() = 0;
    virtual std::weak_ptr<const IBody> body1() const = 0;

    virtual std::weak_ptr<IBody> body2() = 0;
    virtual std::weak_ptr<const IBody> body2() const = 0;

    virtual void detachFromBodies() = 0;

    virtual bool isCollisionBetweenLinkedBodiesEnabled() const = 0;
    virtual void enableCollisionBetweenLinkedBodies(bool = true) = 0;
};

class IFixedConstraint : public virtual IConstraint
{
public:
    virtual utils::RigidTransform frame1() const = 0;
    virtual utils::RigidTransform frame2() const = 0;
};

class IPointToPointConstraint : public virtual IConstraint
{
public:
    virtual glm::vec3 pivot1() const = 0;
    virtual glm::vec3 pivot2() const = 0;
};

class IHingeConstraint : public virtual IConstraint
{
public:
    virtual utils::RigidTransform frame1() const = 0;
    virtual utils::RigidTransform frame2() const = 0;

    virtual utils::Range angularLimits() const = 0;
    virtual void setAngularLimits(const utils::Range&) = 0;
};

class ISliderConstraint : public virtual IConstraint
{
public:
    virtual utils::RigidTransform frame1() const = 0;
    virtual utils::RigidTransform frame2() const = 0;

    virtual utils::Range linearLimits() const = 0;
    virtual void setLinearLimits(const utils::Range&) = 0;

    virtual utils::Range angularLimits() const = 0;
    virtual void setAngularLimits(const utils::Range&) = 0;
};

class RendererBasePrivate;
class CORE_SHARED_EXPORT RendererBase : public std::enable_shared_from_this<RendererBase>, public IRenderer
{
public:
    RendererBase(const std::string&);
    ~RendererBase() override;

    const std::string& name() const override;

    bool makeCurrent() override final;
    bool doneCurrent() override final;
    static std::shared_ptr<RendererBase> current();

    virtual std::shared_ptr<IPhysicsWorld> world() = 0;
    virtual std::shared_ptr<const IPhysicsWorld> world() const = 0;

    virtual const glm::vec3& gravity() const = 0;
    virtual void setGravity(const glm::vec3&) = 0;

    virtual uint32_t simulationTimeStep() const = 0;
    virtual void setSimulationTimeStep(uint32_t) = 0;

    virtual void stepSimulation(uint32_t) = 0;
    virtual void addBodyToSimulation(const std::shared_ptr<IBody>&) = 0;
    virtual void removeBodyFromSimulation(const std::shared_ptr<IBody>&) = 0;

    virtual std::shared_ptr<IStaticBody> createStaticBody() const = 0;
    virtual std::shared_ptr<IKinematicBody> createKinematicBody() const = 0;
    virtual std::shared_ptr<IDynamicBody> createDynamicBody() const = 0;

    struct LineSegmentIntersectionResult
    {
        std::shared_ptr<IBody> hitBody;
        glm::vec3 hitPosition;
        glm::vec3 hitNormal;
        float hitFraction;

        bool operator<(const LineSegmentIntersectionResult& other) const { return hitFraction < other.hitFraction; }
    };

    virtual std::optional<LineSegmentIntersectionResult> singleIntersection(const utils::LineSegment&) const = 0;
    virtual std::vector<LineSegmentIntersectionResult> allIntersections(const utils::LineSegment&) const = 0;

protected:
    virtual bool doMakeCurrent() = 0;
    virtual bool doDoneCurrent() = 0;

    std::unique_ptr<RendererBasePrivate> m_;
};

} // namespace physics
} // namespace core
} // namespace simplex

#endif // CORE_PHYSICSRENDERERBASE_H
