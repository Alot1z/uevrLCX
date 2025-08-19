/*
 * Context7 Database System Header
 * 
 * This header defines the Context7 database system for intelligent game analysis
 * and VR compatibility assessment.
 * 
 * Copyright (c) 2024 UEVR Cross-Engine Adapter Project
 * Licensed under MIT License
 */

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <nlohmann/json.hpp>

// Forward declarations for AI components
class EngineDetectionModel;
class VRCompatibilityModel;
class PerformancePredictionModel;
class TextAnalysisEngine;
class KeywordExtractor;
class SentimentAnalyzer;
class PatternRecognitionEngine;
class FeatureExtractor;
class ClassificationEngine;
class VRCompatibilityPredictor;
class PerformanceMetricsAnalyzer;
class OptimizationRecommendationEngine;

namespace uevr_context7 {

/**
 * Context7 Database System
 * 
 * This class provides a comprehensive database system for collecting, analyzing,
 * and storing game data from multiple sources including game stores, wikis, and
 * other online resources. It uses AI analysis to predict VR compatibility and
 * provide optimization recommendations.
 */
class Context7Database {
public:
    // Constructor and destructor
    Context7Database();
    ~Context7Database();
    
    // Data collection methods
    void collectGameData(const std::string& game_name);
    
    // Data collection from specific sources
    nlohmann::json collectSteamData(const std::string& game_name);
    nlohmann::json collectEpicData(const std::string& game_name);
    nlohmann::json collectGOGData(const std::string& game_name);
    nlohmann::json collectWikiData(const std::string& game_name);
    
    // Data processing and analysis
    nlohmann::json mergeGameData(
        const nlohmann::json& steam_data,
        const nlohmann::json& epic_data,
        const nlohmann::json& gog_data,
        const nlohmann::json& wiki_data);
    
    nlohmann::json analyzeGameData(const nlohmann::json& game_data);
    
    // AI analysis components
    nlohmann::json performTextAnalysis(const nlohmann::json& game_data);
    nlohmann::json performPatternRecognition(const nlohmann::json& game_data);
    nlohmann::json predictVRCompatibility(const nlohmann::json& game_data);
    
    // Performance analysis
    nlohmann::json analyzePerformanceCharacteristics(const nlohmann::json& game_data);
    
    // Recommendation generation
    nlohmann::json generateRecommendations(
        const nlohmann::json& game_data,
        const nlohmann::json& analysis_result);
    
    nlohmann::json generateEngineSpecificRecommendations(const std::string& engine);
    
    // Confidence scoring
    nlohmann::json calculateConfidenceScores(const nlohmann::json& analysis_result);
    
    // Database management
    void clearAllData();
    nlohmann::json getDatabaseStatistics();
    
    // Utility functions
    std::string encodeUrl(const std::string& input);
    std::string makeHttpRequest(const std::string& url);
    
    // Data storage
    void storeGameAnalysis(
        const std::string& game_name,
        const nlohmann::json& game_data,
        const nlohmann::json& analysis_result);
    
    // Getters for database statistics
    int getTotalGames() const { return m_total_games; }
    int getTotalEngines() const { return m_total_engines; }
    int getTotalAnalyses() const { return m_total_analyses; }
    int getTotalStores() const { return m_total_stores; }
    int getTotalWikis() const { return m_total_wikis; }
    int getTotalVREntries() const { return m_total_vr_entries; }

private:
    // Database initialization
    void initializeDatabaseStructure();
    void createIndexFiles();
    void setupLogging();
    void initializeHttpClient();
    void loadExistingData();
    void initializeAIAnalysisEngine();
    
    // AI component initialization
    void initializeMachineLearningModels();
    void initializeNaturalLanguageProcessing();
    void initializePatternRecognition();
    void initializeVRCompatibilityPrediction();
    
    // Data loading methods
    void loadGamesData();
    void loadEnginesData();
    void loadAnalysesData();
    void loadStoresData();
    void loadWikisData();
    void loadVRCompatibilityData();
    
    // Data saving methods
    void saveAllData();
    void saveGamesData();
    void saveEnginesData();
    void saveAnalysesData();
    void saveStoresData();
    void saveWikisData();
    void saveVRCompatibilityData();
    
    // Cleanup methods
    void cleanupHttpClient();
    void cleanupAIAnalysisEngine();
    
    // AI analysis helper functions
    double calculateTextComplexity(const std::string& text);
    nlohmann::json extractTechnicalTerms(const std::string& text);
    nlohmann::json identifyGenreIndicators(const std::string& text);
    
    // Pattern detection functions
    nlohmann::json detectEnginePatterns(const std::string& text);
    nlohmann::json detectRenderingPatterns(const std::string& text);
    nlohmann::json detectInputPatterns(const std::string& text);
    nlohmann::json detectAudioPatterns(const std::string& text);
    nlohmann::json detectPerformancePatterns(const std::string& text);
    void removeDuplicatePatterns(nlohmann::json& pattern_analysis);
    
    // Performance analysis helper functions
    std::string estimateFPSRange(const std::string& text);
    std::string determinePerformanceCategory(const std::string& text);
    double estimateOptimizationPotential(const nlohmann::json& game_data);
    nlohmann::json analyzeResourceRequirements(const std::string& text);
    nlohmann::json identifyPerformanceBottlenecks(const std::string& text);
    
    // VR compatibility helper functions
    double calculateVRComfortRating(const nlohmann::json& game_data);
    nlohmann::json identifyCompatibilityFactors(const nlohmann::json& game_data);
    nlohmann::json identifyPotentialIssues(const nlohmann::json& game_data);
    nlohmann::json generateOptimizationRecommendations(const nlohmann::json& game_data);
    
    // Database utility functions
    double calculateDatabaseSize();

private:
    // Database data structures
    nlohmann::json m_games_data;
    nlohmann::json m_engines_data;
    nlohmann::json m_analyses_data;
    nlohmann::json m_stores_data;
    nlohmann::json m_wikis_data;
    nlohmann::json m_vr_compatibility_data;
    
    // Database counters
    int m_total_games;
    int m_total_engines;
    int m_total_analyses;
    int m_total_stores;
    int m_total_wikis;
    int m_total_vr_entries;
    
    // HTTP client
    void* m_curl_handle; // CURL handle for HTTP requests
    
    // AI analysis components
    std::unique_ptr<EngineDetectionModel> m_engine_detection_model;
    std::unique_ptr<VRCompatibilityModel> m_vr_compatibility_model;
    std::unique_ptr<PerformancePredictionModel> m_performance_prediction_model;
    
    std::unique_ptr<TextAnalysisEngine> m_text_analysis_engine;
    std::unique_ptr<KeywordExtractor> m_keyword_extractor;
    std::unique_ptr<SentimentAnalyzer> m_sentiment_analyzer;
    
    std::unique_ptr<PatternRecognitionEngine> m_pattern_recognition_engine;
    std::unique_ptr<FeatureExtractor> m_feature_extractor;
    std::unique_ptr<ClassificationEngine> m_classification_engine;
    
    std::unique_ptr<VRCompatibilityPredictor> m_vr_compatibility_predictor;
    std::unique_ptr<PerformanceMetricsAnalyzer> m_performance_metrics_analyzer;
    std::unique_ptr<OptimizationRecommendationEngine> m_optimization_recommendation_engine;
    
    // Thread safety
    mutable std::mutex m_database_mutex;
};

} // namespace uevr_context7
