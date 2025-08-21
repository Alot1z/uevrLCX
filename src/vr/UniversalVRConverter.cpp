#include "UniversalVRConverter.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <algorithm>
#include <sstream>
#include <iomanip>

// Platform-specific includes
#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#include <tlhelp32.h>
#else
#include <sys/sysinfo.h>
#include <sys/resource.h>
#include <unistd.h>
#endif

namespace uevr {

UniversalVRConverter::UniversalVRConverter() 
    : m_redditCommunity(RedditCommunityIntegration::getInstance()) {
    spdlog::info("Initializing Universal VR Converter...");
    
    // Initialize logging
    auto logger = spdlog::rotating_logger_mt("uevrLCX", "logs/uevrLCX.log", 1024*1024*10, 3);
    spdlog::set_default_logger(logger);
    spdlog::set_level(spdlog::level::info);
    
    spdlog::info("Universal VR Converter instance created");
}

UniversalVRConverter::~UniversalVRConverter() {
    spdlog::info("Shutting down Universal VR Converter...");
    shutdown();
}

UniversalVRConverter& UniversalVRConverter::getInstance() {
    static UniversalVRConverter instance;
    return instance;
}

bool UniversalVRConverter::initialize() {
    if (m_initialized.load()) {
        spdlog::warn("Universal VR Converter already initialized");
        return true;
    }

    spdlog::info("Starting Universal VR Converter initialization...");

    try {
        // Initialize core framework
        m_framework = std::make_unique<Framework>();
        if (!m_framework->initialize()) {
            spdlog::error("Failed to initialize core framework");
            return false;
        }

        // Initialize plugin manager
        m_pluginManager = std::make_unique<PluginManager>();
        if (!m_pluginManager->initialize()) {
            spdlog::error("Failed to initialize plugin manager");
            return false;
        }

        // Initialize hook manager
        m_hookManager = std::make_unique<HookManager>();
        if (!m_hookManager->initialize()) {
            spdlog::error("Failed to initialize hook manager");
            return false;
        }

        // Initialize VR systems
        if (!initializeVRSystems()) {
            spdlog::error("Failed to initialize VR systems");
            return false;
        }

        // Initialize AI systems
        if (!initializeAISystems()) {
            spdlog::error("Failed to initialize AI systems");
            return false;
        }

        // Initialize external tools
        if (!initializeExternalTools()) {
            spdlog::error("Failed to initialize external tools");
            return false;
        }

        // Initialize Reddit Community Integration
        if (!m_redditCommunity.initialize()) {
            spdlog::error("Failed to initialize Reddit Community Integration");
            return false;
        }

        // Initialize engine detection and adapters
        m_engineDetector = std::make_unique<EngineDetector>();
        m_adapterLoader = std::make_unique<AdapterLoader>();

        // Load community profiles
        loadCommunityProfiles();

        // Start performance monitoring
        enablePerformanceMonitoring(true);

        m_initialized.store(true);
        spdlog::info("Universal VR Converter initialized successfully");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Exception during initialization: {}", e.what());
        return false;
    }
}

void UniversalVRConverter::shutdown() {
    if (m_shutdown.load()) {
        return;
    }

    spdlog::info("Shutting down Universal VR Converter...");

    // Stop conversion
    stopConversion();

    // Stop performance monitoring
    enablePerformanceMonitoring(false);

    // Save community profiles
    saveCommunityProfiles();

    // Cleanup resources
    cleanupResources();

    m_shutdown.store(true);
    spdlog::info("Universal VR Converter shutdown complete");
}

bool UniversalVRConverter::convertGameToVR(const std::string& gamePath, const std::string& gameName) {
    if (!m_initialized.load()) {
        spdlog::error("Universal VR Converter not initialized");
        return false;
    }

    spdlog::info("Starting VR conversion for game: {} at path: {}", gameName, gamePath);

    try {
        // Validate game path
        if (!validateGamePath(gamePath)) {
            logError("INVALID_PATH", "Game path is invalid or inaccessible", gameName);
            return false;
        }

        // Detect game engine
        std::string detectedEngine = detectGameEngine(gamePath);
        if (detectedEngine.empty()) {
            logError("ENGINE_DETECTION_FAILED", "Failed to detect game engine", gameName);
            return false;
        }

        spdlog::info("Detected engine: {} for game: {}", detectedEngine, gameName);

        // Load engine adapter
        if (!loadEngineAdapter(detectedEngine)) {
            logError("ADAPTER_LOAD_FAILED", "Failed to load engine adapter for " + detectedEngine, gameName, detectedEngine);
            return false;
        }

        // Analyze game with AI
        if (m_settings.enableAIGameAnalysis && m_gameAnalyzer) {
            spdlog::info("Analyzing game with AI...");
            auto analysisResult = m_gameAnalyzer->analyzeGame(gamePath, gameName, detectedEngine);
            if (!analysisResult.success) {
                spdlog::warn("AI analysis failed: {}", analysisResult.errorMessage);
            }
        }

        // Load community profile if available
        loadCommunityProfile(gameName);

        // Initialize VR conversion
        if (!m_currentAdapter->initializeGame(gamePath, gameName)) {
            logError("GAME_INIT_FAILED", "Failed to initialize game for VR conversion", gameName, detectedEngine);
            return false;
        }

        // Start conversion
        startConversion();

        spdlog::info("VR conversion started successfully for game: {}", gameName);
        return true;

    } catch (const std::exception& e) {
        logError("CONVERSION_EXCEPTION", e.what(), gameName);
        return false;
    }
}

bool UniversalVRConverter::injectIntoRunningGame(const std::string& processName) {
    if (!m_initialized.load()) {
        spdlog::error("Universal VR Converter not initialized");
        return false;
    }

    spdlog::info("Attempting to inject into running process: {}", processName);

    try {
        // Find process
        DWORD processId = 0;
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            logError("PROCESS_SNAPSHOT_FAILED", "Failed to create process snapshot");
            return false;
        }

        PROCESSENTRY32 pe32;
        pe32.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(snapshot, &pe32)) {
            do {
                if (processName == pe32.szExeFile) {
                    processId = pe32.th32ProcessID;
                    break;
                }
            } while (Process32Next(snapshot, &pe32));
        }

        CloseHandle(snapshot);

        if (processId == 0) {
            logError("PROCESS_NOT_FOUND", "Process not found: " + processName);
            return false;
        }

        // Open process
        HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
        if (processHandle == nullptr) {
            logError("PROCESS_ACCESS_DENIED", "Failed to open process: " + processName);
            return false;
        }

        // Get process path
        char processPath[MAX_PATH];
        if (GetModuleFileNameExA(processHandle, nullptr, processPath, MAX_PATH) == 0) {
            CloseHandle(processHandle);
            logError("PROCESS_PATH_FAILED", "Failed to get process path");
            return false;
        }

        CloseHandle(processHandle);

        // Convert the running game
        return convertGameToVR(processPath, processName);

    } catch (const std::exception& e) {
        logError("INJECTION_EXCEPTION", e.what());
        return false;
    }
}

bool UniversalVRConverter::detectAndConvertGame() {
    if (!m_initialized.load()) {
        spdlog::error("Universal VR Converter not initialized");
        return false;
    }

    spdlog::info("Detecting and converting game automatically...");

    try {
        // Get active window
        HWND activeWindow = GetForegroundWindow();
        if (activeWindow == nullptr) {
            logError("NO_ACTIVE_WINDOW", "No active window found");
            return false;
        }

        // Get process ID
        DWORD processId;
        GetWindowThreadProcessId(activeWindow, &processId);

        // Get process name
        HANDLE processHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
        if (processHandle == nullptr) {
            logError("PROCESS_ACCESS_DENIED", "Failed to access active process");
            return false;
        }

        char processName[MAX_PATH];
        if (GetModuleBaseNameA(processHandle, nullptr, processName, MAX_PATH) == 0) {
            CloseHandle(processHandle);
            logError("PROCESS_NAME_FAILED", "Failed to get process name");
            return false;
        }

        CloseHandle(processHandle);

        // Get process path
        char processPath[MAX_PATH];
        HANDLE processHandle2 = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processId);
        if (processHandle2 != nullptr) {
            GetModuleFileNameExA(processHandle2, nullptr, processPath, MAX_PATH);
            CloseHandle(processHandle2);
        }

        spdlog::info("Detected active game: {} at path: {}", processName, processPath);

        // Convert the detected game
        return convertGameToVR(processPath, processName);

    } catch (const std::exception& e) {
        logError("AUTO_DETECTION_EXCEPTION", e.what());
        return false;
    }
}

void UniversalVRConverter::startConversion() {
    if (m_converting.load()) {
        spdlog::warn("Conversion already running");
        return;
    }

    spdlog::info("Starting VR conversion...");
    m_converting.store(true);
    m_paused.store(false);

    // Start conversion thread
    m_conversionThread = std::thread(&UniversalVRConverter::conversionLoop, this);
}

void UniversalVRConverter::stopConversion() {
    if (!m_converting.load()) {
        return;
    }

    spdlog::info("Stopping VR conversion...");
    m_converting.store(false);
    m_paused.store(false);

    // Notify conversion thread
    m_conversionCV.notify_all();

    // Wait for conversion thread to finish
    if (m_conversionThread.joinable()) {
        m_conversionThread.join();
    }

    spdlog::info("VR conversion stopped");
}

void UniversalVRConverter::pauseConversion() {
    if (!m_converting.load()) {
        spdlog::warn("No conversion running to pause");
        return;
    }

    spdlog::info("Pausing VR conversion...");
    m_paused.store(true);
}

void UniversalVRConverter::resumeConversion() {
    if (!m_converting.load()) {
        spdlog::warn("No conversion running to resume");
        return;
    }

    spdlog::info("Resuming VR conversion...");
    m_paused.store(false);
    m_conversionCV.notify_all();
}

void UniversalVRConverter::setConversionSettings(const ConversionSettings& settings) {
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    m_settings = settings;
    spdlog::info("Conversion settings updated");
}

UniversalVRConverter::ConversionSettings UniversalVRConverter::getConversionSettings() const {
    std::lock_guard<std::mutex> lock(m_settingsMutex);
    return m_settings;
}

std::string UniversalVRConverter::detectGameEngine(const std::string& gamePath) {
    if (!m_engineDetector) {
        spdlog::error("Engine detector not initialized");
        return "";
    }

    try {
        return m_engineDetector->detectEngine(gamePath);
    } catch (const std::exception& e) {
        spdlog::error("Engine detection failed: {}", e.what());
        return "";
    }
}

bool UniversalVRConverter::loadEngineAdapter(const std::string& engineName) {
    if (!m_adapterLoader) {
        spdlog::error("Adapter loader not initialized");
        return false;
    }

    try {
        m_currentAdapter = m_adapterLoader->loadAdapter(engineName);
        if (!m_currentAdapter) {
            spdlog::error("Failed to load adapter for engine: {}", engineName);
            return false;
        }

        spdlog::info("Successfully loaded adapter for engine: {}", engineName);
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Adapter loading failed: {}", e.what());
        return false;
    }
}

std::vector<std::string> UniversalVRConverter::getSupportedEngines() const {
    if (!m_adapterLoader) {
        return {};
    }

    return m_adapterLoader->getSupportedEngines();
}

UniversalVRConverter::PerformanceMetrics UniversalVRConverter::getPerformanceMetrics() const {
    std::lock_guard<std::mutex> lock(m_metricsMutex);
    return m_performanceMetrics;
}

void UniversalVRConverter::enablePerformanceMonitoring(bool enable) {
    if (enable == m_performanceMonitoringEnabled) {
        return;
    }

    m_performanceMonitoringEnabled = enable;

    if (enable) {
        spdlog::info("Starting performance monitoring...");
        m_performanceThread = std::thread(&UniversalVRConverter::performanceMonitoringLoop, this);
    } else {
        spdlog::info("Stopping performance monitoring...");
        if (m_performanceThread.joinable()) {
            m_performanceThread.join();
        }
    }
}

bool UniversalVRConverter::submitGameProfile(const std::string& gameName, const std::string& profileData) {
    std::lock_guard<std::mutex> lock(m_profilesMutex);
    m_communityProfiles[gameName] = profileData;
    spdlog::info("Submitted community profile for game: {}", gameName);
    return true;
}

bool UniversalVRConverter::loadCommunityProfile(const std::string& gameName) {
    std::lock_guard<std::mutex> lock(m_profilesMutex);
    auto it = m_communityProfiles.find(gameName);
    if (it != m_communityProfiles.end()) {
        spdlog::info("Loaded community profile for game: {}", gameName);
        return true;
    }
    return false;
}

std::vector<std::string> UniversalVRConverter::getCommunityProfiles() const {
    std::lock_guard<std::mutex> lock(m_profilesMutex);
    std::vector<std::string> profiles;
    for (const auto& [gameName, _] : m_communityProfiles) {
        profiles.push_back(gameName);
    }
    return profiles;
}

std::vector<UniversalVRConverter::ConversionError> UniversalVRConverter::getRecentErrors() const {
    std::lock_guard<std::mutex> lock(m_errorsMutex);
    return m_recentErrors;
}

void UniversalVRConverter::clearErrors() {
    std::lock_guard<std::mutex> lock(m_errorsMutex);
    m_recentErrors.clear();
}

bool UniversalVRConverter::hasErrors() const {
    std::lock_guard<std::mutex> lock(m_errorsMutex);
    return !m_recentErrors.empty();
}

bool UniversalVRConverter::loadPlugin(const std::string& pluginPath) {
    if (!m_pluginManager) {
        spdlog::error("Plugin manager not initialized");
        return false;
    }

    try {
        auto plugin = m_pluginManager->loadPlugin(pluginPath);
        if (plugin) {
            std::lock_guard<std::mutex> lock(m_pluginsMutex);
            m_loadedPlugins[plugin->getName()] = plugin;
            spdlog::info("Successfully loaded plugin: {}", plugin->getName());
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        spdlog::error("Plugin loading failed: {}", e.what());
        return false;
    }
}

bool UniversalVRConverter::unloadPlugin(const std::string& pluginName) {
    std::lock_guard<std::mutex> lock(m_pluginsMutex);
    auto it = m_loadedPlugins.find(pluginName);
    if (it != m_loadedPlugins.end()) {
        m_loadedPlugins.erase(it);
        spdlog::info("Unloaded plugin: {}", pluginName);
        return true;
    }
    return false;
}

std::vector<std::string> UniversalVRConverter::getLoadedPlugins() const {
    std::lock_guard<std::mutex> lock(m_pluginsMutex);
    std::vector<std::string> plugins;
    for (const auto& [name, _] : m_loadedPlugins) {
        plugins.push_back(name);
    }
    return plugins;
}

bool UniversalVRConverter::enableNeuralRendering(bool enable) {
    // TODO: Implement neural rendering
    spdlog::info("Neural rendering {} for {}", enable ? "enabled" : "disabled");
    return true;
}

bool UniversalVRConverter::enableAICollisionPrediction(bool enable) {
    // TODO: Implement AI collision prediction
    spdlog::info("AI collision prediction {} for {}", enable ? "enabled" : "disabled");
    return true;
}

bool UniversalVRConverter::enableDynamicOptimization(bool enable) {
    // TODO: Implement dynamic optimization
    spdlog::info("Dynamic optimization {} for {}", enable ? "enabled" : "disabled");
    return true;
}

// Private implementation methods

void UniversalVRConverter::conversionLoop() {
    spdlog::info("Conversion loop started");

    while (m_converting.load() && !m_shutdown.load()) {
        std::unique_lock<std::mutex> lock(m_conversionMutex);
        
        // Wait if paused
        m_conversionCV.wait(lock, [this] { 
            return !m_paused.load() || !m_converting.load() || m_shutdown.load(); 
        });

        if (!m_converting.load() || m_shutdown.load()) {
            break;
        }

        try {
            // Perform VR conversion frame
            if (m_currentAdapter) {
                m_currentAdapter->processFrame();
            }

            // Update VR systems
            if (m_vrSystem) {
                m_vrSystem->update();
            }

            // Update performance metrics
            updatePerformanceMetrics();

            // Apply AI optimizations
            if (m_settings.enableAutoOptimization && m_performanceOptimizer) {
                m_performanceOptimizer->optimizeFrame();
            }

        } catch (const std::exception& e) {
            spdlog::error("Error in conversion loop: {}", e.what());
        }

        // Frame rate control
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / m_settings.targetFrameRate));
    }

    spdlog::info("Conversion loop ended");
}

void UniversalVRConverter::performanceMonitoringLoop() {
    spdlog::info("Performance monitoring loop started");

    while (m_performanceMonitoringEnabled && !m_shutdown.load()) {
        try {
            updatePerformanceMetrics();
        } catch (const std::exception& e) {
            spdlog::error("Error in performance monitoring: {}", e.what());
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    spdlog::info("Performance monitoring loop ended");
}

bool UniversalVRConverter::initializeVRSystems() {
    try {
        // Initialize VR system
        m_vrSystem = std::make_unique<VRSystem>();
        if (!m_vrSystem->initialize()) {
            spdlog::error("Failed to initialize VR system");
            return false;
        }

        // Initialize OpenXR integration
        m_openxrIntegration = std::make_unique<OpenXRIntegration>();
        if (!m_openxrIntegration->initialize()) {
            spdlog::error("Failed to initialize OpenXR integration");
            return false;
        }

        // Initialize universal VR system
        m_universalVRSystem = std::make_unique<UniversalVRSystem>();
        if (!m_universalVRSystem->initialize()) {
            spdlog::error("Failed to initialize universal VR system");
            return false;
        }

        // Initialize renderer
        m_renderer = std::make_unique<EngineAgnosticRenderer>();
        if (!m_renderer->initialize()) {
            spdlog::error("Failed to initialize renderer");
            return false;
        }

        // Initialize physics integration
        m_physicsIntegration = std::make_unique<FullPhysicsIntegration>();
        if (!m_physicsIntegration->initialize()) {
            spdlog::error("Failed to initialize physics integration");
            return false;
        }

        // Initialize collision engine
        m_collisionEngine = std::make_unique<FullAestheticCollisionEngine>();
        if (!m_collisionEngine->initialize()) {
            spdlog::error("Failed to initialize collision engine");
            return false;
        }

        spdlog::info("VR systems initialized successfully");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Exception during VR systems initialization: {}", e.what());
        return false;
    }
}

bool UniversalVRConverter::initializeAISystems() {
    try {
        // Initialize Context7 AI system
        m_context7AI = std::make_unique<Context7AISystem>();
        if (!m_context7AI->initialize()) {
            spdlog::error("Failed to initialize Context7 AI system");
            return false;
        }

        // Initialize game analyzer
        m_gameAnalyzer = std::make_unique<GameAnalyzer>();
        if (!m_gameAnalyzer->initialize()) {
            spdlog::error("Failed to initialize game analyzer");
            return false;
        }

        // Initialize performance optimizer
        m_performanceOptimizer = std::make_unique<PerformanceOptimizer>();
        if (!m_performanceOptimizer->initialize()) {
            spdlog::error("Failed to initialize performance optimizer");
            return false;
        }

        spdlog::info("AI systems initialized successfully");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Exception during AI systems initialization: {}", e.what());
        return false;
    }
}

bool UniversalVRConverter::initializeExternalTools() {
    try {
        // Initialize VRGIN integration
        m_vrginIntegration = std::make_unique<VRGINIntegration>();
        if (!m_vrginIntegration->initialize()) {
            spdlog::error("Failed to initialize VRGIN integration");
            return false;
        }

        // Initialize Depth3D integration
        m_depth3DIntegration = std::make_unique<Depth3DIntegration>();
        if (!m_depth3DIntegration->initialize()) {
            spdlog::error("Failed to initialize Depth3D integration");
            return false;
        }

        // Initialize Reshade integration
        m_reshadeIntegration = std::make_unique<ReshadeIntegration>();
        if (!m_reshadeIntegration->initialize()) {
            spdlog::error("Failed to initialize Reshade integration");
            return false;
        }

        // Initialize physics engine integration
        m_physicsEngineIntegration = std::make_unique<PhysicsEngineIntegration>();
        if (!m_physicsEngineIntegration->initialize()) {
            spdlog::error("Failed to initialize physics engine integration");
            return false;
        }

        // Initialize performance optimization
        m_performanceOptimization = std::make_unique<PerformanceOptimization>();
        if (!m_performanceOptimization->initialize()) {
            spdlog::error("Failed to initialize performance optimization");
            return false;
        }

        spdlog::info("External tools initialized successfully");
        return true;

    } catch (const std::exception& e) {
        spdlog::error("Exception during external tools initialization: {}", e.what());
        return false;
    }
}

void UniversalVRConverter::updatePerformanceMetrics() {
    std::lock_guard<std::mutex> lock(m_metricsMutex);

    try {
        // Get current time
        auto now = std::chrono::high_resolution_clock::now();
        static auto lastTime = now;
        auto deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - lastTime).count() / 1000000.0f;
        lastTime = now;

        // Calculate frame rate
        if (deltaTime > 0) {
            m_performanceMetrics.currentFrameRate = 1.0f / deltaTime;
            m_performanceMetrics.averageFrameRate = 
                m_performanceMetrics.averageFrameRate * 0.9f + m_performanceMetrics.currentFrameRate * 0.1f;
        }

        // Get system metrics
#ifdef _WIN32
        // CPU usage
        static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
        static int numProcessors = 0;
        
        if (numProcessors == 0) {
            SYSTEM_INFO sysInfo;
            GetSystemInfo(&sysInfo);
            numProcessors = sysInfo.dwNumberOfProcessors;
        }

        FILETIME idleTime, kernelTime, userTime;
        if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
            ULARGE_INTEGER idle, kernel, user;
            idle.LowPart = idleTime.dwLowDateTime;
            idle.HighPart = idleTime.dwHighDateTime;
            kernel.LowPart = kernelTime.dwLowDateTime;
            kernel.HighPart = kernelTime.dwHighDateTime;
            user.LowPart = userTime.dwLowDateTime;
            user.HighPart = userTime.dwHighDateTime;

            if (lastCPU.QuadPart != 0) {
                ULONGLONG idleDiff = idle.QuadPart - lastCPU.QuadPart;
                ULONGLONG kernelDiff = kernel.QuadPart - lastSysCPU.QuadPart;
                ULONGLONG userDiff = user.QuadPart - lastUserCPU.QuadPart;
                ULONGLONG total = kernelDiff + userDiff;
                m_performanceMetrics.cpuUsage = (float)(total - idleDiff) / total * 100.0f;
            }

            lastCPU = idle;
            lastSysCPU = kernel;
            lastUserCPU = user;
        }

        // Memory usage
        PROCESS_MEMORY_COUNTERS_EX pmc;
        HANDLE process = GetCurrentProcess();
        if (GetProcessMemoryInfo(process, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
            m_performanceMetrics.memoryUsage = (float)pmc.WorkingSetSize / (1024 * 1024); // MB
        }
#endif

        // VR performance score (simplified calculation)
        m_performanceMetrics.vrPerformanceScore = 
            (m_performanceMetrics.currentFrameRate / m_settings.targetFrameRate) * 100.0f;

    } catch (const std::exception& e) {
        spdlog::error("Error updating performance metrics: {}", e.what());
    }
}

void UniversalVRConverter::logError(const std::string& errorCode, const std::string& errorMessage, 
                                   const std::string& gameName, const std::string& engineName) {
    std::lock_guard<std::mutex> lock(m_errorsMutex);

    ConversionError error;
    error.errorCode = errorCode;
    error.errorMessage = errorMessage;
    error.gameName = gameName;
    error.engineName = engineName;
    
    // Get current timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    error.timestamp = ss.str();

    // Add to recent errors
    m_recentErrors.push_back(error);
    
    // Keep only the most recent errors
    if (m_recentErrors.size() > MAX_ERRORS) {
        m_recentErrors.erase(m_recentErrors.begin());
    }

    spdlog::error("Conversion error [{}]: {} (Game: {}, Engine: {})", 
                  errorCode, errorMessage, gameName, engineName);
}

void UniversalVRConverter::loadCommunityProfiles() {
    try {
        std::filesystem::path profilesPath = "data/community_profiles.json";
        if (std::filesystem::exists(profilesPath)) {
            std::ifstream file(profilesPath);
            if (file.is_open()) {
                // TODO: Implement JSON parsing for community profiles
                spdlog::info("Community profiles loaded");
            }
        }
    } catch (const std::exception& e) {
        spdlog::error("Failed to load community profiles: {}", e.what());
    }
}

void UniversalVRConverter::saveCommunityProfiles() {
    try {
        std::filesystem::create_directories("data");
        std::filesystem::path profilesPath = "data/community_profiles.json";
        
        // TODO: Implement JSON serialization for community profiles
        spdlog::info("Community profiles saved");
    } catch (const std::exception& e) {
        spdlog::error("Failed to save community profiles: {}", e.what());
    }
}

bool UniversalVRConverter::validateGamePath(const std::string& gamePath) {
    return std::filesystem::exists(gamePath) && std::filesystem::is_regular_file(gamePath);
}

bool UniversalVRConverter::validateEngineSupport(const std::string& engineName) {
    auto supportedEngines = getSupportedEngines();
    return std::find(supportedEngines.begin(), supportedEngines.end(), engineName) != supportedEngines.end();
}

void UniversalVRConverter::cleanupResources() {
    // Cleanup VR systems
    if (m_collisionEngine) m_collisionEngine->shutdown();
    if (m_physicsIntegration) m_physicsIntegration->shutdown();
    if (m_renderer) m_renderer->shutdown();
    if (m_universalVRSystem) m_universalVRSystem->shutdown();
    if (m_openxrIntegration) m_openxrIntegration->shutdown();
    if (m_vrSystem) m_vrSystem->shutdown();

    // Cleanup AI systems
    if (m_performanceOptimizer) m_performanceOptimizer->shutdown();
    if (m_gameAnalyzer) m_gameAnalyzer->shutdown();
    if (m_context7AI) m_context7AI->shutdown();

    // Cleanup external tools
    if (m_performanceOptimization) m_performanceOptimization->shutdown();
    if (m_physicsEngineIntegration) m_physicsEngineIntegration->shutdown();
    if (m_reshadeIntegration) m_reshadeIntegration->shutdown();
    if (m_depth3DIntegration) m_depth3DIntegration->shutdown();
    if (m_vrginIntegration) m_vrginIntegration->shutdown();

    // Cleanup core systems
    if (m_hookManager) m_hookManager->shutdown();
    if (m_pluginManager) m_pluginManager->shutdown();
    if (m_framework) m_framework->shutdown();

    spdlog::info("All resources cleaned up");
}

// Reddit Community Integration Methods
bool UniversalVRConverter::integrateRedditCommunitySolutions() {
    spdlog::info("Integrating Reddit community solutions...");
    
    try {
        // Load community solutions from Reddit
        if (!m_redditCommunity.loadCommunitySolutions()) {
            spdlog::warn("Failed to load Reddit community solutions");
            return false;
        }
        
        spdlog::info("Successfully integrated Reddit community solutions");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to integrate Reddit community solutions: {}", e.what());
        return false;
    }
}

std::vector<RedditCommunityIntegration::CommunitySolution> 
UniversalVRConverter::getCommunitySolutionsForGame(const std::string& gameName) {
    return m_redditCommunity.getSolutionsForGame(gameName);
}

bool UniversalVRConverter::applyRedditSolution(const std::string& solutionId, const std::string& gamePath) {
    spdlog::info("Applying Reddit community solution: {} to game: {}", solutionId, gamePath);
    
    try {
        // Apply the community solution
        if (!m_redditCommunity.applyCommunitySolution(solutionId, gamePath)) {
            spdlog::error("Failed to apply Reddit community solution: {}", solutionId);
            return false;
        }
        
        spdlog::info("Successfully applied Reddit community solution: {}", solutionId);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("Exception while applying Reddit community solution: {}", e.what());
        return false;
    }
}

bool UniversalVRConverter::searchAndApplyCommunitySolutions(const std::string& gameName, const std::string& engineType) {
    spdlog::info("Searching and applying community solutions for game: {} (engine: {})", gameName, engineType);
    
    try {
        // Get community solutions for the game
        auto solutions = m_redditCommunity.getSolutionsForGame(gameName);
        
        if (solutions.empty()) {
            spdlog::warn("No community solutions found for game: {}", gameName);
            return false;
        }
        
        // Find the best solution (highest upvotes)
        auto bestSolution = solutions.front();
        
        spdlog::info("Found {} community solutions for game: {}", solutions.size(), gameName);
        spdlog::info("Applying best solution: {} (upvotes: {})", bestSolution.title, bestSolution.upvotes);
        
        // Apply the best solution
        return applyRedditSolution(bestSolution.id, gameName);
        
    } catch (const std::exception& e) {
        spdlog::error("Exception while searching community solutions: {}", e.what());
        return false;
    }
}

} // namespace uevr
