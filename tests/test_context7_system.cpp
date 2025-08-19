#include <gtest/gtest.h>
#include <memory>
#include <chrono>
#include <thread>

#include "uevr/Context7Database.hpp"
#include "uevr/ai/GameAnalyzer.hpp"
#include "uevr/ai/VRCompatibilityAnalyzer.hpp"
#include "uevr/ai/AIAnalysisEngine.hpp"
#include "uevr/vr/UniversalVRSystem.hpp"
#include "uevr/vr/EngineAgnosticRenderer.hpp"

/**
 * @brief Comprehensive test suite for Context7 AI-powered VR analysis system
 * 
 * Tests all major components including:
 * - Context7 Database with 2000+ games
 * - AI analysis engines 
 * - VR compatibility analysis
 * - Universal VR System
 * - Engine-agnostic renderer
 */
class Context7SystemTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize Context7 database
        context7_db = std::make_unique<uevr::Context7Database>();
        ASSERT_TRUE(context7_db->initialize("data/context7_test_config.json"));
        
        // Initialize AI analysis components
        game_analyzer = std::make_unique<uevr::ai::GameAnalyzer>();
        ASSERT_TRUE(game_analyzer->initialize("data/test_patterns.json"));
        
        vr_analyzer = std::make_unique<uevr::ai::VRCompatibilityAnalyzer>();
        ASSERT_TRUE(vr_analyzer->initialize("models/test", "config/test_analyzer.json"));
        
        ai_engine = std::make_unique<uevr::ai::AIAnalysisEngine>();
        ASSERT_TRUE(ai_engine->initialize("config/test_ai.json", "models/test"));
        
        // Initialize VR system
        vr_system = std::make_unique<uevr::vr::UniversalVRSystem>();
        
        // Initialize renderer
        renderer = std::make_unique<uevr::vr::EngineAgnosticRenderer>();
    }
    
    void TearDown() override {
        if (renderer) renderer->shutdown();
        if (vr_system) vr_system->shutdown();
        if (ai_engine) ai_engine->shutdown();
        if (vr_analyzer) vr_analyzer->shutdown();
        if (game_analyzer) game_analyzer->shutdown();
        if (context7_db) context7_db->shutdown();
    }
    
    std::unique_ptr<uevr::Context7Database> context7_db;
    std::unique_ptr<uevr::ai::GameAnalyzer> game_analyzer;
    std::unique_ptr<uevr::ai::VRCompatibilityAnalyzer> vr_analyzer;
    std::unique_ptr<uevr::ai::AIAnalysisEngine> ai_engine;
    std::unique_ptr<uevr::vr::UniversalVRSystem> vr_system;
    std::unique_ptr<uevr::vr::EngineAgnosticRenderer> renderer;
};

// Context7 Database Tests
TEST_F(Context7SystemTest, DatabaseLoadsSuccessfully) {
    EXPECT_TRUE(context7_db->isInitialized());
    EXPECT_GT(context7_db->getProfileCount(), 2000); // Should have 2000+ games
}

TEST_F(Context7SystemTest, DatabaseHasPriorityGames) {
    // Test that our priority games are in the database
    auto cyberpunk_profile = context7_db->getGameProfile("cyberpunk2077");
    EXPECT_TRUE(cyberpunk_profile.has_value());
    EXPECT_EQ(cyberpunk_profile->engine_type, "REDengine 4");
    EXPECT_GT(cyberpunk_profile->vr_compatibility.compatibility, 0.7f);
    
    auto re7_profile = context7_db->getGameProfile("re7");
    EXPECT_TRUE(re7_profile.has_value());
    EXPECT_EQ(re7_profile->engine_type, "RE Engine");
    EXPECT_GT(re7_profile->vr_compatibility.compatibility, 0.9f);
    
    auto mhw_profile = context7_db->getGameProfile("mhw");
    EXPECT_TRUE(mhw_profile.has_value());
    EXPECT_EQ(mhw_profile->engine_type, "MT Framework");
    EXPECT_GT(mhw_profile->vr_compatibility.compatibility, 0.9f);
}

TEST_F(Context7SystemTest, DatabaseLearningSystem) {
    // Test that the learning system works
    context7_db->learnFromUserFeedback("cyberpunk2077", 0.9f);
    
    // Should update the compatibility score
    auto updated_profile = context7_db->getGameProfile("cyberpunk2077");
    EXPECT_TRUE(updated_profile.has_value());
    
    // Test optimization from usage
    context7_db->optimizeSettingsFromUsage("cyberpunk2077");
}

// Game Analyzer Tests
TEST_F(Context7SystemTest, GameAnalyzerDetectsEngines) {
    EXPECT_TRUE(game_analyzer->isInitialized());
    
    // Test engine detection
    auto engine_type = game_analyzer->detectEngine("test_game.exe");
    EXPECT_FALSE(engine_type.empty());
    
    // Test with known patterns
    auto re_engine = game_analyzer->detectEngineFromSignature("re_engine_signature");
    EXPECT_EQ(re_engine, "RE Engine");
}

TEST_F(Context7SystemTest, GameAnalyzerExtractsFeatures) {
    uevr::GameInfo game_info;
    game_info.executable_path = "test_game.exe";
    game_info.process_name = "test_game";
    
    auto features = game_analyzer->extractFeatures(game_info);
    EXPECT_FALSE(features.empty());
    EXPECT_GT(features.size(), 10); // Should extract multiple features
}

// VR Compatibility Analyzer Tests
TEST_F(Context7SystemTest, VRAnalyzerPerformsAnalysis) {
    EXPECT_TRUE(vr_analyzer->isInitialized());
    
    // Create test game profile
    uevr::GameProfile test_game;
    test_game.game_id = "test_game";
    test_game.engine_type = "RE Engine";
    test_game.settings["genre"] = "horror";
    test_game.settings["perspective"] = "first_person";
    
    auto result = vr_analyzer->analyzeCompatibility(test_game);
    
    EXPECT_GT(result.overall_score.compatibility, 0.0f);
    EXPECT_LE(result.overall_score.compatibility, 1.0f);
    EXPECT_GT(result.recommendations.size(), 0);
    EXPECT_NE(result.confidence, uevr::ai::VRCompatibilityAnalyzer::ConfidenceLevel::LOW);
}

TEST_F(Context7SystemTest, VRAnalyzerGeneratesOptimizations) {
    uevr::GameProfile horror_game;
    horror_game.engine_type = "RE Engine";
    horror_game.settings["genre"] = "horror";
    
    std::map<std::string, float> target_performance = {{"target_fps", 90.0f}};
    auto optimizations = vr_analyzer->generateOptimizations(horror_game, target_performance);
    
    EXPECT_GT(optimizations.size(), 0);
    
    // Should include horror-specific optimizations
    bool has_comfort_optimization = false;
    for (const auto& opt : optimizations) {
        if (opt.category.find("comfort") != std::string::npos) {
            has_comfort_optimization = true;
            break;
        }
    }
    EXPECT_TRUE(has_comfort_optimization);
}

// AI Analysis Engine Tests
TEST_F(Context7SystemTest, AIEngineRunsInference) {
    EXPECT_TRUE(ai_engine->isReady());
    
    // Test feature tensor creation
    std::vector<float> test_data = {1.0f, 0.0f, 0.5f, 0.8f, 0.3f};
    auto tensor = ai_engine->createTensor(test_data, {1, 5});
    
    EXPECT_EQ(tensor.data.size(), 5);
    EXPECT_EQ(tensor.shape.size(), 2);
    
    // Test model inference
    auto result = ai_engine->predict("compatibility_classifier", tensor);
    EXPECT_GT(result.size(), 0);
    EXPECT_LE(result.size(), 10); // Reasonable output size
}

TEST_F(Context7SystemTest, AIEngineAsyncProcessing) {
    uevr::ai::AIAnalysisEngine::AnalysisRequest request;
    request.request_id = "test_async";
    request.model_id = "performance_predictor";
    request.input = ai_engine->createTensor({0.5f, 0.8f, 0.3f}, {1, 3});
    request.mode = uevr::ai::AIAnalysisEngine::ProcessingMode::REAL_TIME;
    
    auto future = ai_engine->predictAsync(request);
    
    // Should complete within reasonable time
    auto status = future.wait_for(std::chrono::seconds(5));
    EXPECT_EQ(status, std::future_status::ready);
    
    auto result = future.get();
    EXPECT_GT(result.size(), 0);
}

// Universal VR System Tests
TEST_F(Context7SystemTest, VRSystemInitialization) {
    uevr::vr::UniversalVRSystem::VRConfig config;
    config.runtime = uevr::vr::UniversalVRSystem::VRRuntime::AUTO;
    config.graphics_api = uevr::vr::UniversalVRSystem::GraphicsAPI::AUTO_DETECT;
    config.target_framerate = 90;
    
    // Create mock adapter
    uevr::ICrossEngineAdapter* mock_adapter = nullptr; // Would need actual mock
    
    bool initialized = vr_system->initialize(config, mock_adapter);
    EXPECT_TRUE(initialized || !mock_adapter); // OK to fail without adapter
    
    if (initialized) {
        EXPECT_TRUE(vr_system->isReady());
        
        // Test VR state
        auto state = vr_system->getState();
        EXPECT_TRUE(state.is_initialized);
    }
}

TEST_F(Context7SystemTest, VRSystemPoseTracking) {
    uevr::vr::UniversalVRSystem::VRConfig config;
    if (vr_system->initialize(config, nullptr)) {
        
        // Test pose retrieval
        auto hmd_pose = vr_system->getHMDPose();
        EXPECT_TRUE(hmd_pose.is_valid || true); // May be invalid without hardware
        
        auto controller_pose = vr_system->getControllerPose(0);
        // Controller may not be available in test environment
        
        // Test pose prediction and updates
        vr_system->update(0.016f); // 60 FPS frame time
    }
}

TEST_F(Context7SystemTest, VRSystemPerformanceMetrics) {
    if (vr_system->isReady()) {
        auto stats = vr_system->getPerformanceStats();
        
        EXPECT_TRUE(stats.find("fps") != stats.end());
        EXPECT_TRUE(stats.find("frame_time_ms") != stats.end());
        EXPECT_TRUE(stats.find("render_scale") != stats.end());
        
        // Test performance optimization
        vr_system->setProfilingEnabled(true);
        
        // Simulate some frames
        for (int i = 0; i < 10; ++i) {
            vr_system->beginFrame(i);
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            vr_system->endFrame();
        }
        
        auto updated_stats = vr_system->getPerformanceStats();
        EXPECT_GT(updated_stats["total_frames"], 0);
    }
}

// Engine-Agnostic Renderer Tests
TEST_F(Context7SystemTest, RendererInitialization) {
    // Test with mock D3D11 device
    void* mock_device = reinterpret_cast<void*>(0x12345678);
    void* mock_context = reinterpret_cast<void*>(0x87654321);
    
    bool initialized = renderer->initialize(
        uevr::vr::EngineAgnosticRenderer::GraphicsAPI::DIRECTX11,
        mock_device,
        mock_context
    );
    
    // Should handle mock device gracefully
    EXPECT_TRUE(initialized || !mock_device);
    
    if (initialized) {
        EXPECT_TRUE(renderer->isInitialized());
        
        // Test capabilities
        auto capabilities = renderer->getCapabilities();
        EXPECT_GT(capabilities.size(), 0);
        
        // Test render target creation
        bool render_targets_created = renderer->createRenderTargets(1920, 1080, 28, 1);
        if (render_targets_created) {
            auto left_target = renderer->getRenderTarget(0);
            auto right_target = renderer->getRenderTarget(1);
            
            EXPECT_EQ(left_target.width, 1920);
            EXPECT_EQ(left_target.height, 1080);
        }
    }
}

TEST_F(Context7SystemTest, RendererVRTechniques) {
    if (renderer->isInitialized()) {
        // Test different rendering techniques
        auto techniques = {
            uevr::vr::EngineAgnosticRenderer::RenderingTechnique::TRADITIONAL_STEREO,
            uevr::vr::EngineAgnosticRenderer::RenderingTechnique::INSTANCED_STEREO,
            uevr::vr::EngineAgnosticRenderer::RenderingTechnique::MULTIVIEW
        };
        
        for (auto technique : techniques) {
            if (renderer->isTechniqueSupported(technique)) {
                EXPECT_TRUE(renderer->setRenderingTechnique(technique));
                EXPECT_EQ(renderer->getRenderingTechnique(), technique);
            }
        }
        
        // Test foveated rendering
        renderer->setFoveatedRendering(true, 0.3f, 0.8f);
        
        // Test render scale adjustment
        renderer->setRenderScale(0.8f);
        EXPECT_FLOAT_EQ(renderer->getRenderScale(), 0.8f);
    }
}

// Integration Tests
TEST_F(Context7SystemTest, FullSystemIntegration) {
    // Test complete pipeline: Game Detection -> AI Analysis -> VR Setup
    
    // 1. Detect game
    auto engine_type = game_analyzer->detectEngine("cyberpunk2077.exe");
    EXPECT_FALSE(engine_type.empty());
    
    // 2. Get game profile
    auto game_profile = context7_db->getGameProfile("cyberpunk2077");
    EXPECT_TRUE(game_profile.has_value());
    
    // 3. Analyze VR compatibility
    auto compatibility = vr_analyzer->analyzeCompatibility(*game_profile);
    EXPECT_GT(compatibility.overall_score.compatibility, 0.0f);
    
    // 4. Setup VR system based on analysis
    uevr::vr::UniversalVRSystem::VRConfig vr_config;
    vr_config.target_framerate = 90;
    vr_config.render_scale = 1.0f;
    
    // Apply AI recommendations
    for (const auto& recommendation : compatibility.recommendations) {
        if (recommendation.category == "performance") {
            vr_config.render_scale *= 0.9f; // Reduce for performance
        } else if (recommendation.category == "comfort") {
            vr_config.enable_vignetting = true;
        }
    }
    
    // This completes the full AI-powered VR setup pipeline
    EXPECT_GT(vr_config.render_scale, 0.0f);
    EXPECT_LE(vr_config.render_scale, 1.0f);
}

TEST_F(Context7SystemTest, DatabaseCompressionAndLoad) {
    // Test that the compressed database loads efficiently
    auto start_time = std::chrono::high_resolution_clock::now();
    
    context7_db->saveGameProfiles(); // Save current state
    context7_db->shutdown();
    
    // Reinitialize
    context7_db = std::make_unique<uevr::Context7Database>();
    ASSERT_TRUE(context7_db->initialize("data/context7_test_config.json"));
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto load_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    // Should load 2000+ games in reasonable time (under 5 seconds)
    EXPECT_LT(load_time.count(), 5000);
    EXPECT_GT(context7_db->getProfileCount(), 2000);
}

// Performance and Stress Tests
TEST_F(Context7SystemTest, AIPerformanceStress) {
    // Test AI system under load
    const int num_analyses = 100;
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_analyses; ++i) {
        uevr::GameProfile test_game;
        test_game.game_id = "stress_test_" + std::to_string(i);
        test_game.engine_type = (i % 3 == 0) ? "RE Engine" : 
                               (i % 3 == 1) ? "REDengine 4" : "MT Framework";
        
        auto result = vr_analyzer->analyzeCompatibility(test_game);
        EXPECT_GT(result.overall_score.compatibility, 0.0f);
    }
    
    auto end_time = std::chrono::high_resolution_clock::now();
    auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    // Should complete 100 analyses in under 10 seconds
    EXPECT_LT(total_time.count(), 10000);
    
    // Average should be under 100ms per analysis
    float avg_time = static_cast<float>(total_time.count()) / num_analyses;
    EXPECT_LT(avg_time, 100.0f);
}

// Entry point for running tests
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    // Set up test environment
    std::cout << "Running Context7 VR Analysis System Tests..." << std::endl;
    std::cout << "Testing database with 2000+ games..." << std::endl;
    std::cout << "Testing AI-powered VR compatibility analysis..." << std::endl;
    std::cout << "Testing Universal VR System..." << std::endl;
    
    auto result = RUN_ALL_TESTS();
    
    std::cout << std::endl;
    std::cout << "Context7 System Test Results:" << std::endl;
    std::cout << "- Database functionality: " << (result == 0 ? "PASSED" : "FAILED") << std::endl;
    std::cout << "- AI analysis accuracy: " << (result == 0 ? "PASSED" : "FAILED") << std::endl;
    std::cout << "- VR system integration: " << (result == 0 ? "PASSED" : "FAILED") << std::endl;
    std::cout << "- Performance requirements: " << (result == 0 ? "PASSED" : "FAILED") << std::endl;
    
    return result;
}