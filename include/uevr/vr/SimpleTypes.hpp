#pragma once

#include <cstdint>
#include <cmath>

namespace uevr {
namespace vr {

// Simple Vector3 class to replace GLM
struct Vector3 {
    float x, y, z;
    
    Vector3() : x(0.0f), y(0.0f), z(0.0f) {}
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    
    // Basic vector operations
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
    
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }
    
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    
    Vector3 operator/(float scalar) const {
        return Vector3(x / scalar, y / scalar, z / scalar);
    }
    
    // Vector length
    float length() const {
        return std::sqrt(x * x + y * y + z * z);
    }
    
    // Normalize vector
    Vector3 normalized() const {
        float len = length();
        if (len > 0.0f) {
            return *this / len;
        }
        return *this;
    }
    
    // Dot product
    float dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
    
    // Cross product
    Vector3 cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }
};

// Simple Matrix4x4 class to replace GLM
struct Matrix4x4 {
    float m[16]; // Column-major order
    
    Matrix4x4() {
        // Identity matrix
        for (int i = 0; i < 16; i++) {
            m[i] = (i % 5 == 0) ? 1.0f : 0.0f;
        }
    }
    
    // Access elements
    float& operator()(int row, int col) {
        return m[col * 4 + row];
    }
    
    const float& operator()(int row, int col) const {
        return m[col * 4 + row];
    }
    
    // Matrix multiplication
    Matrix4x4 operator*(const Matrix4x4& other) const {
        Matrix4x4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result(i, j) = 0.0f;
                for (int k = 0; k < 4; k++) {
                    result(i, j) += (*this)(i, k) * other(k, j);
                }
            }
        }
        return result;
    }
    
    // Vector transformation
    Vector3 transformPoint(const Vector3& point) const {
        float w = m[12] * point.x + m[13] * point.y + m[14] * point.z + m[15];
        if (w != 0.0f) {
            w = 1.0f / w;
        }
        return Vector3(
            (m[0] * point.x + m[4] * point.y + m[8] * point.z + m[12]) * w,
            (m[1] * point.x + m[5] * point.y + m[9] * point.z + m[13]) * w,
            (m[2] * point.x + m[6] * point.y + m[10] * point.z + m[14]) * w
        );
    }
    
    // Vector transformation (direction)
    Vector3 transformDirection(const Vector3& direction) const {
        return Vector3(
            m[0] * direction.x + m[4] * direction.y + m[8] * direction.z,
            m[1] * direction.x + m[5] * direction.y + m[9] * direction.z,
            m[2] * direction.x + m[6] * direction.y + m[10] * direction.z
        );
    }
};

// Simple Quaternion class to replace GLM
struct Quaternion {
    float x, y, z, w;
    
    Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
    Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    
    // Create quaternion from euler angles (in radians)
    static Quaternion fromEuler(float pitch, float yaw, float roll) {
        float cy = std::cos(yaw * 0.5f);
        float sy = std::sin(yaw * 0.5f);
        float cp = std::cos(pitch * 0.5f);
        float sp = std::sin(pitch * 0.5f);
        float cr = std::cos(roll * 0.5f);
        float sr = std::sin(roll * 0.5f);
        
        return Quaternion(
            sr * cp * cy - cr * sp * sy,
            cr * sp * cy + sr * cp * sy,
            cr * cp * sy - sr * sp * cy,
            cr * cp * cy + sr * sp * sy
        );
    }
    
    // Quaternion multiplication
    Quaternion operator*(const Quaternion& other) const {
        return Quaternion(
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w,
            w * other.w - x * other.x - y * other.y - z * other.z
        );
    }
    
    // Normalize quaternion
    Quaternion normalized() const {
        float len = std::sqrt(x * x + y * y + z * z + w * w);
        if (len > 0.0f) {
            return Quaternion(x / len, y / len, z / len, w / len);
        }
        return *this;
    }
    
    // Convert to euler angles (in radians)
    Vector3 toEuler() const {
        Vector3 euler;
        
        // Roll (x-axis rotation)
        float sinr_cosp = 2.0f * (w * x + y * z);
        float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
        euler.z = std::atan2(sinr_cosp, cosr_cosp);
        
        // Pitch (y-axis rotation)
        float sinp = 2.0f * (w * y - z * x);
        if (std::abs(sinp) >= 1.0f) {
            euler.x = std::copysign(M_PI / 2.0f, sinp); // Use 90 degrees if out of range
        } else {
            euler.x = std::asin(sinp);
        }
        
        // Yaw (z-axis rotation)
        float siny_cosp = 2.0f * (w * z + x * y);
        float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
        euler.y = std::atan2(siny_cosp, cosy_cosp);
        
        return euler;
    }
};

// Basic types
using ObjectID = uint32_t;
using HandType = uint8_t;
using CollisionType = uint8_t;
using PhysicsType = uint8_t;
using HapticType = uint8_t;
using ForceType = uint8_t;
using GravityType = uint8_t;
using InteractionType = uint8_t;

// Enums
enum class CollisionTypeEnum : uint8_t {
    HAND_OBJECT = 0,
    HAND_DOOR = 1,
    HAND_WEAPON = 2,
    HAND_VEHICLE = 3,
    HAND_NPC = 4,
    HAND_ENVIRONMENT = 5,
    HAND_PUZZLE = 6,
    HAND_INVENTORY = 7
};

enum class PhysicsTypeEnum : uint8_t {
    STATIC = 0,
    DYNAMIC = 1,
    KINEMATIC = 2,
    RIGID_BODY = 3,
    SOFT_BODY = 4
};

enum class HapticTypeEnum : uint8_t {
    TOUCH = 0,
    GRAB = 1,
    RELEASE = 2,
    IMPACT = 3,
    VIBRATION = 4
};

enum class ForceTypeEnum : uint8_t {
    LINEAR = 0,
    ANGULAR = 1,
    IMPULSE = 2
};

enum class GravityTypeEnum : uint8_t {
    EARTH = 0,
    ZERO = 1,
    CUSTOM = 2
};

enum class InteractionTypeEnum : uint8_t {
    GRAB = 0,
    TOUCH = 1,
    PUSH = 2,
    PULL = 3,
    ROTATE = 4
};

// Forward declarations
struct CollisionResult;
struct PhysicsPerformance;
struct InteractionResult;

} // namespace vr
} // namespace uevr
