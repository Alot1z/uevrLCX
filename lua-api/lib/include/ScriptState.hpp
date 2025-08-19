#pragma once

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <atomic>
#include <unordered_map>
#include <mutex>

#include <sol/sol.hpp>
#include <uevr/API.hpp>

#include <vector>

#include "ScriptContext.hpp"

namespace uevr {
class ScriptState {
public:
    enum class GarbageCollectionHandler : uint32_t {
        UEVR_MANAGED = 0,
        LUA_MANAGED = 1,
        LAST
    };

    enum class GarbageCollectionType : uint32_t {
        STEP = 0,
        FULL = 1,
        LAST
    };

    enum class GarbageCollectionMode : uint32_t {
        GENERATIONAL = 0,
        INCREMENTAL = 1,
        LAST
    };

    struct GarbageCollectionData {
        GarbageCollectionHandler gc_handler{GarbageCollectionHandler::UEVR_MANAGED};
        GarbageCollectionType gc_type{GarbageCollectionType::FULL};
        GarbageCollectionMode gc_mode{GarbageCollectionMode::GENERATIONAL};
        std::chrono::microseconds gc_budget{1000};

        uint32_t gc_minor_multiplier{1};
        uint32_t gc_major_multiplier{100};
    };

    ScriptState(const GarbageCollectionData& gc_data, UEVR_PluginInitializeParam* param, bool is_main_state);
    ~ScriptState();

    void run_script(const std::string& p);
    sol::protected_function_result handle_protected_result(sol::protected_function_result result); // because protected_functions don't throw

    void gc_data_changed(GarbageCollectionData data);
    void on_frame();
    void on_draw_ui();
    void on_script_reset();
    void dispatch_event(std::string_view event_name, std::string_view event_data);

    auto& context() {
        return m_context;
    }

    std::optional<ScriptContext::ScriptErrorState> get_last_script_error() const {
        if (m_context != nullptr) {
            return m_context->get_last_script_error();
        }

        return std::nullopt;
    }

    auto& lua() { return m_lua; }

    // Multi-state management
    uint32_t get_state_id() const { return m_state_id; }
    std::string get_state_namespace() const { return "uevr_state_" + std::to_string(m_state_id); }
    bool is_main_state() const { return m_is_main_state; }
    
    // State isolation and management
    void isolate_state();
    void restore_state();
    void clear_state_environment();
    
    // State communication (for inter-state messaging)
    void send_message_to_state(uint32_t target_state_id, const std::string& message);
    std::vector<std::string> get_pending_messages() const;
    
    // State statistics and monitoring
    size_t get_memory_usage() const;
    size_t get_script_count() const;
    std::chrono::steady_clock::time_point get_last_activity() const;

private:
    std::shared_ptr<sol::state> m_lua_impl{std::make_shared<sol::state>()};
    sol::state& m_lua{*m_lua_impl.get()};
    std::shared_ptr<ScriptContext> m_context{nullptr};

    GarbageCollectionData m_gc_data{};
    bool m_is_main_state;
    
    // Multi-state support
    uint32_t m_state_id{0};
    std::string m_state_namespace;
    
    // State isolation
    sol::table m_original_uevr_table;
    std::string m_original_package_path;
    std::string m_original_package_cpath;
    sol::table m_original_package_searchers;
    
    // State communication
    std::vector<std::string> m_incoming_messages;
    std::vector<std::string> m_outgoing_messages;
    mutable std::mutex m_message_mutex;
    
    // State monitoring
    size_t m_script_count{0};
    std::chrono::steady_clock::time_point m_last_activity;
    size_t m_memory_usage{0};
    
    // Helper functions
    uint32_t generate_unique_state_id();
    void initialize_state_environment();
    void cleanup_state_environment();
    void update_memory_usage();
    void log_state_activity();
    
    // Static state management
    static std::atomic<uint32_t> s_next_state_id;
    static std::unordered_map<uint32_t, ScriptState*> s_active_states;
    static std::mutex s_states_mutex;
    
    static void register_state(ScriptState* state);
    static void unregister_state(ScriptState* state);
    static ScriptState* find_state(uint32_t state_id);
};
}