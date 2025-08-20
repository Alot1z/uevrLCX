#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <future>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

namespace UEVR {
namespace Context7 {

// Forward declarations
class QueryAnalyzer;
class ContentAnalyzer;
class RelevanceCalculator;
class SuggestionGenerator;
class KnowledgeUpdater;

// DeepWiki AI configuration
struct DeepWikiAIConfig {
    std::string api_endpoint;            // DeepWiki API endpoint
    std::string api_key;                 // API authentication key
    bool enable_intelligent_fetching;     // Enable AI-powered fetching
    bool enable_content_analysis;         // Enable content analysis
    uint32_t max_concurrent_requests;     // Maximum concurrent requests
    float request_throttle_rate;          // Request throttle rate (requests per second)
    uint32_t request_timeout_ms;          // Request timeout in milliseconds
    bool enable_caching;                  // Enable response caching
    uint32_t cache_size_mb;              // Cache size in MB
    uint32_t cache_ttl_hours;            // Cache time-to-live in hours
    bool enable_retry_on_failure;         // Enable automatic retry on failure
    uint32_t max_retry_attempts;          // Maximum retry attempts
    uint32_t retry_delay_ms;              // Delay between retries in milliseconds
};

// Query types for knowledge retrieval
enum class QueryType {
    GameInformation,           // General game information
    VRCompatibility,          // VR compatibility analysis
    PerformanceData,          // Performance benchmarks and data
    UserReviews,              // User reviews and feedback
    TechnicalSpecs,           // Technical specifications
    OptimizationTips,         // Performance optimization tips
    Troubleshooting,          // Common issues and solutions
    Custom                    // Custom query type
};

// Knowledge query structure
struct KnowledgeQuery {
    std::string query_id;                 // Unique query identifier
    std::string query_text;               // Natural language query text
    QueryType type;                       // Query type classification
    std::vector<std::string> keywords;    // Extracted query keywords
    std::string context;                  // Query context and background
    uint32_t max_results;                 // Maximum results to return
    bool enable_semantic_search;          // Enable semantic search
    bool enable_keyword_matching;         // Enable keyword matching
    std::map<std::string, std::string> filters; // Query filters
    std::chrono::system_clock::time_point created_at; // Query creation timestamp
    uint32_t priority;                    // Query priority (1-10)
    bool is_async;                        // Whether to process asynchronously
};

// Knowledge result structure
struct KnowledgeResult {
    std::string result_id;                // Unique result identifier
    std::string title;                    // Result title
    std::string content;                  // Result content
    std::string summary;                  // Content summary
    std::string source;                   // Information source
    std::string url;                      // Source URL if available
    float relevance_score;                // Relevance score (0.0-1.0)
    float confidence_score;               // Confidence score (0.0-1.0)
    float quality_score;                  // Content quality score (0.0-1.0)
    std::vector<std::string> tags;       // Content tags
    std::vector<std::string> categories; // Content categories
    std::chrono::system_clock::time_point timestamp; // Content timestamp
    std::chrono::system_clock::time_point retrieved_at; // When retrieved
    uint32_t view_count;                  // Number of times viewed
    uint32_t rating;                      // User rating (1-5)
    std::string language;                 // Content language
    bool is_verified;                     // Whether content is verified
    std::string verification_source;      // Source of verification
};

// Content analysis result
struct ContentAnalysis {
    std::string content_id;               // Content identifier
    float sentiment_score;                // Sentiment score (-1.0 to 1.0)
    std::vector<std::string> entities;   // Named entities found
    std::vector<std::string> topics;     // Identified topics
    std::vector<std::string> keywords;   // Extracted keywords
    float readability_score;              // Readability score
    std::string content_type;             // Type of content
    std::string target_audience;          // Target audience
    float technical_complexity;           // Technical complexity (0.0-1.0)
    std::vector<std::string> languages;  // Languages detected
    bool has_technical_content;           // Whether contains technical info
    bool has_user_feedback;               // Whether contains user feedback
    float factual_accuracy;               // Factual accuracy estimate
    std::vector<std::string> sources;    // Cited sources
    std::chrono::system_clock::time_point analyzed_at; // Analysis timestamp
};

// Relevance calculation parameters
struct RelevanceParams {
    float keyword_weight;                 // Weight for keyword matching
    float semantic_weight;                // Weight for semantic similarity
    float recency_weight;                 // Weight for content recency
    float source_weight;                  // Weight for source credibility
    float user_preference_weight;         // Weight for user preferences
    float content_quality_weight;         // Weight for content quality
    float category_relevance_weight;      // Weight for category relevance
    float language_preference_weight;     // Weight for language preference
    std::map<std::string, float> custom_weights; // Custom weight overrides
    float minimum_relevance_threshold;    // Minimum relevance to include
    uint32_t max_relevance_results;      // Maximum relevant results
};

// Suggestion generation parameters
struct SuggestionParams {
    bool enable_personalization;          // Enable user personalization
    bool enable_context_awareness;        // Enable context awareness
    bool enable_trend_analysis;           // Enable trend analysis
    bool enable_collaborative_filtering;  // Enable collaborative filtering
    uint32_t max_suggestions;             // Maximum suggestions to generate
    float minimum_confidence;             // Minimum confidence for suggestions
    bool include_explanation;             // Include explanation for suggestions
    std::vector<std::string> preferred_categories; // Preferred content categories
    std::vector<std::string> excluded_categories;  // Excluded content categories
    float novelty_factor;                 // Factor for suggesting new content
    float diversity_factor;               // Factor for diverse suggestions
};

// Knowledge update operation
struct KnowledgeUpdate {
    std::string update_id;                // Update identifier
    std::string operation_type;           // Type of update operation
    std::string target_content;           // Target content to update
    std::vector<std::string> changes;    // Changes made
    std::string reason;                   // Reason for update
    std::string source;                   // Source of update
    float confidence;                     // Confidence in update
    std::chrono::system_clock::time_point timestamp; // Update timestamp
    bool requires_verification;           // Whether update needs verification
    std::string verification_status;      // Current verification status
    std::vector<std::string> affected_queries; // Queries affected by update
};

// API response structure
struct APIResponse {
    bool success;                         // Whether request was successful
    uint32_t status_code;                 // HTTP status code
    std::string response_body;            // Response body content
    std::map<std::string, std::string> headers; // Response headers
    std::string error_message;            // Error message if failed
    uint32_t response_time_ms;            // Response time in milliseconds
    std::chrono::system_clock::time_point received_at; // When received
    bool was_cached;                      // Whether response was from cache
    uint32_t cache_age_seconds;           // Age of cached response
    std::string request_id;               // Request identifier for tracking
};

// Cache entry structure
struct CacheEntry {
    std::string key;                      // Cache key
    std::vector<uint8_t> data;            // Cached data
    std::chrono::system_clock::time_point created_at; // Creation timestamp
    std::chrono::system_clock::time_point last_accessed; // Last access
    uint32_t access_count;                // Number of accesses
    uint32_t size_bytes;                  // Size in bytes
    float priority_score;                 // Priority for eviction
    bool is_persistent;                   // Whether to persist to disk
    std::string checksum;                 // Data integrity checksum
    std::vector<std::string> tags;       // Cache tags for organization
};

// Main DeepWiki AI class
class DeepWikiAI {
public:
    DeepWikiAI();
    ~DeepWikiAI();
    
    // Core initialization and management
    bool initialize(const DeepWikiAIConfig& config);
    void shutdown();
    bool isInitialized() const;
    DeepWikiAIConfig getConfiguration() const;
    
    // Knowledge query operations
    std::vector<KnowledgeResult> queryKnowledge(const KnowledgeQuery& query);
    std::future<std::vector<KnowledgeResult>> queryKnowledgeAsync(const KnowledgeQuery& query);
    std::vector<KnowledgeResult> queryKnowledgeBatch(const std::vector<KnowledgeQuery>& queries);
    
    // Content analysis
    ContentAnalysis analyzeContent(const std::string& content, const std::string& context = "");
    float analyzeContentRelevance(const std::string& content, const std::string& query);
    std::vector<std::string> extractKeywords(const std::string& content);
    std::vector<std::string> extractTopics(const std::string& content);
    
    // Intelligent suggestions
    std::vector<std::string> generateSuggestions(const KnowledgeQuery& query);
    std::vector<std::string> generateSuggestions(const std::string& context);
    std::vector<KnowledgeResult> getRelatedContent(const std::string& content_id);
    std::vector<KnowledgeResult> getTrendingContent(const std::string& category = "");
    
    // Knowledge base management
    bool updateLocalKnowledge(const std::vector<KnowledgeResult>& results);
    bool updateLocalKnowledge(const KnowledgeUpdate& update);
    bool removeLocalKnowledge(const std::string& content_id);
    std::vector<KnowledgeResult> searchLocalKnowledge(const std::string& query);
    
    // Cache management
    void clearCache();
    void setCacheSize(uint32_t size_mb);
    uint32_t getCacheSize() const;
    std::map<std::string, uint32_t> getCacheStatistics() const;
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    std::map<std::string, float> getPerformanceMetrics() const;
    void resetPerformanceMetrics();
    
    // Configuration updates
    void updateConfiguration(const DeepWikiAIConfig& config);
    bool validateConfiguration(const DeepWikiAIConfig& config) const;
    
private:
    // Configuration and state
    DeepWikiAIConfig m_config;
    bool m_initialized;
    std::atomic<bool> m_shutdown_requested;
    
    // AI components
    std::unique_ptr<QueryAnalyzer> m_query_analyzer;
    std::unique_ptr<ContentAnalyzer> m_content_analyzer;
    std::unique_ptr<RelevanceCalculator> m_relevance_calculator;
    std::unique_ptr<SuggestionGenerator> m_suggestion_generator;
    std::unique_ptr<KnowledgeUpdater> m_knowledge_updater;
    
    // HTTP client and API management
    CURL* m_curl_handle;
    std::mutex m_curl_mutex;
    std::atomic<uint32_t> m_active_requests;
    std::condition_variable m_request_cv;
    
    // Caching system
    std::map<std::string, CacheEntry> m_cache;
    std::mutex m_cache_mutex;
    uint32_t m_current_cache_size;
    uint32_t m_max_cache_size;
    
    // Performance metrics
    struct PerformanceMetrics {
        uint32_t total_requests;
        uint32_t successful_requests;
        uint32_t failed_requests;
        uint32_t cached_responses;
        float average_response_time_ms;
        float cache_hit_rate;
        uint32_t concurrent_requests;
        uint32_t total_data_transferred_mb;
    };
    
    PerformanceMetrics m_performance_metrics;
    bool m_performance_monitoring_enabled;
    std::mutex m_metrics_mutex;
    
    // Helper methods
    bool initializeCurl();
    void shutdownCurl();
    bool validateQuery(const KnowledgeQuery& query) const;
    std::string buildAPIRequest(const KnowledgeQuery& query);
    APIResponse makeAPIRequest(const std::string& url, const std::string& data = "");
    bool parseAPIResponse(const APIResponse& response, std::vector<KnowledgeResult>& results);
    
    // Cache management
    void addToCache(const std::string& key, const std::vector<uint8_t>& data);
    std::vector<uint8_t> getFromCache(const std::string& key);
    void removeFromCache(const std::string& key);
    void evictCacheEntries();
    float calculateCachePriority(const CacheEntry& entry);
    
    // Performance monitoring
    void updatePerformanceMetrics(const APIResponse& response);
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
    
    // Error handling
    void logError(const std::string& error_message);
    void logWarning(const std::string& warning_message);
    void logInfo(const std::string& info_message);
    
    // Rate limiting
    bool checkRateLimit();
    void updateRateLimit();
    std::chrono::steady_clock::time_point m_last_request_time;
    uint32_t m_requests_this_second;
    
    // Thread safety
    mutable std::mutex m_system_mutex;
    std::vector<std::thread> m_worker_threads;
    std::queue<std::function<void()>> m_work_queue;
    std::condition_variable m_work_cv;
    
    void workerThreadFunction();
    void submitWork(std::function<void()> work);
};

} // namespace Context7
} // namespace UEVR
