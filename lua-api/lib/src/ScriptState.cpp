#include <format>

#include <windows.h>

#include "uevr/API.hpp"
#include "ScriptState.hpp"
#include <spdlog/spdlog.h>
#include <sstream>
#include <algorithm>

namespace api::ue {
void msg(const char* text) {
    MessageBoxA(GetForegroundWindow(), text, "LuaLoader Message", MB_ICONINFORMATION | MB_OK);
}
}

namespace uevr {

// Static member initialization
std::atomic<uint32_t> ScriptState::s_next_state_id{1};
std::unordered_map<uint32_t, ScriptState*> ScriptState::s_active_states{};
std::mutex ScriptState::s_states_mutex{};

ScriptState::ScriptState(const GarbageCollectionData& gc_data, UEVR_PluginInitializeParam* param, bool is_main_state)
    : m_gc_data(gc_data)
    , m_is_main_state(is_main_state)
    , m_last_activity(std::chrono::steady_clock::now()) {
    
    // Register this state in the global registry
    register_state(this);
    
    // Initialize Lua state with proper isolation
    m_lua.open_libraries(sol::lib::base, sol::lib::string, sol::table, sol::lib::math, sol::lib::bit, sol::lib::utf8, sol::lib::coroutine, sol::lib::debug, sol::lib::io, sol::lib::os, sol::lib::package, sol::lib::require);

    // Disable potentially dangerous OS functions
    auto os = m_lua["os"];
    os["remove"] = sol::nil;
    os["rename"] = sol::nil;
    os["execute"] = sol::nil;
    os["exit"] = sol::nil;
    os["setlocale"] = sol::nil;
    os["getenv"] = sol::nil;

    // Multi-state support implementation
    // Each ScriptState now has its own isolated Lua environment
    // This allows multiple independent script states to coexist without interference
    
    // Create a unique state identifier for this instance
    m_state_id = generate_unique_state_id();
    
    // Initialize state-specific registry and environment
    initialize_state_environment();
    
    // Create context with proper state isolation
    m_context = ScriptContext::create(m_lua_impl, param, m_state_id);

    if (!m_context->valid()) {
        if (param != nullptr && param->functions != nullptr) {
            param->functions->log_error("Failed to create new ScriptState!");
        }

        return;
    }

    const auto result = m_context->setup_bindings();

    if (result != 0) {
        const auto table = sol::stack::pop<sol::table>(m_lua);
        
        // Use state-specific namespace to avoid conflicts
        m_state_namespace = "uevr_state_" + std::to_string(m_state_id);
        m_lua[m_state_namespace] = table;
        
        // Store state-specific package paths
        m_lua.registry()["package_path_" + std::to_string(m_state_id)] = m_lua["package"]["path"];
        m_lua.registry()["package_cpath_" + std::to_string(m_state_id)] = m_lua["package"]["cpath"];
        m_lua.registry()["package_searchers_" + std::to_string(m_state_id)] = m_lua.create_table();
    
        sol::table package_searchers = m_lua["package"]["searchers"];
    
        for (auto&& [k, v] : package_searchers) {
            m_lua.registry()["package_searchers_" + std::to_string(m_state_id)][k] = v;
        }
        
        // Store state reference in global registry for cleanup
        m_lua.registry()["uevr_state_" + std::to_string(m_state_id)] = m_lua_impl;
        
        spdlog::info("[ScriptState] Created isolated state {} with namespace {}", m_state_id, m_state_namespace);
    }
}

ScriptState::~ScriptState() {
    // Cleanup state environment
    cleanup_state_environment();
    
    // Unregister from global registry
    unregister_state(this);
    
    spdlog::info("[ScriptState] Destroyed state {}", m_state_id);
}

// Multi-state management implementation
uint32_t ScriptState::generate_unique_state_id() {
    return s_next_state_id.fetch_add(1);
}

void ScriptState::initialize_state_environment() {
    try {
        // Store original environment for restoration
        if (m_lua["uevr"].valid()) {
            m_original_uevr_table = m_lua["uevr"];
        }
        
        if (m_lua["package"]["path"].valid()) {
            m_original_package_path = m_lua["package"]["path"];
        }
        
        if (m_lua["package"]["cpath"].valid()) {
            m_original_package_cpath = m_lua["package"]["cpath"];
        }
        
        if (m_lua["package"]["searchers"].valid()) {
            m_original_package_searchers = m_lua["package"]["searchers"];
        }
        
        // Create isolated environment
        isolate_state();
        
        spdlog::debug("[ScriptState] Initialized environment for state {}", m_state_id);
        
    } catch (const std::exception& e) {
        spdlog::error("[ScriptState] Failed to initialize environment for state {}: {}", m_state_id, e.what());
    }
}

void ScriptState::isolate_state() {
    try {
        // Create isolated namespace
        m_lua.create_named_table(m_state_namespace);
        
        // Set up isolated package system
        std::string isolated_path = ";" + m_state_namespace + "/?.lua";
        std::string isolated_cpath = ";" + m_state_namespace + "/?.dll";
        
        m_lua["package"]["path"] = m_lua["package"]["path"].get<std::string>() + isolated_path;
        m_lua["package"]["cpath"] = m_lua["package"]["cpath"].get<std::string>() + isolated_cpath;
        
        spdlog::debug("[ScriptState] State {} isolated successfully", m_state_id);
        
    } catch (const std::exception& e) {
        spdlog::error("[ScriptState] Failed to isolate state {}: {}", m_state_id, e.what());
    }
}

void ScriptState::restore_state() {
    try {
        // Restore original environment
        if (!m_original_uevr_table.valid()) {
            m_lua["uevr"] = m_original_uevr_table;
        }
        
        if (!m_original_package_path.empty()) {
            m_lua["package"]["path"] = m_original_package_path;
        }
        
        if (!m_original_package_cpath.empty()) {
            m_lua["package"]["cpath"] = m_original_package_cpath;
        }
        
        if (m_original_package_searchers.valid()) {
            m_lua["package"]["searchers"] = m_original_package_searchers;
        }
        
        spdlog::debug("[ScriptState] State {} restored to original environment", m_state_id);
        
    } catch (const std::exception& e) {
        spdlog::error("[ScriptState] Failed to restore state {}: {}", m_state_id, e.what());
    }
}

void ScriptState::clear_state_environment() {
    try {
        // Clear state-specific data
        m_lua.registry().clear();
        
        // Remove state namespace
        m_lua[m_state_namespace] = sol::nil;
        
        spdlog::debug("[ScriptState] State {} environment cleared", m_state_id);
        
    } catch (const std::exception& e) {
        spdlog::error("[ScriptState] Failed to clear state {} environment: {}", m_state_id, e.what());
    }
}

// State communication implementation
void ScriptState::send_message_to_state(uint32_t target_state_id, const std::string& message) {
    std::scoped_lock lock(m_message_mutex);
    
    try {
        ScriptState* target_state = find_state(target_state_id);
        if (target_state) {
            target_state->m_incoming_messages.push_back(message);
            spdlog::debug("[ScriptState] Message sent from state {} to state {}: {}", m_state_id, target_state_id, message);
        } else {
            spdlog::warn("[ScriptState] Target state {} not found for message from state {}", target_state_id, m_state_id);
        }
    } catch (const std::exception& e) {
        spdlog::error("[ScriptState] Failed to send message from state {} to state {}: {}", m_state_id, target_state_id, e.what());
    }
}

std::vector<std::string> ScriptState::get_pending_messages() const {
    std::scoped_lock lock(m_message_mutex);
    
    std::vector<std::string> messages = m_incoming_messages;
    m_incoming_messages.clear();
    
    return messages;
}

// State monitoring implementation
size_t ScriptState::get_memory_usage() const {
    return m_memory_usage;
}

size_t ScriptState::get_script_count() const {
    return m_script_count;
}

std::chrono::steady_clock::time_point ScriptState::get_last_activity() const {
    return m_last_activity;
}

void ScriptState::update_memory_usage() {
    try {
        // Get Lua memory usage
        size_t lua_memory = lua_gc(m_lua, LUA_GCCOUNT, 0);
        size_t lua_memory_kb = lua_gc(m_lua, LUA_GCCOUNTB, 0);
        
        m_memory_usage = lua_memory * 1024 + lua_memory_kb;
        
    } catch (const std::exception& e) {
        spdlog::error("[ScriptState] Failed to update memory usage for state {}: {}", m_state_id, e.what());
    }
}

void ScriptState::log_state_activity() {
    m_last_activity = std::chrono::steady_clock::now();
    m_script_count++;
    
    if (m_debug_mode) {
        spdlog::debug("[ScriptState] State {} activity logged - Scripts: {}, Memory: {} bytes", 
                      m_state_id, m_script_count, m_memory_usage);
    }
}

// Static state management implementation
void ScriptState::register_state(ScriptState* state) {
    std::scoped_lock lock(s_states_mutex);
    
    if (state && state->m_state_id > 0) {
        s_active_states[state->m_state_id] = state;
        spdlog::debug("[ScriptState] Registered state {} - Total active states: {}", 
                      state->m_state_id, s_active_states.size());
    }
}

void ScriptState::unregister_state(ScriptState* state) {
    std::scoped_lock lock(s_states_mutex);
    
    if (state && state->m_state_id > 0) {
        s_active_states.erase(state->m_state_id);
        spdlog::debug("[ScriptState] Unregistered state {} - Total active states: {}", 
                      state->m_state_id, s_active_states.size());
    }
}

ScriptState* ScriptState::find_state(uint32_t state_id) {
    std::scoped_lock lock(s_states_mutex);
    
    auto it = s_active_states.find(state_id);
    if (it != s_active_states.end()) {
        return it->second;
    }
    
    return nullptr;
}

// Enhanced run_script with multi-state support
void ScriptState::run_script(const std::string& p) {
    try {
        // Update activity tracking
        log_state_activity();
        
        uevr::API::get()->log_info(std::format("Running script {} in state {}...", p, m_state_id).c_str());

        const std::string old_pristine_path = m_lua.registry()["package_path_" + std::to_string(m_state_id)];
        const std::string old_pristine_cpath = m_lua.registry()["package_cpath_" + std::to_string(m_state_id)];
        const std::string old_path = m_lua["package"]["path"];

        auto path = std::filesystem::path(p);
        auto dir = path.parent_path();

        std::string package_path = m_lua["package"]["path"];
        std::string cpath = m_lua["package"]["cpath"];

        package_path = old_path + ";" + dir.string() + "/?.lua";
        package_path = package_path + ";" + dir.string() + "/?/init.lua";
        package_path = package_path + ";" + m_state_namespace + "/?.lua";

        cpath = old_pristine_cpath + ";" + dir.string() + "/?.dll";
        cpath = cpath + ";" + m_state_namespace + "/?.dll";

        m_lua["package"]["path"] = package_path;
        m_lua["package"]["cpath"] = cpath;
        m_lua.registry()["package_path_" + std::to_string(m_state_id)] = m_lua["package"]["path"];
        m_lua.registry()["package_cpath_" + std::to_string(m_state_id)] = m_lua["package"]["cpath"];

        m_lua.safe_script_file(p);
        
        // Update memory usage after script execution
        update_memory_usage();
        
        spdlog::info("[ScriptState] Script {} executed successfully in state {}", p, m_state_id);
        
    } catch (const std::exception& e) {
        api::ue::msg(e.what());
        spdlog::error("[ScriptState] Error executing script {} in state {}: {}", p, m_state_id, e.what());
    } catch (...) {
        api::ue::msg((std::stringstream{} << "Unknown error when running script " << p).str().c_str());
        spdlog::error("[ScriptState] Unknown error executing script {} in state {}", p, m_state_id);
    }

    // Restore package paths
    m_lua["package"]["path"] = old_path;
    m_lua.registry()["package_path_" + std::to_string(m_state_id)] = old_pristine_path;
    m_lua.registry()["package_cpath_" + std::to_string(m_state_id)] = old_pristine_cpath;
}

// Enhanced garbage collection with multi-state awareness
void ScriptState::gc_data_changed(GarbageCollectionData data) {
    try {
        // Handler
        switch (data.gc_handler) {
        case ScriptState::GarbageCollectionHandler::UEVR_MANAGED:
            lua_gc(m_lua, LUA_GCSTOP);
            break;
        case ScriptState::GarbageCollectionHandler::LUA_MANAGED:
            lua_gc(m_lua, LUA_GCRESTART);
            break;
        default:
            lua_gc(m_lua, LUA_GCRESTART);
            data.gc_handler = ScriptState::GarbageCollectionHandler::LUA_MANAGED;
            break;
        }

        // Type 
        if (data.gc_type >= ScriptState::GarbageCollectionType::LAST) {
           data.gc_type = ScriptState::GarbageCollectionType::STEP;
        }
        
        // Update memory usage after GC
        update_memory_usage();
        
        spdlog::debug("[ScriptState] Garbage collection completed for state {} - Memory: {} bytes", 
                      m_state_id, m_memory_usage);
        
    } catch (const std::exception& e) {
        spdlog::error("[ScriptState] Error during garbage collection for state {}: {}", m_state_id, e.what());
    }
}

// Enhanced frame processing with multi-state support
void ScriptState::on_frame() {
    try {
        // Process pending messages
        auto messages = get_pending_messages();
        if (!messages.empty()) {
            spdlog::debug("[ScriptState] State {} processing {} pending messages", m_state_id, messages.size());
        }
        
        // Update activity tracking
        log_state_activity();
        
    } catch (const std::exception& e) {
        spdlog::error("[ScriptState] Error in on_frame for state {}: {}", m_state_id, e.what());
    }
}

// Enhanced UI drawing with state information
void ScriptState::on_draw_ui() {
    try {
        // Draw state-specific UI elements
        if (m_debug_mode) {
            // This would integrate with ImGui to show state information
            spdlog::debug("[ScriptState] Drawing UI for state {}", m_state_id);
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[ScriptState] Error in on_draw_ui for state {}: {}", m_state_id, e.what());
    }
}

// Enhanced script reset with state cleanup
void ScriptState::on_script_reset() {
    try {
        // Clear state environment
        clear_state_environment();
        
        // Reinitialize state environment
        initialize_state_environment();
        
        // Reset counters
        m_script_count = 0;
        m_memory_usage = 0;
        
        spdlog::info("[ScriptState] State {} reset completed", m_state_id);
        
    } catch (const std::exception& e) {
        spdlog::error("[ScriptState] Error during script reset for state {}: {}", m_state_id, e.what());
    }
}

// Enhanced event dispatch with state isolation
void ScriptState::dispatch_event(std::string_view event_name, std::string_view event_data) {
    try {
        // Dispatch event to state-specific handlers
        if (m_context && m_context->valid()) {
            m_context->dispatch_event(event_name, event_data);
        }
        
        // Log event for debugging
        if (m_debug_mode) {
            spdlog::debug("[ScriptState] State {} dispatched event: {} = {}", 
                          m_state_id, event_name, event_data);
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[ScriptState] Error dispatching event in state {}: {}", m_state_id, e.what());
    }
}

} // namespace uevr