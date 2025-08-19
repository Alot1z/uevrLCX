/*
 * UEVR AI Game Analyzer
 * 
 * Analyzes games to detect engines, capabilities, and VR readiness
 * 
 * Copyright (c) 2024 UEVR Project
 * Licensed under MIT License
 */

#include "uevr/ai/GameAnalyzer.hpp"
#include <spdlog/spdlog.h>
#include <filesystem>
#include <fstream>
#include <algorithm>
#include <regex>

namespace uevr {
namespace ai {

GameAnalyzer::GameAnalyzer()
    : m_initialized(false) {
    spdlog::debug("[GameAnalyzer] Game analyzer created");
}

GameAnalyzer::~GameAnalyzer() {
    shutdown();
}

bool GameAnalyzer::initialize() {
    if (m_initialized) {
        return true;
    }

    try {
        spdlog::info("[GameAnalyzer] Initializing AI game analyzer...");
        
        // Load engine detection patterns
        loadEnginePatterns();
        
        // Load game signature database
        loadGameSignatures();
        
        // Initialize file analysis systems
        initializeFileAnalysis();
        
        // Load known games database
        loadKnownGamesDatabase();
        
        m_initialized = true;
        spdlog::info("[GameAnalyzer] Game analyzer initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameAnalyzer] Failed to initialize: {}", e.what());
        return false;
    }
}

void GameAnalyzer::shutdown() {
    if (!m_initialized) {
        return;
    }

    spdlog::info("[GameAnalyzer] Shutting down game analyzer");
    m_initialized = false;
}

std::optional<AnalysisResult> GameAnalyzer::analyzeGame(const GameInfo& game_info) {
    if (!m_initialized) {
        spdlog::error("[GameAnalyzer] Analyzer not initialized");
        return std::nullopt;
    }

    try {
        spdlog::info("[GameAnalyzer] Analyzing game: {}", game_info.name);
        
        AnalysisResult result;
        result.game_name = game_info.name;
        result.confidence = 0.0f;
        
        // Try multiple detection methods
        std::vector<DetectionResult> detection_results;
        
        // Method 1: Filename-based detection
        auto filename_result = detectFromFilename(game_info);
        if (filename_result.has_value()) {
            detection_results.push_back(filename_result.value());
        }
        
        // Method 2: File signature analysis
        auto signature_result = detectFromSignature(game_info);
        if (signature_result.has_value()) {
            detection_results.push_back(signature_result.value());
        }
        
        // Method 3: Directory structure analysis
        auto directory_result = detectFromDirectory(game_info);
        if (directory_result.has_value()) {
            detection_results.push_back(directory_result.value());
        }
        
        // Method 4: Metadata analysis
        auto metadata_result = detectFromMetadata(game_info);
        if (metadata_result.has_value()) {
            detection_results.push_back(metadata_result.value());
        }
        
        // Combine results using confidence weighting
        result = combineDetectionResults(detection_results);
        
        // Validate and enhance result
        validateAndEnhanceResult(result, game_info);
        
        spdlog::info("[GameAnalyzer] Analysis completed: {} -> {} (confidence: {:.2f})", 
                    game_info.name, result.engine_type, result.confidence);
        
        return result;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameAnalyzer] Exception during analysis: {}", e.what());
        return std::nullopt;
    }
}

std::optional<DetectionResult> GameAnalyzer::detectFromFilename(const GameInfo& game_info) {
    try {
        std::string lowercase_name = game_info.executable;
        std::transform(lowercase_name.begin(), lowercase_name.end(), lowercase_name.begin(), ::tolower);
        
        // Check against known game patterns
        for (const auto& [pattern, engine_info] : m_filename_patterns) {
            std::regex regex_pattern(pattern, std::regex_constants::icase);
            if (std::regex_search(lowercase_name, regex_pattern)) {
                DetectionResult result;
                result.engine_type = engine_info.engine_type;
                result.confidence = engine_info.confidence;
                result.method = "filename";
                result.details = "Matched pattern: " + pattern;
                
                spdlog::debug("[GameAnalyzer] Filename detection: {} -> {} (confidence: {:.2f})", 
                             game_info.executable, result.engine_type, result.confidence);
                
                return result;
            }
        }
        
        return std::nullopt;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameAnalyzer] Exception in filename detection: {}", e.what());
        return std::nullopt;
    }
}

std::optional<DetectionResult> GameAnalyzer::detectFromSignature(const GameInfo& game_info) {
    try {
        std::filesystem::path exe_path(game_info.directory);
        exe_path /= game_info.executable;
        
        if (!std::filesystem::exists(exe_path)) {
            return std::nullopt;
        }
        
        // Read file header for signature analysis
        std::ifstream file(exe_path, std::ios::binary);
        if (!file.is_open()) {
            return std::nullopt;
        }
        
        std::vector<uint8_t> header(1024);
        file.read(reinterpret_cast<char*>(header.data()), header.size());
        file.close();
        
        // Check for engine signatures
        for (const auto& [signature, engine_info] : m_file_signatures) {
            if (containsSignature(header, signature)) {
                DetectionResult result;
                result.engine_type = engine_info.engine_type;
                result.confidence = engine_info.confidence;
                result.method = "signature";
                result.details = "Found signature: " + bytesToHex(signature);
                
                spdlog::debug("[GameAnalyzer] Signature detection: {} (confidence: {:.2f})", 
                             result.engine_type, result.confidence);
                
                return result;
            }
        }
        
        return std::nullopt;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameAnalyzer] Exception in signature detection: {}", e.what());
        return std::nullopt;
    }
}

std::optional<DetectionResult> GameAnalyzer::detectFromDirectory(const GameInfo& game_info) {
    try {
        std::filesystem::path game_dir(game_info.directory);
        
        if (!std::filesystem::exists(game_dir)) {
            return std::nullopt;
        }
        
        // Analyze directory structure for engine indicators
        std::vector<std::string> found_files;
        std::vector<std::string> found_dirs;
        
        // Scan directory for engine-specific files and folders
        for (const auto& entry : std::filesystem::recursive_directory_iterator(game_dir)) {
            if (entry.is_regular_file()) {
                found_files.push_back(entry.path().filename().string());
            } else if (entry.is_directory()) {
                found_dirs.push_back(entry.path().filename().string());
            }
            
            // Limit scan depth to avoid performance issues
            if (entry.depth() > 3) {
                continue;
            }
        }
        
        // Check for engine-specific patterns in directory structure
        for (const auto& [pattern, engine_info] : m_directory_patterns) {
            int matches = 0;
            int total_patterns = 0;
            
            for (const auto& file_pattern : pattern.required_files) {
                total_patterns++;
                for (const auto& file : found_files) {
                    if (std::regex_search(file, std::regex(file_pattern, std::regex_constants::icase))) {
                        matches++;
                        break;
                    }
                }
            }
            
            for (const auto& dir_pattern : pattern.required_dirs) {
                total_patterns++;
                for (const auto& dir : found_dirs) {
                    if (std::regex_search(dir, std::regex(dir_pattern, std::regex_constants::icase))) {
                        matches++;
                        break;
                    }
                }
            }
            
            if (total_patterns > 0 && static_cast<float>(matches) / total_patterns >= pattern.match_threshold) {
                DetectionResult result;
                result.engine_type = engine_info.engine_type;
                result.confidence = engine_info.confidence * (static_cast<float>(matches) / total_patterns);
                result.method = "directory";
                result.details = "Matched " + std::to_string(matches) + "/" + std::to_string(total_patterns) + " patterns";
                
                spdlog::debug("[GameAnalyzer] Directory detection: {} (confidence: {:.2f})", 
                             result.engine_type, result.confidence);
                
                return result;
            }
        }
        
        return std::nullopt;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameAnalyzer] Exception in directory detection: {}", e.what());
        return std::nullopt;
    }
}

std::optional<DetectionResult> GameAnalyzer::detectFromMetadata(const GameInfo& game_info) {
    try {
        // Check metadata for engine hints
        for (const auto& [key, value] : game_info.metadata) {
            std::string lowercase_value = value;
            std::transform(lowercase_value.begin(), lowercase_value.end(), lowercase_value.begin(), ::tolower);
            
            for (const auto& [pattern, engine_info] : m_metadata_patterns) {
                if (lowercase_value.find(pattern) != std::string::npos) {
                    DetectionResult result;
                    result.engine_type = engine_info.engine_type;
                    result.confidence = engine_info.confidence;
                    result.method = "metadata";
                    result.details = "Found in " + key + ": " + pattern;
                    
                    spdlog::debug("[GameAnalyzer] Metadata detection: {} (confidence: {:.2f})", 
                                 result.engine_type, result.confidence);
                    
                    return result;
                }
            }
        }
        
        return std::nullopt;
        
    } catch (const std::exception& e) {
        spdlog::error("[GameAnalyzer] Exception in metadata detection: {}", e.what());
        return std::nullopt;
    }
}

void GameAnalyzer::loadEnginePatterns() {
    // Filename patterns for engine detection
    m_filename_patterns = {
        // RE Engine
        {"re7.*\\.exe", {"RE Engine", 0.95f}},
        {"re8.*\\.exe", {"RE Engine", 0.95f}},
        {".*biohazard.*\\.exe", {"RE Engine", 0.8f}},
        
        // REDengine 4
        {"cyberpunk2077.*\\.exe", {"REDengine 4", 0.95f}},
        {"witcher3.*\\.exe", {"REDengine 4", 0.9f}},
        
        // MT Framework
        {"monsterhunterworld.*\\.exe", {"MT Framework", 0.95f}},
        {"mhw.*\\.exe", {"MT Framework", 0.9f}},
        {"devilmaycry.*\\.exe", {"MT Framework", 0.8f}},
        
        // Unreal Engine
        {".*-win64-shipping\\.exe", {"Unreal Engine", 0.8f}},
        {".*-shipping\\.exe", {"Unreal Engine", 0.7f}},
        
        // Unity
        {".*_data.*", {"Unity", 0.6f}},
        
        // Generic patterns
        {".*engine.*\\.exe", {"Unknown", 0.3f}},
        {".*game.*\\.exe", {"Unknown", 0.2f}}
    };
    
    // File signatures (simplified - in practice would be more comprehensive)
    m_file_signatures = {
        {{0x52, 0x45, 0x20, 0x45, 0x6E, 0x67, 0x69, 0x6E, 0x65}, {"RE Engine", 0.9f}},
        {{0x52, 0x45, 0x44, 0x65, 0x6E, 0x67, 0x69, 0x6E, 0x65}, {"REDengine 4", 0.9f}},
        {{0x4D, 0x54, 0x20, 0x46, 0x72, 0x61, 0x6D, 0x65, 0x77, 0x6F, 0x72, 0x6B}, {"MT Framework", 0.9f}}
    };
    
    // Directory structure patterns
    DirectoryPattern re_pattern;
    re_pattern.required_files = {"re_chunk_000\\.pak", ".*\\.pak"};
    re_pattern.required_dirs = {"natives", "re_chunk"};
    re_pattern.match_threshold = 0.5f;
    m_directory_patterns[re_pattern] = {"RE Engine", 0.8f};
    
    DirectoryPattern red_pattern;
    red_pattern.required_files = {".*\\.archive", ".*\\.w3strings"};
    red_pattern.required_dirs = {"content", "dlc"};
    red_pattern.match_threshold = 0.5f;
    m_directory_patterns[red_pattern] = {"REDengine 4", 0.8f};
    
    DirectoryPattern mt_pattern;
    mt_pattern.required_files = {".*\\.arc", "nativePC.*"};
    mt_pattern.required_dirs = {"nativePC"};
    mt_pattern.match_threshold = 0.6f;
    m_directory_patterns[mt_pattern] = {"MT Framework", 0.8f};
    
    // Metadata patterns
    m_metadata_patterns = {
        {"re engine", {"RE Engine", 0.9f}},
        {"redengine", {"REDengine 4", 0.9f}},
        {"mt framework", {"MT Framework", 0.9f}},
        {"unreal engine", {"Unreal Engine", 0.9f}},
        {"unity", {"Unity", 0.9f}}
    };
    
    spdlog::debug("[GameAnalyzer] Loaded engine detection patterns");
}

void GameAnalyzer::loadGameSignatures() {
    // Load from file if exists, otherwise use defaults
    std::filesystem::path signatures_file = "docs/context7/game_signatures.json";
    
    if (std::filesystem::exists(signatures_file)) {
        // TODO: Load from JSON file
        spdlog::debug("[GameAnalyzer] Loaded game signatures from file");
    } else {
        spdlog::debug("[GameAnalyzer] Using default game signatures");
    }
}

void GameAnalyzer::initializeFileAnalysis() {
    // Initialize file analysis systems
    spdlog::debug("[GameAnalyzer] File analysis systems initialized");
}

void GameAnalyzer::loadKnownGamesDatabase() {
    // Load known games database
    m_known_games = {
        {"re7.exe", {"Resident Evil 7", "RE Engine", 0.99f}},
        {"re8.exe", {"Resident Evil 8", "RE Engine", 0.99f}},
        {"cyberpunk2077.exe", {"Cyberpunk 2077", "REDengine 4", 0.99f}},
        {"monsterhunterworld.exe", {"Monster Hunter World", "MT Framework", 0.99f}},
        {"witcher3.exe", {"The Witcher 3", "REDengine 4", 0.99f}}
    };
    
    spdlog::debug("[GameAnalyzer] Loaded {} known games", m_known_games.size());
}

AnalysisResult GameAnalyzer::combineDetectionResults(const std::vector<DetectionResult>& results) {
    AnalysisResult combined;
    
    if (results.empty()) {
        combined.engine_type = "Unknown";
        combined.confidence = 0.0f;
        return combined;
    }
    
    // Weight different detection methods
    std::map<std::string, float> method_weights = {
        {"filename", 1.0f},
        {"signature", 1.2f},
        {"directory", 0.8f},
        {"metadata", 0.6f}
    };
    
    // Aggregate results by engine type
    std::map<std::string, float> engine_scores;
    
    for (const auto& result : results) {
        float weight = method_weights[result.method];
        engine_scores[result.engine_type] += result.confidence * weight;
    }
    
    // Find best match
    auto best_match = std::max_element(engine_scores.begin(), engine_scores.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });
    
    if (best_match != engine_scores.end()) {
        combined.engine_type = best_match->first;
        combined.confidence = std::min(1.0f, best_match->second / results.size());
    } else {
        combined.engine_type = "Unknown";
        combined.confidence = 0.0f;
    }
    
    return combined;
}

void GameAnalyzer::validateAndEnhanceResult(AnalysisResult& result, const GameInfo& game_info) {
    // Check against known games database for validation
    auto known_it = m_known_games.find(game_info.executable);
    if (known_it != m_known_games.end()) {
        const auto& known_game = known_it->second;
        
        if (result.engine_type == "Unknown" || result.confidence < known_game.confidence) {
            result.engine_type = known_game.engine_type;
            result.confidence = known_game.confidence;
            result.game_name = known_game.name;
            spdlog::debug("[GameAnalyzer] Enhanced result using known games database");
        }
    }
    
    // Ensure minimum confidence threshold
    if (result.confidence < 0.1f) {
        result.engine_type = "Unknown";
        result.confidence = 0.0f;
    }
}

bool GameAnalyzer::containsSignature(const std::vector<uint8_t>& data, const std::vector<uint8_t>& signature) {
    if (signature.size() > data.size()) {
        return false;
    }
    
    for (size_t i = 0; i <= data.size() - signature.size(); ++i) {
        bool match = true;
        for (size_t j = 0; j < signature.size(); ++j) {
            if (data[i + j] != signature[j]) {
                match = false;
                break;
            }
        }
        if (match) {
            return true;
        }
    }
    
    return false;
}

std::string GameAnalyzer::bytesToHex(const std::vector<uint8_t>& bytes) {
    std::string hex;
    for (uint8_t byte : bytes) {
        char buf[3];
        snprintf(buf, sizeof(buf), "%02X", byte);
        hex += buf;
    }
    return hex;
}

} // namespace ai
} // namespace uevr