#include "uevr/hooks/OpenGLHooks.hpp"
#include <spdlog/spdlog.h>
#include <MinHook.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <memory>
#include <vector>

namespace uevr {
namespace Hooks {

OpenGLHooks::OpenGLHooks() 
    : m_hooks_installed(false)
    , m_swap_buffers_hook(nullptr)
    , m_swap_layer_buffers_hook(nullptr)
    , m_draw_arrays_hook(nullptr)
    , m_draw_elements_hook(nullptr)
    , m_gl_begin_hook(nullptr)
    , m_gl_end_hook(nullptr)
    , m_make_current_hook(nullptr)
    , m_create_context_hook(nullptr) {
    
    spdlog::info("[OpenGLHooks] OpenGL Hooks initialized");
}

OpenGLHooks::~OpenGLHooks() {
    removeHooks();
    spdlog::info("[OpenGLHooks] OpenGL Hooks cleanup completed");
}

bool OpenGLHooks::installHooks() {
    if (m_hooks_installed) {
        spdlog::warn("[OpenGLHooks] Hooks already installed");
        return true;
    }

    try {
        spdlog::info("[OpenGLHooks] Installing OpenGL hooks...");
        
        // Initialize MinHook
        if (MH_Initialize() != MH_OK) {
            spdlog::error("[OpenGLHooks] Failed to initialize MinHook");
            return false;
        }
        
        // Install OpenGL function hooks
        if (!installOpenGLFunctionHooks()) {
            spdlog::warn("[OpenGLHooks] OpenGL function hooks installation failed");
        }
        
        // Install WGL function hooks
        if (!installWGLFunctionHooks()) {
            spdlog::warn("[OpenGLHooks] WGL function hooks installation failed");
        }
        
        // Enable all hooks
        if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) {
            spdlog::error("[OpenGLHooks] Failed to enable hooks");
            return false;
        }
        
        m_hooks_installed = true;
        spdlog::info("[OpenGLHooks] OpenGL hooks installed successfully");
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[OpenGLHooks] Exception during hook installation: {}", e.what());
        return false;
    } catch (...) {
        spdlog::error("[OpenGLHooks] Unknown error during hook installation");
        return false;
    }
}

void OpenGLHooks::removeHooks() {
    if (!m_hooks_installed) {
        return;
    }

    try {
        spdlog::info("[OpenGLHooks] Removing OpenGL hooks...");
        
        // Disable all hooks first
        MH_DisableHook(MH_ALL_HOOKS);
        
        // Uninstall OpenGL function hooks
        if (m_swap_buffers_hook) {
            MH_UncreateHook(m_swap_buffers_hook);
            m_swap_buffers_hook = nullptr;
        }
        
        if (m_swap_layer_buffers_hook) {
            MH_UncreateHook(m_swap_layer_buffers_hook);
            m_swap_layer_buffers_hook = nullptr;
        }
        
        if (m_draw_arrays_hook) {
            MH_UncreateHook(m_draw_arrays_hook);
            m_draw_arrays_hook = nullptr;
        }
        
        if (m_draw_elements_hook) {
            MH_UncreateHook(m_draw_elements_hook);
            m_draw_elements_hook = nullptr;
        }
        
        if (m_gl_begin_hook) {
            MH_UncreateHook(m_gl_begin_hook);
            m_gl_begin_hook = nullptr;
        }
        
        if (m_gl_end_hook) {
            MH_UncreateHook(m_gl_end_hook);
            m_gl_end_hook = nullptr;
        }
        
        // Uninstall WGL function hooks
        if (m_make_current_hook) {
            MH_UncreateHook(m_make_current_hook);
            m_make_current_hook = nullptr;
        }
        
        if (m_create_context_hook) {
            MH_UncreateHook(m_create_context_hook);
            m_create_context_hook = nullptr;
        }
        
        // Uninitialize MinHook
        MH_Uninitialize();
        
        m_hooks_installed = false;
        spdlog::info("[OpenGLHooks] OpenGL hooks removed successfully");
        
    } catch (const std::exception& e) {
        spdlog::error("[OpenGLHooks] Exception during hook removal: {}", e.what());
    } catch (...) {
        spdlog::error("[OpenGLHooks] Unknown error during hook removal");
    }
}

bool OpenGLHooks::installOpenGLFunctionHooks() {
    try {
        // Get OpenGL module
        HMODULE opengl_module = GetModuleHandleA("opengl32.dll");
        if (!opengl_module) {
            spdlog::warn("[OpenGLHooks] OpenGL module not found");
            return false;
        }
        
        // Hook SwapBuffers
        auto swap_buffers = reinterpret_cast<LPVOID>(GetProcAddress(opengl_module, "wglSwapBuffers"));
        if (swap_buffers) {
            if (MH_CreateHook(swap_buffers, &OpenGLHooks::SwapBuffers_Hook, 
                             reinterpret_cast<LPVOID*>(&m_original_swap_buffers)) == MH_OK) {
                m_swap_buffers_hook = swap_buffers;
                spdlog::info("[OpenGLHooks] SwapBuffers hook installed");
            }
        }
        
        // Hook SwapLayerBuffers
        auto swap_layer_buffers = reinterpret_cast<LPVOID>(GetProcAddress(opengl_module, "wglSwapLayerBuffers"));
        if (swap_layer_buffers) {
            if (MH_CreateHook(swap_layer_buffers, &OpenGLHooks::SwapLayerBuffers_Hook, 
                             reinterpret_cast<LPVOID*>(&m_original_swap_layer_buffers)) == MH_OK) {
                m_swap_layer_buffers_hook = swap_layer_buffers;
                spdlog::info("[OpenGLHooks] SwapLayerBuffers hook installed");
            }
        }
        
        // Hook glDrawArrays
        auto draw_arrays = reinterpret_cast<LPVOID>(GetProcAddress(opengl_module, "glDrawArrays"));
        if (draw_arrays) {
            if (MH_CreateHook(draw_arrays, &OpenGLHooks::DrawArrays_Hook, 
                             reinterpret_cast<LPVOID*>(&m_original_draw_arrays)) == MH_OK) {
                m_draw_arrays_hook = draw_arrays;
                spdlog::info("[OpenGLHooks] glDrawArrays hook installed");
            }
        }
        
        // Hook glDrawElements
        auto draw_elements = reinterpret_cast<LPVOID>(GetProcAddress(opengl_module, "glDrawElements"));
        if (draw_elements) {
            if (MH_CreateHook(draw_elements, &OpenGLHooks::DrawElements_Hook, 
                             reinterpret_cast<LPVOID*>(&m_original_draw_elements)) == MH_OK) {
                m_draw_elements_hook = draw_elements;
                spdlog::info("[OpenGLHooks] glDrawElements hook installed");
            }
        }
        
        // Hook glBegin
        auto gl_begin = reinterpret_cast<LPVOID>(GetProcAddress(opengl_module, "glBegin"));
        if (gl_begin) {
            if (MH_CreateHook(gl_begin, &OpenGLHooks::Begin_Hook, 
                             reinterpret_cast<LPVOID*>(&m_original_gl_begin)) == MH_OK) {
                m_gl_begin_hook = gl_begin;
                spdlog::info("[OpenGLHooks] glBegin hook installed");
            }
        }
        
        // Hook glEnd
        auto gl_end = reinterpret_cast<LPVOID>(GetProcAddress(opengl_module, "glEnd"));
        if (gl_end) {
            if (MH_CreateHook(gl_end, &OpenGLHooks::End_Hook, 
                             reinterpret_cast<LPVOID*>(&m_original_gl_end)) == MH_OK) {
                m_gl_end_hook = gl_end;
                spdlog::info("[OpenGLHooks] glEnd hook installed");
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[OpenGLHooks] Exception during OpenGL function hook installation: {}", e.what());
        return false;
    }
}

bool OpenGLHooks::installWGLFunctionHooks() {
    try {
        // Get WGL module
        HMODULE wgl_module = GetModuleHandleA("opengl32.dll");
        if (!wgl_module) {
            spdlog::warn("[OpenGLHooks] WGL module not found");
            return false;
        }
        
        // Hook wglMakeCurrent
        auto make_current = reinterpret_cast<LPVOID>(GetProcAddress(wgl_module, "wglMakeCurrent"));
        if (make_current) {
            if (MH_CreateHook(make_current, &OpenGLHooks::MakeCurrent_Hook, 
                             reinterpret_cast<LPVOID*>(&m_original_make_current)) == MH_OK) {
                m_make_current_hook = make_current;
                spdlog::info("[OpenGLHooks] wglMakeCurrent hook installed");
            }
        }
        
        // Hook wglCreateContext
        auto create_context = reinterpret_cast<LPVOID>(GetProcAddress(wgl_module, "wglCreateContext"));
        if (create_context) {
            if (MH_CreateHook(create_context, &OpenGLHooks::CreateContext_Hook, 
                             reinterpret_cast<LPVOID*>(&m_original_create_context)) == MH_OK) {
                m_create_context_hook = create_context;
                spdlog::info("[OpenGLHooks] wglCreateContext hook installed");
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("[OpenGLHooks] Exception during WGL function hook installation: {}", e.what());
        return false;
    }
}

// Hook callback functions
BOOL OpenGLHooks::SwapBuffers_Hook(HDC hdc) {
    try {
        spdlog::debug("[OpenGLHooks] SwapBuffers called");
        
        // Call original function
        BOOL result = m_original_swap_buffers(hdc);
        
        // Post-processing for VR
        if (result) {
            // Notify framework about present
            if (m_present_callback) {
                m_present_callback(PresentType::OpenGL, hdc);
            }
        }
        
        return result;
        
    } catch (const std::exception& e) {
        spdlog::error("[OpenGLHooks] Exception in SwapBuffers hook: {}", e.what());
        return m_original_swap_buffers(hdc);
    }
}

BOOL OpenGLHooks::SwapLayerBuffers_Hook(HDC hdc, UINT fuPlanes) {
    try {
        spdlog::debug("[OpenGLHooks] SwapLayerBuffers called");
        
        // Call original function
        BOOL result = m_original_swap_layer_buffers(hdc, fuPlanes);
        
        // Post-processing for VR
        if (result) {
            // Notify framework about present
            if (m_present_callback) {
                m_present_callback(PresentType::OpenGL, hdc);
            }
        }
        
        return result;
        
    } catch (const std::exception& e) {
        spdlog::error("[OpenGLHooks] Exception in SwapLayerBuffers hook: {}", e.what());
        return m_original_swap_layer_buffers(hdc, fuPlanes);
    }
}

void OpenGLHooks::DrawArrays_Hook(GLenum mode, GLint first, GLsizei count) {
    try {
        spdlog::debug("[OpenGLHooks] glDrawArrays called: mode={}, first={}, count={}", mode, first, count);
        
        // Call original function
        m_original_draw_arrays(mode, first, count);
        
        // Post-processing for VR
        if (m_draw_callback) {
            m_draw_callback(DrawType::OpenGL, mode, first, count);
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[OpenGLHooks] Exception in glDrawArrays hook: {}", e.what());
        m_original_draw_arrays(mode, first, count);
    }
}

void OpenGLHooks::DrawElements_Hook(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices) {
    try {
        spdlog::debug("[OpenGLHooks] glDrawElements called: mode={}, count={}, type={}", mode, count, type);
        
        // Call original function
        m_original_draw_elements(mode, count, type, indices);
        
        // Post-processing for VR
        if (m_draw_callback) {
            m_draw_callback(DrawType::OpenGL, mode, count, type, indices);
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[OpenGLHooks] Exception in glDrawElements hook: {}", e.what());
        m_original_draw_elements(mode, count, type, indices);
    }
}

void OpenGLHooks::Begin_Hook(GLenum mode) {
    try {
        spdlog::debug("[OpenGLHooks] glBegin called: mode={}", mode);
        
        // Call original function
        m_original_gl_begin(mode);
        
        // Post-processing for VR
        if (m_begin_callback) {
            m_begin_callback(mode);
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[OpenGLHooks] Exception in glBegin hook: {}", e.what());
        m_original_gl_begin(mode);
    }
}

void OpenGLHooks::End_Hook() {
    try {
        spdlog::debug("[OpenGLHooks] glEnd called");
        
        // Call original function
        m_original_gl_end();
        
        // Post-processing for VR
        if (m_end_callback) {
            m_end_callback();
        }
        
    } catch (const std::exception& e) {
        spdlog::error("[OpenGLHooks] Exception in glEnd hook: {}", e.what());
        m_original_gl_end();
    }
}

BOOL OpenGLHooks::MakeCurrent_Hook(HDC hdc, HGLRC hglrc) {
    try {
        spdlog::debug("[OpenGLHooks] wglMakeCurrent called");
        
        // Call original function
        BOOL result = m_original_make_current(hdc, hglrc);
        
        // Post-processing for VR
        if (result) {
            // Notify framework about context change
            if (m_context_callback) {
                m_context_callback(ContextType::MakeCurrent, hdc, hglrc);
            }
        }
        
        return result;
        
    } catch (const std::exception& e) {
        spdlog::error("[OpenGLHooks] Exception in wglMakeCurrent hook: {}", e.what());
        return m_original_make_current(hdc, hglrc);
    }
}

HGLRC OpenGLHooks::CreateContext_Hook(HDC hdc) {
    try {
        spdlog::debug("[OpenGLHooks] wglCreateContext called");
        
        // Call original function
        HGLRC result = m_original_create_context(hdc);
        
        // Post-processing for VR
        if (result) {
            // Notify framework about context creation
            if (m_context_callback) {
                m_context_callback(ContextType::CreateContext, hdc, result);
            }
        }
        
        return result;
        
    } catch (const std::exception& e) {
        spdlog::error("[OpenGLHooks] Exception in wglCreateContext hook: {}", e.what());
        return m_original_create_context(hdc);
    }
}

// Callback setters
void OpenGLHooks::setPresentCallback(PresentCallback callback) {
    m_present_callback = callback;
}

void OpenGLHooks::setDrawCallback(DrawCallback callback) {
    m_draw_callback = callback;
}

void OpenGLHooks::setBeginCallback(BeginCallback callback) {
    m_begin_callback = callback;
}

void OpenGLHooks::setEndCallback(EndCallback callback) {
    m_end_callback = callback;
}

void OpenGLHooks::setContextCallback(ContextCallback callback) {
    m_context_callback = callback;
}

// Utility functions
bool OpenGLHooks::areHooksValid() const {
    return m_hooks_installed && 
           (m_swap_buffers_hook || m_swap_layer_buffers_hook || 
            m_draw_arrays_hook || m_draw_elements_hook ||
            m_gl_begin_hook || m_gl_end_hook ||
            m_make_current_hook || m_create_context_hook);
}

bool OpenGLHooks::restoreHooks() {
    try {
        spdlog::info("[OpenGLHooks] Restoring OpenGL hooks...");
        
        // Remove existing hooks
        removeHooks();
        
        // Reinstall hooks
        return installHooks();
        
    } catch (const std::exception& e) {
        spdlog::error("[OpenGLHooks] Exception during hook restoration: {}", e.what());
        return false;
    }
}

} // namespace Hooks
} // namespace uevr
