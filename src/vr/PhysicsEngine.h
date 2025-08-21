#pragma once

#include <string>
#include <memory>
#include <vector>
#include "VRConversionSystem.h"

namespace UEVRLCX {
namespace VR {

class PhysicsEngine {
public:
    PhysicsEngine();
    ~PhysicsEngine();

    bool Initialize();
    bool SetupCollisionWorld();
    bool AddCollisionObject(const std::string& objectId, const std::vector<float>& vertices);
    bool RemoveCollisionObject(const std::string& objectId);
    bool UpdateCollisionWorld();
    bool TestCollision(const std::string& objectId1, const std::string& objectId2);
    
    // Physics engine features
    bool SetGravity(float x, float y, float z);
    bool EnablePhysicsSimulation(bool enable);
    bool SetPhysicsAccuracy(double accuracy);
    bool EnableRealTimePhysics(bool enable);
    double GetPhysicsAccuracy() const;

private:
    bool m_isInitialized;
    bool m_physicsEnabled;
    bool m_realTimePhysics;
    double m_physicsAccuracy;
    std::vector<std::string> m_collisionObjects;
    
    // Physics engine integration (Bullet/PhysX)
    void* m_physicsWorld;
    void* m_collisionDispatcher;
    void* m_broadphase;
    void* m_constraintSolver;
};

} // namespace VR
} // namespace UEVRLCX
