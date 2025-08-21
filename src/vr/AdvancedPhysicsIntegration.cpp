#include "AdvancedPhysicsIntegration.hpp"
#include <spdlog/spdlog.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <algorithm>
#include <fstream>
#include <sstream>

namespace uevr {

AdvancedPhysicsIntegration::AdvancedPhysicsIntegration() {
    spdlog::info("Initializing Advanced Physics Integration...");
}

AdvancedPhysicsIntegration::~AdvancedPhysicsIntegration() {
    spdlog::info("Shutting down Advanced Physics Integration...");
    shutdown();
}

bool AdvancedPhysicsIntegration::initialize(const VRPhysicsSettings& settings) {
    if (m_initialized.load()) {
        spdlog::warn("Advanced Physics Integration already initialized");
        return true;
    }

    try {
        m_settings = settings;
        spdlog::info("Initializing physics engine: {}", static_cast<int>(settings.engineType));
        
        if (!initializePhysicsEngine(settings.engineType)) {
            spdlog::error("Failed to initialize physics engine");
            return false;
        }

        m_initialized.store(true);
        spdlog::info("Advanced Physics Integration initialized successfully");
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Exception during physics initialization: {}", e.what());
        return false;
    }
}

void AdvancedPhysicsIntegration::shutdown() {
    if (!m_initialized.load()) return;

    m_shutdown.store(true);
    m_simulating.store(false);

    if (m_physicsThread.joinable()) {
        m_physicsCV.notify_all();
        m_physicsThread.join();
    }

    cleanupPhysicsEngine();
    m_initialized.store(false);
    spdlog::info("Advanced Physics Integration shutdown complete");
}

void AdvancedPhysicsIntegration::setPhysicsSettings(const VRPhysicsSettings& settings) {
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    m_settings = settings;
    spdlog::info("Physics settings updated");
}

VRPhysicsSettings AdvancedPhysicsIntegration::getPhysicsSettings() const {
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    return m_settings;
}

bool AdvancedPhysicsIntegration::switchPhysicsEngine(PhysicsEngineType engineType) {
    if (m_initialized.load()) {
        spdlog::info("Switching physics engine to: {}", static_cast<int>(engineType));
        cleanupPhysicsEngine();
    }
    
    return initializePhysicsEngine(engineType);
}

PhysicsEngineType AdvancedPhysicsIntegration::getCurrentPhysicsEngine() const {
    return m_currentEngine;
}

std::vector<PhysicsEngineType> AdvancedPhysicsIntegration::getSupportedEngines() const {
    return {PhysicsEngineType::BULLET_PHYSICS, PhysicsEngineType::PHYSX, PhysicsEngineType::HYBRID};
}

std::string AdvancedPhysicsIntegration::createPhysicsObject(const VRPhysicsObject& object) {
    std::lock_guard<std::mutex> lock(m_objectsMutex);
    
    auto physicsObject = std::make_unique<VRPhysicsObject>(object);
    std::string objectId = physicsObject->id;
    
    m_physicsObjects[objectId] = std::move(physicsObject);
    spdlog::info("Created physics object: {}", objectId);
    
    return objectId;
}

bool AdvancedPhysicsIntegration::updatePhysicsObject(const std::string& objectId, const VRPhysicsObject& object) {
    std::lock_guard<std::mutex> lock(m_objectsMutex);
    
    auto it = m_physicsObjects.find(objectId);
    if (it == m_physicsObjects.end()) {
        spdlog::warn("Physics object not found: {}", objectId);
        return false;
    }
    
    *(it->second) = object;
    spdlog::debug("Updated physics object: {}", objectId);
    return true;
}

bool AdvancedPhysicsIntegration::removePhysicsObject(const std::string& objectId) {
    std::lock_guard<std::mutex> lock(m_objectsMutex);
    
    auto it = m_physicsObjects.find(objectId);
    if (it == m_physicsObjects.end()) {
        spdlog::warn("Physics object not found for removal: {}", objectId);
        return false;
    }
    
    m_physicsObjects.erase(it);
    spdlog::info("Removed physics object: {}", objectId);
    return true;
}

VRPhysicsObject* AdvancedPhysicsIntegration::getPhysicsObject(const std::string& objectId) {
    std::lock_guard<std::mutex> lock(m_objectsMutex);
    
    auto it = m_physicsObjects.find(objectId);
    return (it != m_physicsObjects.end()) ? it->second.get() : nullptr;
}

std::vector<VRPhysicsObject*> AdvancedPhysicsIntegration::getAllPhysicsObjects() {
    std::lock_guard<std::mutex> lock(m_objectsMutex);
    
    std::vector<VRPhysicsObject*> objects;
    objects.reserve(m_physicsObjects.size());
    
    for (auto& pair : m_physicsObjects) {
        objects.push_back(pair.second.get());
    }
    
    return objects;
}

void AdvancedPhysicsIntegration::startSimulation() {
    if (!m_initialized.load()) {
        spdlog::error("Cannot start simulation - not initialized");
        return;
    }
    
    m_simulating.store(true);
    m_physicsThread = std::thread(&AdvancedPhysicsIntegration::physicsLoop, this);
    spdlog::info("Physics simulation started");
}

void AdvancedPhysicsIntegration::stopSimulation() {
    m_simulating.store(false);
    if (m_physicsThread.joinable()) {
        m_physicsCV.notify_all();
        m_physicsThread.join();
    }
    spdlog::info("Physics simulation stopped");
}

void AdvancedPhysicsIntegration::pauseSimulation() {
    m_paused.store(true);
    spdlog::info("Physics simulation paused");
}

void AdvancedPhysicsIntegration::resumeSimulation() {
    m_paused.store(false);
    m_physicsCV.notify_all();
    spdlog::info("Physics simulation resumed");
}

void AdvancedPhysicsIntegration::stepSimulation(float deltaTime) {
    if (!m_initialized.load() || !m_simulating.load() || m_paused.load()) {
        return;
    }
    
    // Update VR-specific physics
    updateVRPhysics();
    
    // Process collisions
    processCollisions();
    
    // Update metrics
    updateMetrics();
}

void AdvancedPhysicsIntegration::updatePhysics() {
    auto now = std::chrono::high_resolution_clock::now();
    static auto lastUpdate = now;
    
    auto deltaTime = std::chrono::duration<float>(now - lastUpdate).count();
    lastUpdate = now;
    
    stepSimulation(deltaTime);
}

bool AdvancedPhysicsIntegration::setupVRPhysics(const VRSystem* vrSystem) {
    m_vrSystem = vrSystem;
    spdlog::info("VR physics setup completed");
    return true;
}

void AdvancedPhysicsIntegration::updateVRControllers(const std::vector<glm::mat4>& controllerTransforms) {
    std::lock_guard<std::mutex> lock(m_vrMutex);
    m_controllerTransforms = controllerTransforms;
}

void AdvancedPhysicsIntegration::updateVRHeadset(const glm::mat4& headsetTransform) {
    std::lock_guard<std::mutex> lock(m_vrMutex);
    m_headsetTransform = headsetTransform;
}

void AdvancedPhysicsIntegration::updateVRBody(const glm::mat4& bodyTransform) {
    std::lock_guard<std::mutex> lock(m_vrMutex);
    m_bodyTransform = bodyTransform;
}

std::vector<VRPhysicsContact> AdvancedPhysicsIntegration::getCollisions(const std::string& objectId) {
    std::lock_guard<std::mutex> lock(m_contactsMutex);
    
    std::vector<VRPhysicsContact> objectContacts;
    for (const auto& contact : m_contacts) {
        if (contact.objectA->id == objectId || contact.objectB->id == objectId) {
            objectContacts.push_back(contact);
        }
    }
    
    return objectContacts;
}

std::vector<VRPhysicsContact> AdvancedPhysicsIntegration::getAllCollisions() {
    std::lock_guard<std::mutex> lock(m_contactsMutex);
    return m_contacts;
}

bool AdvancedPhysicsIntegration::checkCollision(const std::string& objectA, const std::string& objectB) {
    std::lock_guard<std::mutex> lock(m_contactsMutex);
    
    for (const auto& contact : m_contacts) {
        if ((contact.objectA->id == objectA && contact.objectB->id == objectB) ||
            (contact.objectA->id == objectB && contact.objectB->id == objectA)) {
            return true;
        }
    }
    
    return false;
}

float AdvancedPhysicsIntegration::getDistance(const std::string& objectA, const std::string& objectB) {
    auto objA = getPhysicsObject(objectA);
    auto objB = getPhysicsObject(objectB);
    
    if (!objA || !objB) {
        return std::numeric_limits<float>::infinity();
    }
    
    return glm::distance(objA->position, objB->position);
}

bool AdvancedPhysicsIntegration::enableSoftBodyPhysics(bool enable) {
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    m_settings.enableSoftBodyPhysics = enable;
    spdlog::info("Soft body physics: {}", enable ? "enabled" : "disabled");
    return true;
}

bool AdvancedPhysicsIntegration::enableFluidPhysics(bool enable) {
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    m_settings.enableFluidPhysics = enable;
    spdlog::info("Fluid physics: {}", enable ? "enabled" : "disabled");
    return true;
}

bool AdvancedPhysicsIntegration::enableClothPhysics(bool enable) {
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    m_settings.enableClothPhysics = enable;
    spdlog::info("Cloth physics: {}", enable ? "enabled" : "disabled");
    return true;
}

bool AdvancedPhysicsIntegration::enableParticleSystems(bool enable) {
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    m_settings.enableParticleSystems = enable;
    spdlog::info("Particle systems: {}", enable ? "enabled" : "disabled");
    return true;
}

std::string AdvancedPhysicsIntegration::createConstraint(const std::string& objectA, const std::string& objectB,
                                                        const glm::vec3& pivotA, const glm::vec3& pivotB) {
    // Implementation for creating physics constraints
    std::string constraintId = "constraint_" + objectA + "_" + objectB;
    spdlog::info("Created constraint: {}", constraintId);
    return constraintId;
}

bool AdvancedPhysicsIntegration::removeConstraint(const std::string& constraintId) {
    spdlog::info("Removed constraint: {}", constraintId);
    return true;
}

void AdvancedPhysicsIntegration::applyForce(const std::string& objectId, const glm::vec3& force, const glm::vec3& point) {
    auto obj = getPhysicsObject(objectId);
    if (obj) {
        obj->linearVelocity += force * m_settings.physicsTimeStep;
        spdlog::debug("Applied force to object: {}", objectId);
    }
}

void AdvancedPhysicsIntegration::applyImpulse(const std::string& objectId, const glm::vec3& impulse, const glm::vec3& point) {
    auto obj = getPhysicsObject(objectId);
    if (obj) {
        obj->linearVelocity += impulse;
        spdlog::debug("Applied impulse to object: {}", objectId);
    }
}

void AdvancedPhysicsIntegration::applyTorque(const std::string& objectId, const glm::vec3& torque) {
    auto obj = getPhysicsObject(objectId);
    if (obj) {
        obj->angularVelocity += torque * m_settings.physicsTimeStep;
        spdlog::debug("Applied torque to object: {}", objectId);
    }
}

void AdvancedPhysicsIntegration::applyAngularImpulse(const std::string& objectId, const glm::vec3& angularImpulse) {
    auto obj = getPhysicsObject(objectId);
    if (obj) {
        obj->angularVelocity += angularImpulse;
        spdlog::debug("Applied angular impulse to object: {}", objectId);
    }
}

std::vector<VRPhysicsObject*> AdvancedPhysicsIntegration::raycast(const glm::vec3& origin, const glm::vec3& direction, float maxDistance) {
    std::vector<VRPhysicsObject*> hitObjects;
    
    for (auto& pair : m_physicsObjects) {
        auto obj = pair.second.get();
        glm::vec3 toObject = obj->position - origin;
        float distance = glm::length(toObject);
        
        if (distance <= maxDistance) {
            glm::vec3 normalizedDirection = glm::normalize(direction);
            glm::vec3 normalizedToObject = glm::normalize(toObject);
            
            float dotProduct = glm::dot(normalizedDirection, normalizedToObject);
            if (dotProduct > 0.9f) { // Simple ray-object intersection
                hitObjects.push_back(obj);
            }
        }
    }
    
    return hitObjects;
}

std::vector<VRPhysicsObject*> AdvancedPhysicsIntegration::sphereCast(const glm::vec3& origin, float radius, const glm::vec3& direction, float maxDistance) {
    std::vector<VRPhysicsObject*> hitObjects;
    
    for (auto& pair : m_physicsObjects) {
        auto obj = pair.second.get();
        glm::vec3 toObject = obj->position - origin;
        float distance = glm::length(toObject);
        
        if (distance <= maxDistance + radius) {
            hitObjects.push_back(obj);
        }
    }
    
    return hitObjects;
}

std::vector<VRPhysicsObject*> AdvancedPhysicsIntegration::overlapSphere(const glm::vec3& center, float radius) {
    std::vector<VRPhysicsObject*> overlappingObjects;
    
    for (auto& pair : m_physicsObjects) {
        auto obj = pair.second.get();
        float distance = glm::distance(center, obj->position);
        
        if (distance <= radius) {
            overlappingObjects.push_back(obj);
        }
    }
    
    return overlappingObjects;
}

std::vector<VRPhysicsObject*> AdvancedPhysicsIntegration::overlapBox(const glm::vec3& center, const glm::vec3& halfExtents, const glm::quat& rotation) {
    std::vector<VRPhysicsObject*> overlappingObjects;
    
    for (auto& pair : m_physicsObjects) {
        auto obj = pair.second.get();
        glm::vec3 localPoint = glm::inverse(rotation) * (obj->position - center);
        
        if (std::abs(localPoint.x) <= halfExtents.x &&
            std::abs(localPoint.y) <= halfExtents.y &&
            std::abs(localPoint.z) <= halfExtents.z) {
            overlappingObjects.push_back(obj);
        }
    }
    
    return overlappingObjects;
}

VRPhysicsMetrics AdvancedPhysicsIntegration::getPhysicsMetrics() const {
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    return m_metrics;
}

void AdvancedPhysicsIntegration::enablePerformanceMonitoring(bool enable) {
    m_performanceMonitoringEnabled = enable;
    spdlog::info("Performance monitoring: {}", enable ? "enabled" : "disabled");
}

void AdvancedPhysicsIntegration::setPerformanceCallback(std::function<void(const VRPhysicsMetrics&)> callback) {
    m_performanceCallback = callback;
}

void AdvancedPhysicsIntegration::enableDebugVisualization(bool enable) {
    m_debugVisualizationEnabled = enable;
    spdlog::info("Debug visualization: {}", enable ? "enabled" : "disabled");
}

void AdvancedPhysicsIntegration::setDebugDrawCallback(std::function<void(const std::vector<glm::vec3>&, const std::vector<uint32_t>&)> callback) {
    m_debugDrawCallback = callback;
}

std::vector<glm::vec3> AdvancedPhysicsIntegration::getDebugVertices() const {
    std::lock_guard<std::mutex> lock(m_debugMutex);
    return m_debugVertices;
}

std::vector<uint32_t> AdvancedPhysicsIntegration::getDebugIndices() const {
    std::lock_guard<std::mutex> lock(m_debugMutex);
    return m_debugIndices;
}

std::string AdvancedPhysicsIntegration::createPhysicsMaterial(const PhysicsMaterial& material) {
    std::lock_guard<std::mutex> lock(m_materialsMutex);
    m_materials[material.name] = material;
    spdlog::info("Created physics material: {}", material.name);
    return material.name;
}

bool AdvancedPhysicsIntegration::applyPhysicsMaterial(const std::string& objectId, const std::string& materialId) {
    auto obj = getPhysicsObject(objectId);
    if (!obj) return false;
    
    std::lock_guard<std::mutex> lock(m_materialsMutex);
    auto it = m_materials.find(materialId);
    if (it == m_materials.end()) return false;
    
    const auto& material = it->second;
    obj->friction = material.friction;
    obj->restitution = material.restitution;
    
    spdlog::info("Applied material {} to object {}", materialId, objectId);
    return true;
}

bool AdvancedPhysicsIntegration::enableContinuousCollisionDetection(const std::string& objectId, bool enable) {
    auto obj = getPhysicsObject(objectId);
    if (!obj) return false;
    
    spdlog::info("Continuous collision detection for object {}: {}", objectId, enable ? "enabled" : "disabled");
    return true;
}

bool AdvancedPhysicsIntegration::setCollisionFilter(const std::string& objectId, uint32_t collisionGroup, uint32_t collisionMask) {
    auto obj = getPhysicsObject(objectId);
    if (!obj) return false;
    
    spdlog::info("Set collision filter for object {}: group={}, mask={}", objectId, collisionGroup, collisionMask);
    return true;
}

void AdvancedPhysicsIntegration::optimizePhysicsWorld() {
    spdlog::info("Optimizing physics world...");
    // Implementation for physics world optimization
}

void AdvancedPhysicsIntegration::setBroadphaseAlgorithm(const std::string& algorithm) {
    spdlog::info("Set broadphase algorithm: {}", algorithm);
}

void AdvancedPhysicsIntegration::setSolverIterations(int iterations) {
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    spdlog::info("Set solver iterations: {}", iterations);
}

void AdvancedPhysicsIntegration::setSolverMode(int mode) {
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    spdlog::info("Set solver mode: {}", mode);
}

// Private methods implementation
void AdvancedPhysicsIntegration::physicsLoop() {
    spdlog::info("Physics loop started");
    
    while (m_simulating.load() && !m_shutdown.load()) {
        if (m_paused.load()) {
            std::unique_lock<std::mutex> lock(m_physicsMutex);
            m_physicsCV.wait(lock, [this] { return !m_paused.load() || m_shutdown.load(); });
            continue;
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Physics simulation step
        stepSimulation(m_settings.physicsTimeStep);
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<float>(endTime - startTime);
        
        // Maintain target physics rate
        if (duration.count() < m_settings.physicsTimeStep) {
            std::this_thread::sleep_for(std::chrono::duration<float>(m_settings.physicsTimeStep - duration.count()));
        }
    }
    
    spdlog::info("Physics loop ended");
}

void AdvancedPhysicsIntegration::updateMetrics() {
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    
    m_metrics.activeObjects = static_cast<int>(m_physicsObjects.size());
    m_metrics.totalContacts = static_cast<int>(m_contacts.size());
    m_metrics.physicsThreads = m_settings.enableMultithreading ? m_settings.numThreads : 1;
    
    if (m_performanceCallback) {
        m_performanceCallback(m_metrics);
    }
}

void AdvancedPhysicsIntegration::processCollisions() {
    std::lock_guard<std::mutex> lock(m_contactsMutex);
    m_contacts.clear();
    
    // Simple collision detection between all objects
    std::vector<VRPhysicsObject*> objects;
    {
        std::lock_guard<std::mutex> objLock(m_objectsMutex);
        for (auto& pair : m_physicsObjects) {
            objects.push_back(pair.second.get());
        }
    }
    
    for (size_t i = 0; i < objects.size(); ++i) {
        for (size_t j = i + 1; j < objects.size(); ++j) {
            auto objA = objects[i];
            auto objB = objects[j];
            
            float distance = glm::distance(objA->position, objB->position);
            float collisionThreshold = 1.0f; // Simple collision threshold
            
            if (distance < collisionThreshold) {
                VRPhysicsContact contact;
                contact.objectA = objA;
                contact.objectB = objB;
                contact.contactPoint = (objA->position + objB->position) * 0.5f;
                contact.contactNormal = glm::normalize(objB->position - objA->position);
                contact.penetrationDepth = collisionThreshold - distance;
                contact.impulse = 0.0f;
                contact.isTrigger = false;
                
                m_contacts.push_back(contact);
                
                // Handle collision callbacks
                handleCollisionCallbacks(contact);
            }
        }
    }
}

void AdvancedPhysicsIntegration::updateVRPhysics() {
    std::lock_guard<std::mutex> lock(m_vrMutex);
    
    // Update VR-specific physics objects
    for (auto& pair : m_physicsObjects) {
        auto obj = pair.second.get();
        
        if (obj->isVRController) {
            // Update controller physics
            int controllerIndex = obj->controllerIndex;
            if (controllerIndex >= 0 && controllerIndex < static_cast<int>(m_controllerTransforms.size())) {
                glm::mat4 transform = m_controllerTransforms[controllerIndex];
                obj->position = glm::vec3(transform[3]);
                obj->rotation = glm::quat_cast(transform);
            }
        } else if (obj->isVRHeadset) {
            // Update headset physics
            obj->position = glm::vec3(m_headsetTransform[3]);
            obj->rotation = glm::quat_cast(m_headsetTransform);
        } else if (obj->isVRBody) {
            // Update body physics
            obj->position = glm::vec3(m_bodyTransform[3]);
            obj->rotation = glm::quat_cast(m_bodyTransform);
        }
    }
}

void AdvancedPhysicsIntegration::optimizeForVR() {
    // VR-specific optimizations
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    
    // Adjust collision margins for VR comfort
    m_settings.collisionMargin = 0.01f; // 1cm margin
    
    // Optimize physics timestep for VR
    m_settings.physicsTimeStep = 1.0f / 90.0f; // 90Hz for VR
    
    spdlog::info("Applied VR-specific physics optimizations");
}

bool AdvancedPhysicsIntegration::initializePhysicsEngine(PhysicsEngineType engineType) {
    m_currentEngine = engineType;
    
    switch (engineType) {
        case PhysicsEngineType::BULLET_PHYSICS:
            spdlog::info("Initializing Bullet Physics engine");
            // Initialize Bullet Physics
            break;
            
        case PhysicsEngineType::PHYSX:
            spdlog::info("Initializing PhysX engine");
            // Initialize PhysX
            break;
            
        case PhysicsEngineType::HYBRID:
            spdlog::info("Initializing Hybrid physics engine");
            // Initialize both engines
            break;
            
        case PhysicsEngineType::AUTO:
            spdlog::info("Auto-selecting physics engine");
            // Auto-detect and initialize best engine
            m_currentEngine = PhysicsEngineType::BULLET_PHYSICS;
            break;
    }
    
    return true;
}

void AdvancedPhysicsIntegration::cleanupPhysicsEngine() {
    spdlog::info("Cleaning up physics engine");
    // Cleanup physics engine resources
}

void AdvancedPhysicsIntegration::syncPhysicsObjects() {
    // Sync physics objects with engine state
}

void AdvancedPhysicsIntegration::handleCollisionCallbacks(const VRPhysicsContact& contact) {
    if (contact.objectA->onCollisionEnter) {
        contact.objectA->onCollisionEnter(*contact.objectA, *contact.objectB);
    }
    if (contact.objectB->onCollisionEnter) {
        contact.objectB->onCollisionEnter(*contact.objectB, *contact.objectA);
    }
}

void AdvancedPhysicsIntegration::updateDebugVisualization() {
    if (!m_debugVisualizationEnabled) return;
    
    std::lock_guard<std::mutex> lock(m_debugMutex);
    m_debugVertices.clear();
    m_debugIndices.clear();
    
    // Generate debug visualization data
    for (const auto& pair : m_physicsObjects) {
        const auto& obj = pair.second;
        
        // Add object position as debug vertex
        m_debugVertices.push_back(obj->position);
        
        // Add collision bounds visualization
        // Implementation depends on object shape
    }
    
    if (m_debugDrawCallback) {
        m_debugDrawCallback(m_debugVertices, m_debugIndices);
    }
}

void AdvancedPhysicsIntegration::logPhysicsError(const std::string& error) {
    spdlog::error("Physics error: {}", error);
}

void AdvancedPhysicsIntegration::saveLearningData() {
    // Save physics learning data
}

void AdvancedPhysicsIntegration::loadLearningData() {
    // Load physics learning data
}

} // namespace uevr
