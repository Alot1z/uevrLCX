#pragma once

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

namespace uevr {
namespace vr {

// Basic math structs to avoid external dependencies
struct Vec3 { float x{0}, y{0}, z{0}; };
struct Quat { float x{0}, y{0}, z{0}, w{1}; };

// Supported physics backends (opaque; actual integration lives in adapters)
enum class PhysicsBackend {
    NONE = 0,
    BULLET,
    PHYSX,
    HAVOK,
    CHAOS,
    CUSTOM
};

// Generic collision event
struct CollisionEvent {
    uint64_t idA{0};
    uint64_t idB{0};
    Vec3 contactPoint{};
    Vec3 normal{};
    float impulse{0.f};
    bool began{false};
};

// Rigid body creation params (minimal, engine-agnostic)
struct RigidBodyDesc {
    uint64_t id{0};
    float mass{1.f};
    Vec3 position{};
    Quat rotation{};
    Vec3 inertia{}; // optional
    bool kinematic{false};
};

// Raycast result
struct RaycastHit {
    bool hit{false};
    uint64_t id{0};
    float distance{0.f};
    Vec3 point{};
    Vec3 normal{};
};

// Pure virtual interface for the physics bridge
class IPhysicsBridge {
public:
    virtual ~IPhysicsBridge() = default;

    // Lifecycle
    virtual bool initialize(PhysicsBackend backend) = 0;
    virtual void shutdown() = 0;
    virtual bool isInitialized() const = 0;

    // Simulation
    virtual bool stepSimulation(double deltaSeconds) = 0;
    virtual void setGravity(const Vec3& g) = 0;
    virtual Vec3 getGravity() const = 0;
    virtual void setWorldScale(float metersPerUnit) = 0; // compatibility with varying engine scales

    // Bodies (opaque IDs so adapters can map to engine types)
    virtual bool createRigidBody(const RigidBodyDesc& desc) = 0;
    virtual bool destroyRigidBody(uint64_t id) = 0;
    virtual bool setKinematic(uint64_t id, bool kinematic) = 0;
    virtual bool setLinearVelocity(uint64_t id, const Vec3& v) = 0;
    virtual bool setAngularVelocity(uint64_t id, const Vec3& w) = 0;
    virtual bool setTransform(uint64_t id, const Vec3& pos, const Quat& rot) = 0;
    virtual bool getTransform(uint64_t id, Vec3& outPos, Quat& outRot) const = 0;

    // Queries
    virtual RaycastHit raycast(const Vec3& origin, const Vec3& dir, float maxDistance) const = 0;
    virtual std::vector<uint64_t> overlapSphere(const Vec3& center, float radius) const = 0;

    // Events
    using CollisionCallback = void(*)(const CollisionEvent&);
    virtual void setCollisionCallback(CollisionCallback cb) = 0;

    // Debugging
    virtual void enableDebugDraw(bool enable) = 0;
    virtual bool isDebugDrawEnabled() const = 0;
};

// Factory (implementation provided by adapters/backends)
std::unique_ptr<IPhysicsBridge> CreatePhysicsBridge();

} // namespace vr
} // namespace uevr
