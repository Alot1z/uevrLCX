# Dependencies management for uevrLCX

# Core dependencies
include(FetchContent)

# spdlog - Logging library
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.12.0
)
FetchContent_MakeAvailable(spdlog)

# nlohmann/json - JSON library
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

# libcurl - HTTP client
FetchContent_Declare(
    curl
    GIT_REPOSITORY https://github.com/curl/curl.git
    GIT_TAG curl-8_5_0
)
FetchContent_MakeAvailable(curl)

# SQLite3 - Database
FetchContent_Declare(
    sqlite3
    GIT_REPOSITORY https://github.com/sqlite/sqlite.git
    GIT_TAG version-3.44.2
)
FetchContent_MakeAvailable(sqlite3)

# Google Test - Testing framework
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

# Bullet Physics - Physics engine
FetchContent_Declare(
    bullet
    GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
    GIT_TAG 3.25
)
FetchContent_MakeAvailable(bullet)

# OpenVR - VR SDK
if(WIN32)
    FetchContent_Declare(
        openvr
        GIT_REPOSITORY https://github.com/ValveSoftware/openvr.git
        GIT_TAG v1.0.26
    )
    FetchContent_MakeAvailable(openvr)
endif()

# Create dependency targets
add_library(uevr_dependencies INTERFACE)

target_link_libraries(uevr_dependencies INTERFACE
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    CURL::libcurl
    Threads::Threads
)

if(TARGET sqlite3)
    target_link_libraries(uevr_dependencies INTERFACE sqlite3)
endif()

if(TARGET gtest)
    target_link_libraries(uevr_dependencies INTERFACE gtest gtest_main)
endif()

if(TARGET Bullet3Common)
    target_link_libraries(uevr_dependencies INTERFACE 
        Bullet3Common
        Bullet3Dynamics
        Bullet3Collision
        Bullet3Geometry
    )
endif()

if(TARGET openvr_api)
    target_link_libraries(uevr_dependencies INTERFACE openvr_api)
endif()

# Include directories
target_include_directories(uevr_dependencies INTERFACE
    ${CMAKE_SOURCE_DIR}/dependencies/include
    ${CMAKE_SOURCE_DIR}/dependencies/src
)

# Compiler definitions
target_compile_definitions(uevr_dependencies INTERFACE
    SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_INFO
    NLOHMANN_JSON_VERSION_MAJOR=3
    NLOHMANN_JSON_VERSION_MINOR=11
    NLOHMANN_JSON_VERSION_PATCH=3
)
