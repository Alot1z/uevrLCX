/*
 * Context7 Database System Implementation
 * 
 * This file provides the complete implementation of the Context7 database system
 * for intelligent game analysis and VR compatibility assessment.
 * 
 * Copyright (c) 2024 UEVR Cross-Engine Adapter Project
 * Licensed under MIT License
 */

#include "Context7Database.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <thread>
#include <future>
#include <chrono>
#include <random>
#include <iomanip>
#include <regex>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

namespace uevr_context7 {

// Constructor implementation
Context7Database::Context7Database() {
    spdlog::info("Initializing Context7 Database System");
    
    // Initialize database structure
    initializeDatabaseStructure();
    
    // Setup logging
    setupLogging();
    
    // Initialize HTTP client
    initializeHttpClient();
    
    // Load existing data
    loadExistingData();
    
    // Initialize AI analysis engine
    initializeAIAnalysisEngine();
    
    spdlog::info("Context7 Database System initialized successfully");
}

// Destructor implementation
Context7Database::~Context7Database() {
    spdlog::info("Shutting down Context7 Database System");
    
    // Save all data
    saveAllData();
    
    // Cleanup HTTP client
    cleanupHttpClient();
    
    // Cleanup AI engine
    cleanupAIAnalysisEngine();
    
    spdlog::info("Context7 Database System shutdown complete");
}

// Database initialization
void Context7Database::initializeDatabaseStructure() {
    try {
        // Create main data directory
        std::filesystem::path data_dir("docs/data");
        if (!std::filesystem::exists(data_dir)) {
            std::filesystem::create_directories(data_dir);
        }
        
        // Create subdirectories
        std::vector<std::string> subdirs = {
            "games", "engines", "analyses", "stores", "wikis", "vr_compatibility"
        };
        
        for (const auto& subdir : subdirs) {
            std::filesystem::path subdir_path = data_dir / subdir;
            if (!std::filesystem::exists(subdir_path)) {
                std::filesystem::create_directories(subdir_path);
            }
        }
        
        // Create index files
        createIndexFiles();
        
        spdlog::info("Database structure initialized successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error initializing database structure: {}", e.what());
    }
}

void Context7Database::createIndexFiles() {
    try {
        std::filesystem::path data_dir("docs/data");
        
        // Create games index
        nlohmann::json games_index = {
            {"total_games", 0},
            {"last_updated", std::chrono::system_clock::now().time_since_epoch().count()},
            {"games", nlohmann::json::array()}
        };
        
        std::ofstream games_file(data_dir / "games" / "index.json");
        games_file << games_index.dump(2);
        games_file.close();
        
        // Create engines index
        nlohmann::json engines_index = {
            {"total_engines", 0},
            {"last_updated", std::chrono::system_clock::now().time_since_epoch().count()},
            {"engines", nlohmann::json::array()}
        };
        
        std::ofstream engines_file(data_dir / "engines" / "index.json");
        engines_file << engines_index.dump(2);
        engines_file.close();
        
        // Create analyses index
        nlohmann::json analyses_index = {
            {"total_analyses", 0},
            {"last_updated", std::chrono::system_clock::now().time_since_epoch().count()},
            {"analyses", nlohmann::json::array()}
        };
        
        std::ofstream analyses_file(data_dir / "analyses" / "index.json");
        analyses_file << analyses_index.dump(2);
        analyses_file.close();
        
        // Create stores index
        nlohmann::json stores_index = {
            {"total_stores", 0},
            {"last_updated", std::chrono::system_clock::now().time_since_epoch().count()},
            {"stores", nlohmann::json::array()}
        };
        
        std::ofstream stores_file(data_dir / "stores" / "index.json");
        stores_file << stores_index.dump(2);
        stores_file.close();
        
        // Create wikis index
        nlohmann::json wikis_index = {
            {"total_wikis", 0},
            {"last_updated", std::chrono::system_clock::now().time_since_epoch().count()},
            {"wikis", nlohmann::json::array()}
        };
        
        std::ofstream wikis_file(data_dir / "wikis" / "index.json");
        wikis_file << wikis_index.dump(2);
        wikis_file.close();
        
        // Create VR compatibility index
        nlohmann::json vr_compatibility_index = {
            {"total_entries", 0},
            {"last_updated", std::chrono::system_clock::now().time_since_epoch().count()},
            {"compatibility_data", nlohmann::json::array()}
        };
        
        std::ofstream vr_file(data_dir / "vr_compatibility" / "index.json");
        vr_file << vr_compatibility_index.dump(2);
        vr_file.close();
        
        spdlog::info("Index files created successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error creating index files: {}", e.what());
    }
}

// Setup logging
void Context7Database::setupLogging() {
    try {
        // Create logs directory if it doesn't exist
        std::filesystem::path logs_dir("logs");
        if (!std::filesystem::exists(logs_dir)) {
            std::filesystem::create_directories(logs_dir);
        }
        
        // Setup rotating file sink
        auto rotating_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "logs/context7_database.log", 1024 * 1024 * 10, 5
        );
        
        // Setup console sink
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        
        // Create logger with multiple sinks
        auto logger = std::make_shared<spdlog::logger>("context7_database", 
            spdlog::sinks_init_list{rotating_sink, console_sink});
        
        // Set as default logger
        spdlog::set_default_logger(logger);
        
        // Set log level
        spdlog::set_level(spdlog::level::debug);
        
        spdlog::info("Context7 Database logging system initialized");
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to setup logging: {}", e.what());
    }
}

// HTTP client initialization
void Context7Database::initializeHttpClient() {
    try {
        // Initialize libcurl
        if (curl_global_init(CURL_GLOBAL_ALL) != CURLE_OK) {
            spdlog::error("Failed to initialize libcurl");
            return;
        }
        
        // Create CURL handle
        m_curl_handle = curl_easy_init();
        if (!m_curl_handle) {
            spdlog::error("Failed to create CURL handle");
            return;
        }
        
        // Set default options
        curl_easy_setopt(m_curl_handle, CURLOPT_TIMEOUT, 30L);
        curl_easy_setopt(m_curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(m_curl_handle, CURLOPT_USERAGENT, "UEVR-Context7/1.0");
        
        spdlog::info("HTTP client initialized successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error initializing HTTP client: {}", e.what());
    }
}

// Data loading
void Context7Database::loadExistingData() {
    try {
        spdlog::info("Loading existing data from database");
        
        // Load games data
        loadGamesData();
        
        // Load engines data
        loadEnginesData();
        
        // Load analyses data
        loadAnalysesData();
        
        // Load stores data
        loadStoresData();
        
        // Load wikis data
        loadWikisData();
        
        // Load VR compatibility data
        loadVRCompatibilityData();
        
        spdlog::info("Existing data loaded successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error loading existing data: {}", e.what());
    }
}

void Context7Database::loadGamesData() {
    try {
        std::filesystem::path games_file("docs/data/games/index.json");
        if (std::filesystem::exists(games_file)) {
            std::ifstream file(games_file);
            nlohmann::json data = nlohmann::json::parse(file);
            
            m_total_games = data.value("total_games", 0);
            m_games_data = data.value("games", nlohmann::json::array());
            
            spdlog::info("Loaded {} games from database", m_total_games);
        }
        
    } catch (const std::exception& e) {
        spdlog::error("Error loading games data: {}", e.what());
    }
}

void Context7Database::loadEnginesData() {
    try {
        std::filesystem::path engines_file("docs/data/engines/index.json");
        if (std::filesystem::exists(engines_file)) {
            std::ifstream file(engines_file);
            nlohmann::json data = nlohmann::json::parse(file);
            
            m_total_engines = data.value("total_engines", 0);
            m_engines_data = data.value("engines", nlohmann::json::array());
            
            spdlog::info("Loaded {} engines from database", m_total_engines);
        }
        
    } catch (const std::exception& e) {
        spdlog::error("Error loading engines data: {}", e.what());
    }
}

void Context7Database::loadAnalysesData() {
    try {
        std::filesystem::path analyses_file("docs/data/analyses/index.json");
        if (std::filesystem::exists(analyses_file)) {
            std::ifstream file(analyses_file);
            nlohmann::json data = nlohmann::json::parse(file);
            
            m_total_analyses = data.value("total_analyses", 0);
            m_analyses_data = data.value("analyses", nlohmann::json::array());
            
            spdlog::info("Loaded {} analyses from database", m_total_analyses);
        }
        
    } catch (const std::exception& e) {
        spdlog::error("Error loading analyses data: {}", e.what());
    }
}

void Context7Database::loadStoresData() {
    try {
        std::filesystem::path stores_file("docs/data/stores/index.json");
        if (std::filesystem::exists(stores_file)) {
            std::ifstream file(stores_file);
            nlohmann::json data = nlohmann::json::parse(file);
            
            m_total_stores = data.value("total_stores", 0);
            m_stores_data = data.value("stores", nlohmann::json::array());
            
            spdlog::info("Loaded {} stores from database", m_total_stores);
        }
        
    } catch (const std::exception& e) {
        spdlog::error("Error loading stores data: {}", e.what());
    }
}

void Context7Database::loadWikisData() {
    try {
        std::filesystem::path wikis_file("docs/data/wikis/index.json");
        if (std::filesystem::exists(wikis_file)) {
            std::ifstream file(wikis_file);
            nlohmann::json data = nlohmann::json::parse(file);
            
            m_total_wikis = data.value("total_wikis", 0);
            m_wikis_data = data.value("wikis", nlohmann::json::array());
            
            spdlog::info("Loaded {} wikis from database", m_total_wikis);
        }
        
    } catch (const std::exception& e) {
        spdlog::error("Error loading wikis data: {}", e.what());
    }
}

void Context7Database::loadVRCompatibilityData() {
    try {
        std::filesystem::path vr_file("docs/data/vr_compatibility/index.json");
        if (std::filesystem::exists(vr_file)) {
            std::ifstream file(vr_file);
            nlohmann::json data = nlohmann::json::parse(file);
            
            m_total_vr_entries = data.value("total_entries", 0);
            m_vr_compatibility_data = data.value("compatibility_data", nlohmann::json::array());
            
            spdlog::info("Loaded {} VR compatibility entries from database", m_total_vr_entries);
        }
        
    } catch (const std::exception& e) {
        spdlog::error("Error loading VR compatibility data: {}", e.what());
    }
}

// AI analysis engine initialization
void Context7Database::initializeAIAnalysisEngine() {
    try {
        spdlog::info("Initializing AI analysis engine");
        
        // Initialize machine learning models
        initializeMachineLearningModels();
        
        // Initialize natural language processing
        initializeNaturalLanguageProcessing();
        
        // Initialize pattern recognition
        initializePatternRecognition();
        
        // Initialize VR compatibility prediction
        initializeVRCompatibilityPrediction();
        
        spdlog::info("AI analysis engine initialized successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error initializing AI analysis engine: {}", e.what());
    }
}

void Context7Database::initializeMachineLearningModels() {
    try {
        // Initialize engine detection model
        m_engine_detection_model = std::make_unique<EngineDetectionModel>();
        
        // Initialize VR compatibility model
        m_vr_compatibility_model = std::make_unique<VRCompatibilityModel>();
        
        // Initialize performance prediction model
        m_performance_prediction_model = std::make_unique<PerformancePredictionModel>();
        
        spdlog::info("Machine learning models initialized successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error initializing machine learning models: {}", e.what());
    }
}

void Context7Database::initializeNaturalLanguageProcessing() {
    try {
        // Initialize text analysis engine
        m_text_analysis_engine = std::make_unique<TextAnalysisEngine>();
        
        // Initialize keyword extraction
        m_keyword_extractor = std::make_unique<KeywordExtractor>();
        
        // Initialize sentiment analysis
        m_sentiment_analyzer = std::make_unique<SentimentAnalyzer>();
        
        spdlog::info("Natural language processing initialized successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error initializing natural language processing: {}", e.what());
    }
}

void Context7Database::initializePatternRecognition() {
    try {
        // Initialize pattern recognition engine
        m_pattern_recognition_engine = std::make_unique<PatternRecognitionEngine>();
        
        // Initialize feature extraction
        m_feature_extractor = std::make_unique<FeatureExtractor>();
        
        // Initialize classification engine
        m_classification_engine = std::make_unique<ClassificationEngine>();
        
        spdlog::info("Pattern recognition initialized successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error initializing pattern recognition: {}", e.what());
    }
}

void Context7Database::initializeVRCompatibilityPrediction() {
    try {
        // Initialize VR compatibility prediction engine
        m_vr_compatibility_predictor = std::make_unique<VRCompatibilityPredictor>();
        
        // Initialize performance metrics analyzer
        m_performance_metrics_analyzer = std::make_unique<PerformanceMetricsAnalyzer>();
        
        // Initialize optimization recommendation engine
        m_optimization_recommendation_engine = std::make_unique<OptimizationRecommendationEngine>();
        
        spdlog::info("VR compatibility prediction initialized successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error initializing VR compatibility prediction: {}", e.what());
    }
}

// Data saving
void Context7Database::saveAllData() {
    try {
        spdlog::info("Saving all data to database");
        
        // Save games data
        saveGamesData();
        
        // Save engines data
        saveEnginesData();
        
        // Save analyses data
        saveAnalysesData();
        
        // Save stores data
        saveStoresData();
        
        // Save wikis data
        saveWikisData();
        
        // Save VR compatibility data
        saveVRCompatibilityData();
        
        spdlog::info("All data saved successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error saving data: {}", e.what());
    }
}

void Context7Database::saveGamesData() {
    try {
        std::filesystem::path games_file("docs/data/games/index.json");
        
        nlohmann::json data = {
            {"total_games", m_total_games},
            {"last_updated", std::chrono::system_clock::now().time_since_epoch().count()},
            {"games", m_games_data}
        };
        
        std::ofstream file(games_file);
        file << data.dump(2);
        file.close();
        
        spdlog::info("Games data saved successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error saving games data: {}", e.what());
    }
}

void Context7Database::saveEnginesData() {
    try {
        std::filesystem::path engines_file("docs/data/engines/index.json");
        
        nlohmann::json data = {
            {"total_engines", m_total_engines},
            {"last_updated", std::chrono::system_clock::now().time_since_epoch().count()},
            {"engines", m_engines_data}
        };
        
        std::ofstream file(engines_file);
        file << data.dump(2);
        file.close();
        
        spdlog::info("Engines data saved successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error saving engines data: {}", e.what());
    }
}

void Context7Database::saveAnalysesData() {
    try {
        std::filesystem::path analyses_file("docs/data/analyses/index.json");
        
        nlohmann::json data = {
            {"total_analyses", m_total_analyses},
            {"last_updated", std::chrono::system_clock::now().time_since_epoch().count()},
            {"analyses", m_analyses_data}
        };
        
        std::ofstream file(analyses_file);
        file << data.dump(2);
        file.close();
        
        spdlog::info("Analyses data saved successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error saving analyses data: {}", e.what());
    }
}

void Context7Database::saveStoresData() {
    try {
        std::filesystem::path stores_file("docs/data/stores/index.json");
        
        nlohmann::json data = {
            {"total_stores", m_total_stores},
            {"last_updated", std::chrono::system_clock::now().time_since_epoch().count()},
            {"stores", m_stores_data}
        };
        
        std::ofstream file(stores_file);
        file << data.dump(2);
        file.close();
        
        spdlog::info("Stores data saved successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error saving stores data: {}", e.what());
    }
}

void Context7Database::saveWikisData() {
    try {
        std::filesystem::path wikis_file("docs/data/wikis/index.json");
        
        nlohmann::json data = {
            {"total_wikis", m_total_wikis},
            {"last_updated", std::chrono::system_clock::now().time_since_epoch().count()},
            {"wikis", m_wikis_data}
        };
        
        std::ofstream file(wikis_file);
        file << data.dump(2);
        file.close();
        
        spdlog::info("Wikis data saved successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error saving wikis data: {}", e.what());
    }
}

void Context7Database::saveVRCompatibilityData() {
    try {
        std::filesystem::path vr_file("docs/data/vr_compatibility/index.json");
        
        nlohmann::json data = {
            {"total_entries", m_total_vr_entries},
            {"last_updated", std::chrono::system_clock::now().time_since_epoch().count()},
            {"compatibility_data", m_vr_compatibility_data}
        };
        
        std::ofstream file(vr_file);
        file << data.dump(2);
        file.close();
        
        spdlog::info("VR compatibility data saved successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error saving VR compatibility data: {}", e.what());
    }
}

// Cleanup functions
void Context7Database::cleanupHttpClient() {
    try {
        if (m_curl_handle) {
            curl_easy_cleanup(m_curl_handle);
            m_curl_handle = nullptr;
        }
        
        curl_global_cleanup();
        
        spdlog::info("HTTP client cleaned up successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error cleaning up HTTP client: {}", e.what());
    }
}

void Context7Database::cleanupAIAnalysisEngine() {
    try {
        // Cleanup machine learning models
        m_engine_detection_model.reset();
        m_vr_compatibility_model.reset();
        m_performance_prediction_model.reset();
        
        // Cleanup natural language processing
        m_text_analysis_engine.reset();
        m_keyword_extractor.reset();
        m_sentiment_analyzer.reset();
        
        // Cleanup pattern recognition
        m_pattern_recognition_engine.reset();
        m_feature_extractor.reset();
        m_classification_engine.reset();
        
        // Cleanup VR compatibility prediction
        m_vr_compatibility_predictor.reset();
        m_performance_metrics_analyzer.reset();
        m_optimization_recommendation_engine.reset();
        
        spdlog::info("AI analysis engine cleaned up successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error cleaning up AI analysis engine: {}", e.what());
    }
}

} // namespace uevr_context7

// Data collection methods
void uevr_context7::Context7Database::collectGameData(const std::string& game_name) {
    try {
        spdlog::info("Starting data collection for game: {}", game_name);
        
        // Collect data from multiple sources
        auto steam_data = collectSteamData(game_name);
        auto epic_data = collectEpicData(game_name);
        auto gog_data = collectGOGData(game_name);
        auto wiki_data = collectWikiData(game_name);
        
        // Merge and analyze collected data
        auto merged_data = mergeGameData(steam_data, epic_data, gog_data, wiki_data);
        
        // Perform AI analysis
        auto analysis_result = analyzeGameData(merged_data);
        
        // Store results in database
        storeGameAnalysis(game_name, merged_data, analysis_result);
        
        spdlog::info("Data collection completed for game: {}", game_name);
        
    } catch (const std::exception& e) {
        spdlog::error("Error collecting data for game {}: {}", game_name, e.what());
    }
}

nlohmann::json uevr_context7::Context7Database::collectSteamData(const std::string& game_name) {
    try {
        spdlog::info("Collecting Steam data for: {}", game_name);
        
        // Construct Steam API URL
        std::string search_url = "https://store.steampowered.com/api/storesearch?term=" + 
                                encodeUrl(game_name) + "&l=english&cc=US";
        
        // Make HTTP request
        auto response = makeHttpRequest(search_url);
        
        if (response.empty()) {
            spdlog::warn("No Steam data received for: {}", game_name);
            return nlohmann::json::object();
        }
        
        // Parse JSON response
        auto steam_data = nlohmann::json::parse(response);
        
        // Extract relevant information
        nlohmann::json extracted_data = {
            {"source", "steam"},
            {"game_name", game_name},
            {"timestamp", std::chrono::system_clock::now().time_since_epoch().count()},
            {"raw_data", steam_data}
        };
        
        // Extract specific fields if available
        if (steam_data.contains("items") && steam_data["items"].is_array() && 
            !steam_data["items"].empty()) {
            
            auto first_item = steam_data["items"][0];
            
            if (first_item.contains("appid")) {
                extracted_data["steam_app_id"] = first_item["appid"];
            }
            
            if (first_item.contains("name")) {
                extracted_data["steam_name"] = first_item["name"];
            }
            
            if (first_item.contains("price")) {
                extracted_data["steam_price"] = first_item["price"];
            }
            
            if (first_item.contains("metascore")) {
                extracted_data["steam_metascore"] = first_item["metascore"];
            }
        }
        
        spdlog::info("Steam data collected successfully for: {}", game_name);
        return extracted_data;
        
    } catch (const std::exception& e) {
        spdlog::error("Error collecting Steam data for {}: {}", game_name, e.what());
        return nlohmann::json::object();
    }
}

nlohmann::json uevr_context7::Context7Database::collectEpicData(const std::string& game_name) {
    try {
        spdlog::info("Collecting Epic Games data for: {}", game_name);
        
        // Epic Games Store doesn't have a public API, so we'll use web scraping
        // This is a simplified approach - in production you'd want proper web scraping
        
        std::string search_url = "https://store.epicgames.com/en-US/browse?q=" + 
                                encodeUrl(game_name) + "&sortBy=relevancy";
        
        // Make HTTP request
        auto response = makeHttpRequest(search_url);
        
        if (response.empty()) {
            spdlog::warn("No Epic Games data received for: {}", game_name);
            return nlohmann::json::object();
        }
        
        // Extract basic information from HTML response
        nlohmann::json extracted_data = {
            {"source", "epic_games"},
            {"game_name", game_name},
            {"timestamp", std::chrono::system_clock::now().time_since_epoch().count()},
            {"raw_data", response.substr(0, 1000)} // Store first 1000 chars for analysis
        };
        
        // Try to extract price information using regex
        std::regex price_regex(R"(\$\d+\.\d+)");
        std::smatch price_match;
        
        if (std::regex_search(response, price_match, price_regex)) {
            extracted_data["epic_price"] = price_match.str();
        }
        
        spdlog::info("Epic Games data collected successfully for: {}", game_name);
        return extracted_data;
        
    } catch (const std::exception& e) {
        spdlog::error("Error collecting Epic Games data for {}: {}", game_name, e.what());
        return nlohmann::json::object();
    }
}

nlohmann::json uevr_context7::Context7Database::collectGOGData(const std::string& game_name) {
    try {
        spdlog::info("Collecting GOG data for: {}", game_name);
        
        // GOG doesn't have a public API, so we'll use web scraping
        std::string search_url = "https://www.gog.com/games/ajax/filtered?mediaType=game&search=" + 
                                encodeUrl(game_name);
        
        // Make HTTP request
        auto response = makeHttpRequest(search_url);
        
        if (response.empty()) {
            spdlog::warn("No GOG data received for: {}", game_name);
            return nlohmann::json::object();
        }
        
        // Parse JSON response if available
        nlohmann::json extracted_data = {
            {"source", "gog"},
            {"game_name", game_name},
            {"timestamp", std::chrono::system_clock::now().time_since_epoch().count()}
        };
        
        try {
            auto gog_data = nlohmann::json::parse(response);
            extracted_data["raw_data"] = gog_data;
            
            // Extract specific fields if available
            if (gog_data.contains("products") && gog_data["products"].is_array() && 
                !gog_data["products"].empty()) {
                
                auto first_product = gog_data["products"][0];
                
                if (first_product.contains("title")) {
                    extracted_data["gog_title"] = first_product["title"];
                }
                
                if (first_product.contains("price")) {
                    extracted_data["gog_price"] = first_product["price"];
                }
                
                if (first_product.contains("rating")) {
                    extracted_data["gog_rating"] = first_product["rating"];
                }
            }
            
        } catch (const nlohmann::json::exception& e) {
            // If JSON parsing fails, store raw response
            extracted_data["raw_data"] = response.substr(0, 1000);
            spdlog::warn("Failed to parse GOG JSON response for {}: {}", game_name, e.what());
        }
        
        spdlog::info("GOG data collected successfully for: {}", game_name);
        return extracted_data;
        
    } catch (const std::exception& e) {
        spdlog::error("Error collecting GOG data for {}: {}", game_name, e.what());
        return nlohmann::json::object();
    }
}

nlohmann::json uevr_context7::Context7Database::collectWikiData(const std::string& game_name) {
    try {
        spdlog::info("Collecting Wiki data for: {}", game_name);
        
        // Try multiple wiki sources
        std::vector<std::string> wiki_sources = {
            "https://en.wikipedia.org/api/rest_v1/page/summary/" + encodeUrl(game_name),
            "https://www.ign.com/games/" + encodeUrl(game_name),
            "https://www.metacritic.com/game/" + encodeUrl(game_name)
        };
        
        nlohmann::json extracted_data = {
            {"source", "wikis"},
            {"game_name", game_name},
            {"timestamp", std::chrono::system_clock::now().time_since_epoch().count()},
            {"wiki_sources", nlohmann::json::array()}
        };
        
        for (const auto& wiki_url : wiki_sources) {
            try {
                auto response = makeHttpRequest(wiki_url);
                
                if (!response.empty()) {
                    nlohmann::json wiki_source_data = {
                        {"url", wiki_url},
                        {"response_length", response.length()},
                        {"raw_data", response.substr(0, 2000)} // Store first 2000 chars
                    };
                    
                    extracted_data["wiki_sources"].push_back(wiki_source_data);
                }
                
            } catch (const std::exception& e) {
                spdlog::warn("Failed to collect data from wiki source {}: {}", wiki_url, e.what());
            }
        }
        
        spdlog::info("Wiki data collected successfully for: {}", game_name);
        return extracted_data;
        
    } catch (const std::exception& e) {
        spdlog::error("Error collecting Wiki data for {}: {}", game_name, e.what());
        return nlohmann::json::object();
    }
}

nlohmann::json uevr_context7::Context7Database::mergeGameData(
    const nlohmann::json& steam_data,
    const nlohmann::json& epic_data,
    const nlohmann::json& gog_data,
    const nlohmann::json& wiki_data) {
    
    try {
        spdlog::info("Merging game data from multiple sources");
        
        nlohmann::json merged_data = {
            {"game_name", ""},
            {"timestamp", std::chrono::system_clock::now().time_since_epoch().count()},
            {"sources", nlohmann::json::array()},
            {"consolidated_data", nlohmann::json::object()}
        };
        
        // Determine game name from available sources
        if (!steam_data.empty() && steam_data.contains("game_name")) {
            merged_data["game_name"] = steam_data["game_name"];
        } else if (!epic_data.empty() && epic_data.contains("game_name")) {
            merged_data["game_name"] = epic_data["game_name"];
        } else if (!gog_data.empty() && gog_data.contains("game_name")) {
            merged_data["game_name"] = gog_data["game_name"];
        } else if (!wiki_data.empty() && wiki_data.contains("game_name")) {
            merged_data["game_name"] = wiki_data["game_name"];
        }
        
        // Add source data
        if (!steam_data.empty()) merged_data["sources"].push_back(steam_data);
        if (!epic_data.empty()) merged_data["sources"].push_back(epic_data);
        if (!gog_data.empty()) merged_data["sources"].push_back(gog_data);
        if (!wiki_data.empty()) merged_data["sources"].push_back(wiki_data);
        
        // Consolidate pricing information
        nlohmann::json pricing = nlohmann::json::object();
        if (!steam_data.empty() && steam_data.contains("steam_price")) {
            pricing["steam"] = steam_data["steam_price"];
        }
        if (!epic_data.empty() && epic_data.contains("epic_price")) {
            pricing["epic"] = epic_data["epic_price"];
        }
        if (!gog_data.empty() && gog_data.contains("gog_price")) {
            pricing["gog"] = gog_data["gog_price"];
        }
        
        if (!pricing.empty()) {
            merged_data["consolidated_data"]["pricing"] = pricing;
        }
        
        // Consolidate ratings and scores
        nlohmann::json ratings = nlohmann::json::object();
        if (!steam_data.empty() && steam_data.contains("steam_metascore")) {
            ratings["steam_metascore"] = steam_data["steam_metascore"];
        }
        if (!gog_data.empty() && gog_data.contains("gog_rating")) {
            ratings["gog_rating"] = gog_data["gog_rating"];
        }
        
        if (!ratings.empty()) {
            merged_data["consolidated_data"]["ratings"] = ratings;
        }
        
        // Add source count
        merged_data["source_count"] = merged_data["sources"].size();
        
        spdlog::info("Game data merged successfully from {} sources", merged_data["source_count"].get<int>());
        return merged_data;
        
    } catch (const std::exception& e) {
        spdlog::error("Error merging game data: {}", e.what());
        return nlohmann::json::object();
    }
}

nlohmann::json uevr_context7::Context7Database::analyzeGameData(const nlohmann::json& game_data) {
    try {
        spdlog::info("Starting AI analysis of game data");
        
        if (game_data.empty()) {
            spdlog::warn("No game data provided for analysis");
            return nlohmann::json::object();
        }
        
        nlohmann::json analysis_result = {
            {"analysis_timestamp", std::chrono::system_clock::now().time_since_epoch().count()},
            {"game_name", game_data.value("game_name", "")},
            {"ai_analysis", nlohmann::json::object()},
            {"vr_compatibility_prediction", nlohmann::json::object()},
            {"performance_metrics", nlohmann::json::object()},
            {"recommendations", nlohmann::json::array()}
        };
        
        // Perform text analysis on collected data
        auto text_analysis = performTextAnalysis(game_data);
        analysis_result["ai_analysis"]["text_analysis"] = text_analysis;
        
        // Perform pattern recognition
        auto pattern_analysis = performPatternRecognition(game_data);
        analysis_result["ai_analysis"]["pattern_analysis"] = pattern_analysis;
        
        // Predict VR compatibility
        auto vr_prediction = predictVRCompatibility(game_data);
        analysis_result["vr_compatibility_prediction"] = vr_prediction;
        
        // Analyze performance characteristics
        auto performance_analysis = analyzePerformanceCharacteristics(game_data);
        analysis_result["performance_metrics"] = performance_analysis;
        
        // Generate recommendations
        auto recommendations = generateRecommendations(game_data, analysis_result);
        analysis_result["recommendations"] = recommendations;
        
        // Calculate confidence scores
        auto confidence_scores = calculateConfidenceScores(analysis_result);
        analysis_result["confidence_scores"] = confidence_scores;
        
        spdlog::info("AI analysis completed successfully");
        return analysis_result;
        
    } catch (const std::exception& e) {
        spdlog::error("Error during AI analysis: {}", e.what());
        return nlohmann::json::object();
    }
}

nlohmann::json uevr_context7::Context7Database::performTextAnalysis(const nlohmann::json& game_data) {
    try {
        nlohmann::json text_analysis = {
            {"keywords", nlohmann::json::array()},
            {"sentiment", "neutral"},
            {"complexity_score", 0.0},
            {"technical_terms", nlohmann::json::array()},
            {"genre_indicators", nlohmann::json::array()}
        };
        
        // Extract all text data for analysis
        std::string combined_text = "";
        
        if (game_data.contains("sources")) {
            for (const auto& source : game_data["sources"]) {
                if (source.contains("raw_data")) {
                    combined_text += source["raw_data"].get<std::string>();
                }
            }
        }
        
        if (combined_text.empty()) {
            spdlog::warn("No text data available for analysis");
            return text_analysis;
        }
        
        // Extract keywords using AI engine
        if (m_keyword_extractor) {
            auto keywords = m_keyword_extractor->extractKeywords(combined_text);
            text_analysis["keywords"] = keywords;
        }
        
        // Perform sentiment analysis
        if (m_sentiment_analyzer) {
            auto sentiment = m_sentiment_analyzer->analyzeSentiment(combined_text);
            text_analysis["sentiment"] = sentiment;
        }
        
        // Calculate text complexity
        text_analysis["complexity_score"] = calculateTextComplexity(combined_text);
        
        // Extract technical terms
        text_analysis["technical_terms"] = extractTechnicalTerms(combined_text);
        
        // Identify genre indicators
        text_analysis["genre_indicators"] = identifyGenreIndicators(combined_text);
        
        return text_analysis;
        
    } catch (const std::exception& e) {
        spdlog::error("Error during text analysis: {}", e.what());
        return nlohmann::json::object();
    }
}

nlohmann::json uevr_context7::Context7Database::performPatternRecognition(const nlohmann::json& game_data) {
    try {
        nlohmann::json pattern_analysis = {
            {"engine_patterns", nlohmann::json::array()},
            {"rendering_patterns", nlohmann::json::array()},
            {"input_patterns", nlohmann::json::array()},
            {"audio_patterns", nlohmann::json::array()},
            {"performance_patterns", nlohmann::json::array()}
        };
        
        // Extract patterns from raw data
        if (game_data.contains("sources")) {
            for (const auto& source : game_data["sources"]) {
                if (source.contains("raw_data")) {
                    auto raw_data = source["raw_data"].get<std::string>();
                    
                    // Look for engine patterns
                    auto engine_patterns = detectEnginePatterns(raw_data);
                    pattern_analysis["engine_patterns"].insert(
                        pattern_analysis["engine_patterns"].end(),
                        engine_patterns.begin(),
                        engine_patterns.end()
                    );
                    
                    // Look for rendering patterns
                    auto rendering_patterns = detectRenderingPatterns(raw_data);
                    pattern_analysis["rendering_patterns"].insert(
                        pattern_analysis["rendering_patterns"].end(),
                        rendering_patterns.begin(),
                        rendering_patterns.end()
                    );
                    
                    // Look for input patterns
                    auto input_patterns = detectInputPatterns(raw_data);
                    pattern_analysis["input_patterns"].insert(
                        pattern_analysis["input_patterns"].end(),
                        input_patterns.begin(),
                        input_patterns.end()
                    );
                    
                    // Look for audio patterns
                    auto audio_patterns = detectAudioPatterns(raw_data);
                    pattern_analysis["audio_patterns"].insert(
                        pattern_analysis["audio_patterns"].end(),
                        audio_patterns.begin(),
                        audio_patterns.end()
                    );
                    
                    // Look for performance patterns
                    auto performance_patterns = detectPerformancePatterns(raw_data);
                    pattern_analysis["performance_patterns"].insert(
                        pattern_analysis["performance_patterns"].end(),
                        performance_patterns.begin(),
                        performance_patterns.end()
                    );
                }
            }
        }
        
        // Remove duplicates
        removeDuplicatePatterns(pattern_analysis);
        
        return pattern_analysis;
        
    } catch (const std::exception& e) {
        spdlog::error("Error during pattern recognition: {}", e.what());
        return nlohmann::json::object();
    }
}

nlohmann::json uevr_context7::Context7Database::predictVRCompatibility(const nlohmann::json& game_data) {
    try {
        nlohmann::json vr_prediction = {
            {"overall_compatibility_score", 0.0},
            {"rendering_compatibility", 0.0},
            {"input_compatibility", 0.0},
            {"audio_compatibility", 0.0},
            {"performance_compatibility", 0.0},
            {"comfort_rating", 0.0},
            {"optimization_potential", 0.0},
            {"estimated_vr_performance", "unknown"},
            {"compatibility_factors", nlohmann::json::array()},
            {"potential_issues", nlohmann::json::array()},
            {"optimization_recommendations", nlohmann::json::array()}
        };
        
        // Use AI models to predict compatibility
        if (m_vr_compatibility_model) {
            auto compatibility_scores = m_vr_compatibility_model->predictCompatibility(game_data);
            
            vr_prediction["overall_compatibility_score"] = compatibility_scores["overall"];
            vr_prediction["rendering_compatibility"] = compatibility_scores["rendering"];
            vr_prediction["input_compatibility"] = compatibility_scores["input"];
            vr_prediction["audio_compatibility"] = compatibility_scores["audio"];
            vr_prediction["performance_compatibility"] = compatibility_scores["performance"];
        }
        
        // Calculate comfort rating based on game characteristics
        vr_prediction["comfort_rating"] = calculateVRComfortRating(game_data);
        
        // Estimate optimization potential
        vr_prediction["optimization_potential"] = estimateOptimizationPotential(game_data);
        
        // Determine estimated VR performance
        auto overall_score = vr_prediction["overall_compatibility_score"].get<double>();
        if (overall_score >= 0.8) {
            vr_prediction["estimated_vr_performance"] = "excellent";
        } else if (overall_score >= 0.6) {
            vr_prediction["estimated_vr_performance"] = "good";
        } else if (overall_score >= 0.4) {
            vr_prediction["estimated_vr_performance"] = "moderate";
        } else if (overall_score >= 0.2) {
            vr_prediction["estimated_vr_performance"] = "poor";
        } else {
            vr_prediction["estimated_vr_performance"] = "very_poor";
        }
        
        // Identify compatibility factors
        vr_prediction["compatibility_factors"] = identifyCompatibilityFactors(game_data);
        
        // Identify potential issues
        vr_prediction["potential_issues"] = identifyPotentialIssues(game_data);
        
        // Generate optimization recommendations
        vr_prediction["optimization_recommendations"] = generateOptimizationRecommendations(game_data);
        
        return vr_prediction;
        
    } catch (const std::exception& e) {
        spdlog::error("Error during VR compatibility prediction: {}", e.what());
        return nlohmann::json::object();
    }
}

// Utility functions
std::string uevr_context7::Context7Database::encodeUrl(const std::string& input) {
    std::string encoded;
    for (char c : input) {
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded += c;
        } else {
            char hex[4];
            snprintf(hex, sizeof(hex), "%%%02X", (unsigned char)c);
            encoded += hex;
        }
    }
    return encoded;
}

std::string uevr_context7::Context7Database::makeHttpRequest(const std::string& url) {
    try {
        if (!m_curl_handle) {
            spdlog::error("CURL handle not initialized");
            return "";
        }
        
        std::string response;
        
        // Set URL
        curl_easy_setopt(m_curl_handle, CURLOPT_URL, url.c_str());
        
        // Set write callback
        curl_easy_setopt(m_curl_handle, CURLOPT_WRITEFUNCTION, 
            [](void* contents, size_t size, size_t nmemb, std::string* userp) -> size_t {
                userp->append((char*)contents, size * nmemb);
                return size * nmemb;
            });
        curl_easy_setopt(m_curl_handle, CURLOPT_WRITEDATA, &response);
        
        // Perform request
        CURLcode res = curl_easy_perform(m_curl_handle);
        
        if (res != CURLE_OK) {
            spdlog::error("CURL request failed: {}", curl_easy_strerror(res));
            return "";
        }
        
        // Get HTTP response code
        long response_code;
        curl_easy_getinfo(m_curl_handle, CURLINFO_RESPONSE_CODE, &response_code);
        
        if (response_code != 200) {
            spdlog::warn("HTTP request returned status code: {}", response_code);
        }
        
        return response;
        
    } catch (const std::exception& e) {
        spdlog::error("Error making HTTP request: {}", e.what());
        return "";
    }
}

void uevr_context7::Context7Database::storeGameAnalysis(
    const std::string& game_name,
    const nlohmann::json& game_data,
    const nlohmann::json& analysis_result) {
    
    try {
        spdlog::info("Storing game analysis for: {}", game_name);
        
        // Create analysis entry
        nlohmann::json analysis_entry = {
            {"game_name", game_name},
            {"timestamp", std::chrono::system_clock::now().time_since_epoch().count()},
            {"game_data", game_data},
            {"analysis_result", analysis_result}
        };
        
        // Add to analyses data
        m_analyses_data.push_back(analysis_entry);
        m_total_analyses++;
        
        // Update games data if not already present
        bool game_exists = false;
        for (auto& game : m_games_data) {
            if (game.value("name", "") == game_name) {
                game["last_analysis"] = analysis_entry;
                game["analysis_count"] = game.value("analysis_count", 0) + 1;
                game_exists = true;
                break;
            }
        }
        
        if (!game_exists) {
            nlohmann::json new_game = {
                {"name", game_name},
                {"first_analysis", analysis_entry},
                {"last_analysis", analysis_entry},
                {"analysis_count", 1},
                {"created_timestamp", std::chrono::system_clock::now().time_since_epoch().count()}
            };
            m_games_data.push_back(new_game);
            m_total_games++;
        }
        
        // Save updated data
        saveAnalysesData();
        saveGamesData();
        
        spdlog::info("Game analysis stored successfully for: {}", game_name);
        
    } catch (const std::exception& e) {
        spdlog::error("Error storing game analysis for {}: {}", game_name, e.what());
    }
}

// AI analysis helper functions
double uevr_context7::Context7Database::calculateTextComplexity(const std::string& text) {
    try {
        if (text.empty()) return 0.0;
        
        // Simple complexity calculation based on:
        // - Average word length
        // - Sentence complexity
        // - Technical term density
        
        std::vector<std::string> words;
        std::istringstream iss(text);
        std::string word;
        
        while (iss >> word) {
            // Remove punctuation
            word.erase(std::remove_if(word.begin(), word.end(), ispunct), word.end());
            if (!word.empty()) {
                words.push_back(word);
            }
        }
        
        if (words.empty()) return 0.0;
        
        // Calculate average word length
        double total_length = 0.0;
        for (const auto& w : words) {
            total_length += w.length();
        }
        double avg_word_length = total_length / words.size();
        
        // Calculate sentence count (rough estimate)
        int sentence_count = std::count(text.begin(), text.end(), '.') + 
                           std::count(text.begin(), text.end(), '!') + 
                           std::count(text.begin(), text.end(), '?');
        if (sentence_count == 0) sentence_count = 1;
        
        double avg_sentence_length = static_cast<double>(words.size()) / sentence_count;
        
        // Calculate technical term density
        std::vector<std::string> technical_terms = {
            "engine", "rendering", "shader", "texture", "polygon", "vertex", "fragment",
            "directx", "opengl", "vulkan", "api", "gpu", "cpu", "memory", "optimization"
        };
        
        int technical_count = 0;
        for (const auto& term : technical_terms) {
            std::string lower_text = text;
            std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
            std::string lower_term = term;
            std::transform(lower_term.begin(), lower_term.end(), lower_term.begin(), ::tolower);
            
            size_t pos = 0;
            while ((pos = lower_text.find(lower_term, pos)) != std::string::npos) {
                technical_count++;
                pos += lower_term.length();
            }
        }
        
        double technical_density = static_cast<double>(technical_count) / words.size();
        
        // Combine factors into complexity score (0.0 to 1.0)
        double complexity = (avg_word_length / 10.0) * 0.3 + 
                          (avg_sentence_length / 50.0) * 0.3 + 
                          (technical_density * 10.0) * 0.4;
        
        return std::min(1.0, std::max(0.0, complexity));
        
    } catch (const std::exception& e) {
        spdlog::error("Error calculating text complexity: {}", e.what());
        return 0.0;
    }
}

nlohmann::json uevr_context7::Context7Database::extractTechnicalTerms(const std::string& text) {
    try {
        nlohmann::json technical_terms = nlohmann::json::array();
        
        std::vector<std::string> known_terms = {
            "Unreal Engine", "Unity", "CryEngine", "Source Engine", "id Tech",
            "DirectX", "OpenGL", "Vulkan", "Metal", "Mantle",
            "Shader", "Texture", "Polygon", "Vertex", "Fragment",
            "GPU", "CPU", "RAM", "VRAM", "SSD", "HDD",
            "Ray Tracing", "Global Illumination", "Ambient Occlusion",
            "Anti-aliasing", "V-Sync", "Frame Rate", "Resolution"
        };
        
        std::string lower_text = text;
        std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
        
        for (const auto& term : known_terms) {
            std::string lower_term = term;
            std::transform(lower_term.begin(), lower_term.end(), lower_term.begin(), ::tolower);
            
            if (lower_text.find(lower_term) != std::string::npos) {
                technical_terms.push_back(term);
            }
        }
        
        return technical_terms;
        
    } catch (const std::exception& e) {
        spdlog::error("Error extracting technical terms: {}", e.what());
        return nlohmann::json::array();
    }
}

nlohmann::json uevr_context7::Context7Database::identifyGenreIndicators(const std::string& text) {
    try {
        nlohmann::json genre_indicators = nlohmann::json::array();
        
        std::map<std::string, std::vector<std::string>> genre_keywords = {
            {"FPS", {"shooter", "first-person", "fps", "gun", "weapon", "combat"}},
            {"RPG", {"role-playing", "rpg", "quest", "character", "level", "experience"}},
            {"Strategy", {"strategy", "tactical", "turn-based", "resource", "management"}},
            {"Adventure", {"adventure", "exploration", "puzzle", "story", "narrative"}},
            {"Racing", {"racing", "car", "vehicle", "speed", "track", "driving"}},
            {"Sports", {"sports", "football", "basketball", "soccer", "athletic"}},
            {"Simulation", {"simulation", "sim", "realistic", "physics", "accurate"}},
            {"Horror", {"horror", "scary", "survival", "thriller", "fear"}}
        };
        
        std::string lower_text = text;
        std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
        
        for (const auto& [genre, keywords] : genre_keywords) {
            for (const auto& keyword : keywords) {
                if (lower_text.find(keyword) != std::string::npos) {
                    genre_indicators.push_back(genre);
                    break; // Only add each genre once
                }
            }
        }
        
        return genre_indicators;
        
    } catch (const std::exception& e) {
        spdlog::error("Error identifying genre indicators: {}", e.what());
        return nlohmann::json::array();
    }
}

// Pattern detection functions
nlohmann::json uevr_context7::Context7Database::detectEnginePatterns(const std::string& text) {
    try {
        nlohmann::json patterns = nlohmann::json::array();
        
        std::map<std::string, std::vector<std::string>> engine_patterns = {
            {"Unreal Engine", {"unreal", "ue4", "ue5", "blueprint", "unreal engine"}},
            {"Unity", {"unity", "unity engine", "mono", "c#", "unity3d"}},
            {"CryEngine", {"cryengine", "crytek", "cry", "cryengine3"}},
            {"Source Engine", {"source", "source engine", "valve", "hammer", "bsp"}},
            {"REDengine", {"redengine", "red engine", "witcher", "cyberpunk"}},
            {"MT Framework", {"mt framework", "capcom", "resident evil", "monster hunter"}}
        };
        
        std::string lower_text = text;
        std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
        
        for (const auto& [engine, keywords] : engine_patterns) {
            for (const auto& keyword : keywords) {
                if (lower_text.find(keyword) != std::string::npos) {
                    nlohmann::json pattern = {
                        {"engine", engine},
                        {"confidence", 0.8},
                        {"detected_by", "keyword_match"}
                    };
                    patterns.push_back(pattern);
                    break;
                }
            }
        }
        
        return patterns;
        
    } catch (const std::exception& e) {
        spdlog::error("Error detecting engine patterns: {}", e.what());
        return nlohmann::json::array();
    }
}

nlohmann::json uevr_context7::Context7Database::detectRenderingPatterns(const std::string& text) {
    try {
        nlohmann::json patterns = nlohmann::json::array();
        
        std::vector<std::string> rendering_keywords = {
            "directx", "opengl", "vulkan", "metal", "mantle",
            "ray tracing", "global illumination", "ambient occlusion",
            "anti-aliasing", "v-sync", "frame rate", "resolution",
            "shader", "texture", "polygon", "vertex", "fragment"
        };
        
        std::string lower_text = text;
        std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
        
        for (const auto& keyword : rendering_keywords) {
            if (lower_text.find(keyword) != std::string::npos) {
                nlohmann::json pattern = {
                    {"type", "rendering"},
                    {"keyword", keyword},
                    {"confidence", 0.7}
                };
                patterns.push_back(pattern);
            }
        }
        
        return patterns;
        
    } catch (const std::exception& e) {
        spdlog::error("Error detecting rendering patterns: {}", e.what());
        return nlohmann::json::array();
    }
}

nlohmann::json uevr_context7::Context7Database::detectInputPatterns(const std::string& text) {
    try {
        nlohmann::json patterns = nlohmann::json::array();
        
        std::vector<std::string> input_keywords = {
            "keyboard", "mouse", "gamepad", "controller", "joystick",
            "touch", "gesture", "motion", "vr", "ar", "haptic"
        };
        
        std::string lower_text = text;
        std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
        
        for (const auto& keyword : input_keywords) {
            if (lower_text.find(keyword) != std::string::npos) {
                nlohmann::json pattern = {
                    {"type", "input"},
                    {"keyword", keyword},
                    {"confidence", 0.7}
                };
                patterns.push_back(pattern);
            }
        }
        
        return patterns;
        
    } catch (const std::exception& e) {
        spdlog::error("Error detecting input patterns: {}", e.what());
        return nlohmann::json::array();
    }
}

nlohmann::json uevr_context7::Context7Database::detectAudioPatterns(const std::string& text) {
    try {
        nlohmann::json patterns = nlohmann::json::array();
        
        std::vector<std::string> audio_keywords = {
            "audio", "sound", "music", "voice", "speech",
            "3d audio", "spatial audio", "surround", "stereo", "mono",
            "sample rate", "bit depth", "compression", "codec"
        };
        
        std::string lower_text = text;
        std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
        
        for (const auto& keyword : audio_keywords) {
            if (lower_text.find(keyword) != std::string::npos) {
                nlohmann::json pattern = {
                    {"type", "audio"},
                    {"keyword", keyword},
                    {"confidence", 0.7}
                };
                patterns.push_back(pattern);
            }
        }
        
        return patterns;
        
    } catch (const std::exception& e) {
        spdlog::error("Error detecting audio patterns: {}", e.what());
        return nlohmann::json::array();
    }
}

nlohmann::json uevr_context7::Context7Database::detectPerformancePatterns(const std::string& text) {
    try {
        nlohmann::json patterns = nlohmann::json::array();
        
        std::vector<std::string> performance_keywords = {
            "fps", "frame rate", "performance", "optimization", "lag",
            "stutter", "drop", "smooth", "fast", "slow", "responsive"
        };
        
        std::string lower_text = text;
        std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
        
        for (const auto& keyword : performance_keywords) {
            if (lower_text.find(keyword) != std::string::npos) {
                nlohmann::json pattern = {
                    {"type", "performance"},
                    {"keyword", keyword},
                    {"confidence", 0.7}
                };
                patterns.push_back(pattern);
            }
        }
        
        return patterns;
        
    } catch (const std::exception& e) {
        spdlog::error("Error detecting performance patterns: {}", e.what());
        return nlohmann::json::array();
    }
}

void uevr_context7::Context7Database::removeDuplicatePatterns(nlohmann::json& pattern_analysis) {
    try {
        for (auto& [category, patterns] : pattern_analysis.items()) {
            if (patterns.is_array()) {
                std::set<std::string> seen;
                nlohmann::json unique_patterns = nlohmann::json::array();
                
                for (const auto& pattern : patterns) {
                    std::string key;
                    if (pattern.contains("engine")) {
                        key = pattern["engine"].get<std::string>();
                    } else if (pattern.contains("keyword")) {
                        key = pattern["keyword"].get<std::string>();
                    } else if (pattern.contains("type")) {
                        key = pattern["type"].get<std::string>();
                    }
                    
                    if (!key.empty() && seen.find(key) == seen.end()) {
                        seen.insert(key);
                        unique_patterns.push_back(pattern);
                    }
                }
                
                patterns = unique_patterns;
            }
        }
        
    } catch (const std::exception& e) {
        spdlog::error("Error removing duplicate patterns: {}", e.what());
    }
}

// Remaining AI analysis functions
nlohmann::json uevr_context7::Context7Database::analyzePerformanceCharacteristics(const nlohmann::json& game_data) {
    try {
        nlohmann::json performance_analysis = {
            {"estimated_fps_range", "unknown"},
            {"performance_category", "unknown"},
            {"optimization_potential", 0.0},
            {"resource_requirements", nlohmann::json::object()},
            {"bottleneck_analysis", nlohmann::json::array()}
        };
        
        // Analyze patterns to estimate performance characteristics
        if (game_data.contains("sources")) {
            std::string combined_text = "";
            for (const auto& source : game_data["sources"]) {
                if (source.contains("raw_data")) {
                    combined_text += source["raw_data"].get<std::string>();
                }
            }
            
            // Estimate FPS range based on engine and features
            performance_analysis["estimated_fps_range"] = estimateFPSRange(combined_text);
            
            // Determine performance category
            performance_analysis["performance_category"] = determinePerformanceCategory(combined_text);
            
            // Estimate optimization potential
            performance_analysis["optimization_potential"] = estimateOptimizationPotential(game_data);
            
            // Analyze resource requirements
            performance_analysis["resource_requirements"] = analyzeResourceRequirements(combined_text);
            
            // Identify potential bottlenecks
            performance_analysis["bottleneck_analysis"] = identifyPerformanceBottlenecks(combined_text);
        }
        
        return performance_analysis;
        
    } catch (const std::exception& e) {
        spdlog::error("Error analyzing performance characteristics: {}", e.what());
        return nlohmann::json::object();
    }
}

nlohmann::json uevr_context7::Context7Database::generateRecommendations(
    const nlohmann::json& game_data,
    const nlohmann::json& analysis_result) {
    
    try {
        nlohmann::json recommendations = nlohmann::json::array();
        
        // Generate VR-specific recommendations
        if (analysis_result.contains("vr_compatibility_prediction")) {
            auto vr_prediction = analysis_result["vr_compatibility_prediction"];
            auto overall_score = vr_prediction.value("overall_compatibility_score", 0.0);
            
            if (overall_score < 0.5) {
                recommendations.push_back({
                    {"type", "vr_compatibility"},
                    {"priority", "high"},
                    {"message", "This game may have significant VR compatibility issues. Consider extensive testing and optimization."},
                    {"suggested_actions", nlohmann::json::array({
                        "Implement comprehensive VR rendering pipeline",
                        "Add VR-specific input handling",
                        "Optimize performance for VR requirements"
                    })}
                });
            } else if (overall_score < 0.7) {
                recommendations.push_back({
                    {"type", "vr_compatibility"},
                    {"priority", "medium"},
                    {"message", "Moderate VR compatibility. Some optimization may be needed."},
                    {"suggested_actions", nlohmann::json::array({
                        "Review VR rendering performance",
                        "Test VR input compatibility",
                        "Optimize critical performance bottlenecks"
                    })}
                });
            } else {
                recommendations.push_back({
                    {"type", "vr_compatibility"},
                    {"priority", "low"},
                    {"message", "Good VR compatibility. Minor optimizations may improve experience."},
                    {"suggested_actions", nlohmann::json::array({
                        "Fine-tune VR settings",
                        "Optimize for specific VR hardware",
                        "Add VR-specific features"
                    })}
                });
            }
        }
        
        // Generate performance recommendations
        if (analysis_result.contains("performance_metrics")) {
            auto performance = analysis_result["performance_metrics"];
            auto optimization_potential = performance.value("optimization_potential", 0.0);
            
            if (optimization_potential > 0.7) {
                recommendations.push_back({
                    {"type", "performance"},
                    {"priority", "high"},
                    {"message", "High optimization potential detected. Significant performance improvements possible."},
                    {"suggested_actions", nlohmann::json::array({
                        "Profile rendering pipeline",
                        "Optimize shader performance",
                        "Review memory management",
                        "Implement LOD systems"
                    })}
                });
            }
        }
        
        // Generate engine-specific recommendations
        if (analysis_result.contains("ai_analysis") && 
            analysis_result["ai_analysis"].contains("pattern_analysis")) {
            
            auto pattern_analysis = analysis_result["ai_analysis"]["pattern_analysis"];
            if (pattern_analysis.contains("engine_patterns")) {
                for (const auto& engine_pattern : pattern_analysis["engine_patterns"]) {
                    if (engine_pattern.contains("engine")) {
                        std::string engine = engine_pattern["engine"].get<std::string>();
                        auto engine_recommendations = generateEngineSpecificRecommendations(engine);
                        recommendations.insert(recommendations.end(), 
                            engine_recommendations.begin(), engine_recommendations.end());
                    }
                }
            }
        }
        
        return recommendations;
        
    } catch (const std::exception& e) {
        spdlog::error("Error generating recommendations: {}", e.what());
        return nlohmann::json::array();
    }
}

nlohmann::json uevr_context7::Context7Database::calculateConfidenceScores(const nlohmann::json& analysis_result) {
    try {
        nlohmann::json confidence_scores = {
            {"overall_confidence", 0.0},
            {"data_quality_score", 0.0},
            {"ai_model_confidence", 0.0},
            {"pattern_detection_confidence", 0.0},
            {"source_reliability_score", 0.0}
        };
        
        // Calculate data quality score based on source count and data completeness
        if (analysis_result.contains("game_data") && 
            analysis_result["game_data"].contains("source_count")) {
            
            int source_count = analysis_result["game_data"]["source_count"].get<int>();
            double data_quality = std::min(1.0, source_count / 4.0); // Normalize to 0-1
            confidence_scores["data_quality_score"] = data_quality;
        }
        
        // Calculate AI model confidence (placeholder - would use actual model confidence)
        confidence_scores["ai_model_confidence"] = 0.8; // Default confidence
        
        // Calculate pattern detection confidence
        if (analysis_result.contains("ai_analysis") && 
            analysis_result["ai_analysis"].contains("pattern_analysis")) {
            
            auto pattern_analysis = analysis_result["ai_analysis"]["pattern_analysis"];
            double pattern_confidence = 0.0;
            int pattern_count = 0;
            
            for (const auto& [category, patterns] : pattern_analysis.items()) {
                if (patterns.is_array() && !patterns.empty()) {
                    pattern_confidence += 0.2; // Each category with patterns adds confidence
                    pattern_count++;
                }
            }
            
            confidence_scores["pattern_detection_confidence"] = std::min(1.0, pattern_confidence);
        }
        
        // Calculate source reliability score
        confidence_scores["source_reliability_score"] = 0.7; // Default reliability
        
        // Calculate overall confidence as weighted average
        double overall_confidence = 
            confidence_scores["data_quality_score"].get<double>() * 0.3 +
            confidence_scores["ai_model_confidence"].get<double>() * 0.3 +
            confidence_scores["pattern_detection_confidence"].get<double>() * 0.2 +
            confidence_scores["source_reliability_score"].get<double>() * 0.2;
        
        confidence_scores["overall_confidence"] = overall_confidence;
        
        return confidence_scores;
        
    } catch (const std::exception& e) {
        spdlog::error("Error calculating confidence scores: {}", e.what());
        return nlohmann::json::object();
    }
}

// Helper functions for performance analysis
std::string uevr_context7::Context7Database::estimateFPSRange(const std::string& text) {
    try {
        std::string lower_text = text;
        std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
        
        // Simple FPS estimation based on engine and features
        if (lower_text.find("unreal engine") != std::string::npos) {
            if (lower_text.find("ray tracing") != std::string::npos) {
                return "30-60 FPS";
            } else {
                return "60-120 FPS";
            }
        } else if (lower_text.find("unity") != std::string::npos) {
            return "60-90 FPS";
        } else if (lower_text.find("cryengine") != std::string::npos) {
            return "30-60 FPS";
        } else if (lower_text.find("source engine") != std::string::npos) {
            return "60-144 FPS";
        } else if (lower_text.find("redengine") != std::string::npos) {
            return "30-60 FPS";
        } else if (lower_text.find("mt framework") != std::string::npos) {
            return "30-60 FPS";
        }
        
        return "Unknown";
        
    } catch (const std::exception& e) {
        spdlog::error("Error estimating FPS range: {}", e.what());
        return "Unknown";
    }
}

std::string uevr_context7::Context7Database::determinePerformanceCategory(const std::string& text) {
    try {
        std::string lower_text = text;
        std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
        
        // Determine performance category based on features and requirements
        if (lower_text.find("ray tracing") != std::string::npos ||
            lower_text.find("4k") != std::string::npos ||
            lower_text.find("8k") != std::string::npos) {
            return "High-End";
        } else if (lower_text.find("1080p") != std::string::npos ||
                   lower_text.find("1440p") != std::string::npos) {
            return "Mid-Range";
        } else if (lower_text.find("720p") != std::string::npos ||
                   lower_text.find("low") != std::string::npos) {
            return "Low-End";
        }
        
        return "Standard";
        
    } catch (const std::exception& e) {
        spdlog::error("Error determining performance category: {}", e.what());
        return "Unknown";
    }
}

double uevr_context7::Context7Database::estimateOptimizationPotential(const nlohmann::json& game_data) {
    try {
        double optimization_potential = 0.5; // Base potential
        
        if (game_data.contains("sources")) {
            std::string combined_text = "";
            for (const auto& source : game_data["sources"]) {
                if (source.contains("raw_data")) {
                    combined_text += source["raw_data"].get<std::string>();
                }
            }
            
            std::string lower_text = combined_text;
            std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
            
            // Adjust based on detected patterns
            if (lower_text.find("unreal engine") != std::string::npos) {
                optimization_potential += 0.2; // Unreal has good optimization tools
            }
            
            if (lower_text.find("ray tracing") != std::string::npos) {
                optimization_potential += 0.15; // Ray tracing has high optimization potential
            }
            
            if (lower_text.find("4k") != std::string::npos) {
                optimization_potential += 0.1; // High resolution has optimization potential
            }
            
            if (lower_text.find("vr") != std::string::npos) {
                optimization_potential += 0.1; // VR has specific optimization needs
            }
        }
        
        return std::min(1.0, std::max(0.0, optimization_potential));
        
    } catch (const std::exception& e) {
        spdlog::error("Error estimating optimization potential: {}", e.what());
        return 0.5;
    }
}

nlohmann::json uevr_context7::Context7Database::analyzeResourceRequirements(const std::string& text) {
    try {
        nlohmann::json requirements = {
            {"gpu_requirements", "Unknown"},
            {"cpu_requirements", "Unknown"},
            {"ram_requirements", "Unknown"},
            {"storage_requirements", "Unknown"}
        };
        
        std::string lower_text = text;
        std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
        
        // Analyze GPU requirements
        if (lower_text.find("ray tracing") != std::string::npos) {
            requirements["gpu_requirements"] = "High (RTX 3000+ or equivalent)";
        } else if (lower_text.find("4k") != std::string::npos) {
            requirements["gpu_requirements"] = "Medium-High (GTX 1070+ or equivalent)";
        } else if (lower_text.find("1080p") != std::string::npos) {
            requirements["gpu_requirements"] = "Medium (GTX 1060+ or equivalent)";
        }
        
        // Analyze CPU requirements
        if (lower_text.find("open world") != std::string::npos ||
            lower_text.find("sandbox") != std::string::npos) {
            requirements["cpu_requirements"] = "High (6+ cores recommended)";
        } else if (lower_text.find("multiplayer") != std::string::npos) {
            requirements["cpu_requirements"] = "Medium-High (4+ cores recommended)";
        }
        
        // Analyze RAM requirements
        if (lower_text.find("4k") != std::string::npos ||
            lower_text.find("ray tracing") != std::string::npos) {
            requirements["ram_requirements"] = "16GB+ recommended";
        } else {
            requirements["ram_requirements"] = "8GB+ recommended";
        }
        
        // Analyze storage requirements
        if (lower_text.find("open world") != std::string::npos ||
            lower_text.find("large") != std::string::npos) {
            requirements["storage_requirements"] = "50GB+";
        } else {
            requirements["storage_requirements"] = "20GB+";
        }
        
        return requirements;
        
    } catch (const std::exception& e) {
        spdlog::error("Error analyzing resource requirements: {}", e.what());
        return nlohmann::json::object();
    }
}

nlohmann::json uevr_context7::Context7Database::identifyPerformanceBottlenecks(const std::string& text) {
    try {
        nlohmann::json bottlenecks = nlohmann::json::array();
        
        std::string lower_text = text;
        std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
        
        // Identify common performance bottlenecks
        if (lower_text.find("ray tracing") != std::string::npos) {
            bottlenecks.push_back({
                {"type", "rendering"},
                {"severity", "high"},
                {"description", "Ray tracing can be GPU-intensive and cause performance bottlenecks"},
                {"mitigation", "Implement ray tracing optimization techniques and fallback rendering"}
            });
        }
        
        if (lower_text.find("4k") != std::string::npos) {
            bottlenecks.push_back({
                {"type", "resolution"},
                {"severity", "medium"},
                {"description", "4K resolution significantly increases GPU load"},
                {"mitigation", "Implement dynamic resolution scaling and LOD systems"}
            });
        }
        
        if (lower_text.find("open world") != std::string::npos) {
            bottlenecks.push_back({
                {"type", "cpu"},
                {"severity", "medium"},
                {"description", "Open world games can be CPU-intensive due to AI and physics"},
                {"mitigation", "Optimize AI systems and implement efficient physics simulation"}
            });
        }
        
        if (lower_text.find("vr") != std::string::npos) {
            bottlenecks.push_back({
                {"type", "vr_specific"},
                {"severity", "high"},
                {"description", "VR requires consistent 90+ FPS for comfort"},
                {"mitigation", "Implement VR-specific optimizations and maintain high frame rates"}
            });
        }
        
        return bottlenecks;
        
    } catch (const std::exception& e) {
        spdlog::error("Error identifying performance bottlenecks: {}", e.what());
        return nlohmann::json::array();
    }
}

// VR compatibility helper functions
double uevr_context7::Context7Database::calculateVRComfortRating(const nlohmann::json& game_data) {
    try {
        double comfort_rating = 0.5; // Base comfort rating
        
        if (game_data.contains("sources")) {
            std::string combined_text = "";
            for (const auto& source : game_data["sources"]) {
                if (source.contains("raw_data")) {
                    combined_text += source["raw_data"].get<std::string>();
                }
            }
            
            std::string lower_text = combined_text;
            std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
            
            // Adjust comfort rating based on game characteristics
            if (lower_text.find("first-person") != std::string::npos) {
                comfort_rating += 0.1; // FPS games generally work well in VR
            }
            
            if (lower_text.find("racing") != std::string::npos ||
                lower_text.find("driving") != std::string::npos) {
                comfort_rating += 0.15; // Racing games are comfortable in VR
            }
            
            if (lower_text.find("horror") != std::string::npos ||
                lower_text.find("scary") != std::string::npos) {
                comfort_rating -= 0.1; // Horror games can be uncomfortable in VR
            }
            
            if (lower_text.find("fast-paced") != std::string::npos ||
                lower_text.find("action") != std::string::npos) {
                comfort_rating -= 0.05; // Fast action can cause motion sickness
            }
        }
        
        return std::min(1.0, std::max(0.0, comfort_rating));
        
    } catch (const std::exception& e) {
        spdlog::error("Error calculating VR comfort rating: {}", e.what());
        return 0.5;
    }
}

nlohmann::json uevr_context7::Context7Database::identifyCompatibilityFactors(const nlohmann::json& game_data) {
    try {
        nlohmann::json factors = nlohmann::json::array();
        
        if (game_data.contains("sources")) {
            std::string combined_text = "";
            for (const auto& source : game_data["sources"]) {
                if (source.contains("raw_data")) {
                    combined_text += source["raw_data"].get<std::string>();
                }
            }
            
            std::string lower_text = combined_text;
            std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
            
            // Identify positive compatibility factors
            if (lower_text.find("unreal engine") != std::string::npos) {
                factors.push_back({
                    {"type", "positive"},
                    {"factor", "Unreal Engine has excellent VR support"},
                    {"impact", "high"}
                });
            }
            
            if (lower_text.find("first-person") != std::string::npos) {
                factors.push_back({
                    {"type", "positive"},
                    {"factor", "First-person perspective works well in VR"},
                    {"impact", "medium"}
                });
            }
            
            if (lower_text.find("vr") != std::string::npos ||
                lower_text.find("virtual reality") != std::string::npos) {
                factors.push_back({
                    {"type", "positive"},
                    {"factor", "Game already has VR support or was designed with VR in mind"},
                    {"impact", "high"}
                });
            }
            
            // Identify negative compatibility factors
            if (lower_text.find("third-person") != std::string::npos) {
                factors.push_back({
                    {"type", "negative"},
                    {"factor", "Third-person perspective may need camera adjustments for VR"},
                    {"impact", "medium"}
                });
            }
            
            if (lower_text.find("2d") != std::string::npos ||
                lower_text.find("side-scrolling") != std::string::npos) {
                factors.push_back({
                    {"type", "negative"},
                    {"factor", "2D games may not benefit from VR immersion"},
                    {"impact", "low"}
                });
            }
        }
        
        return factors;
        
    } catch (const std::exception& e) {
        spdlog::error("Error identifying compatibility factors: {}", e.what());
        return nlohmann::json::array();
    }
}

nlohmann::json uevr_context7::Context7Database::identifyPotentialIssues(const nlohmann::json& game_data) {
    try {
        nlohmann::json issues = nlohmann::json::array();
        
        if (game_data.contains("sources")) {
            std::string combined_text = "";
            for (const auto& source : game_data["sources"]) {
                if (source.contains("raw_data")) {
                    combined_text += source["raw_data"].get<std::string>();
                }
            }
            
            std::string lower_text = combined_text;
            std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
            
            // Identify potential VR issues
            if (lower_text.find("motion sickness") != std::string::npos ||
                lower_text.find("nausea") != std::string::npos) {
                issues.push_back({
                    {"type", "comfort"},
                    {"severity", "high"},
                    {"description", "Game may cause motion sickness in VR"},
                    {"mitigation", "Implement comfort options and smooth locomotion"}
                });
            }
            
            if (lower_text.find("fast camera") != std::string::npos ||
                lower_text.find("rapid movement") != std::string::npos) {
                issues.push_back({
                    {"type", "comfort"},
                    {"severity", "medium"},
                    {"description", "Fast camera movements can be disorienting in VR"},
                    {"mitigation", "Implement VR-specific camera smoothing and comfort options"}
                });
            }
            
            if (lower_text.find("complex ui") != std::string::npos ||
                lower_text.find("small text") != std::string::npos) {
                issues.push_back({
                    {"type", "usability"},
                    {"severity", "medium"},
                    {"description", "Complex UI elements may be difficult to read in VR"},
                    {"mitigation", "Implement VR-optimized UI scaling and layout"}
                });
            }
        }
        
        return issues;
        
    } catch (const std::exception& e) {
        spdlog::error("Error identifying potential issues: {}", e.what());
        return nlohmann::json::array();
    }
}

nlohmann::json uevr_context7::Context7Database::generateOptimizationRecommendations(const nlohmann::json& game_data) {
    try {
        nlohmann::json recommendations = nlohmann::json::array();
        
        if (game_data.contains("sources")) {
            std::string combined_text = "";
            for (const auto& source : game_data["sources"]) {
                if (source.contains("raw_data")) {
                    combined_text += source["raw_data"].get<std::string>();
                }
            }
            
            std::string lower_text = combined_text;
            std::transform(lower_text.begin(), lower_text.end(), lower_text.begin(), ::tolower);
            
            // Generate engine-specific recommendations
            if (lower_text.find("unreal engine") != std::string::npos) {
                recommendations.push_back({
                    {"category", "engine_specific"},
                    {"recommendation", "Use Unreal Engine's built-in VR rendering optimizations"},
                    {"priority", "high"}
                });
                
                recommendations.push_back({
                    {"category", "engine_specific"},
                    {"recommendation", "Implement VR-specific input mappings using Unreal's input system"},
                    {"priority", "medium"}
                });
            }
            
            if (lower_text.find("unity") != std::string::npos) {
                recommendations.push_back({
                    {"category", "engine_specific"},
                    {"recommendation", "Use Unity's XR Interaction Toolkit for VR input handling"},
                    {"priority", "high"}
                });
                
                recommendations.push_back({
                    {"category", "engine_specific"},
                    {"recommendation", "Implement Unity's Single Pass Instanced rendering for VR performance"},
                    {"priority", "high"}
                });
            }
            
            // Generate general VR optimization recommendations
            recommendations.push_back({
                {"category", "performance"},
                {"recommendation", "Maintain consistent 90+ FPS for VR comfort"},
                {"priority", "critical"}
            });
            
            recommendations.push_back({
                {"category", "rendering"},
                {"recommendation", "Implement efficient stereo rendering pipeline"},
                {"priority", "high"}
            });
            
            recommendations.push_back({
                {"category", "input"},
                {"recommendation", "Add VR controller support and gesture recognition"},
                {"priority", "medium"}
            });
            
            recommendations.push_back({
                {"category", "audio"},
                {"recommendation", "Implement 3D spatial audio for immersive VR experience"},
                {"priority", "medium"}
            });
        }
        
        return recommendations;
        
    } catch (const std::exception& e) {
        spdlog::error("Error generating optimization recommendations: {}", e.what());
        return nlohmann::json::array();
    }
}

nlohmann::json uevr_context7::Context7Database::generateEngineSpecificRecommendations(const std::string& engine) {
    try {
        nlohmann::json recommendations = nlohmann::json::array();
        
        if (engine == "Unreal Engine") {
            recommendations.push_back({
                {"category", "engine_specific"},
                {"recommendation", "Use Unreal Engine's VR Preview mode for testing"},
                {"priority", "high"}
            });
            
            recommendations.push_back({
                {"category", "engine_specific"},
                {"recommendation", "Implement VR-specific input mappings using Unreal's input system"},
                {"priority", "medium"}
            });
            
        } else if (engine == "Unity") {
            recommendations.push_back({
                {"category", "engine_specific"},
                {"recommendation", "Use Unity's XR Interaction Toolkit for VR input handling"},
                {"priority", "high"}
            });
            
            recommendations.push_back({
                {"category", "engine_specific"},
                {"recommendation", "Implement Unity's Single Pass Instanced rendering for VR performance"},
                {"priority", "high"}
            });
            
        } else if (engine == "REDengine") {
            recommendations.push_back({
                {"category", "engine_specific"},
                {"recommendation", "REDengine may require custom VR rendering pipeline implementation"},
                {"priority", "high"}
            });
            
        } else if (engine == "MT Framework") {
            recommendations.push_back({
                {"category", "engine_specific"},
                {"recommendation", "MT Framework may need custom VR integration due to limited VR support"},
                {"priority", "high"}
            });
        }
        
        return recommendations;
        
    } catch (const std::exception& e) {
        spdlog::error("Error generating engine-specific recommendations: {}", e.what());
        return nlohmann::json::array();
    }
}

// Database management functions
void uevr_context7::Context7Database::clearAllData() {
    try {
        spdlog::info("Clearing all Context7 database data");
        
        // Clear all data structures
        m_games_data.clear();
        m_engines_data.clear();
        m_analyses_data.clear();
        m_stores_data.clear();
        m_wikis_data.clear();
        m_vr_compatibility_data.clear();
        
        // Reset counters
        m_total_games = 0;
        m_total_engines = 0;
        m_total_analyses = 0;
        m_total_stores = 0;
        m_total_wikis = 0;
        m_total_vr_entries = 0;
        
        // Save empty data
        saveAllData();
        
        spdlog::info("All Context7 database data cleared successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("Error clearing database data: {}", e.what());
    }
}

nlohmann::json uevr_context7::Context7Database::getDatabaseStatistics() {
    try {
        nlohmann::json stats = {
            {"total_games", m_total_games},
            {"total_engines", m_total_engines},
            {"total_analyses", m_total_analyses},
            {"total_stores", m_total_stores},
            {"total_wikis", m_total_wikis},
            {"total_vr_entries", m_total_vr_entries},
            {"last_updated", std::chrono::system_clock::now().time_since_epoch().count()},
            {"database_size_mb", calculateDatabaseSize()}
        };
        
        return stats;
        
    } catch (const std::exception& e) {
        spdlog::error("Error getting database statistics: {}", e.what());
        return nlohmann::json::object();
    }
}

double uevr_context7::Context7Database::calculateDatabaseSize() {
    try {
        double total_size = 0.0;
        
        // Calculate size of data structures (rough estimate)
        total_size += m_games_data.size() * 0.001;      // ~1KB per game
        total_size += m_engines_data.size() * 0.0005;   // ~0.5KB per engine
        total_size += m_analyses_data.size() * 0.002;   // ~2KB per analysis
        total_size += m_stores_data.size() * 0.0003;    // ~0.3KB per store
        total_size += m_wikis_data.size() * 0.0003;     // ~0.3KB per wiki
        total_size += m_vr_compatibility_data.size() * 0.001; // ~1KB per VR entry
        
        return total_size;
        
    } catch (const std::exception& e) {
        spdlog::error("Error calculating database size: {}", e.what());
        return 0.0;
    }
}

} // namespace uevr_context7
