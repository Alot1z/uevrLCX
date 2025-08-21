#pragma once

#include <string>
#include <memory>
#include <vector>
#include "VRConversionSystem.h"

namespace UEVRLCX {
namespace VR {

struct CollisionResult {
    bool hasCollision;
    float distance;
    std::vector<float> collisionPoint;
    std::vector<float> normal;
    std::string objectId;
};

class CollisionDetector {
public:
    CollisionDetector();
    ~CollisionDetector();

    bool Initialize();
    bool Setup(const GameInfo& gameInfo);
    bool Test();
    bool EnableRealCollision(bool enable);
    bool SetCollisionAccuracy(double accuracy);
    
    // Collision detection methods
    CollisionResult DetectCollision(const std::vector<float>& position, const std::vector<float>& direction);
    bool AddCollisionMesh(const std::string& meshId, const std::vector<float>& vertices, const std::vector<int>& indices);
    bool RemoveCollisionMesh(const std::string& meshId);
    bool UpdateCollisionMeshes();
    
    // Real collision features
    bool EnableSpatialHashing(bool enable);
    bool EnableOctreeOptimization(bool enable);
    bool SetCollisionThreshold(float threshold);
    double GetCollisionAccuracy() const;

private:
    bool m_isInitialized;
    bool m_realCollisionEnabled;
    bool m_spatialHashingEnabled;
    bool m_octreeOptimizationEnabled;
    double m_collisionAccuracy;
    float m_collisionThreshold;
    
    // Collision detection system
    void* m_collisionWorld;
    void* m_spatialHash;
    void* m_octree;
    std::unordered_map<std::string, void*> m_collisionMeshes;
};

} // namespace VR
} // namespace UEVRLCX
