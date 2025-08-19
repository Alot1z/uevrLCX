#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <future>
#include <chrono>

namespace uevr {
namespace deepwiki {

/**
 * DeepWiki Game Data Structure
 * Contains comprehensive game information from DeepWiki
 */
struct DeepWikiGameData {
    std::string game_name;
    std::string engine_name;
    std::string engine_version;
    std::string release_date;
    std::string developer;
    std::string publisher;
    
    // Technical specifications
    std::vector<std::string> supported_platforms;
    std::vector<std::string> graphics_apis;
    std::vector<std::string> rendering_features;
    
    // VR compatibility data
    bool has_vr_support;
    std::string vr_runtime;
    std::vector<std::string> vr_features;
    float vr_compatibility_score;
    
    // Performance metrics
    std::map<std::string, float> performance_metrics;
    std::vector<std::string> optimization_tips;
    
    // Engine-specific data
    std::map<std::string, std::string> engine_config;
    std::vector<std::string> known_issues;
    std::vector<std::string> workarounds;
    
    // Timestamp
    std::chrono::system_clock::time_point last_updated;
};

/**
 * DeepWiki Connection Status
 */
enum class ConnectionStatus {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    ERROR,
    RATE_LIMITED
};

/**
 * DeepWiki Connector Interface
 * Provides connection and data fetching capabilities
 */
class DeepWikiConnector {
public:
    DeepWikiConnector();
    ~DeepWikiConnector();
    
    // Connection management
    bool connect(const std::string& api_key = "");
    void disconnect();
    ConnectionStatus get_status() const;
    
    // Data fetching
    std::future<DeepWikiGameData> fetch_game_data_async(const std::string& game_name);
    DeepWikiGameData fetch_game_data(const std::string& game_name);
    
    // Batch operations
    std::future<std::vector<DeepWikiGameData>> fetch_multiple_games_async(
        const std::vector<std::string>& game_names);
    
    // Search functionality
    std::vector<std::string> search_games(const std::string& query);
    std::vector<std::string> search_by_engine(const std::string& engine_name);
    
    // Rate limiting and caching
    void set_rate_limit(int requests_per_minute);
    bool is_rate_limited() const;
    std::chrono::seconds get_rate_limit_reset_time() const;
    
    // Error handling
    std::string get_last_error() const;
    void clear_errors();
    
    // Configuration
    void set_timeout(std::chrono::milliseconds timeout);
    void set_retry_attempts(int attempts);
    void enable_caching(bool enable);
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

/**
 * DeepWiki Game Data Fetcher
 * High-level interface for game data operations
 */
class GameDataFetcher {
public:
    GameDataFetcher(std::shared_ptr<DeepWikiConnector> connector);
    ~GameDataFetcher();
    
    // Game data operations
    bool update_game_database(const std::string& game_name);
    bool update_all_games();
    bool refresh_expired_data();
    
    // VR compatibility analysis
    bool analyze_vr_compatibility(const std::string& game_name);
    std::vector<std::string> get_vr_optimization_tips(const std::string& game_name);
    
    // Engine-specific data
    bool fetch_engine_documentation(const std::string& engine_name);
    std::vector<std::string> get_engine_tips(const std::string& engine_name);
    
    // Performance optimization
    bool fetch_performance_data(const std::string& game_name);
    std::map<std::string, float> get_performance_metrics(const std::string& game_name);
    
    // Batch operations
    void update_games_batch(const std::vector<std::string>& game_names);
    void analyze_vr_compatibility_batch(const std::vector<std::string>& game_names);
    
    // Status and monitoring
    bool is_operation_complete(const std::string& operation_id) const;
    float get_operation_progress(const std::string& operation_id) const;
    std::vector<std::string> get_active_operations() const;
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
    std::shared_ptr<DeepWikiConnector> m_connector;
};

} // namespace deepwiki
} // namespace uevr
