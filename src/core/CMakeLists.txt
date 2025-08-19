# Core sources - minimal build with only working files
set(CORE_SOURCES
    ExceptionHandler.cpp
)

# Create core library
add_library(uevr_core STATIC ${CORE_SOURCES})

# Include directories
target_include_directories(uevr_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
    $<INSTALL_INTERFACE:include>
)

# Compile definitions
target_compile_definitions(uevr_core PRIVATE
    UEVR_CORE_EXPORTS
    _CRT_SECURE_NO_WARNINGS
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _WIN32_WINNT=0x0601
)

# Set output properties
set_target_properties(uevr_core PROPERTIES
    OUTPUT_NAME "uevr_core"
    PREFIX ""
)

# Context7 database (if enabled)
if(uevr_ENABLE_CONTEXT7)
    add_library(uevr_context7_database STATIC
        Context7Database.cpp
    )
    
    target_include_directories(uevr_context7_database
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
            $<INSTALL_INTERFACE:include>
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
    )
    
    target_link_libraries(uevr_context7_database
        PUBLIC
            spdlog::spdlog
            nlohmann_json::nlohmann_json
    )
    
    message(STATUS "Context7 database enabled")
endif()

message(STATUS "Configured minimal core build")