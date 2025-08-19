# 🛠️ UEVR Utility Files Analysis
## Complete analysis of all utility files

**Generated:** 2025-08-19 12:20:16  
**Number of files:** 3  
**Status:** ✅ Complete

---

## 📁 ImGui.cpp
**Path:** $filePath  
**Size:** 665 bytes  
**Last modified:** 2025-08-19 11:20:13

### 📥 Includes
```cpp
#include <imgui.h>
```

### 🏗️ Classes
```cpp

```

### ⚙️ Utility Functions
```cpp

```

### ⚙️ General Functions
```cpp

```

### 📄 Complete Content
```cpp
#include <imgui.h>
#include <imgui_internal.h>

#include "ImGui.hpp"

namespace imgui {
bool is_point_intersecting_any(float x, float y) {
    const auto ctx = ImGui::GetCurrentContext();

    if (ctx == nullptr) {
        return false;
    }

    for (int i = 0; i < ctx->Windows.Size; i++) {
        const auto window = ctx->Windows[i];

        if (window->WasActive && window->Active) {
            if (x >= window->Pos.x && x <= window->Pos.x + window->Size.x &&
                y >= window->Pos.y && y <= window->Pos.y + window->Size.y)
            {
                return true;
            }
        }
    }

    return false;
}
}
```

---

## 📁 ImGui.hpp
**Path:** $filePath  
**Size:** 87 bytes  
**Last modified:** 2025-08-19 11:20:13

### 📥 Includes
```cpp

```

### 🏗️ Classes
```cpp

```

### ⚙️ Utility Functions
```cpp

```

### ⚙️ General Functions
```cpp

```

### 📄 Complete Content
```cpp
#pragma once

namespace imgui {
bool is_point_intersecting_any(float x, float y);
}
```

---

## 📁 Logging.hpp
**Path:** $filePath  
**Size:** 1109 bytes  
**Last modified:** 2025-08-19 11:20:13

### 📥 Includes
```cpp
#include <chrono>
```

### 🏗️ Classes
```cpp

```

### ⚙️ Utility Functions
```cpp

```

### ⚙️ General Functions
```cpp

```

### 📄 Complete Content
```cpp
#pragma once

#include <chrono>

#include <spdlog/spdlog.h>

#define SPDLOG_INFO_ONCE(...) {static bool once = true; if (once) { SPDLOG_INFO(__VA_ARGS__); once = false; }}
#define SPDLOG_WARN_ONCE(...) {static bool once = true; if (once) { SPDLOG_WARN(__VA_ARGS__); once = false; }}
#define SPDLOG_ERROR_ONCE(...) {static bool once = true; if (once) { SPDLOG_ERROR(__VA_ARGS__); once = false; }}

#define SPDLOG_INFO_EVERY_N_SEC(n, ...) {static auto last = std::chrono::steady_clock::time_point{}; auto now = std::chrono::steady_clock::now(); if (now - last >= std::chrono::seconds(n)) { SPDLOG_INFO(__VA_ARGS__); last = now; }}
#define SPDLOG_WARNING_EVERY_N_SEC(n, ...) {static auto last = std::chrono::steady_clock::time_point{}; auto now = std::chrono::steady_clock::now(); if (now - last >= std::chrono::seconds(n)) { SPDLOG_WARN(__VA_ARGS__); last = now; }}
#define SPDLOG_ERROR_EVERY_N_SEC(n, ...) {static auto last = std::chrono::steady_clock::time_point{}; auto now = std::chrono::steady_clock::now(); if (now - last >= std::chrono::seconds(n)) { SPDLOG_INFO(__VA_ARGS__); last = now; }}
```

---
