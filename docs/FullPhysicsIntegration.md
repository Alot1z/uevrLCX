# FullPhysicsIntegration Class Documentation

## Overview

The `FullPhysicsIntegration` class is a comprehensive physics simulation system designed specifically for VR applications. It provides a complete physics engine with support for various object types, constraints, collision detection, and performance optimization.

## Class Definition

```cpp
class FullPhysicsIntegration
```

**Namespace:** `uevr::vr`

**Header:** `include/uevr/vr/FullPhysicsIntegration.hpp`

## Features

### Core Physics Simulation
- **Fixed Timestep Simulation**: Uses accumulator pattern for stable physics
- **Multi-Engine Support**: Built-in physics engine with extensible architecture
- **Quality Settings**: Configurable physics quality (LOW, MEDIUM, HIGH, ULTRA)
- **Performance Profiling**: Built-in performance metrics and profiling

### Object Management
- **Physics Objects**: Support for static, dynamic, and kinematic objects
- **Mass Properties**: Configurable mass, friction, and restitution
- **Collision Shapes**: Flexible collision shape system
- **Material Properties**: Advanced material system for realistic interactions

### Constraint System
- **Hinge Constraints**: Rotational constraints around a single axis
- **Ball and Socket**: 3D rotational constraints
- **Slider Constraints**: Linear movement constraints with limits
- **Fixed Constraints**: Rigid body connections

### Advanced Features
- **Gravity System**: Configurable global gravity with multiple types
- **Collision Engine Integration**: Seamless integration with collision detection
- **Debug Visualization**: Built-in debug drawing capabilities
- **Simulation Control**: Pause, resume, and reset functionality

## Constructor and Destructor

### Constructor
```cpp
FullPhysicsIntegration::FullPhysicsIntegration()
```

**Initialization:**
- Sets default physics engine to `PhysicsEngine::BUILTIN`
- Configures medium quality physics settings
- Sets global gravity to (0, -9.81, 0) m/s²
- Initializes object and constraint ID counters
- Disables profiling and debug drawing by default

### Destructor
```cpp
FullPhysicsIntegration::~FullPhysicsIntegration()
```

**Cleanup:**
- Automatically calls `shutdown()` method
- Logs destruction for debugging purposes

## Public Methods

### Initialization and Shutdown

#### `initialize()`
```cpp
bool initialize()
```

**Purpose:** Initializes the physics system and all internal components.

**Returns:** `true` on success, `false` on failure.

**Implementation Details:**
- Clears all existing physics objects and constraints
- Resets performance metrics
- Sets simulation state to running
- Initializes internal systems

#### `shutdown()`
```cpp
void shutdown()
```

**Purpose:** Gracefully shuts down the physics system.

**Implementation Details:**
- Stops physics simulation
- Clears all objects and constraints
- Resets internal state
- Logs shutdown completion

### Core Physics Simulation

#### `updatePhysicsSimulation(float delta_time)`
```cpp
void updatePhysicsSimulation(float delta_time)
```

**Purpose:** Main physics update loop called each frame.

**Parameters:**
- `delta_time`: Time elapsed since last frame in seconds

**Implementation Details:**
- Uses fixed timestep simulation with accumulator
- Updates all active physics objects
- Processes constraints and collision detection
- Updates performance metrics
- Handles multiple sub-steps for stability

### Physics Object Management

#### `createPhysicsObject(const PhysicsObjectCreateInfo& create_info)`
```cpp
ObjectID createPhysicsObject(const PhysicsObjectCreateInfo& create_info)
```

**Purpose:** Creates a new physics object in the simulation.

**Parameters:**
- `create_info`: Structure containing object creation parameters

**Returns:** Unique object ID or `INVALID_OBJECT_ID` on failure

**Implementation Details:**
- Allocates new physics object with unique ID
- Sets initial properties (mass, position, rotation, velocity)
- Configures collision shape and material properties
- Adds object to internal tracking system

#### `destroyPhysicsObject(ObjectID object_id)`
```cpp
bool destroyPhysicsObject(ObjectID object_id)
```

**Purpose:** Removes a physics object from the simulation.

**Parameters:**
- `object_id`: ID of the object to destroy

**Returns:** `true` if object was found and destroyed, `false` otherwise

#### `getPhysicsObject(ObjectID object_id)`
```cpp
PhysicsObject* getPhysicsObject(ObjectID object_id)
```

**Purpose:** Retrieves a physics object by ID.

**Parameters:**
- `object_id`: ID of the object to retrieve

**Returns:** Pointer to physics object or `nullptr` if not found

### Constraint Management

#### `createConstraint(const ConstraintCreateInfo& create_info)`
```cpp
ObjectID createConstraint(const ConstraintCreateInfo& create_info)
```

**Purpose:** Creates a new constraint between two physics objects.

**Parameters:**
- `create_info`: Structure containing constraint creation parameters

**Returns:** Unique constraint ID or `INVALID_OBJECT_ID` on failure

**Constraint Types Supported:**
- **HINGE**: Rotational constraint around single axis
- **BALL_AND_SOCKET**: 3D rotational constraint
- **SLIDER**: Linear movement constraint
- **FIXED**: Rigid body connection

#### `destroyConstraint(ObjectID constraint_id)`
```cpp
bool destroyConstraint(ObjectID constraint_id)
```

**Purpose:** Removes a constraint from the simulation.

**Parameters:**
- `constraint_id`: ID of the constraint to destroy

**Returns:** `true` if constraint was found and destroyed, `false` otherwise

### Physics Engine Configuration

#### `setPhysicsEngine(PhysicsEngine engine)`
```cpp
void setPhysicsEngine(PhysicsEngine engine)
```

**Purpose:** Sets the active physics engine.

**Parameters:**
- `engine`: Physics engine enumeration value

**Available Engines:**
- `BUILTIN`: Default built-in physics engine
- `BULLET`: Bullet Physics integration
- `PHYSX`: NVIDIA PhysX integration
- `HAVOK`: Havok Physics integration

#### `setPhysicsQuality(PhysicsQuality quality)`
```cpp
void setPhysicsQuality(PhysicsQuality quality)
```

**Purpose:** Configures physics simulation quality and performance.

**Parameters:**
- `quality`: Quality level enumeration

**Quality Levels:**
- **LOW**: 5 sub-steps, 33ms timestep (30 FPS equivalent)
- **MEDIUM**: 10 sub-steps, 16ms timestep (60 FPS equivalent)
- **HIGH**: 20 sub-steps, 8ms timestep (120 FPS equivalent)
- **ULTRA**: 40 sub-steps, 4ms timestep (240 FPS equivalent)

### Gravity and Environment

#### `setGlobalGravity(const Vector3& gravity)`
```cpp
void setGlobalGravity(const Vector3& gravity)
```

**Purpose:** Sets the global gravity vector for the simulation.

**Parameters:**
- `gravity`: 3D gravity vector in m/s²

**Default Value:** (0, -9.81, 0) - Earth gravity

#### `setGravityType(GravityType type)`
```cpp
void setGravityType(GravityType type)
```

**Purpose:** Sets the gravity type for the simulation.

**Parameters:**
- `type`: Gravity type enumeration

**Available Types:**
- `UNIFORM`: Constant gravity throughout space
- `RADIAL`: Gravity that decreases with distance
- `DIRECTIONAL`: Gravity that varies by direction

### Collision Engine Integration

#### `setCollisionEngine(FullAestheticCollisionEngine* collision_engine)`
```cpp
void setCollisionEngine(FullAestheticCollisionEngine* collision_engine)
```

**Purpose:** Integrates with the collision detection system.

**Parameters:**
- `collision_engine`: Pointer to collision engine instance

**Integration Features:**
- Automatic collision detection during physics updates
- Collision response and resolution
- Performance metrics integration

### Performance and Debugging

#### `setProfilingEnabled(bool enabled)`
```cpp
void setProfilingEnabled(bool enabled)
```

**Purpose:** Enables or disables performance profiling.

**Parameters:**
- `enabled`: Whether to enable profiling

**Profiling Metrics:**
- Frame time and FPS
- Active object count
- Collision detection performance
- Constraint resolution time

#### `setDebugDrawEnabled(bool enabled)`
```cpp
void setDebugDrawEnabled(bool enabled)
```

**Purpose:** Enables or disables debug visualization.

**Debug Features:**
- Object bounding boxes
- Constraint visualization
- Collision point markers
- Performance overlay

#### `getPerformanceMetrics()`
```cpp
PhysicsPerformance getPerformanceMetrics() const
```

**Purpose:** Retrieves current performance metrics.

**Returns:** `PhysicsPerformance` structure with current metrics

### Simulation Control

#### `resetSimulation()`
```cpp
void resetSimulation()
```

**Purpose:** Resets the entire physics simulation to initial state.

**Reset Operations:**
- Sets all object velocities to zero
- Resets constraint states
- Clears performance metrics
- Resets time accumulator

#### `pauseSimulation()`
```cpp
void pauseSimulation()
```

**Purpose:** Pauses physics simulation updates.

#### `resumeSimulation()`
```cpp
void resumeSimulation()
```

**Purpose:** Resumes physics simulation updates.

## Private Methods

### Physics Update Helpers

#### `updatePhysicsObject(std::unique_ptr<PhysicsObject>& obj, float delta_time)`
```cpp
void updatePhysicsObject(std::unique_ptr<PhysicsObject>& obj, float delta_time)
```

**Purpose:** Updates a single physics object for one timestep.

**Implementation Details:**
- Applies gravity forces
- Updates velocity and position using Euler integration
- Applies damping to prevent infinite motion
- Handles mass-based physics calculations

#### `updateConstraint(std::unique_ptr<PhysicsConstraint>& constraint, float delta_time)`
```cpp
void updateConstraint(std::unique_ptr<PhysicsConstraint>& constraint, float delta_time)
```

**Purpose:** Updates a single constraint for one timestep.

**Implementation Details:**
- Retrieves connected objects
- Applies constraint-specific physics
- Maintains constraint relationships
- Handles constraint limits and breaking

### Constraint-Specific Updates

#### `updateHingeConstraint()`
```cpp
void updateHingeConstraint(PhysicsConstraint* constraint, 
                           PhysicsObject* obj_a, 
                           PhysicsObject* obj_b, 
                           float delta_time)
```

**Purpose:** Updates hinge constraint physics.

**Implementation Details:**
- Calculates hinge axis in world space
- Projects velocities onto hinge axis
- Applies corrective forces to maintain constraint
- Handles mass-based force distribution

#### `updateBallAndSocketConstraint()`
```cpp
void updateBallAndSocketConstraint(PhysicsConstraint* constraint, 
                                  PhysicsObject* obj_a, 
                                  PhysicsObject* obj_b, 
                                  float delta_time)
```

**Purpose:** Updates ball and socket constraint physics.

**Implementation Details:**
- Calculates distance between anchor points
- Applies corrective forces to maintain distance
- Distributes forces between connected objects
- Handles constraint breaking conditions

#### `updateSliderConstraint()`
```cpp
void updateSliderConstraint(PhysicsConstraint* constraint, 
                           PhysicsObject* obj_a, 
                           PhysicsObject* obj_b, 
                           float delta_time)
```

**Purpose:** Updates slider constraint physics.

**Implementation Details:**
- Projects positions onto slider axis
- Applies movement limits if specified
- Corrects object positions to maintain constraint
- Handles axis-aligned movement

#### `updateFixedConstraint()`
```cpp
void updateFixedConstraint(PhysicsConstraint* constraint, 
                          PhysicsObject* obj_a, 
                          PhysicsObject* obj_b, 
                          float delta_time)
```

**Purpose:** Updates fixed constraint physics.

**Implementation Details:**
- Maintains relative position and orientation
- Applies strong corrective forces
- Handles rigid body connections
- Prevents any relative movement

### Performance Monitoring

#### `updatePerformanceMetrics(float delta_time)`
```cpp
void updatePerformanceMetrics(float delta_time)
```

**Purpose:** Updates internal performance tracking.

**Metrics Tracked:**
- Frame time and FPS calculation
- Active object and constraint counts
- Collision detection performance
- Memory usage statistics

## Data Structures

### PhysicsObject
```cpp
struct PhysicsObject {
    ObjectID id;
    PhysicsType type;
    float mass;
    Vector3 position;
    Quaternion rotation;
    Vector3 velocity;
    Vector3 angular_velocity;
    bool is_active;
    CollisionShape collision_shape;
    MaterialProperties material_properties;
};
```

### PhysicsConstraint
```cpp
struct PhysicsConstraint {
    ObjectID id;
    ConstraintType type;
    ObjectID object_a;
    ObjectID object_b;
    Vector3 anchor_point;
    Vector3 axis;
    ConstraintLimits limits;
    bool is_active;
};
```

### PhysicsPerformance
```cpp
struct PhysicsPerformance {
    float frame_time;
    float fps;
    size_t active_objects;
    size_t active_constraints;
    size_t collision_count;
    float average_update_time;
};
```

## Usage Examples

### Basic Physics Setup
```cpp
#include "uevr/vr/FullPhysicsIntegration.hpp"

// Create physics system
uevr::vr::FullPhysicsIntegration physics;

// Initialize
if (!physics.initialize()) {
    // Handle initialization failure
    return;
}

// Set physics quality
physics.setPhysicsQuality(uevr::vr::PhysicsQuality::HIGH);

// Enable profiling
physics.setProfilingEnabled(true);
```

### Creating Physics Objects
```cpp
// Create physics object
uevr::vr::PhysicsObjectCreateInfo create_info;
create_info.type = uevr::vr::PhysicsType::DYNAMIC;
create_info.mass = 10.0f;
create_info.position = uevr::vr::Vector3(0.0f, 5.0f, 0.0f);
create_info.velocity = uevr::vr::Vector3(0.0f, 0.0f, 0.0f);

ObjectID obj_id = physics.createPhysicsObject(create_info);
```

### Creating Constraints
```cpp
// Create hinge constraint
uevr::vr::ConstraintCreateInfo constraint_info;
constraint_info.type = uevr::vr::ConstraintType::HINGE;
constraint_info.object_a = obj_id_1;
constraint_info.object_b = obj_id_2;
constraint_info.anchor_point = uevr::vr::Vector3(0.0f, 0.0f, 0.0f);
constraint_info.axis = uevr::vr::Vector3(0.0f, 1.0f, 0.0f);

ObjectID constraint_id = physics.createConstraint(constraint_info);
```

### Main Physics Loop
```cpp
// Main game loop
while (game_running) {
    float delta_time = get_delta_time();
    
    // Update physics simulation
    physics.updatePhysicsSimulation(delta_time);
    
    // Get performance metrics
    auto metrics = physics.getPerformanceMetrics();
    
    // Render objects at their new positions
    render_physics_objects();
}
```

## Performance Considerations

### Quality vs Performance Trade-offs
- **LOW**: Suitable for mobile/VR devices with limited performance
- **MEDIUM**: Balanced performance and accuracy for most applications
- **HIGH**: High accuracy for critical physics simulations
- **ULTRA**: Maximum accuracy for professional/scientific applications

### Optimization Tips
1. **Object Pooling**: Reuse physics objects instead of creating/destroying
2. **Spatial Partitioning**: Use collision engine's spatial optimization
3. **Constraint Limits**: Set appropriate limits to prevent instability
4. **Mass Distribution**: Balance object masses for stable simulation
5. **Timestep Tuning**: Adjust fixed timestep based on performance requirements

### Memory Management
- Objects and constraints are automatically managed with smart pointers
- No manual memory deallocation required
- Automatic cleanup on shutdown
- Efficient memory pooling for frequently created objects

## Error Handling

### Exception Safety
- All public methods are exception-safe
- Comprehensive error logging with spdlog
- Graceful degradation on errors
- Automatic cleanup on exceptions

### Common Error Scenarios
1. **Initialization Failure**: Check system requirements and dependencies
2. **Object Creation Failure**: Verify creation parameters and available memory
3. **Constraint Creation Failure**: Ensure connected objects exist and are valid
4. **Physics Update Errors**: Check for invalid object states or constraints

## Integration with Other Systems

### Collision Engine
- Seamless integration with `FullAestheticCollisionEngine`
- Automatic collision detection during physics updates
- Shared object management and synchronization

### Rendering System
- Physics objects provide position and orientation data
- Automatic synchronization with visual representation
- Support for debug visualization

### Input System
- Physics objects can respond to user input
- Force application and object manipulation
- Interactive physics simulation

## Future Enhancements

### Planned Features
1. **Multi-threaded Physics**: Parallel constraint solving
2. **GPU Acceleration**: CUDA/OpenCL physics computation
3. **Advanced Materials**: PBR material system integration
4. **Soft Body Physics**: Cloth, rope, and deformable objects
5. **Fluid Simulation**: Liquid and gas physics
6. **Particle Systems**: Large-scale particle physics

### Extensibility
- Plugin architecture for custom physics engines
- Custom constraint types and solvers
- Advanced collision detection algorithms
- Performance monitoring and optimization tools

## Troubleshooting

### Common Issues
1. **Unstable Simulation**: Reduce timestep or increase sub-steps
2. **Performance Problems**: Lower physics quality or optimize object count
3. **Constraint Breaking**: Adjust constraint limits and solver parameters
4. **Memory Leaks**: Ensure proper shutdown and cleanup

### Debug Tools
- Enable debug drawing for visual inspection
- Use performance profiling to identify bottlenecks
- Check spdlog output for error messages
- Validate object and constraint parameters

## License and Copyright

**Copyright (c) 2024 UEVR Project**  
**Licensed under MIT License**

This class is part of the UEVR project, providing comprehensive physics simulation capabilities for virtual reality applications.
