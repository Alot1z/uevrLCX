/*
 * UEVR Context7 AI-Powered Game Analysis Database
 * 
 * Provides intelligent game compatibility analysis and automatic VR optimization
 * 
 * Copyright (c) 2024 UEVR Project
 * Licensed under MIT License
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <filesystem>

namespace uevr {
namespace ai {

// Forward declarations
class AIAnalysisEngine;
class GameAnalyzer;
class VRCompatibilityAnalyzer;

/**
 * @brief VR compatibility scoring system
 */
struct VRCompatibilityScore {
    float overall_score = 0.0f;           // Overall VR compatibility (0.0 - 1.0)
    float rendering_compatibility = 0.0f;  // Rendering pipeline compatibility
    float input_compatibility = 0.0f;      // Input system compatibility
    float ui_compatibility = 0.8f;         // UI/HUD compatibility
    float audio_compatibility = 0.9f;      // Audio system compatibility
    float performance_impact = 0.0f;       // Expected performance impact (0.0 = minimal, 1.0 = severe)
    float motion_sickness_risk = 0.0f;     // Motion sickness risk factor
    std::string assessment_notes;          // Detailed assessment notes
};

/**
 * @brief Optimization hint for VR adaptation
 */
struct OptimizationHint {
    std::string category;        // Category (e.g., "Rendering", "Performance", "Input")
    std::string setting;         // Setting name
    std::string value;          // Recommended value
    float priority = 0.0f;      // Priority (0.0 - 1.0)
    std::string description;    // Human-readable description
    std::string rationale;      // Why this optimization is recommended
    
    bool operator==(const OptimizationHint& other) const {
        return category == other.category && setting == other.setting;
    }
};

/**
 * @brief Game usage data for learning and optimization
 */
struct UsageData {
    std::chrono::system_clock::time_point timestamp;
    float average_fps = 0.0f;
    float average_frame_time = 0.0f;
    float memory_usage_mb = 0.0f;
    float gpu_usage_percent = 0.0f;
    float cpu_usage_percent = 0.0f;
    bool vr_enabled = false;
    float vr_comfort_rating = 0.0f;  // User-reported comfort (0.0 - 1.0)
    std::map<std::string, std::string> active_settings;
    std::string notes;
};

/**
 * @brief Engine compatibility information
 */
struct EngineCompatibility {
    float base_score = 0.0f;        // Base compatibility score
    float vr_readiness = 0.0f;      // How VR-ready the engine is
    float performance_factor = 0.0f; // Performance impact factor
    float complexity_factor = 0.0f;  // Implementation complexity
};

/**
 * @brief Game information extracted from executable
 */
struct GameInfo {
    std::string name;
    std::string executable;
    std::string directory;
    std::string version;
    std::string publisher;
    std::string developer;
    size_t file_size = 0;
    std::vector<std::string> detected_engines;
    std::map<std::string, std::string> metadata;
};

/**
 * @brief Complete game profile with analysis data
 */
struct GameProfile {
    std::string name;
    std::string engine_type;
    std::string executable;
    std::string file_path;
    std::string version;
    std::string camera_type = "unknown"; // "first_person", "third_person", "mixed"
    
    VRCompatibilityScore compatibility;
    std::vector<OptimizationHint> optimizations;
    std::vector<UsageData> usage_data;
    std::map<std::string, std::string> settings;
    
    // Basic profile info
    std::string description;
    std::string genre;
    std::string platform;
    std::string release_date;
    
    // VR-specific data
    bool vr_supported = false;
    bool vr_tested = false;
    float vr_comfort_rating = 0.0f;
    std::vector<std::string> vr_requirements;
    
    // Performance data
    float min_fps = 0.0f;
    float target_fps = 90.0f;
    float avg_fps = 0.0f;
    float memory_usage = 0.0f;
    float gpu_usage = 0.0f;
    float cpu_usage = 0.0f;
    
    // Compatibility flags
    bool supports_motion_controllers = false;
    bool supports_room_scale = false;
    bool supports_seated_play = false;
    bool supports_standing_play = false;
    
    // Engine detection
    std::vector<std::string> detected_engines;
    std::string primary_engine;
    std::string engine_version;
    
    // File analysis
    size_t executable_size = 0;
    std::string executable_hash;
    std::vector<std::string> dependencies;
    
    // User feedback
    float user_rating = 0.0f;
    int rating_count = 0;
    std::vector<std::string> user_comments;
    
    // Last update
    std::chrono::system_clock::time_point last_usage;
    
    // AI learning data
    float user_satisfaction_score = 0.0f;
    int usage_count = 0;
    bool verified_working = false;
};

/**
 * @brief Context7 AI-Powered Game Analysis Database
 * 
 * This class provides intelligent game analysis, VR compatibility assessment,
 * and automatic optimization generation using AI and machine learning.
 */
class Context7Database {
public:
    Context7Database();
    virtual ~Context7Database();

    // Core lifecycle
    bool initialize();
    void shutdown();
    bool isInitialized() const;

    // Game analysis
    bool analyzeGame(const std::string& executable_path);
    VRCompatibilityScore assessVRCompatibility(const GameProfile& profile);
    std::vector<OptimizationHint> generateOptimizations(const GameProfile& profile);

    // Profile management
    bool loadGameProfile(const std::string& game_name);
    const GameProfile* getGameProfile(const std::string& game_name) const;
    void updateProfileWithUsageData(const std::string& game_name, const UsageData& data);
    
    // Database operations
    void saveGameProfiles();
    void loadGameProfiles();
    size_t getProfileCount() const;
    
    // AI learning and optimization
    void learnFromUserFeedback(const std::string& game_name, float satisfaction_score);
    void optimizeSettingsFromUsage(const std::string& game_name);
    
    // Compatibility database
    void updateEngineCompatibility(const std::string& engine, const EngineCompatibility& compat);
    EngineCompatibility getEngineCompatibility(const std::string& engine) const;

private:
    // Core state
    bool m_initialized;
    
    // AI components - simplified for now
    // std::unique_ptr<AIAnalysisEngine> m_ai_engine;
    // std::unique_ptr<GameAnalyzer> m_game_analyzer;
    // std::unique_ptr<VRCompatibilityAnalyzer> m_vr_analyzer;
    
    // Data storage
    std::map<std::string, GameProfile> m_game_profiles;
    std::map<std::string, EngineCompatibility> m_engine_compatibility;
    
    // Internal methods
    bool extractGameInfo(const std::string& executable_path, GameInfo& info);
    void initializeNeuralNetworks();
    bool loadContext7Database(const std::filesystem::path& database_path);
    
    // Profile management
    bool loadGameProfileFromFile(const std::filesystem::path& file_path);
    bool saveGameProfileToFile(const GameProfile& profile, const std::filesystem::path& file_path);
    
    // Compatibility database
    void loadCompatibilityDatabase();
    void saveCompatibilityDatabase();
    void createDefaultCompatibilityDatabase();
    
    // Analysis helpers
    VRCompatibilityScore performRuleBasedCompatibilityAssessment(const GameProfile& profile);
    std::vector<OptimizationHint> generateEngineSpecificOptimizations(const GameProfile& profile);
    std::vector<OptimizationHint> generateVRSpecificOptimizations(const GameProfile& profile);
    std::vector<OptimizationHint> generatePerformanceOptimizations(const GameProfile& profile);
    
    // Compatibility assessment helpers
    void adjustScoreForEngine(VRCompatibilityScore& score, const std::string& engine_type);
    void assessPerformanceImpact(VRCompatibilityScore& score, const GameProfile& profile);
    void assessMotionSicknessRisk(VRCompatibilityScore& score, const GameProfile& profile);
    
    float assessInputCompatibility(const GameProfile& profile);
    float assessUICompatibility(const GameProfile& profile);
    float assessAudioCompatibility(const GameProfile& profile);
    
    // Learning and optimization
    void updateCompatibilityFromUsage(GameProfile& profile, const UsageData& data);
    
    // Game info extraction
    bool extractVersionInfo(const std::string& executable_path, GameInfo& info);
    bool analyzeExecutableSignature(const std::string& executable_path, GameInfo& info);
    void detectGameFromFilename(GameInfo& info);
};

} // namespace ai
} // namespace uevr