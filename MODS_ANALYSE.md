# ðŸŽ® UEVR Mods Files Analysis
## Komplet analyse af alle mod filer

**Genereret:** 2025-08-19 12:20:11  
**Antal filer:** 12  
**Status:** âœ… Komplet

---

## ðŸ“ FrameworkConfig.cpp
**Sti:** $filePath  
**StÃ¸rrelse:** 2323 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include "Framework.hpp"
`

### ðŸ—ï¸ Classes
`cpp

`

### âš™ï¸ Mod Event Functions
`cpp
on_initialize()
`

### âš™ï¸ General Functions
`cpp
void FrameworkConfig::draw_main()
`

### ðŸ“„ Komplet Indhold
`cpp
#include "Framework.hpp"

#include "FrameworkConfig.hpp"

std::shared_ptr<FrameworkConfig>& FrameworkConfig::get() {
     static std::shared_ptr<FrameworkConfig> instance{std::make_shared<FrameworkConfig>()};
     return instance;
}

std::optional<std::string> FrameworkConfig::on_initialize() {
    return Mod::on_initialize();
}

void FrameworkConfig::draw_main() {
    m_menu_key->draw("Menu Key");
    m_show_cursor_key->draw("Show Cursor Key");
    m_remember_menu_state->draw("Remember Menu Open/Closed State");
    m_enable_l3_r3_toggle->draw("Enable L3 + R3 Toggle");
    ImGui::SameLine();
    m_l3_r3_long_press->draw("L3 + R3 Long Press Menu Toggle");
    m_always_show_cursor->draw("Always Show Cursor");

    ImGui::Separator();
    if (m_log_level->draw("Log Level")) {
        if (m_log_level->value() >= 0 && m_log_level->value() <= spdlog::level::level_enum::n_levels) {
            spdlog::set_level((spdlog::level::level_enum)m_log_level->value());   
        }
    }
}

void FrameworkConfig::draw_themes() {
    get_imgui_theme()->draw("Select GUI Theme");

    if (m_font_size->draw("Font Size")) {
        g_framework->set_font_size(m_font_size->value());
    }
}

void FrameworkConfig::on_draw_sidebar_entry(std::string_view in_entry) {
    on_draw_ui();
    ImGui::Separator();

    if (in_entry == "Main") {
        draw_main();
    } else if (in_entry == "GUI/Themes") {
        draw_themes();
    }
}

void FrameworkConfig::on_frame() {
    if (m_show_cursor_key->is_key_down_once()) {
        m_always_show_cursor->toggle();
    }
}

void FrameworkConfig::on_config_load(const utility::Config& cfg, bool set_defaults) {
    for (IModValue& option : m_options) {
        option.config_load(cfg, set_defaults);
    }

    if (m_remember_menu_state->value()) {
        g_framework->set_draw_ui(m_menu_open->value(), false);
    }
    
    g_framework->set_font_size(m_font_size->value());

    if (m_log_level->value() >= 0 && m_log_level->value() <= spdlog::level::level_enum::n_levels) {
        spdlog::set_level((spdlog::level::level_enum)m_log_level->value());   
    }
}

void FrameworkConfig::on_config_save(utility::Config& cfg) {
    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}

`

---
## ðŸ“ FrameworkConfig.hpp
**Sti:** $filePath  
**StÃ¸rrelse:** 3949 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include <spdlog/common.h>
`

### ðŸ—ï¸ Classes
`cpp
class FrameworkConfig : public Mod {
`

### âš™ï¸ Mod Event Functions
`cpp
on_initialize()
`

### âš™ï¸ General Functions
`cpp
void on_frame()
`

### ðŸ“„ Komplet Indhold
`cpp
#pragma once


#include <spdlog/common.h>
#include "Mod.hpp"

class FrameworkConfig : public Mod {
public:
    static std::shared_ptr<FrameworkConfig>& get();

public:
    FrameworkConfig() {
        m_options = {
            *m_menu_key,
            *m_show_cursor_key,
            *m_menu_open,
            *m_remember_menu_state,
            *m_enable_l3_r3_toggle,
            *m_l3_r3_long_press,
            *m_advanced_mode,
            *m_imgui_theme,
            *m_log_level,
            *m_always_show_cursor,
            *m_font_size,
        };
    }

    std::string_view get_name() const {
        return "FrameworkConfig";
    }

    std::vector<SidebarEntryInfo> get_sidebar_entries() override { 
        return {
                    { "Main", false },
                    { "GUI/Themes", false }
        };
    }

    std::optional<std::string> on_initialize() override;
    void on_frame() override;
    void on_config_load(const utility::Config& cfg, bool set_defaults) override;
    void on_config_save(utility::Config& cfg) override;
    void on_draw_sidebar_entry(std::string_view in_entry) override;

    void draw_themes();
    void draw_main();

    auto& get_menu_key() {
        return m_menu_key;
    }

    auto& get_menu_open() {
        return m_menu_open;
    }

    bool is_remember_menu_state() {
        return m_remember_menu_state->value();
    }

    bool is_enable_l3_r3_toggle() {
        return m_enable_l3_r3_toggle->value();
    }

    bool is_l3_r3_long_press() {
        return m_l3_r3_long_press->value();
    }

    bool is_always_show_cursor() const {
        return m_always_show_cursor->value();
    }

    bool is_advanced_mode() const {
        return m_advanced_mode->value();
    }

    void toggle_advanced_mode() const {
        m_advanced_mode->toggle();
    }

    auto& get_advanced_mode() const {
        return m_advanced_mode;
    }

    auto& get_imgui_theme_value() const {
        return m_imgui_theme->value();
    }

    auto& get_imgui_theme() const {
        return m_imgui_theme;
    }

    int32_t get_font_size() const {
        return m_font_size->value();
    }

    spdlog::level::level_enum get_log_level() const {
        return (spdlog::level::level_enum)m_log_level->value();
    }

private:
    static const inline std::vector<std::string> s_imgui_themes {
        "Default Dark",
        "Alternative Dark",
        "Default Light",
        "High Contrast",
    };

    static inline std::vector<std::string> s_get_log_levels() {
        std::vector<std::string> log_levels{};
        for (auto& level : SPDLOG_LEVEL_NAMES) {
            log_levels.emplace_back(level.data());
        }

        return log_levels;
    };
    
    ModKey::Ptr m_menu_key{ ModKey::create(generate_name("MenuKey"), VK_INSERT) };
    ModToggle::Ptr m_menu_open{ ModToggle::create(generate_name("MenuOpen"), true) };
    ModToggle::Ptr m_remember_menu_state{ ModToggle::create(generate_name("RememberMenuState"), false) };
    ModToggle::Ptr m_enable_l3_r3_toggle{ ModToggle::create(generate_name("EnableL3R3Toggle"), true) };
    ModToggle::Ptr m_l3_r3_long_press{ ModToggle::create(generate_name("L3R3LongPress"), false) };
    ModToggle::Ptr m_always_show_cursor{ ModToggle::create(generate_name("AlwaysShowCursor"), false) };
    ModToggle::Ptr m_advanced_mode{ ModToggle::create(generate_name("AdvancedMode"), false) };
    
    ModCombo::Ptr m_imgui_theme{ ModCombo::create(generate_name("ImGuiTheme"), s_imgui_themes, Framework::ImGuiThemes::DEFAULT_DARK) };
    ModCombo::Ptr m_log_level{ ModCombo::create(generate_name("LogLevel"), s_get_log_levels(), spdlog::level::info) };
    
    ModKey::Ptr m_show_cursor_key{ ModKey::create(generate_name("ShowCursorKey")) };
    ModInt32::Ptr m_font_size{ModInt32::create(generate_name("FontSize"), 16)};
};

`

---
## ðŸ“ ImGuiThemeHelpers.cpp
**Sti:** $filePath  
**StÃ¸rrelse:** 16471 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include "ImGuiThemeHelpers.hpp"
`

### ðŸ—ï¸ Classes
`cpp

`

### âš™ï¸ Mod Event Functions
`cpp

`

### âš™ï¸ General Functions
`cpp
void ImGuiThemeHelper::StyleColorsDefaultDark()
`

### ðŸ“„ Komplet Indhold
`cpp
#include "ImGuiThemeHelpers.hpp"
#include <imgui.h>


void ImGuiThemeHelper::StyleColorsDefaultDark() {
    
    ImGui::StyleColorsDark();
    
    // Unreal style by dev0-1 from ImThemes
    auto& style = ImGui::GetStyle();
    style.Alpha = 1.0f;
    style.DisabledAlpha = 1.0f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.WindowRounding = 0.0f;
    style.WindowBorderSize = 1.0f;
    style.WindowMinSize = ImVec2(32.0f, 32.0f);
    style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ChildRounding = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 0.0f;
    style.PopupBorderSize = 1.0f;
    style.FramePadding = ImVec2(4.0f, 3.0f);
    style.FrameRounding = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.CellPadding = ImVec2(4.0f, 2.0f);
    style.IndentSpacing = 21.0f;
    style.ColumnsMinSpacing = 6.0f;
    style.ScrollbarSize = 14.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabMinSize = 10.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 4.0f;
    style.TabBorderSize = 0.0f;
    style.TabMinWidthForCloseButton = 0.0f;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    auto& colors = style.Colors;
    
    colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.4980392158031464f, 0.4980392158031464f, 0.4980392158031464f, 1.0f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.05882352963089943f, 0.05882352963089943f, 0.05882352963089943f, 1.0f);
    colors[ImGuiCol_ChildBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.0784313753247261f, 0.0784313753247261f, 0.0784313753247261f, 0.9399999976158142f);
    colors[ImGuiCol_Border] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.2000000029802322f, 0.2078431397676468f, 0.2196078449487686f, 0.9400000214576721f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4000000059604645f, 0.4000000059604645f, 0.4000000059604645f, 0.4000000059604645f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.1764705926179886f, 0.1764705926179886f, 0.1764705926179886f, 1.0f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.03921568766236305f, 0.03921568766236305f, 0.03921568766236305f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.2862745225429535f, 0.2862745225429535f, 0.2862745225429535f, 1.0f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5099999904632568f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.1372549086809158f, 0.1372549086809158f, 0.1372549086809158f, 1.0f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.5299999713897705f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3098039329051971f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.407843142747879f, 0.407843142747879f, 0.407843142747879f, 1.0f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.9372549057006836f, 0.9372549057006836f, 0.9372549057006836f, 1.0f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.8588235378265381f, 0.8588235378265381f, 0.8588235378265381f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(0.4392156898975372f, 0.4392156898975372f, 0.4392156898975372f, 0.4000000059604645f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.4588235318660736f, 0.4666666686534882f, 0.47843137383461f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.4196078479290009f, 0.4196078479290009f, 0.4196078479290009f, 1.0f);
    colors[ImGuiCol_Header] = ImVec4(0.6980392336845398f, 0.6980392336845398f, 0.6980392336845398f, 0.3100000023841858f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.6980392336845398f, 0.6980392336845398f, 0.6980392336845398f, 0.800000011920929f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.47843137383461f, 0.4980392158031464f, 0.5176470875740051f, 1.0f);
    colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.7176470756530762f, 0.7176470756530762f, 0.7176470756530762f, 0.7799999713897705f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.5098039507865906f, 0.5098039507865906f, 0.5098039507865906f, 1.0f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.9098039269447327f, 0.9098039269447327f, 0.9098039269447327f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.8078431487083435f, 0.8078431487083435f, 0.8078431487083435f, 0.6700000166893005f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.4588235318660736f, 0.4588235318660736f, 0.4588235318660736f, 0.949999988079071f);
    colors[ImGuiCol_Tab] = ImVec4(0.1764705926179886f, 0.3490196168422699f, 0.5764706134796143f, 0.8619999885559082f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.800000011920929f);
    colors[ImGuiCol_TabActive] = ImVec4(0.196078434586525f, 0.407843142747879f, 0.6784313917160034f, 1.0f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.1019607856869698f, 0.1450980454683304f, 0.9724000096321106f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1333333402872086f, 0.2588235437870026f, 0.4235294163227081f, 1.0f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.729411780834198f, 0.6000000238418579f, 0.1490196138620377f, 1.0f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.8666666746139526f, 0.8666666746139526f, 0.8666666746139526f, 0.3499999940395355f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.6000000238418579f, 0.6000000238418579f, 0.6000000238418579f, 1.0f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.3499999940395355f);
}

void ImGuiThemeHelper::StyleColorsAlternativeDark() {
    
    ImGui::StyleColorsDark();
    auto& style = ImGui::GetStyle();
    
    style.WindowRounding = 0.0f;
    style.ChildRounding = 0.0f;
    style.PopupRounding = 0.0f;
    style.FrameRounding = 0.0f;
    style.ScrollbarRounding = 2.0f;
    style.GrabRounding = 0.0f;
    style.TabRounding = 0.0f;
    style.WindowBorderSize = 2.0f;
    style.WindowPadding = ImVec2(2.0f, 0.0f);

    auto& colors = style.Colors;
    
    // Window BG
    colors[ImGuiCol_WindowBg] = ImVec4{0.1f, 0.105f, 0.11f, 1.0f};

    // Navigatation highlight
    colors[ImGuiCol_NavHighlight] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};

    // Progress Bar
    colors[ImGuiCol_PlotHistogram] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};

    // Headers
    colors[ImGuiCol_Header] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_HeaderHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_HeaderActive] = ImVec4{0.55f, 0.5505f, 0.551f, 1.0f};

    // Buttons
    colors[ImGuiCol_Button] = ImVec4{0.2f, 0.205f, 0.21f, 1.0f};
    colors[ImGuiCol_ButtonHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_ButtonActive] = ImVec4{0.55f, 0.5505f, 0.551f, 1.0f};

    // Checkbox
    colors[ImGuiCol_CheckMark] = ImVec4(0.55f, 0.5505f, 0.551f, 1.0f);

    // Frame BG
    colors[ImGuiCol_FrameBg] = ImVec4{0.211f, 0.210f, 0.25f, 1.0f};
    colors[ImGuiCol_FrameBgHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_FrameBgActive] = ImVec4{0.55f, 0.5505f, 0.551f, 1.0f};

    // Tabs
    colors[ImGuiCol_Tab] = ImVec4{0.25f, 0.2505f, 0.251f, 1.0f};
    colors[ImGuiCol_TabHovered] = ImVec4{0.38f, 0.3805f, 0.381f, 1.0f};
    colors[ImGuiCol_TabActive] = ImVec4{0.28f, 0.2805f, 0.281f, 1.0f};
    colors[ImGuiCol_TabUnfocused] = ImVec4{0.25f, 0.2505f, 0.251f, 1.0f};
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{0.8f, 0.805f, 0.81f, 1.0f};

    // Resize Grip
    colors[ImGuiCol_ResizeGrip] = ImVec4{0.2f, 0.205f, 0.21f, 0.0f};
    colors[ImGuiCol_ResizeGripHovered] = ImVec4{0.3f, 0.305f, 0.31f, 1.0f};
    colors[ImGuiCol_ResizeGripActive] = ImVec4{0.55f, 0.5505f, 0.551f, 1.0f};

    // Title
    colors[ImGuiCol_TitleBg] = ImVec4{0.25f, 0.2505f, 0.251f, 1.0f};
    colors[ImGuiCol_TitleBgActive] = ImVec4{0.55f, 0.5505f, 0.551f, 1.0f};
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{0.25f, 0.2505f, 0.251f, 1.0f};
}

void ImGuiThemeHelper::StyleColorsDefaultLight() {
    
    ImGui::StyleColorsLight();
}

void ImGuiThemeHelper::StyleColorsHighContrast() {
    
    ImGui::StyleColorsDark();
    auto& style = ImGui::GetStyle();
    
    // Enemymouse style by enemymouse from ImThemes
    style.Alpha = 1.0f;
    style.DisabledAlpha = 0.6000000238418579f;
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.WindowRounding = 3.0f;
    style.WindowBorderSize = 1.0f;
    style.WindowMinSize = ImVec2(32.0f, 32.0f);
    style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;
    style.ChildRounding = 3.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupRounding = 0.0f;
    style.PopupBorderSize = 1.0f;
    style.FramePadding = ImVec2(4.0f, 3.0f);
    style.FrameRounding = 3.0f;
    style.FrameBorderSize = 0.0f;
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.CellPadding = ImVec2(4.0f, 2.0f);
    style.IndentSpacing = 21.0f;
    style.ColumnsMinSpacing = 6.0f;
    style.ScrollbarSize = 14.0f;
    style.ScrollbarRounding = 9.0f;
    style.GrabMinSize = 20.0f;
    style.GrabRounding = 1.0f;
    style.TabRounding = 4.0f;
    style.TabBorderSize = 0.0f;
    style.TabMinWidthForCloseButton = 0.0f;
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

    auto& colors = style.Colors;
    
    colors[ImGuiCol_Text] = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.0f, 0.4000000059604645f, 0.407843142747879f, 1.0f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.1568627506494522f, 0.239215686917305f, 0.2196078449487686f, 1.0f);
    colors[ImGuiCol_Border] = ImVec4(0.0f, 1.0f, 1.0f, 0.6499999761581421f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.4392156898975372f, 0.800000011920929f, 0.800000011920929f, 0.1800000071525574f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4392156898975372f, 0.800000011920929f, 0.800000011920929f, 0.2700000107288361f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.4392156898975372f, 0.8078431487083435f, 0.8588235378265381f, 0.6600000262260437f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.1372549086809158f, 0.1764705926179886f, 0.2078431397676468f, 0.7300000190734863f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 1.0f, 1.0f, 0.2700000107288361f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.5400000214576721f);
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.2000000029802322f);
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.2196078449487686f, 0.2862745225429535f, 0.2980392277240753f, 0.7099999785423279f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.0f, 1.0f, 1.0f, 0.4399999976158142f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.0f, 1.0f, 1.0f, 0.7400000095367432f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1.0f, 1.0f, 0.6800000071525574f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.0f, 1.0f, 1.0f, 0.3600000143051147f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.0f, 1.0f, 1.0f, 0.7599999904632568f);
    colors[ImGuiCol_Button] = ImVec4(0.0f, 0.6470588445663452f, 0.6470588445663452f, 0.4600000083446503f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.007843137718737125f, 1.0f, 1.0f, 0.4300000071525574f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 1.0f, 1.0f, 0.6200000047683716f);
    colors[ImGuiCol_Header] = ImVec4(0.0f, 1.0f, 1.0f, 0.3300000131130219f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.0f, 1.0f, 1.0f, 0.4199999868869781f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.0f, 1.0f, 1.0f, 0.5400000214576721f);
    colors[ImGuiCol_Separator] = ImVec4(0.0f, 0.4980392158031464f, 0.4980392158031464f, 0.3300000131130219f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.0f, 0.4980392158031464f, 0.4980392158031464f, 0.4699999988079071f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.0f, 0.6980392336845398f, 0.6980392336845398f, 1.0f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.0f, 1.0f, 1.0f, 0.5400000214576721f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.0f, 1.0f, 1.0f, 0.7400000095367432f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_Tab] = ImVec4(0.1764705926179886f, 0.3490196168422699f, 0.5764706134796143f, 0.8619999885559082f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 0.800000011920929f);
    colors[ImGuiCol_TabActive] = ImVec4(0.196078434586525f, 0.407843142747879f, 0.6784313917160034f, 1.0f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667014360428f, 0.1019607856869698f, 0.1450980454683304f, 0.9724000096321106f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.1333333402872086f, 0.2588235437870026f, 0.4235294163227081f, 1.0f);
    colors[ImGuiCol_PlotLines] = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882352977991104f, 0.1882352977991104f, 0.2000000029802322f, 1.0f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.3098039329051971f, 0.3098039329051971f, 0.3490196168422699f, 1.0f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.2274509817361832f, 0.2274509817361832f, 0.2470588237047195f, 1.0f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.05999999865889549f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.0f, 1.0f, 1.0f, 0.2199999988079071f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
    colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.699999988079071f);
    colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.800000011920929f, 0.800000011920929f, 0.800000011920929f, 0.2000000029802322f);
    colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.03921568766236305f, 0.09803921729326248f, 0.08627451211214066f, 0.5099999904632568f);
}

`

---
## ðŸ“ ImGuiThemeHelpers.hpp
**Sti:** $filePath  
**StÃ¸rrelse:** 233 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp

`

### ðŸ—ï¸ Classes
`cpp
class ImGuiThemeHelper {
`

### âš™ï¸ Mod Event Functions
`cpp

`

### âš™ï¸ General Functions
`cpp
void StyleColorsDefaultDark()
`

### ðŸ“„ Komplet Indhold
`cpp
#pragma once

class ImGuiThemeHelper {
public:
    static void StyleColorsDefaultDark();
    static void StyleColorsAlternativeDark();
    static void StyleColorsDefaultLight();
    static void StyleColorsHighContrast();
};

`

---
## ðŸ“ LuaLoader.cpp
**Sti:** $filePath  
**StÃ¸rrelse:** 11690 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include <cstdint>
`

### ðŸ—ï¸ Classes
`cpp

`

### âš™ï¸ Mod Event Functions
`cpp
on_initialize_d3d_thread()
`

### âš™ï¸ General Functions
`cpp
void LuaLoader::on_config_load(const utility::Config& cfg, bool set_defaults)
`

### ðŸ“„ Komplet Indhold
`cpp
#include <cstdint>
#include <filesystem>

#include "Framework.hpp"
#include "PluginLoader.hpp"
#include "LuaLoader.hpp"

#include <sdk/threading/GameThreadWorker.hpp>

#include <lstate.h> // weird include order because of sol
#include <lgc.h>

#include "bindings/ImGui.hpp"
#include "bindings/FS.hpp"
#include "bindings/Json.hpp"

std::shared_ptr<LuaLoader>& LuaLoader::get() {
    static auto instance = std::make_shared<LuaLoader>();
    return instance;
}

std::optional<std::string> LuaLoader::on_initialize_d3d_thread() {
    // TODO?
    return Mod::on_initialize_d3d_thread();
}

void LuaLoader::on_config_load(const utility::Config& cfg, bool set_defaults) {
    std::scoped_lock _{m_access_mutex};

    for (IModValue& option : m_options) {
        option.config_load(cfg, set_defaults);
    }

    if (m_main_state != nullptr) {
        m_main_state->gc_data_changed(make_gc_data());
    }
}

void LuaLoader::on_config_save(utility::Config& cfg) {
    std::scoped_lock _{m_access_mutex};

    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }


    // TODO: Add config save callback to ScriptState
    if (m_main_state != nullptr) {
        //m_main_state->on_config_save();
    }
}

void LuaLoader::on_frame() {
    // Only run on the game thread
    // on_frame can sometimes run in the DXGI thread, this happens
    // before tick is hooked, which is where the game thread is.
    // once tick is hooked, on_frame will always run on the game thread.
    if (!GameThreadWorker::get().is_same_thread()) {
        return;
    }

    std::scoped_lock _{m_access_mutex};

    if (m_needs_first_reset) {
        spdlog::info("[LuaLoader] Initializing Lua state for the first time...");

        // Calling reset_scripts even though the scripts have never been set yet still works.
        reset_scripts();
        m_needs_first_reset = false;

        spdlog::info("[LuaLoader] Lua state initialized.");
    }

    for (auto state_to_delete : m_states_to_delete) {
        std::erase_if(m_states, [&](std::shared_ptr<ScriptState> state) { return state->lua().lua_state() == state_to_delete; });
    }

    m_states_to_delete.clear();

    if (m_main_state == nullptr) {
        return;
    }

    for (auto &state : m_states) {
        state->on_frame();
    }
}

void LuaLoader::on_draw_sidebar_entry(std::string_view in_entry) {
    if (in_entry == "Main") {
        if (ImGui::Button("Run script")) {
            OPENFILENAME ofn{};
            char file[260]{};

            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = g_framework->get_window();
            ofn.lpstrFile = file;
            ofn.nMaxFile = sizeof(file);
            ofn.lpstrFilter = "Lua script files (*.lua)\0*.lua\0";
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

            if (GetOpenFileName(&ofn) != FALSE) {
                std::scoped_lock _{ m_access_mutex };
                m_main_state->run_script(file);
                m_loaded_scripts.emplace_back(std::filesystem::path{file}.filename().string());
            }
        }

        ImGui::SameLine();

        if (ImGui::Button("Reset scripts")) {
            reset_scripts();
        }

        ImGui::SameLine();

        if (ImGui::Button("Spawn Debug Console")) {
            if (!m_console_spawned) {
                AllocConsole();
                freopen("CONIN$", "r", stdin);
                freopen("CONOUT$", "w", stdout);
                freopen("CONOUT$", "w", stderr);

                m_console_spawned = true;
            }
        }

        //Garbage collection currently only showing from main lua state, might rework to show total later?
        if (ImGui::TreeNode("Garbage Collection Stats")) {
            std::scoped_lock _{ m_access_mutex };

            auto g = G(m_main_state->lua().lua_state());
            const auto bytes_in_use = g->totalbytes + g->GCdebt;

            ImGui::Text("Megabytes in use: %.2f", (float)bytes_in_use / 1024.0f / 1024.0f);

            ImGui::TreePop();
        }

        if (m_gc_handler->draw("Garbage Collection Handler")) {
            std::scoped_lock _{ m_access_mutex };
            m_main_state->gc_data_changed(make_gc_data());
        }

        if (m_gc_mode->draw("Garbage Collection Mode")) {
            std::scoped_lock _{ m_access_mutex };
            m_main_state->gc_data_changed(make_gc_data());
        }

        if ((uint32_t)m_gc_mode->value() == (uint32_t)ScriptState::GarbageCollectionMode::GENERATIONAL) {
            if (m_gc_minor_multiplier->draw("Minor GC Multiplier")) {
                std::scoped_lock _{ m_access_mutex };
                m_main_state->gc_data_changed(make_gc_data());
            }

            if (m_gc_major_multiplier->draw("Major GC Multiplier")) {
                std::scoped_lock _{ m_access_mutex };
                m_main_state->gc_data_changed(make_gc_data());
            }
        }

        if (m_gc_handler->value() == (int32_t)ScriptState::GarbageCollectionHandler::UEVR_MANAGED) {
            if (m_gc_type->draw("Garbage Collection Type")) {
                std::scoped_lock _{ m_access_mutex };
                m_main_state->gc_data_changed(make_gc_data());
            }

            if ((uint32_t)m_gc_mode->value() != (uint32_t)ScriptState::GarbageCollectionMode::GENERATIONAL) {
                if (m_gc_budget->draw("Garbage Collection Budget")) {
                    std::scoped_lock _{ m_access_mutex };
                    m_main_state->gc_data_changed(make_gc_data());
                }
            }
        }

        m_log_to_disk->draw("Log Lua Errors to Disk");

        auto last_script_error = m_main_state != nullptr ? m_main_state->get_last_script_error() : std::nullopt;

        if (last_script_error.has_value() && !last_script_error->e.empty()) {
            const auto now = std::chrono::system_clock::now();
            const auto diff = now - last_script_error->t;
            const auto sec = std::chrono::duration<float>(diff).count();

            ImGui::TextWrapped("Last Error Time: %.2f seconds ago", sec);

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::TextWrapped("Last Script Error: %s", last_script_error->e.c_str());
            ImGui::PopStyleColor();
        } else {
            ImGui::TextWrapped("No Script Errors... yet!");
        }

        if (!m_known_scripts.empty()) {
            ImGui::Text("Known scripts:");

            for (auto&& name : m_known_scripts) {
                if (ImGui::Checkbox(name.data(), &m_loaded_scripts_map[name])) {
                    reset_scripts();
                    break;
                }
            }
        } else {
            ImGui::Text("No scripts loaded.");
        }

        ImGui::TreePop();
    }


    if (in_entry == "Script UI") {
        std::scoped_lock _{m_access_mutex};

        if (m_states.empty()) {
            return;
        }

        for (auto& state : m_states) {
            state->on_draw_ui();
        }

        ImGui::TreePop();
    }

    {
        std::scoped_lock __{ m_access_mutex };

        for (auto& entry : m_script_panels) {
            if (in_entry != entry.name) {
                continue;
            }

            if (entry.state.expired()) {
                continue;
            }
    
            auto state = entry.state.lock();
            if (state == nullptr) {
                continue;
            }
    
            std::scoped_lock _{state->context()->get_mutex()};
    
            try {
                state->context()->handle_protected_result(entry.fn());
            } catch (const std::exception& e) {
                state->context()->log_error(std::format("[LuaLoader] Exception in script panel {}: {}", entry.name, e.what()));
            } catch (...) {
                state->context()->log_error(std::format("[LuaLoader] Unknown exception in script panel {}", entry.name));
            }
        }
    }
}

void LuaLoader::reset_scripts() {
    spdlog::info("[LuaLoader] Resetting scripts...");

    std::scoped_lock _{ m_access_mutex };

    if (m_main_state != nullptr) {
        /*auto& mods = g_framework->get_mods()->get_mods();

        for (auto& mod : mods) {
            mod->on_lua_state_destroyed(m_main_state->lua());
        }*/

        m_main_state->on_script_reset();
    }

    m_main_state.reset();
    m_states.clear();
    m_script_panels.clear();

    spdlog::info("[LuaLoader] Destroyed all Lua states.");

    m_main_state = std::make_shared<ScriptState>(make_gc_data(), &g_plugin_initialize_param, true);
    m_states.insert(m_states.begin(), m_main_state);

    for (auto& state : m_states) {
        state_post_init(state);
    }

    //callback functions for main lua state creation
    /*auto& mods = g_framework->get_mods()->get_mods();
    for (auto& mod : mods) {
        mod->on_lua_state_created(m_main_state->lua());
    }*/

    m_loaded_scripts.clear();
    m_known_scripts.clear();

    const auto autorun_path = Framework::get_persistent_dir() / "scripts";
    const auto global_autorun_path = Framework::get_persistent_dir()  / ".." / "UEVR" / "scripts";

    spdlog::info("[LuaLoader] Creating directories {}", autorun_path.string());
    std::filesystem::create_directories(autorun_path);
    spdlog::info("[LuaLoader] Loading scripts...");
    namespace fs = std::filesystem;

	auto load_scripts_from_dir = [this](std::filesystem::path path) {
        if (!fs::exists(path) || !fs::is_directory(path)) {
            return;
        }

		for (auto&& entry : std::filesystem::directory_iterator{path}) {
			auto&& path = entry.path();

			if (path.has_extension() && path.extension() == ".lua") {
				if (!m_loaded_scripts_map.contains(path.filename().string())) {
					m_loaded_scripts_map.emplace(path.filename().string(), true);
				}

				if (m_loaded_scripts_map[path.filename().string()] == true) {
					m_main_state->run_script(path.string());
					m_loaded_scripts.emplace_back(path.filename().string());
				}

				m_known_scripts.emplace_back(path.filename().string());
			}
		}
	};

    load_scripts_from_dir(global_autorun_path);
    load_scripts_from_dir(autorun_path);
    std::sort(m_known_scripts.begin(), m_known_scripts.end());
    std::sort(m_loaded_scripts.begin(), m_loaded_scripts.end());
}

void LuaLoader::state_post_init(std::shared_ptr<ScriptState>& state) {
    std::scoped_lock _{state->context()->get_mutex()};
    auto& lua = state->lua();
    auto lua_table = lua.create_table();

    // TODO: Rework this to be within add_additional_bindings somehow without a weak_ptr
    lua_table["add_script_panel"] = [this, &state](sol::this_state s, std::string name, sol::function fn) {
        m_script_panels.emplace_back(PanelEntry{
            .state = std::weak_ptr<ScriptState>{state},
            .name = name,
            .fn = fn
        });
    };

    lua["uevr"]["lua"] = lua_table;
}

void LuaLoader::add_additional_bindings(sol::state_view& lua) {
    bindings::open_imgui(lua);
    bindings::open_json(lua);
    bindings::open_fs(lua);
}

void LuaLoader::dispatch_event(std::string_view event_name, std::string_view event_data) {
    std::scoped_lock _{m_access_mutex};

    if (m_main_state == nullptr) {
        return;
    }

    m_main_state->dispatch_event(event_name, event_data);
}
`

---
## ðŸ“ LuaLoader.hpp
**Sti:** $filePath  
**StÃ¸rrelse:** 4976 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include <deque>
`

### ðŸ—ï¸ Classes
`cpp
class LuaLoader : public Mod {
`

### âš™ï¸ Mod Event Functions
`cpp
on_initialize_d3d_thread()
`

### âš™ï¸ General Functions
`cpp
void on_draw_sidebar_entry(std::string_view in_entry)
`

### ðŸ“„ Komplet Indhold
`cpp
#pragma once

#include <deque>
#include <vector>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <shared_mutex>

#include "ScriptContext.hpp"
#include "ScriptState.hpp"

#include "Mod.hpp"

using namespace uevr;

class LuaLoader : public Mod {
public:
    static std::shared_ptr<LuaLoader>& get();

    std::string_view get_name() const override { return "LuaLoader"; }
    bool is_advanced_mod() const override { return true; }
    std::optional<std::string> on_initialize_d3d_thread() override;

    std::vector<SidebarEntryInfo> get_sidebar_entries() override {
        if (m_script_panels.empty()) {
            return {
                {"Main", true},
                {"Script UI", true}
            };
        }

        std::vector<SidebarEntryInfo> entries{
            {"Main", true},
            {"Script UI", true}
        };

        for (auto& entry : m_script_panels) {
            entries.emplace_back(entry.name, true);
        }

        return entries;
    }

    void on_draw_sidebar_entry(std::string_view in_entry);
    void on_frame() override;

    void on_config_load(const utility::Config& cfg, bool set_defaults) override;
    void on_config_save(utility::Config& cfg) override;


    const auto& get_state() {
        return m_main_state;
    }

    const auto& get_state(int index) { 
        return m_states[index];
    }

    std::scoped_lock<std::recursive_mutex> get_access_lock() {
        return std::scoped_lock<std::recursive_mutex>{m_access_mutex};
    }

    // Resets the ScriptState and runs autorun scripts again.
    void reset_scripts();
    void state_post_init(std::shared_ptr<ScriptState>& state);
    void add_additional_bindings(sol::state_view& lua);
    void dispatch_event(std::string_view event_name, std::string_view event_data);

private:
    ScriptState::GarbageCollectionData make_gc_data() const {
        ScriptState::GarbageCollectionData data{};

        data.gc_handler = (decltype(ScriptState::GarbageCollectionData::gc_handler))m_gc_handler->value();
        data.gc_type = (decltype(ScriptState::GarbageCollectionData::gc_type))m_gc_type->value();
        data.gc_mode = (decltype(ScriptState::GarbageCollectionData::gc_mode))m_gc_mode->value();
        data.gc_budget = std::chrono::microseconds{(uint32_t)m_gc_budget->value()};
        data.gc_minor_multiplier = (uint32_t)m_gc_minor_multiplier->value();
        data.gc_major_multiplier = (uint32_t)m_gc_major_multiplier->value();

        return data;
    }

    std::shared_ptr<ScriptState> m_main_state{};
    std::vector<std::shared_ptr<ScriptState>> m_states{};
    std::recursive_mutex m_access_mutex{};

    // A list of Lua files that have been explicitly loaded either through the user manually loading the script, or
    // because the script was in the autorun directory.
    std::vector<std::string> m_loaded_scripts{};
    std::vector<std::string> m_known_scripts{};
    std::unordered_map<std::string, bool> m_loaded_scripts_map{};
    std::vector<lua_State*> m_states_to_delete{};
    struct PanelEntry {
        std::weak_ptr<uevr::ScriptState> state;
        std::string name;
        sol::protected_function fn;
    };
    std::vector<PanelEntry> m_script_panels{};

    bool m_console_spawned{false};
    bool m_needs_first_reset{true};

    const ModToggle::Ptr m_log_to_disk{ ModToggle::create(generate_name("LogToDisk"), false) };

    const ModCombo::Ptr m_gc_handler { 
        ModCombo::create(generate_name("GarbageCollectionHandler"),
        {
            "Managed by UEVR",
            "Managed by Lua"
        }, (int)ScriptState::GarbageCollectionHandler::UEVR_MANAGED)
    };

    const ModCombo::Ptr m_gc_type {
        ModCombo::create(generate_name("GarbageCollectionType"),
        {
            "Step",
            "Full",
        }, (int)ScriptState::GarbageCollectionType::STEP)
    };

    const ModCombo::Ptr m_gc_mode {
        ModCombo::create(generate_name("GarbageCollectionMode"),
        {
            "Generational",
            "Incremental (Mark & Sweep)",
        }, (int)ScriptState::GarbageCollectionMode::GENERATIONAL)
    };

    // Garbage collection budget in microseconds.
    const ModSlider::Ptr m_gc_budget {
        ModSlider::create(generate_name("GarbageCollectionBudget"), 0.0f, 2000.0f, 1000.0f)
    };

    const ModSlider::Ptr m_gc_minor_multiplier {
        ModSlider::create(generate_name("GarbageCollectionMinorMultiplier"), 1.0f, 200.0f, 1.0f)
    };

    const ModSlider::Ptr m_gc_major_multiplier {
        ModSlider::create(generate_name("GarbageCollectionMajorMultiplier"), 1.0f, 1000.0f, 100.0f)
    };

    ValueList m_options{
        *m_log_to_disk,
        *m_gc_handler,
        *m_gc_type,
        *m_gc_mode,
        *m_gc_budget,
        *m_gc_minor_multiplier,
        *m_gc_major_multiplier
    };
};
`

---
## ðŸ“ PluginLoader.cpp
**Sti:** $filePath  
**StÃ¸rrelse:** 75014 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include <filesystem>
`

### ðŸ—ï¸ Classes
`cpp
ClassHandle klass, UEVR_UObjectHandle outer) -> UEVR_UObjectHandle {
`

### âš™ï¸ Mod Event Functions
`cpp
on_present(UEVR_OnPresentCb cb)
`

### âš™ï¸ General Functions
`cpp
void log_error(const char* format, ...)
`

### ðŸ“„ Komplet Indhold
`cpp
#include <filesystem>

#include <imgui.h>
#include <spdlog/spdlog.h>

#include <openvr.h>

#include "Framework.hpp"
#include "CommitHash.autogenerated"
#include "uevr/API.h"

#include <utility/String.hpp>
#include <utility/Module.hpp>

#include <sdk/UEngine.hpp>
#include <sdk/CVar.hpp>
#include <sdk/UObjectArray.hpp>
#include <sdk/UObject.hpp>
#include <sdk/FField.hpp>
#include <sdk/FProperty.hpp>
#include <sdk/UFunction.hpp>
#include <sdk/UGameplayStatics.hpp>
#include <sdk/APlayerController.hpp>
#include <sdk/USceneComponent.hpp>
#include <sdk/FArrayProperty.hpp>
#include <sdk/FBoolProperty.hpp>
#include <sdk/FStructProperty.hpp>
#include <sdk/FEnumProperty.hpp>
#include <sdk/AActor.hpp>
#include <sdk/UGameViewportClient.hpp>

#include "pluginloader/FFakeStereoRenderingFunctions.hpp"
#include "pluginloader/FRenderTargetPoolHook.hpp"
#include "pluginloader/FRHITexture2DFunctions.hpp"
#include "pluginloader/FUObjectArrayFunctions.hpp"
#include "pluginloader/UScriptStructFunctions.hpp"

#include "LuaLoader.hpp"
#include "UObjectHook.hpp"
#include "VR.hpp"

#include "Mods.hpp"
#include "PluginLoader.hpp"

UEVR_PluginVersion g_plugin_version{
    UEVR_PLUGIN_VERSION_MAJOR, UEVR_PLUGIN_VERSION_MINOR, UEVR_PLUGIN_VERSION_PATCH};

namespace uevr {
UEVR_RendererData g_renderer_data{
    UEVR_RENDERER_D3D12, nullptr, nullptr, nullptr
};
}

namespace uevr {
void log_error(const char* format, ...) {
    va_list args{};
    va_start(args, format);
    auto str = utility::format_string(format, args);
    va_end(args);
    spdlog::error("[Plugin] {}", str);
}
void log_warn(const char* format, ...) {
    va_list args{};
    va_start(args, format);
    auto str = utility::format_string(format, args);
    va_end(args);
    spdlog::warn("[Plugin] {}", str);
}
void log_info(const char* format, ...) {
    va_list args{};
    va_start(args, format);
    auto str = utility::format_string(format, args);
    va_end(args);
    spdlog::info("[Plugin] {}", str);
}
bool is_drawing_ui() {
    return g_framework->is_drawing_ui();
}
bool remove_callback(void* cb) {
    return PluginLoader::get()->remove_callback(cb);
}

unsigned int get_persistent_dir(wchar_t* buffer, unsigned int buffer_size) {
    const auto path = g_framework->get_persistent_dir().wstring();
    if (buffer == nullptr || buffer_size == 0) {
        return (unsigned int)path.size();
    }

    const auto size = std::min<size_t>(path.size(), (size_t)buffer_size - 1);
    memcpy(buffer, path.c_str(), size * sizeof(wchar_t));
    buffer[size] = L'\0';

    return (unsigned int)size;
}

int register_inline_hook(void* target, void* dst, void** original) {
    if (target == nullptr || dst == nullptr || original == nullptr) {
        return -1;
    }

    auto hook = safetyhook::create_inline(target, dst, safetyhook::InlineHook::StartDisabled);

    if (!hook) {
        spdlog::error("Failed to create inline hook at {:x}", (uintptr_t)target);
        return -1;
    }

    // Plugin needs to be aware of this before enabling the hook.
    *original = hook.original<void*>();

    if (auto enable_result = hook.enable(); !enable_result.has_value()) {
        spdlog::error("Failed to enable inline hook at {:x}: {}", (uintptr_t)target, enable_result.error().type);
        return -1;
    }

    return PluginLoader::get()->add_inline_hook(std::move(hook));
}

void unregister_inline_hook(int id) {
    PluginLoader::get()->remove_inline_hook(id);
}

void dispatch_lua_event(const char* event_name, const char* event_data) {
    LuaLoader::get()->dispatch_event(event_name, event_data);
}

void dispatch_custom_event(const char* event_name, const char* event_data) {
    PluginLoader::get()->dispatch_custom_event(event_name, event_data);
}
}

namespace uevr {
bool on_present(UEVR_OnPresentCb cb) {
    if (cb == nullptr) {
        return false;
    }

    return PluginLoader::get()->add_on_present(cb);
}

bool on_device_reset(UEVR_OnDeviceResetCb cb) {
    if (cb == nullptr) {
        return false;
    }

    return PluginLoader::get()->add_on_device_reset(cb);
}

bool on_message(UEVR_OnMessageCb cb) {
    if (cb == nullptr) {
        return false;
    }

    return PluginLoader::get()->add_on_message(cb);
}

bool on_xinput_get_state(UEVR_OnXInputGetStateCb cb) {
    if (cb == nullptr) {
        return false;
    }

    return PluginLoader::get()->add_on_xinput_get_state(cb);
}

bool on_xinput_set_state(UEVR_OnXInputSetStateCb cb) {
    if (cb == nullptr) {
        return false;
    }

    return PluginLoader::get()->add_on_xinput_set_state(cb);
}

bool on_post_render_vr_framework_dx11(UEVR_OnPostRenderVRFrameworkDX11Cb cb) {
    if (cb == nullptr) {
        return false;
    }

    return PluginLoader::get()->add_on_post_render_vr_framework_dx11(cb);
}

bool on_post_render_vr_framework_dx12(UEVR_OnPostRenderVRFrameworkDX12Cb cb) {
    if (cb == nullptr) {
        return false;
    }

    return PluginLoader::get()->add_on_post_render_vr_framework_dx12(cb);
}

bool on_custom_event(UEVR_OnCustomEventCb cb) {
    if (cb == nullptr) {
        return false;
    }

    return PluginLoader::get()->add_on_custom_event(cb);
}
}

UEVR_PluginCallbacks g_plugin_callbacks {
    uevr::on_present,
    uevr::on_device_reset,
    uevr::on_message,
    uevr::on_xinput_get_state,
    uevr::on_xinput_set_state,
    uevr::on_post_render_vr_framework_dx11,
    uevr::on_post_render_vr_framework_dx12,
    uevr::on_custom_event
};

UEVR_PluginFunctions g_plugin_functions {
    .log_error = uevr::log_error,
    .log_warn = uevr::log_warn,
    .log_info = uevr::log_info,
    .is_drawing_ui = uevr::is_drawing_ui,
    .remove_callback = uevr::remove_callback,
    .get_persistent_dir = uevr::get_persistent_dir,
    .register_inline_hook = uevr::register_inline_hook,
    .unregister_inline_hook = uevr::unregister_inline_hook,
    .dispatch_lua_event =::dispatch_lua_event,

    .get_commit_hash = []() -> const char* {
        return UEVR_COMMIT_HASH;
    },
    .get_tag = []() -> const char* {
        return UEVR_TAG;
    },
    .get_tag_long = []() -> const char* {
        return UEVR_TAG_LONG;
    },
    .get_branch = []() -> const char* {
        return UEVR_BRANCH;
    },
    .get_build_date = []() -> const char* {
        return UEVR_BUILD_DATE;
    },
    .get_build_time = []() -> const char* {
        return UEVR_BUILD_TIME;
    },
    .get_commits_past_tag = []() -> unsigned int {
        return UEVR_COMMITS_PAST_TAG;
    },
    .get_total_commits = []() -> unsigned int {
        return UEVR_TOTAL_COMMITS;
    },
    .dispatch_custom_event = uevr::dispatch_custom_event
};

#define GET_ENGINE_WORLD_RETNULL() \
    auto engine = sdk::UEngine::get(); \
    if (engine == nullptr) { \
        return nullptr; \
    } \
    auto world = engine->get_world(); \
    if (world == nullptr) { \
        return nullptr; \
    }

UEVR_SDKFunctions g_sdk_functions {
    []() -> UEVR_UEngineHandle {
        return (UEVR_UEngineHandle)sdk::UEngine::get();
    },
    [](const char* module_name, const char* name, int value) -> void {
        static std::unordered_map<std::string, sdk::TConsoleVariableData<int>**> cvars{};

        auto set_cvar = [](sdk::TConsoleVariableData<int>** cvar, int value) {
            if (cvar != nullptr && *cvar != nullptr) {
                (*cvar)->set(value);
            }
        };

        if (!cvars.contains(name)) {
            const auto cvar = sdk::find_cvar_data(utility::widen(module_name), utility::widen(name));

            if (cvar) {
                cvars[name] = (sdk::TConsoleVariableData<int>**)cvar->address();
                set_cvar((sdk::TConsoleVariableData<int>**)cvar->address(), value);
            }
        } else {
            set_cvar(cvars[name], value);
        }
    },
    // get_uobject_array
    []() -> UEVR_UObjectArrayHandle {
        return (UEVR_UObjectArrayHandle)sdk::FUObjectArray::get();
    },
    // get_player_controller
    [](int index) -> UEVR_UObjectHandle {
        GET_ENGINE_WORLD_RETNULL();
        const auto ugameplay_statics = sdk::UGameplayStatics::get();
        if (ugameplay_statics == nullptr) {
            return nullptr;
        }

        return (UEVR_UObjectHandle)ugameplay_statics->get_player_controller(world, index);
    },
    // get_local_pawn
    [](int index) -> UEVR_UObjectHandle {
        GET_ENGINE_WORLD_RETNULL();
        const auto ugameplay_statics = sdk::UGameplayStatics::get();
        if (ugameplay_statics == nullptr) {
            return nullptr;
        }

        const auto pc = ugameplay_statics->get_player_controller(world, index);
        if (pc == nullptr) {
            return nullptr;
        }

        return (UEVR_UObjectHandle)pc->get_acknowledged_pawn();
    },
    // spawn_object
    [](UEVR_UClassHandle klass, UEVR_UObjectHandle outer) -> UEVR_UObjectHandle {
        if (klass == nullptr) {
            return nullptr;
        }

        const auto ugs = sdk::UGameplayStatics::get();
        if (ugs == nullptr) {
            return nullptr;
        }

        return (UEVR_UObjectHandle)ugs->spawn_object((sdk::UClass*)klass, (sdk::UObject*)outer);
    },
    // execute_command
    [](const wchar_t* command) -> void {
        if (command == nullptr) {
            return;
        }

        sdk::UEngine::get()->exec(command);
    },
    // execute_command_ex
    [](UEVR_UObjectHandle world, const wchar_t* command, void* output_device) -> void {
        if (command == nullptr) {
            return;
        }

        sdk::UEngine::get()->exec((sdk::UWorld*)world, command, output_device);
    },
    // get_console_manager
    []() -> UEVR_FConsoleManagerHandle {
        return (UEVR_FConsoleManagerHandle)sdk::FConsoleManager::get();
    },
    // add_component_by_class
    [](UEVR_UObjectHandle actor, UEVR_UClassHandle klass, bool deferred) -> UEVR_UObjectHandle {
        return (UEVR_UObjectHandle)((sdk::AActor*)actor)->add_component_by_class((sdk::UClass*)klass, deferred);
    }
};

namespace uevr {
bool on_pre_engine_tick(UEVR_Engine_TickCb cb) {
    if (cb == nullptr) {
        return false;
    }

    return PluginLoader::get()->add_on_pre_engine_tick(cb);
}

bool on_post_engine_tick(UEVR_Engine_TickCb cb) {
    if (cb == nullptr) {
        return false;
    }

    return PluginLoader::get()->add_on_post_engine_tick(cb);
}

bool on_pre_slate_draw_window_render_thread(UEVR_Slate_DrawWindow_RenderThreadCb cb) {
    if (cb == nullptr) {
        return false;
    }

    return PluginLoader::get()->add_on_pre_slate_draw_window_render_thread(cb);
}

bool on_post_slate_draw_window_render_thread(UEVR_Slate_DrawWindow_RenderThreadCb cb) {
    if (cb == nullptr) {
        return false;
    }

    return PluginLoader::get()->add_on_post_slate_draw_window_render_thread(cb);
}

bool on_early_calculate_stereo_view_offset(UEVR_Stereo_CalculateStereoViewOffsetCb cb) {
    if (cb == nullptr) {
        return false;
    }

    return PluginLoader::get()->add_on_early_calculate_stereo_view_offset(cb);
}

bool on_pre_calculate_stereo_view_offset(UEVR_Stereo_CalculateStereoViewOffsetCb cb) {
    if (cb == nullptr) {
        return false;
    }

    return PluginLoader::get()->add_on_pre_calculate_stereo_view_offset(cb);
}

bool on_post_calculate_stereo_view_offset(UEVR_Stereo_CalculateStereoViewOffsetCb cb) {
    if (cb == nullptr) {
        return false;
    }

    return PluginLoader::get()->add_on_post_calculate_stereo_view_offset(cb);
}

bool on_pre_viewport_client_draw(UEVR_ViewportClient_DrawCb cb) {
    if (cb == nullptr) {
        return false;
    }

    return PluginLoader::get()->add_on_pre_viewport_client_draw(cb);
}

bool on_post_viewport_client_draw(UEVR_ViewportClient_DrawCb cb) {
    if (cb == nullptr) {
        return false;
    }

    return PluginLoader::get()->add_on_post_viewport_client_draw(cb);
}
}

UEVR_SDKCallbacks g_sdk_callbacks {
    uevr::on_pre_engine_tick,
    uevr::on_post_engine_tick,
    uevr::on_pre_slate_draw_window_render_thread,
    uevr::on_post_slate_draw_window_render_thread,
    uevr::on_pre_calculate_stereo_view_offset,
    uevr::on_post_calculate_stereo_view_offset,
    uevr::on_pre_viewport_client_draw,
    uevr::on_post_viewport_client_draw,
    uevr::on_early_calculate_stereo_view_offset,
};

#define UOBJECT(x) ((sdk::UObject*)x)

UEVR_UObjectFunctions g_uobject_functions {
    // get_class
    [](UEVR_UObjectHandle obj) {
        return (UEVR_UClassHandle)UOBJECT(obj)->get_class();
    },
    // get_outer
    [](UEVR_UObjectHandle obj) {
        return (UEVR_UObjectHandle)UOBJECT(obj)->get_outer();
    },
    // get_property_data
    [](UEVR_UObjectHandle obj, const wchar_t* name) {
        return (void*)UOBJECT(obj)->get_property_data(name);
    },
    // is_a
    [](UEVR_UObjectHandle obj, UEVR_UClassHandle cmp) {
        return UOBJECT(obj)->is_a((sdk::UClass*)cmp);
    },
    // process_event
    [](UEVR_UObjectHandle obj, UEVR_UFunctionHandle func, void* params) {
        UOBJECT(obj)->process_event((sdk::UFunction*)func, params);
    },
    // call_function
    [](UEVR_UObjectHandle obj, const wchar_t* name, void* params) {
        UOBJECT(obj)->call_function(name, params);
    },
    // get_fname
    [](UEVR_UObjectHandle obj) {
        return (UEVR_FNameHandle)&UOBJECT(obj)->get_fname();
    },
    // get_bool_property
    [](UEVR_UObjectHandle obj, const wchar_t* name) {
        return UOBJECT(obj)->get_bool_property(name);
    },
    // set_bool_property
    [](UEVR_UObjectHandle obj, const wchar_t* name, bool value) {
        UOBJECT(obj)->set_bool_property(name, value);
    },
};

#define FFIELD(x) ((sdk::FField*)x)

UEVR_FFieldFunctions g_ffield_functions {
    // get_next
    [](UEVR_FFieldHandle field) {
        return (UEVR_FFieldHandle)FFIELD(field)->get_next();
    },
    // get_class
    [](UEVR_FFieldHandle field) {
        return (UEVR_FFieldClassHandle)FFIELD(field)->get_class();
    },
    // get_fname
    [](UEVR_FFieldHandle field) {
        return (UEVR_FNameHandle)&FFIELD(field)->get_field_name();
    },
};

#define FPROPERTY(x) ((sdk::FProperty*)x)

UEVR_FPropertyFunctions g_fproperty_functions {
    // get_offset
    .get_offset = [](UEVR_FPropertyHandle prop) -> int {
        return FPROPERTY(prop)->get_offset();
    },
    .get_property_flags = [](UEVR_FPropertyHandle prop) -> uint64_t {
        return FPROPERTY(prop)->get_property_flags();
    },
    .is_param = [](UEVR_FPropertyHandle prop) -> bool {
        return FPROPERTY(prop)->is_param();
    },
    .is_out_param = [](UEVR_FPropertyHandle prop) -> bool {
        return FPROPERTY(prop)->is_out_param();
    },
    .is_return_param = [](UEVR_FPropertyHandle prop) -> bool {
        return FPROPERTY(prop)->is_return_param();
    },
    .is_reference_param = [](UEVR_FPropertyHandle prop) -> bool {
        return FPROPERTY(prop)->is_reference_param();
    },
    .is_pod = [](UEVR_FPropertyHandle prop) -> bool {
        return FPROPERTY(prop)->is_pod();
    },
};

#define USTRUCT(x) ((sdk::UStruct*)x)

UEVR_UStructFunctions g_ustruct_functions {
    .get_super_struct = [](UEVR_UStructHandle strct) {
        return (UEVR_UStructHandle)USTRUCT(strct)->get_super_struct();
    },
    .get_child_properties = [](UEVR_UStructHandle strct) {
        return (UEVR_FFieldHandle)USTRUCT(strct)->get_child_properties();
    },
    .find_function = [](UEVR_UStructHandle strct, const wchar_t* name) {
        return (UEVR_UFunctionHandle)USTRUCT(strct)->find_function(name);
    },
    .find_property = [](UEVR_UStructHandle strct, const wchar_t* name) {
        return (UEVR_FPropertyHandle)USTRUCT(strct)->find_property(name);
    },
    .get_properties_size = [](UEVR_UStructHandle strct) {
        return USTRUCT(strct)->get_properties_size();
    },
    .get_min_alignment = [](UEVR_UStructHandle strct) {
        return USTRUCT(strct)->get_min_alignment();
    },
    .get_children = [](UEVR_UStructHandle strct) {
        return (UEVR_UFieldHandle)USTRUCT(strct)->get_children();
    }
};

#define UCLASS(x) ((sdk::UClass*)x)

UEVR_UClassFunctions g_uclass_functions {
    // get_class_default_object
    [](UEVR_UClassHandle klass) {
        return (UEVR_UObjectHandle)UCLASS(klass)->get_class_default_object();
    },
};

#define UFUNCTION(x) ((sdk::UFunction*)x)

void PluginLoader::ufunction_hook_intermediary(UEVR_UObjectHandle obj, void* params, void* out_result, sdk::UFunction* func) {
    auto& plugin_loader = PluginLoader::get();
    std::shared_lock _{plugin_loader->m_ufunction_hooks_mtx};

    auto it = plugin_loader->m_ufunction_hooks.find(func);

    // uh...
    if (it == plugin_loader->m_ufunction_hooks.end()) {
        return;
    }

    auto& hook = it->second;
    std::scoped_lock __{hook->mux};

    bool any_false = false;

    for (auto&& cb : hook->pre_callbacks) {
        bool result = cb((UEVR_UFunctionHandle)func, obj, params, out_result);

        if (!result) {
            any_false = true;
        }
    }

    // Call the original
    if (!any_false) {
        auto orig = hook->hook->get_original<UEVR_UFunction_NativeFn>();

        if (orig != nullptr) {
            orig(obj, params, out_result);
        }
    }

    for (auto&& cb : hook->post_callbacks) {
        cb((UEVR_UFunctionHandle)func, obj, params, out_result);
    }
}

bool PluginLoader::hook_ufunction_ptr(UEVR_UFunctionHandle func, UEVR_UFunction_NativePreFn pre, UEVR_UFunction_NativePostFn post) {
    std::unique_lock _{m_ufunction_hooks_mtx};

    if (func == nullptr || (pre == nullptr && post == nullptr)) {
        spdlog::error("hook_ufunction_ptr: Invalid arguments");
        return false;
    }

    const auto offset = sdk::UFunction::get_native_function_offset();

    if (offset == 0) {
        spdlog::error("UFunction::get_native_function_offset() returned 0");
        return false;
    }

    auto ufunc = UFUNCTION(func);

    void** native = (void**)((uintptr_t)ufunc + offset);

    if (*native == nullptr) {
        return false;
    }

    auto& existing_hook = m_ufunction_hooks[ufunc];

    if (existing_hook == nullptr) {
        existing_hook = std::make_unique<UFunctionHookState>();
        std::scoped_lock __{existing_hook->mux};

        //existing_hook->hook = std::make_unique<PointerHook>(native, dst);

        using namespace asmjit;
        using namespace asmjit::x86;
        {
            CodeHolder code{};
            code.init(m_jit_runtime.environment());
            Assembler a{&code};

            a.mov(r9, ufunc);
            auto intermediary_label = a.newLabel();
            a.jmp(ptr(intermediary_label));

            a.bind(intermediary_label);
            a.dq((uintptr_t)&PluginLoader::ufunction_hook_intermediary);

            m_jit_runtime.add((uintptr_t*)&existing_hook->jitted_pre, &code);
        }

        existing_hook->hook = std::make_unique<PointerHook>(native, (void*)existing_hook->jitted_pre);

        if (pre != nullptr) {
            existing_hook->pre_callbacks.push_back(pre);
        }

        if (post != nullptr) {
            existing_hook->post_callbacks.push_back(post);
        }
    } else {
        std::scoped_lock __{existing_hook->mux};

        if (pre != nullptr) {
            // We dont want to call the same function multiple times, could cause issues
            if (std::find(existing_hook->pre_callbacks.begin(), existing_hook->pre_callbacks.end(), pre) == existing_hook->pre_callbacks.end()) {
                existing_hook->pre_callbacks.push_back(pre);
            }
        }

        if (post != nullptr) {
            if (std::find(existing_hook->post_callbacks.begin(), existing_hook->post_callbacks.end(), post) == existing_hook->post_callbacks.end()) {
                existing_hook->post_callbacks.push_back(post);
            }
        }
    }

    return true;
}

UEVR_UFunctionFunctions g_ufunction_functions {
    .get_native_function = [](UEVR_UFunctionHandle func) {
        return (void*)UFUNCTION(func)->get_native_function();
    },
    .hook_ptr = [](UEVR_UFunctionHandle func, UEVR_UFunction_NativePreFn pre, UEVR_UFunction_NativePostFn post) -> bool {
        return PluginLoader::get()->hook_ufunction_ptr(func, pre, post);
    },
    .get_function_flags = [](UEVR_UFunctionHandle func) -> uint32_t {
        return UFUNCTION(func)->get_function_flags();
    },
    .set_function_flags = [](UEVR_UFunctionHandle func, uint32_t flags) {
        UFUNCTION(func)->get_function_flags() = flags;
    },
};

namespace uevr {
namespace uobjecthook {
    void activate() {
        UObjectHook::get()->activate();
    }

    bool exists(UEVR_UObjectHandle obj) {
        auto& instance = UObjectHook::get();
        instance->activate();

        return instance->exists((sdk::UObject*)obj);
    }

    int get_objects_by_class(UEVR_UClassHandle klass, UEVR_UObjectHandle* out_objects, unsigned int max_objects, bool allow_default) {
        auto& instance = UObjectHook::get();
        instance->activate();

        const auto objects = instance->get_objects_by_class((sdk::UClass*)klass);

        if (objects.empty()) {
            return 0;
        }

        unsigned int i = 0;
        for (auto&& obj : objects) {
            if (!allow_default) {
                const auto c = obj->get_class();

                if (c == nullptr || c->get_class_default_object() == obj) {
                    continue;
                }
            }

            if (i < max_objects && out_objects != nullptr) {
                out_objects[i++] = (UEVR_UObjectHandle)obj;
            } else {
                i++;
            }
        }

        return i;
    }

    int get_objects_by_class_name(const wchar_t* class_name, UEVR_UObjectHandle* out_objects, unsigned int max_objects, bool allow_default) {
        const auto c = sdk::find_uobject<sdk::UClass>(class_name);

        if (c == nullptr) {
            return 0;
        }

        return get_objects_by_class((UEVR_UClassHandle)c, out_objects, max_objects, allow_default);
    }

    UEVR_UObjectHandle get_first_object_by_class(UEVR_UClassHandle klass, bool allow_default) {
        auto& instance = UObjectHook::get();
        instance->activate();

        const auto objects = instance->get_objects_by_class((sdk::UClass*)klass);

        if (objects.empty()) {
            return nullptr;
        }

        if (allow_default) {
            return (UEVR_UObjectHandle)*objects.begin();
        }

        for (auto&& obj : objects) {
            const auto c = obj->get_class();

            if (c == nullptr || c->get_class_default_object() == obj) {
                continue;
            }

            return (UEVR_UObjectHandle)obj;
        }

        return (UEVR_UObjectHandle)nullptr;
    }

    UEVR_UObjectHandle get_first_object_by_class_name(const wchar_t* class_name, bool allow_default) {
        const auto c = sdk::find_uobject<sdk::UClass>(class_name);

        if (c == nullptr) {
            return nullptr;
        }

        return get_first_object_by_class((UEVR_UClassHandle)c, allow_default);
    }

    UEVR_UObjectHookMotionControllerStateHandle get_or_add_motion_controller_state(UEVR_UObjectHandle obj_handle) {
        const auto obj = (sdk::USceneComponent*)obj_handle;
        if (obj == nullptr || !obj->is_a(sdk::USceneComponent::static_class())) {
            return nullptr;
        }

        const auto result = UObjectHook::get()->get_or_add_motion_controller_state(obj);

        return (UEVR_UObjectHookMotionControllerStateHandle)result.get();
    }

    UEVR_UObjectHookMotionControllerStateHandle get_motion_controller_state(UEVR_UObjectHandle obj_handle) {
        const auto obj = (sdk::USceneComponent*)obj_handle;
        if (obj == nullptr || !obj->is_a(sdk::USceneComponent::static_class())) {
            return nullptr;
        }

        const auto result = UObjectHook::get()->get_motion_controller_state(obj);

        if (!result.has_value()) {
            return nullptr;
        }

        return (UEVR_UObjectHookMotionControllerStateHandle)result->get();
    }

    void remove_motion_controller_state(UEVR_UObjectHandle obj_handle) {
        const auto obj = (sdk::USceneComponent*)obj_handle;
        if (obj == nullptr || !obj->is_a(sdk::USceneComponent::static_class())) {
            return;
        }

        UObjectHook::get()->remove_motion_controller_state(obj);
    }

    void remove_all_motion_controller_states() {
        UObjectHook::get()->remove_all_motion_controller_states();
    }

    bool disabled() {
        return UObjectHook::get()->is_disabled();
    }

    void set_disabled(bool disabled) {
        UObjectHook::get()->set_disabled(disabled);
    }

namespace mc_state {
    void set_rotation_offset(UEVR_UObjectHookMotionControllerStateHandle state, const UEVR_Quaternionf* rotation) {
        if (state == nullptr) {
            return;
        }

        auto& s = *(UObjectHook::MotionControllerState*)state;
        s.rotation_offset.x = rotation->x;
        s.rotation_offset.y = rotation->y;
        s.rotation_offset.z = rotation->z;
        s.rotation_offset.w = rotation->w;
    }

    void set_location_offset(UEVR_UObjectHookMotionControllerStateHandle state, const UEVR_Vector3f* location) {
        if (state == nullptr) {
            return;
        }

        auto& s = *(UObjectHook::MotionControllerState*)state;
        s.location_offset.x = location->x;
        s.location_offset.y = location->y;
        s.location_offset.z = location->z;
    }

    void set_hand(UEVR_UObjectHookMotionControllerStateHandle state, unsigned int hand) {
        if (state == nullptr) {
            return;
        }

        if (hand > 2) {
            return;
        }

        auto& s = *(UObjectHook::MotionControllerState*)state;
        s.hand = (uint8_t)hand;
    }

    void set_permanent(UEVR_UObjectHookMotionControllerStateHandle state, bool permanent) {
        if (state == nullptr) {
            return;
        }

        auto& s = *(UObjectHook::MotionControllerState*)state;
        s.permanent = permanent;
    }
}
}
}

UEVR_UObjectHookMotionControllerStateFunctions g_mc_functions {
    uevr::uobjecthook::mc_state::set_rotation_offset,
    uevr::uobjecthook::mc_state::set_location_offset,
    uevr::uobjecthook::mc_state::set_hand,
    uevr::uobjecthook::mc_state::set_permanent
};

UEVR_UObjectHookFunctions g_uobjecthook_functions {
    uevr::uobjecthook::activate,
    uevr::uobjecthook::exists,
    uevr::uobjecthook::get_objects_by_class,
    uevr::uobjecthook::get_objects_by_class_name,
    uevr::uobjecthook::get_first_object_by_class,
    uevr::uobjecthook::get_first_object_by_class_name,
    uevr::uobjecthook::get_or_add_motion_controller_state,
    uevr::uobjecthook::get_motion_controller_state,
    &g_mc_functions,
    uevr::uobjecthook::disabled,
    uevr::uobjecthook::set_disabled,
    uevr::uobjecthook::remove_motion_controller_state,
    uevr::uobjecthook::remove_all_motion_controller_states
};

#define FFIELDCLASS(x) ((sdk::FFieldClass*)x)

UEVR_FFieldClassFunctions g_ffield_class_functions {
    // get_fname
    [](UEVR_FFieldClassHandle field) {
        return (UEVR_FNameHandle)&FFIELDCLASS(field)->get_name();
    },
};

#define FNAME(x) ((sdk::FName*)x)

UEVR_FNameFunctions g_fname_functions {
    // to_string
    [](UEVR_FNameHandle name, wchar_t* buffer, unsigned int buffer_size) -> unsigned int {
        const auto result = FNAME(name)->to_string();

        if (buffer == nullptr || buffer_size == 0) {
            return (unsigned int)result.size();
        }

        const auto size = std::min<size_t>(result.size(), (size_t)buffer_size - 1);

        memcpy(buffer, result.c_str(), size * sizeof(wchar_t));
        buffer[size] = L'\0';

        return (unsigned int)size;
    },
    // constructor
    [](UEVR_FNameHandle name, const wchar_t* str, unsigned int find_type) {
        auto& fname = *(sdk::FName*)name;
        fname = sdk::FName{str, (sdk::EFindName)find_type};
    }
};

namespace uevr {
namespace console {
// get_console_objects
UEVR_TArrayHandle get_console_objects(UEVR_FConsoleManagerHandle mgr) {
    const auto console_manager = (sdk::FConsoleManager*)mgr;
    if (console_manager == nullptr) {
        return nullptr;
    }
    return (UEVR_TArrayHandle)&console_manager->get_console_objects();
}

UEVR_IConsoleObjectHandle find_object(UEVR_FConsoleManagerHandle mgr, const wchar_t* name) {
    const auto console_manager = (sdk::FConsoleManager*)mgr;
    if (console_manager == nullptr) {
        return nullptr;
    }
    return (UEVR_IConsoleObjectHandle)console_manager->find(name);
}

// Naive implementation, but it's fine for now
UEVR_IConsoleVariableHandle find_variable(UEVR_FConsoleManagerHandle mgr, const wchar_t* name) {
    return (UEVR_IConsoleVariableHandle)find_object(mgr, name);
}

UEVR_IConsoleCommandHandle find_command(UEVR_FConsoleManagerHandle mgr, const wchar_t* name) {
    auto obj = (sdk::IConsoleObject*)find_object(mgr, name);

    if (obj == nullptr) {
        return nullptr;
    }

    return (UEVR_IConsoleCommandHandle)obj->AsCommand();
}

UEVR_IConsoleCommandHandle as_commmand(UEVR_IConsoleObjectHandle obj) {
    if (obj == nullptr) {
        return nullptr;
    }

    return (UEVR_IConsoleCommandHandle)((sdk::IConsoleObject*)obj)->AsCommand();
}

void variable_set(UEVR_IConsoleVariableHandle var, const wchar_t* value) {
    if (var == nullptr) {
        return;
    }

    ((sdk::IConsoleVariable*)var)->Set(value);
}

void variable_set_ex(UEVR_IConsoleVariableHandle var, const wchar_t* value, unsigned int flags) {
    if (var == nullptr) {
        return;
    }

    ((sdk::IConsoleVariable*)var)->Set(value, flags);
}

int variable_get_int(UEVR_IConsoleVariableHandle var) {
    if (var == nullptr) {
        return 0;
    }

    return ((sdk::IConsoleVariable*)var)->GetInt();
}

float variable_get_float(UEVR_IConsoleVariableHandle var) {
    if (var == nullptr) {
        return 0.0f;
    }

    return ((sdk::IConsoleVariable*)var)->GetFloat();
}

void command_execute(UEVR_IConsoleCommandHandle cmd, const wchar_t* args) {
    if (cmd == nullptr) {
        return;
    }

    ((sdk::IConsoleCommand*)cmd)->Execute(args);
}
}
}

UEVR_ConsoleFunctions g_console_functions {
    uevr::console::get_console_objects,
    uevr::console::find_object,
    uevr::console::find_variable,
    uevr::console::find_command,
    uevr::console::as_commmand,
    uevr::console::variable_set,
    uevr::console::variable_set_ex,
    uevr::console::variable_get_int,
    uevr::console::variable_get_float,
    uevr::console::command_execute
};

namespace uevr {
namespace malloc {
UEVR_FMallocHandle get() {
    return (UEVR_FMallocHandle)sdk::FMalloc::get();
}

void* malloc(UEVR_FMallocHandle malloc, unsigned int size, uint32_t alignment) {
    return ((sdk::FMalloc*)malloc)->malloc(size, alignment);
}

void* realloc(UEVR_FMallocHandle malloc, void* original, unsigned int size, uint32_t alignment) {
    return ((sdk::FMalloc*)malloc)->realloc(original, size, alignment);
}

void free(UEVR_FMallocHandle malloc, void* original) {
    return ((sdk::FMalloc*)malloc)->free(original);
}
}
}

UEVR_FMallocFunctions g_malloc_functions {
    uevr::malloc::get,
    uevr::malloc::malloc,
    uevr::malloc::realloc,
    uevr::malloc::free
};

UEVR_FArrayPropertyFunctions g_farray_property_functions {
    .get_inner = [](UEVR_FArrayPropertyHandle prop) -> UEVR_FPropertyHandle {
        return (UEVR_FPropertyHandle)((sdk::FArrayProperty*)prop)->get_inner();
    }
};

UEVR_FBoolPropertyFunctions g_fbool_property_functions {
    .get_field_size = [](UEVR_FBoolPropertyHandle prop) -> uint32_t {
        return ((sdk::FBoolProperty*)prop)->get_field_size();
    },
    .get_byte_offset = [](UEVR_FBoolPropertyHandle prop) -> uint32_t {
        return ((sdk::FBoolProperty*)prop)->get_byte_offset();
    },
    .get_byte_mask = [](UEVR_FBoolPropertyHandle prop) -> uint32_t {
        return ((sdk::FBoolProperty*)prop)->get_byte_mask();
    },
    .get_field_mask = [](UEVR_FBoolPropertyHandle prop) -> uint32_t {
        return ((sdk::FBoolProperty*)prop)->get_field_mask();
    },
    .get_value_from_object = [](UEVR_FBoolPropertyHandle prop, void* obj) -> bool {
        return ((sdk::FBoolProperty*)prop)->get_value_from_object((sdk::UObject*)obj);
    },
    .get_value_from_propbase = [](UEVR_FBoolPropertyHandle prop, void* propbase) -> bool {
        return ((sdk::FBoolProperty*)prop)->get_value_from_propbase(propbase);
    },
    .set_value_in_object = [](UEVR_FBoolPropertyHandle prop, void* obj, bool value) {
        ((sdk::FBoolProperty*)prop)->set_value_in_object((sdk::UObject*)obj, value);
    },
    .set_value_in_propbase = [](UEVR_FBoolPropertyHandle prop, void* propbase, bool value) {
        ((sdk::FBoolProperty*)prop)->set_value_in_propbase(propbase, value);
    }
};

UEVR_FStructPropertyFunctions g_fstruct_property_functions {
    .get_struct = [](UEVR_FStructPropertyHandle prop) -> UEVR_UScriptStructHandle {
        return (UEVR_UScriptStructHandle)((sdk::FStructProperty*)prop)->get_struct();
    }
};

UEVR_FEnumPropertyFunctions g_fenum_property_functions {
    .get_underlying_prop = [](UEVR_FEnumPropertyHandle prop) -> UEVR_FNumericPropertyHandle {
        return (UEVR_FNumericPropertyHandle)((sdk::FEnumProperty*)prop)->get_underlying_prop();
    },
    .get_enum = [](UEVR_FEnumPropertyHandle prop) -> UEVR_UEnumHandle {
        return (UEVR_UEnumHandle)((sdk::FEnumProperty*)prop)->get_enum();
    }
};

UEVR_UFieldFunctions g_ufield_functions {
    .get_next = [](UEVR_UFieldHandle field) -> UEVR_UFieldHandle {
        return (UEVR_UFieldHandle)((sdk::UField*)field)->get_next();
    }
};

UEVR_UGameViewportClientFunctions g_game_viewport_client_functions {
    .exec = [](UEVR_UGameViewportClientHandle vp, const wchar_t* command) {
        if (command == nullptr) {
            return;
        }

        auto viewport_client = (sdk::UGameViewportClient*)vp;
        viewport_client->exec(command);
    },
    .exec_ex = [](UEVR_UGameViewportClientHandle vp, UEVR_UObjectHandle world, const wchar_t* command, void* output_device) {
        if (command == nullptr) {
            return;
        }

        auto viewport_client = (sdk::UGameViewportClient*)vp;
        viewport_client->exec((sdk::UWorld*)world, command, output_device);
    },
};

UEVR_SDKData g_sdk_data {
    &g_sdk_functions,
    &g_sdk_callbacks,
    &g_uobject_functions,
    &uevr::fuobjectarray::functions,
    &g_ffield_functions,
    &g_fproperty_functions,
    &g_ustruct_functions,
    &g_uclass_functions,
    &g_ufunction_functions,
    &g_uobjecthook_functions,
    &g_ffield_class_functions,
    &g_fname_functions,
    &g_console_functions,
    &g_malloc_functions,
    &uevr::render_target_pool_hook::functions,
    &uevr::stereo_hook::functions,
    &uevr::frhitexture2d::functions,
    &uevr::uscriptstruct::functions,
    &g_farray_property_functions,
    &g_fbool_property_functions,
    &g_fstruct_property_functions,
    &g_fenum_property_functions,
    &g_ufield_functions,
    &g_game_viewport_client_functions,
};

namespace uevr {
namespace vr {
bool is_runtime_ready() {
    return ::VR::get()->get_runtime()->ready();
}

bool is_openvr() {
    return ::VR::get()->get_runtime()->is_openvr();
}

bool is_openxr() {
    return ::VR::get()->get_runtime()->is_openxr();
}

bool is_hmd_active() {
    return ::VR::get()->is_hmd_active();
}

void get_standing_origin(UEVR_Vector3f* out_origin) {
    auto origin = ::VR::get()->get_standing_origin();
    out_origin->x = origin.x;
    out_origin->y = origin.y;
    out_origin->z = origin.z;
}

void get_rotation_offset(UEVR_Quaternionf* out_rotation) {
    auto rotation = ::VR::get()->get_rotation_offset();
    out_rotation->x = rotation.x;
    out_rotation->y = rotation.y;
    out_rotation->z = rotation.z;
    out_rotation->w = rotation.w;
}

void set_standing_origin(const UEVR_Vector3f* origin) {
    ::VR::get()->set_standing_origin({origin->x, origin->y, origin->z, 1.0f});
}

void set_rotation_offset(const UEVR_Quaternionf* rotation) {
    ::VR::get()->set_rotation_offset({rotation->w, rotation->x, rotation->y, rotation->z});
}

UEVR_TrackedDeviceIndex get_hmd_index() {
    return VR::get()->get_hmd_index();
}

UEVR_TrackedDeviceIndex get_left_controller_index() {
    return VR::get()->get_left_controller_index();
}

UEVR_TrackedDeviceIndex get_right_controller_index() {
    return VR::get()->get_right_controller_index();
}

void get_pose(UEVR_TrackedDeviceIndex index, UEVR_Vector3f* out_position, UEVR_Quaternionf* out_rotation) {
    static_assert(sizeof(UEVR_Vector3f) == sizeof(glm::vec3));
    static_assert(sizeof(UEVR_Quaternionf) == sizeof(glm::quat));

    auto transform = ::VR::get()->get_transform(index);

    out_position->x = transform[3].x;
    out_position->y = transform[3].y;
    out_position->z = transform[3].z;

    const auto rot = glm::quat{glm::extractMatrixRotation(transform)};
    out_rotation->x = rot.x;
    out_rotation->y = rot.y;
    out_rotation->z = rot.z;
    out_rotation->w = rot.w;
}

void get_grip_pose(UEVR_TrackedDeviceIndex index, UEVR_Vector3f* out_position, UEVR_Quaternionf* out_rotation) {
    static_assert(sizeof(UEVR_Vector3f) == sizeof(glm::vec3));
    static_assert(sizeof(UEVR_Quaternionf) == sizeof(glm::quat));

    auto transform = ::VR::get()->get_grip_transform(index);

    out_position->x = transform[3].x;
    out_position->y = transform[3].y;
    out_position->z = transform[3].z;

    const auto rot = glm::quat{glm::extractMatrixRotation(transform)};
    out_rotation->x = rot.x;
    out_rotation->y = rot.y;
    out_rotation->z = rot.z;
    out_rotation->w = rot.w;
}

void get_aim_pose(UEVR_TrackedDeviceIndex index, UEVR_Vector3f* out_position, UEVR_Quaternionf* out_rotation) {
    static_assert(sizeof(UEVR_Vector3f) == sizeof(glm::vec3));
    static_assert(sizeof(UEVR_Quaternionf) == sizeof(glm::quat));

    auto transform = ::VR::get()->get_aim_transform(index);

    out_position->x = transform[3].x;
    out_position->y = transform[3].y;
    out_position->z = transform[3].z;

    const auto rot = glm::quat{glm::extractMatrixRotation(transform)};
    out_rotation->x = rot.x;
    out_rotation->y = rot.y;
    out_rotation->z = rot.z;
    out_rotation->w = rot.w;
}

void get_transform(UEVR_TrackedDeviceIndex index, UEVR_Matrix4x4f* out_transform) {
    static_assert(sizeof(UEVR_Matrix4x4f) == sizeof(glm::mat4), "UEVR_Matrix4x4f and glm::mat4 must be the same size");

    const auto transform = ::VR::get()->get_transform(index);
    memcpy(out_transform, &transform, sizeof(UEVR_Matrix4x4f));
}

void get_grip_transform(UEVR_TrackedDeviceIndex index, UEVR_Matrix4x4f* out_transform) {
    static_assert(sizeof(UEVR_Matrix4x4f) == sizeof(glm::mat4), "UEVR_Matrix4x4f and glm::mat4 must be the same size");

    const auto transform = ::VR::get()->get_grip_transform(index);
    memcpy(out_transform, &transform, sizeof(UEVR_Matrix4x4f));
}

void get_aim_transform(UEVR_TrackedDeviceIndex index, UEVR_Matrix4x4f* out_transform) {
    static_assert(sizeof(UEVR_Matrix4x4f) == sizeof(glm::mat4), "UEVR_Matrix4x4f and glm::mat4 must be the same size");

    const auto transform = ::VR::get()->get_aim_transform(index);
    memcpy(out_transform, &transform, sizeof(UEVR_Matrix4x4f));
}

void get_eye_offset(UEVR_Eye eye, UEVR_Vector3f* out_offset) {
    const auto out = ::VR::get()->get_eye_offset((VRRuntime::Eye)eye);

    out_offset->x = out.x;
    out_offset->y = out.y;
    out_offset->z = out.z;
}

void get_ue_projection_matrix(UEVR_Eye eye, UEVR_Matrix4x4f* out_projection) {
    const auto& projection = ::VR::get()->get_runtime()->projections[eye];
    memcpy(out_projection, &projection, sizeof(UEVR_Matrix4x4f));
}

UEVR_InputSourceHandle get_left_joystick_source() {
    return (UEVR_InputSourceHandle)::VR::get()->get_left_joystick();
}

UEVR_InputSourceHandle get_right_joystick_source() {
    return (UEVR_InputSourceHandle)::VR::get()->get_right_joystick();
}

UEVR_ActionHandle get_action_handle(const char* action_path) {
    return (UEVR_ActionHandle)::VR::get()->get_action_handle(action_path);
}

bool is_action_active(UEVR_ActionHandle action_handle, UEVR_InputSourceHandle source) {
    return ::VR::get()->is_action_active((::vr::VRActionHandle_t)action_handle, (::vr::VRInputValueHandle_t)source);
}

bool is_action_active_any_joystick(UEVR_ActionHandle action_handle) {
    return ::VR::get()->is_action_active_any_joystick((::vr::VRActionHandle_t)action_handle);
}

void get_joystick_axis(UEVR_InputSourceHandle source, UEVR_Vector2f* out_axis) {
    const auto axis = ::VR::get()->get_joystick_axis((::vr::VRInputValueHandle_t)source);

    out_axis->x = axis.x;
    out_axis->y = axis.y;
}

void trigger_haptic_vibration(float seconds_from_now, float duration, float frequency, float amplitude, UEVR_InputSourceHandle source) {
    ::VR::get()->trigger_haptic_vibration(seconds_from_now, duration, frequency, amplitude, (::vr::VRInputValueHandle_t)source);
}

bool is_using_controllers() {
    return ::VR::get()->is_using_controllers();
}

bool is_decoupled_pitch_enabled() {
    return ::VR::get()->is_decoupled_pitch_enabled();
}

unsigned int get_movement_orientation() {
    return ::VR::get()->get_movement_orientation();
}

unsigned int get_lowest_xinput_index() {
    return VR::get()->get_lowest_xinput_index();
}

void recenter_view() {
    VR::get()->recenter_view();
}

void recenter_horizon() {
    VR::get()->recenter_horizon();
}

unsigned int get_aim_method() {
    return (unsigned int)VR::get()->get_aim_method();
}

void set_aim_method(unsigned int method) {
    VR::get()->set_aim_method((VR::AimMethod)method);
}

bool is_aim_allowed() {
    return VR::get()->is_aim_allowed();
}

void set_aim_allowed(bool allowed) {
    VR::get()->set_aim_allowed(allowed);
}

unsigned int get_hmd_width() {
    return VR::get()->get_hmd_width();
}

unsigned int get_hmd_height() {
    return VR::get()->get_hmd_height();
}

unsigned int get_ui_width() {
    return (unsigned int)g_framework->get_rt_size().x;
}

unsigned int get_ui_height() {
    return (unsigned int)g_framework->get_rt_size().y;
}

bool is_snap_turn_enabled() {
    return VR::get()->is_snapturn_enabled();
}

void set_snap_turn_enabled(bool enabled) {
    VR::get()->set_snapturn_enabled(enabled);
}

void set_decoupled_pitch_enabled(bool enabled) {
    VR::get()->set_decoupled_pitch(enabled);
}

// TODO: Optimize this with a map
void set_mod_value(const char* key, const char* value) {
    if (key == nullptr || value == nullptr) {
        return;
    }

    auto& mods = g_framework->get_mods()->get_mods();

    for (auto& mod : mods) {
        auto value_entry = mod->get_value(key);

        if (value_entry != nullptr) {
            value_entry->set(value);
            break;
        }
    }
}

void get_mod_value(const char* key, char* out_value, unsigned int max_size) {
    if (key == nullptr || out_value == nullptr || max_size == 0) {
        return;
    }

    auto& mods = g_framework->get_mods()->get_mods();

    for (auto& mod : mods) {
        auto value_entry = mod->get_value(key);

        if (value_entry != nullptr) {
            const auto value = value_entry->get();

            const auto size = std::min<size_t>(value.size(), (size_t)max_size - 1);
            memcpy(out_value, value.c_str(), size * sizeof(char));
            out_value[size] = '\0';
            break;
        }
    }
}

void save_config() {
    g_framework->deferred_save_config();
}

void reload_config() {
    g_framework->deferred_reload_config();
}
}

} // namespace uevr::vr

UEVR_VRData g_vr_data {
    .is_runtime_ready =     uevr::vr::is_runtime_ready,
    .is_openvr =     uevr::vr::is_openvr,
    .is_openxr =     uevr::vr::is_openxr,
    .is_hmd_active =     uevr::vr::is_hmd_active,
    .get_standing_origin =     uevr::vr::get_standing_origin,
    .get_rotation_offset =     uevr::vr::get_rotation_offset,
    .set_standing_origin =     uevr::vr::set_standing_origin,
    .set_rotation_offset =     uevr::vr::set_rotation_offset,
    .get_hmd_index =     uevr::vr::get_hmd_index,
    .get_left_controller_index =     uevr::vr::get_left_controller_index,
    .get_right_controller_index =     uevr::vr::get_right_controller_index,
    .get_pose =     uevr::vr::get_pose,
    .get_transform =     uevr::vr::get_transform,
    .get_grip_pose =     uevr::vr::get_grip_pose,
    .get_aim_pose =     uevr::vr::get_aim_pose,
    .get_grip_transform =     uevr::vr::get_grip_transform,
    .get_aim_transform =     uevr::vr::get_aim_transform,
    .get_eye_offset =     uevr::vr::get_eye_offset,
    .get_ue_projection_matrix =     uevr::vr::get_ue_projection_matrix,
    .get_left_joystick_source =     uevr::vr::get_left_joystick_source,
    .get_right_joystick_source =     uevr::vr::get_right_joystick_source,
    .get_action_handle =     uevr::vr::get_action_handle,
    .is_action_active =     uevr::vr::is_action_active,
    .is_action_active_any_joystick =     uevr::vr::is_action_active_any_joystick,
    .get_joystick_axis =     uevr::vr::get_joystick_axis,
    .trigger_haptic_vibration = uevr::vr::trigger_haptic_vibration,
    .is_using_controllers = uevr::vr::is_using_controllers,
    .is_decoupled_pitch_enabled = uevr::vr::is_decoupled_pitch_enabled,
    .get_movement_orientation =     uevr::vr::get_movement_orientation,
    .get_lowest_xinput_index =     uevr::vr::get_lowest_xinput_index,
    .recenter_view =     uevr::vr::recenter_view,
    .recenter_horizon =     uevr::vr::recenter_horizon,
    .get_aim_method =     uevr::vr::get_aim_method,
    .set_aim_method =     uevr::vr::set_aim_method,
    .is_aim_allowed =     uevr::vr::is_aim_allowed,
    .set_aim_allowed =     uevr::vr::set_aim_allowed,
    .get_hmd_width =     uevr::vr::get_hmd_width,
    .get_hmd_height =     uevr::vr::get_hmd_height,
    .get_ui_width =     uevr::vr::get_ui_width,
    .get_ui_height =     uevr::vr::get_ui_height,

    .is_snap_turn_enabled = uevr::vr::is_snap_turn_enabled,
    .set_snap_turn_enabled = uevr::vr::set_snap_turn_enabled,
    .set_decoupled_pitch_enabled = uevr::vr::set_decoupled_pitch_enabled,

    .set_mod_value = uevr::vr::set_mod_value,
    .get_mod_value = uevr::vr::get_mod_value,
    .save_config = uevr::vr::save_config,
    .reload_config = uevr::vr::reload_config,
};


/*
DECLARE_UEVR_HANDLE(UEVR_IVRSystem);
DECLARE_UEVR_HANDLE(UEVR_IVRChaperone);
DECLARE_UEVR_HANDLE(UEVR_IVRChaperoneSetup);
DECLARE_UEVR_HANDLE(UEVR_IVRCompositor);
DECLARE_UEVR_HANDLE(UEVR_IVROverlay);
DECLARE_UEVR_HANDLE(UEVR_IVROverlayView);
DECLARE_UEVR_HANDLE(UEVR_IVRHeadsetView);
DECLARE_UEVR_HANDLE(UEVR_IVRScreenshots);
DECLARE_UEVR_HANDLE(UEVR_IVRRenderModels);
DECLARE_UEVR_HANDLE(UEVR_IVRApplications);
DECLARE_UEVR_HANDLE(UEVR_IVRSettings);
DECLARE_UEVR_HANDLE(UEVR_IVRResources);
DECLARE_UEVR_HANDLE(UEVR_IVRExtendedDisplay);
DECLARE_UEVR_HANDLE(UEVR_IVRTrackedCamera);
DECLARE_UEVR_HANDLE(UEVR_IVRDriverManager);
DECLARE_UEVR_HANDLE(UEVR_IVRInput);
DECLARE_UEVR_HANDLE(UEVR_IVRIOBuffer);
DECLARE_UEVR_HANDLE(UEVR_IVRSpatialAnchors);
DECLARE_UEVR_HANDLE(UEVR_IVRNotifications);
DECLARE_UEVR_HANDLE(UEVR_IVRDebug);
*/

namespace uevr {
namespace openvr{
UEVR_IVRSystem get_vr_system() {
    return (UEVR_IVRSystem)VR::get()->get_openvr_runtime()->hmd;
}

UEVR_IVRChaperone get_vr_chaperone() {
    return (UEVR_IVRChaperone)::vr::VRChaperone();
}

UEVR_IVRChaperoneSetup get_vr_chaperone_setup() {
    return (UEVR_IVRChaperoneSetup)::vr::VRChaperoneSetup();
}

UEVR_IVRCompositor get_vr_compositor() {
    return (UEVR_IVRCompositor)::vr::VRCompositor();
}

UEVR_IVROverlay get_vr_overlay() {
    return (UEVR_IVROverlay)::vr::VROverlay();
}

UEVR_IVROverlayView get_vr_overlay_view() {
    return (UEVR_IVROverlayView)::vr::VROverlayView();
}

UEVR_IVRHeadsetView get_vr_headset_view() {
    return (UEVR_IVRHeadsetView)::vr::VRHeadsetView();
}

UEVR_IVRScreenshots get_vr_screenshots() {
    return (UEVR_IVRScreenshots)::vr::VRScreenshots();
}

UEVR_IVRRenderModels get_vr_render_models() {
    return (UEVR_IVRRenderModels)::vr::VRRenderModels();
}

UEVR_IVRApplications get_vr_applications() {
    return (UEVR_IVRApplications)::vr::VRApplications();
}

UEVR_IVRSettings get_vr_settings() {
    return (UEVR_IVRSettings)::vr::VRSettings();
}

UEVR_IVRResources get_vr_resources() {
    return (UEVR_IVRResources)::vr::VRResources();
}

UEVR_IVRExtendedDisplay get_vr_extended_display() {
    return (UEVR_IVRExtendedDisplay)::vr::VRExtendedDisplay();
}

UEVR_IVRTrackedCamera get_vr_tracked_camera() {
    return (UEVR_IVRTrackedCamera)::vr::VRTrackedCamera();
}

UEVR_IVRDriverManager get_vr_driver_manager() {
    return (UEVR_IVRDriverManager)::vr::VRDriverManager();
}

UEVR_IVRInput get_vr_input() {
    return (UEVR_IVRInput)::vr::VRInput();
}

UEVR_IVRIOBuffer get_vr_io_buffer() {
    return (UEVR_IVRIOBuffer)::vr::VRIOBuffer();
}

UEVR_IVRSpatialAnchors get_vr_spatial_anchors() {
    return (UEVR_IVRSpatialAnchors)::vr::VRSpatialAnchors();
}

UEVR_IVRNotifications get_vr_notifications() {
    return (UEVR_IVRNotifications)::vr::VRNotifications();
}

UEVR_IVRDebug get_vr_debug() {
    return (UEVR_IVRDebug)::vr::VRDebug();
}
}
}

UEVR_OpenVRData g_openvr_data {
    uevr::openvr::get_vr_system,
    uevr::openvr::get_vr_chaperone,
    uevr::openvr::get_vr_chaperone_setup,
    uevr::openvr::get_vr_compositor,
    uevr::openvr::get_vr_overlay,
    uevr::openvr::get_vr_overlay_view,
    uevr::openvr::get_vr_headset_view,
    uevr::openvr::get_vr_screenshots,
    uevr::openvr::get_vr_render_models,
    uevr::openvr::get_vr_applications,
    uevr::openvr::get_vr_settings,
    uevr::openvr::get_vr_resources,
    uevr::openvr::get_vr_extended_display,
    uevr::openvr::get_vr_tracked_camera,
    uevr::openvr::get_vr_driver_manager,
    uevr::openvr::get_vr_input,
    uevr::openvr::get_vr_io_buffer,
    uevr::openvr::get_vr_spatial_anchors,
    uevr::openvr::get_vr_notifications,
    uevr::openvr::get_vr_debug
};

namespace uevr {
namespace openxr {
UEVR_XrInstance get_xr_instance() {
    return (UEVR_XrInstance)VR::get()->get_openxr_runtime()->instance;
}

UEVR_XrSession get_xr_session() {
    return (UEVR_XrSession)VR::get()->get_openxr_runtime()->session;
}

UEVR_XrSpace get_stage_space() {
    return (UEVR_XrSpace)VR::get()->get_openxr_runtime()->stage_space;
}

UEVR_XrSpace get_view_space() {
    return (UEVR_XrSpace)VR::get()->get_openxr_runtime()->view_space;
}
}
}

UEVR_OpenXRData g_openxr_data {
    uevr::openxr::get_xr_instance,
    uevr::openxr::get_xr_session,
    uevr::openxr::get_stage_space,
    uevr::openxr::get_view_space
};

UEVR_LuaData g_lua_data {
    .get_lua_state = []() -> lua_State* {
        auto& ll = LuaLoader::get();
        if (ll == nullptr) {
            return nullptr;
        }

        auto _ = ll->get_access_lock();
        auto& script_state = ll->get_state();

        if (script_state == nullptr) {
            return nullptr;
        }

        return script_state->lua();
    },
    .add_additional_bindings = [](lua_State* L) {
        auto& ll = LuaLoader::get();
        if (ll == nullptr) {
            return;
        }

        auto _ = ll->get_access_lock();
        auto sv = sol::state_view(L);

        ll->add_additional_bindings(sv);
    }
};

extern "C" __declspec(dllexport) UEVR_PluginInitializeParam g_plugin_initialize_param{
    nullptr, 
    &g_plugin_version, 
    &g_plugin_functions, 
    &g_plugin_callbacks,
    &uevr::g_renderer_data,
    &g_vr_data,
    &g_openvr_data,
    &g_openxr_data,
    &g_sdk_data,
    &g_lua_data,
};

void verify_sdk_pointers() {
    auto verify = [](auto& g) {
        spdlog::info("Verifying...");

        for (auto i = 0; i < sizeof(g) / sizeof(void*); ++i) {
            if (((void**)&g)[i] == nullptr) {
                spdlog::error("SDK pointer is null at index {}", i);
            }
        }
    };

    spdlog::info("Verifying SDK pointers...");

    verify(g_sdk_data);
}

std::shared_ptr<PluginLoader>& PluginLoader::get() {
    static auto instance = std::make_shared<PluginLoader>();
    return instance;
}

void PluginLoader::early_init() try {
    namespace fs = std::filesystem;

    std::scoped_lock _{m_mux};
    std::wstring module_path{};

    module_path.resize(1024, 0);
    module_path.resize(GetModuleFileNameW(nullptr, module_path.data(), module_path.size()));
    spdlog::info("[PluginLoader] Module path {}", utility::narrow(module_path));

    const auto plugin_path = Framework::get_persistent_dir() / "plugins";
    const auto global_plugins_path = Framework::get_persistent_dir() / ".." / "UEVR" / "plugins";
    
    spdlog::info("[PluginLoader] Creating directories {}", plugin_path.string());

    if (!fs::create_directories(plugin_path) && !fs::exists(plugin_path)) {
        spdlog::error("[PluginLoader] Failed to create directory for plugins: {}", plugin_path.string());
    } else {
        spdlog::info("[PluginLoader] Created directory for plugins: {}", plugin_path.string());
    }

    spdlog::info("[PluginLoader] Loading plugins...");

    auto load_plugins_from_dir = [this](std::filesystem::path path) {
        if (!fs::exists(path) || !fs::is_directory(path)) {
            return;
        }

        for (auto&& entry : fs::directory_iterator{path}) {
            auto&& path = entry.path();

            if (path.has_extension() && path.extension() == ".dll") {
                auto module = LoadLibrary(path.string().c_str());

                if (module == nullptr) {
                    spdlog::error("[PluginLoader] Failed to load {}", path.string());
                    m_plugin_load_errors.emplace(path.stem().string(), "Failed to load");
                    continue;
                }

                spdlog::info("[PluginLoader] Loaded {}", path.string());
                m_plugins.emplace(path.stem().string(), module);
            }
        }
    };

    load_plugins_from_dir(global_plugins_path);
    load_plugins_from_dir(plugin_path);
} catch(const std::exception& e) {
    spdlog::error("[PluginLoader] Exception during early init {}", e.what());
} catch(...) {
    spdlog::error("[PluginLoader] Unknown exception during early init");
}

std::optional<std::string> PluginLoader::on_initialize_d3d_thread() {
    std::scoped_lock _{m_mux};

    // Call UEVR_plugin_required_version on any dlls that export it.
    g_plugin_initialize_param.uevr_module = g_framework->get_framework_module();
    uevr::g_renderer_data.renderer_type = (int)g_framework->get_renderer_type();
    
    if (uevr::g_renderer_data.renderer_type == UEVR_RENDERER_D3D11) {
        auto& d3d11 = g_framework->get_d3d11_hook();

        uevr::g_renderer_data.device = d3d11->get_device();
        uevr::g_renderer_data.swapchain = d3d11->get_swap_chain();
    } else if (uevr::g_renderer_data.renderer_type == UEVR_RENDERER_D3D12) {
        auto& d3d12 = g_framework->get_d3d12_hook();

        uevr::g_renderer_data.device = d3d12->get_device();
        uevr::g_renderer_data.swapchain = d3d12->get_swap_chain();
        uevr::g_renderer_data.command_queue = d3d12->get_command_queue();
    } else {
        spdlog::error("[PluginLoader] Unsupported renderer type {}", uevr::g_renderer_data.renderer_type);
        return "PluginLoader: Unsupported renderer type detected";
    }

    verify_sdk_pointers();

    for (auto it = m_plugins.begin(); it != m_plugins.end();) {
        auto name = it->first;
        auto mod = it->second;
        auto required_version_fn = (UEVR_PluginRequiredVersionFn)GetProcAddress(mod, "uevr_plugin_required_version");

        if (required_version_fn == nullptr) {
            spdlog::info("[PluginLoader] {} has no uevr_plugin_required_version function, skipping...", name);

            ++it;
            continue;
        }

        UEVR_PluginVersion required_version{};

        try {
            required_version_fn(&required_version);
        } catch(...) {
            spdlog::error("[PluginLoader] {} has an exception in uevr_plugin_required_version, skipping...", name);
            m_plugin_load_errors.emplace(name, "Exception occurred in uevr_plugin_required_version");
            FreeLibrary(mod);
            it = m_plugins.erase(it);
            continue;
        }

        spdlog::info(
            "[PluginLoader] {} requires version {}.{}.{}", name, required_version.major, required_version.minor, required_version.patch);

        if (required_version.major != g_plugin_version.major) {
            spdlog::error("[PluginLoader] Plugin {} requires a different major version", name);
            m_plugin_load_errors.emplace(name, "Requires a different major version");
            FreeLibrary(mod);
            it = m_plugins.erase(it);
            continue;
        }

        if (required_version.minor > g_plugin_version.minor) {
            spdlog::error("[PluginLoader] Plugin {} requires a newer minor version", name);
            m_plugin_load_errors.emplace(name, "Requires a newer minor version");
            FreeLibrary(mod);
            it = m_plugins.erase(it);
            continue;
        }

        if (required_version.patch > g_plugin_version.patch) {
            spdlog::warn("[PluginLoader] Plugin {} desires a newer patch version", name);
            m_plugin_load_warnings.emplace(name, "Desires a newer patch version");
        }

        ++it;
    }

    // Call UEVR_plugin_initialize on any dlls that export it.
    for (auto it = m_plugins.begin(); it != m_plugins.end();) {
        auto name = it->first;
        auto mod = it->second;
        auto init_fn = (UEVR_PluginInitializeFn)GetProcAddress(mod, "uevr_plugin_initialize");

        if (init_fn == nullptr) {
            ++it;
            continue;
        }

        spdlog::info("[PluginLoader] Initializing {}...", name);
        try {
            if (!init_fn(&g_plugin_initialize_param)) {
                spdlog::error("[PluginLoader] Failed to initialize {}", name);
                m_plugin_load_errors.emplace(name, "Failed to initialize");
                FreeLibrary(mod);
                it = m_plugins.erase(it);
                continue;
            }
        } catch(...) {
            spdlog::error("[PluginLoader] {} has an exception in uevr_plugin_initialize, skipping...", name);
            m_plugin_load_errors.emplace(name, "Exception occurred in uevr_plugin_initialize");
            FreeLibrary(mod);
            it = m_plugins.erase(it);
            continue;
        }

        ++it;
    }

    return std::nullopt;
}

void PluginLoader::attempt_unload_plugins() {
    {
        std::unique_lock _{m_api_cb_mtx};

        for (auto& callbacks : m_plugin_callback_lists) {
            callbacks->clear();
        }

        {
            std::unique_lock _{m_ufunction_hooks_mtx};

            for (auto& [ufunction, hook] : m_ufunction_hooks) {
                hook->remove_callbacks();
            }
        }

        for (auto& pair : m_plugins) {
            FreeLibrary(pair.second);
        }

        m_inline_hooks.clear();
        m_plugins.clear();
    }

    // Done because we remove all the callbacks which Lua uses
    LuaLoader::get()->reset_scripts();
}

void PluginLoader::reload_plugins() {
    early_init();
    on_initialize_d3d_thread();
}

void PluginLoader::on_draw_ui() {
    std::scoped_lock _{m_mux};

    if (ImGui::Button("Attempt Unload Plugins")) {
        attempt_unload_plugins();
    }

    if (ImGui::Button("Reload Plugins")) {
        attempt_unload_plugins();
        reload_plugins();
    }

    if (!m_plugins.empty()) {
        ImGui::Text("Loaded plugins:");

        for (auto&& [name, _] : m_plugins) {
            ImGui::Text(name.c_str());
        }
    } else {
        ImGui::Text("No plugins loaded.");
    }

    if (!m_plugin_load_errors.empty()) {
        ImGui::Spacing();
        ImGui::Text("Errors:");
        for (auto&& [name, error] : m_plugin_load_errors) {
            ImGui::Text("%s - %s", name.c_str(), error.c_str());
        }
    }

    if (!m_plugin_load_warnings.empty()) {
        ImGui::Spacing();
        ImGui::Text("Warnings:");
        for (auto&& [name, warning] : m_plugin_load_warnings) {
            ImGui::Text("%s - %s", name.c_str(), warning.c_str());
        }
    }
}

void PluginLoader::on_present() {
    std::shared_lock _{m_api_cb_mtx};

    uevr::g_renderer_data.renderer_type = (int)g_framework->get_renderer_type();
    
    if (uevr::g_renderer_data.renderer_type == UEVR_RENDERER_D3D11) {
        auto& d3d11 = g_framework->get_d3d11_hook();

        uevr::g_renderer_data.device = d3d11->get_device();
        uevr::g_renderer_data.swapchain = d3d11->get_swap_chain();
    } else if (uevr::g_renderer_data.renderer_type == UEVR_RENDERER_D3D12) {
        auto& d3d12 = g_framework->get_d3d12_hook();

        uevr::g_renderer_data.device = d3d12->get_device();
        uevr::g_renderer_data.swapchain = d3d12->get_swap_chain();
        uevr::g_renderer_data.command_queue = d3d12->get_command_queue();
    }

    for (auto&& cb : m_on_present_cbs) {
        try {
            cb();
        } catch(...) {
            spdlog::error("[PluginLoader] Exception occurred in on_present callback; one of the plugins has an error.");
        }
    }
}

void PluginLoader::on_device_reset() {
    std::shared_lock _{m_api_cb_mtx};

    for (auto&& cb : m_on_device_reset_cbs) {
        try {
            cb();
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_device_reset callback; one of the plugins has an error.");
        }
    }
}

void PluginLoader::on_post_render_vr_framework_dx11(ID3D11DeviceContext* context, ID3D11Texture2D* tex, ID3D11RenderTargetView* rtv) {
    std::shared_lock _{m_api_cb_mtx};

    for (auto&& cb : m_on_post_render_vr_framework_dx11_cbs) {
        try {
            cb((void*)context, (void*)tex, (void*)rtv);
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_post_render_vr_framework_dx11 callback; one of the plugins has an error.");
            continue;
        }
    }
}

void PluginLoader::on_post_render_vr_framework_dx12(ID3D12GraphicsCommandList* command_list, ID3D12Resource* tex, D3D12_CPU_DESCRIPTOR_HANDLE* rtv) {
    std::shared_lock _{m_api_cb_mtx};

    for (auto&& cb : m_on_post_render_vr_framework_dx12_cbs) {
        try {
            cb((void*)command_list, (void*)tex, (void*)rtv);
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_post_render_vr_framework_dx12 callback; one of the plugins has an error.");
            continue;
        }
    }
}

bool PluginLoader::on_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    std::shared_lock _{m_api_cb_mtx};

    for (auto&& cb : m_on_message_cbs) {
        try {
            if (!cb(hwnd, msg, wparam, lparam)) {
                return false;
            }
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_message callback; one of the plugins has an error.");
            continue;
        }
    }

    return true;
}

void PluginLoader::on_xinput_get_state(uint32_t* retval, uint32_t user_index, XINPUT_STATE* state) {
    std::shared_lock _{m_api_cb_mtx};

    for (auto&& cb : m_on_xinput_get_state_cbs) {
        try {
            cb(retval, user_index, (void*)state);
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_xinput_get_state callback; one of the plugins has an error.");
            continue;
        }
    }
}

void PluginLoader::on_xinput_set_state(uint32_t* retval, uint32_t user_index, XINPUT_VIBRATION* vibration) {
    std::shared_lock _{m_api_cb_mtx};

    for (auto&& cb : m_on_xinput_set_state_cbs) {
        try {
            cb(retval, user_index, (void*)vibration);
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_xinput_set_state callback; one of the plugins has an error.");
            continue;
        }
    }
}

void PluginLoader::on_pre_engine_tick(sdk::UGameEngine* engine, float delta) {
    std::shared_lock _{m_api_cb_mtx};

    for (auto&& cb : m_on_pre_engine_tick_cbs) {
        try {
            cb((UEVR_UGameEngineHandle)engine, delta);
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_pre_engine_tick callback; one of the plugins has an error.");
        }
    }
}

void PluginLoader::on_post_engine_tick(sdk::UGameEngine* engine, float delta) {
    std::shared_lock _{m_api_cb_mtx};

    for (auto&& cb : m_on_post_engine_tick_cbs) {
        try {
            cb((UEVR_UGameEngineHandle)engine, delta);
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_post_engine_tick callback; one of the plugins has an error.");
        }
    }
}

void PluginLoader::on_pre_slate_draw_window(void* renderer, void* command_list, sdk::FViewportInfo* viewport_info) {
    std::shared_lock _{m_api_cb_mtx};

    for (auto&& cb : m_on_pre_slate_draw_window_render_thread_cbs) {
        try {
            cb((UEVR_FSlateRHIRendererHandle)renderer, (UEVR_FViewportInfoHandle)viewport_info);
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_pre_slate_draw_window callback; one of the plugins has an error.");
        }
    }
}

void PluginLoader::on_post_slate_draw_window(void* renderer, void* command_list, sdk::FViewportInfo* viewport_info) {
    std::shared_lock _{m_api_cb_mtx};

    for (auto&& cb : m_on_post_slate_draw_window_render_thread_cbs) {
        try {
            cb((UEVR_FSlateRHIRendererHandle)renderer, (UEVR_FViewportInfoHandle)viewport_info);
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_post_slate_draw_window callback; one of the plugins has an error.");
        }
    }
}

void PluginLoader::on_early_calculate_stereo_view_offset(void* stereo_device, const int32_t view_index, Rotator<float>* view_rotation, 
                                                       const float world_to_meters, Vector3f* view_location, bool is_double)
{
    std::shared_lock _{m_api_cb_mtx};

    for (auto&& cb : m_on_early_calculate_stereo_view_offset_cbs) {
        try {
            cb( (UEVR_StereoRenderingDeviceHandle)stereo_device, view_index, world_to_meters, 
                (UEVR_Vector3f*)view_location, (UEVR_Rotatorf*)view_rotation, is_double);
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_early_calculate_stereo_view_offset callback; one of the plugins has an error.");
        }
    }
}

void PluginLoader::on_pre_calculate_stereo_view_offset(void* stereo_device, const int32_t view_index, Rotator<float>* view_rotation, 
                                                       const float world_to_meters, Vector3f* view_location, bool is_double)
{
    std::shared_lock _{m_api_cb_mtx};

    for (auto&& cb : m_on_pre_calculate_stereo_view_offset_cbs) {
        try {
            cb( (UEVR_StereoRenderingDeviceHandle)stereo_device, view_index, world_to_meters, 
                (UEVR_Vector3f*)view_location, (UEVR_Rotatorf*)view_rotation, is_double);
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_pre_calculate_stereo_view_offset callback; one of the plugins has an error.");
        }
    }
}

void PluginLoader::on_post_calculate_stereo_view_offset(void* stereo_device, const int32_t view_index, Rotator<float>* view_rotation, 
                                                        const float world_to_meters, Vector3f* view_location, bool is_double)
{
    std::shared_lock _{m_api_cb_mtx};

    for (auto&& cb : m_on_post_calculate_stereo_view_offset_cbs) {
        try {
            cb( (UEVR_StereoRenderingDeviceHandle)stereo_device, view_index, world_to_meters, 
                (UEVR_Vector3f*)view_location, (UEVR_Rotatorf*)view_rotation, is_double);
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_post_calculate_stereo_view_offset callback; one of the plugins has an error.");
        }
    }
}

void PluginLoader::on_pre_viewport_client_draw(void* viewport_client, void* viewport, void* canvas) {
    std::shared_lock _{m_api_cb_mtx};

    for (auto&& cb : m_on_pre_viewport_client_draw_cbs) {
        try {
            cb((UEVR_UGameViewportClientHandle)viewport_client, (UEVR_FViewportHandle)viewport, (UEVR_FCanvasHandle)canvas);
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_pre_viewport_client_draw callback; one of the plugins has an error.");
        }
    }
}

void PluginLoader::on_post_viewport_client_draw(void* viewport_client, void* viewport, void* canvas) {
    std::shared_lock _{m_api_cb_mtx};

    for (auto&& cb : m_on_post_viewport_client_draw_cbs) {
        try {
            cb((UEVR_UGameViewportClientHandle)viewport_client, (UEVR_FViewportHandle)viewport, (UEVR_FCanvasHandle)canvas);
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in on_post_viewport_client_draw callback; one of the plugins has an error.");
        }
    }
}

void PluginLoader::dispatch_custom_event(const char* event_name, const char* event_data) {
    std::shared_lock _{m_api_cb_mtx};

    for (auto&& cb : m_on_custom_event_cbs) {
        try {
            cb(event_name, event_data);
        } catch(...) {
            spdlog::error("[APIProxy] Exception occurred in custom event callback; one of the plugins has an error.");
        }
    }
}

bool PluginLoader::add_on_present(UEVR_OnPresentCb cb) {
    std::unique_lock _{m_api_cb_mtx};

    m_on_present_cbs.push_back(cb);
    return true;
}

bool PluginLoader::add_on_device_reset(UEVR_OnDeviceResetCb cb) {
    std::unique_lock _{m_api_cb_mtx};

    m_on_device_reset_cbs.push_back(cb);
    return true;
}

bool PluginLoader::add_on_message(UEVR_OnMessageCb cb) {
    std::unique_lock _{m_api_cb_mtx};

    m_on_message_cbs.push_back(cb);
    return true;
}

bool PluginLoader::add_on_xinput_get_state(UEVR_OnXInputGetStateCb cb) {
    std::unique_lock _{m_api_cb_mtx};

    m_on_xinput_get_state_cbs.push_back(cb);
    return true;
}

bool PluginLoader::add_on_xinput_set_state(UEVR_OnXInputSetStateCb cb) {
    std::unique_lock _{m_api_cb_mtx};

    m_on_xinput_set_state_cbs.push_back(cb);
    return true;
}

bool PluginLoader::add_on_post_render_vr_framework_dx11(UEVR_OnPostRenderVRFrameworkDX11Cb cb) {
    std::unique_lock _{m_api_cb_mtx};

    m_on_post_render_vr_framework_dx11_cbs.push_back(cb);
    return true;
}

bool PluginLoader::add_on_post_render_vr_framework_dx12(UEVR_OnPostRenderVRFrameworkDX12Cb cb) {
    std::unique_lock _{m_api_cb_mtx};

    m_on_post_render_vr_framework_dx12_cbs.push_back(cb);
    return true;
}

bool PluginLoader::add_on_custom_event(UEVR_OnCustomEventCb cb) {
    std::unique_lock _{m_api_cb_mtx};

    m_on_custom_event_cbs.push_back(cb);
    return true;
}

bool PluginLoader::add_on_pre_engine_tick(UEVR_Engine_TickCb cb) {
    std::unique_lock _{m_api_cb_mtx};

    m_on_pre_engine_tick_cbs.push_back(cb);
    return true;
}

bool PluginLoader::add_on_post_engine_tick(UEVR_Engine_TickCb cb) {
    std::unique_lock _{m_api_cb_mtx};

    m_on_post_engine_tick_cbs.push_back(cb);
    return true;
}

bool PluginLoader::add_on_pre_slate_draw_window_render_thread(UEVR_Slate_DrawWindow_RenderThreadCb cb) {
    std::unique_lock _{m_api_cb_mtx};

    m_on_pre_slate_draw_window_render_thread_cbs.push_back(cb);
    return true;
}

bool PluginLoader::add_on_post_slate_draw_window_render_thread(UEVR_Slate_DrawWindow_RenderThreadCb cb) {
    std::unique_lock _{m_api_cb_mtx};

    m_on_post_slate_draw_window_render_thread_cbs.push_back(cb);
    return true;
}

bool PluginLoader::add_on_early_calculate_stereo_view_offset(UEVR_Stereo_CalculateStereoViewOffsetCb cb) {
    std::unique_lock _{m_api_cb_mtx};

    m_on_early_calculate_stereo_view_offset_cbs.push_back(cb);
    return true;
}


bool PluginLoader::add_on_pre_calculate_stereo_view_offset(UEVR_Stereo_CalculateStereoViewOffsetCb cb) {
    std::unique_lock _{m_api_cb_mtx};

    m_on_pre_calculate_stereo_view_offset_cbs.push_back(cb);
    return true;
}

bool PluginLoader::add_on_post_calculate_stereo_view_offset(UEVR_Stereo_CalculateStereoViewOffsetCb cb) {
    std::unique_lock _{m_api_cb_mtx};

    m_on_post_calculate_stereo_view_offset_cbs.push_back(cb);
    return true;
}

bool PluginLoader::add_on_pre_viewport_client_draw(UEVR_ViewportClient_DrawCb cb) {
    std::unique_lock _{m_api_cb_mtx};

    m_on_pre_viewport_client_draw_cbs.push_back(cb);
    return true;
}

bool PluginLoader::add_on_post_viewport_client_draw(UEVR_ViewportClient_DrawCb cb) {
    std::unique_lock _{m_api_cb_mtx};

    m_on_post_viewport_client_draw_cbs.push_back(cb);
    return true;
}

`

---
## ðŸ“ PluginLoader.hpp
**Sti:** $filePath  
**StÃ¸rrelse:** 11821 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include <map>
`

### ðŸ—ï¸ Classes
`cpp
class PluginLoader : public Mod {
`

### âš™ï¸ Mod Event Functions
`cpp
on_initialize_d3d_thread()
`

### âš™ï¸ General Functions
`cpp
void early_init()
`

### ðŸ“„ Komplet Indhold
`cpp
#pragma once

#include <map>
#include <mutex>
#include <shared_mutex>

#include <Windows.h>

#include <safetyhook.hpp>
#include <asmjit/asmjit.h>

#include "Mod.hpp"
#include "uevr/API.h"

namespace uevr {
extern UEVR_RendererData g_renderer_data;
}

extern "C" __declspec(dllexport) UEVR_PluginInitializeParam g_plugin_initialize_param;

class PluginLoader : public Mod {
public:
    static std::shared_ptr<PluginLoader>& get();

    // This is called prior to most UEVR initialization so that all plugins are at least **loaded** early on. UEVR_ramework plugin
    // specifics (`uevr_plugin_initialize`) are still delayed until UEVR_ramework is fully setup.
    void early_init();

    std::string_view get_name() const override { return "PluginLoader"; }
    bool is_advanced_mod() const override { return true; }
    std::optional<std::string> on_initialize_d3d_thread() override;
    void on_draw_ui() override;

    void on_present();
    void on_device_reset() override;
    bool on_message(HWND wnd, UINT message, WPARAM w_param, LPARAM l_param) override;
    void on_xinput_get_state(uint32_t* retval, uint32_t user_index, XINPUT_STATE* state) override;
    void on_xinput_set_state(uint32_t* retval, uint32_t user_index, XINPUT_VIBRATION* vibration) override;

    void on_post_render_vr_framework_dx11(ID3D11DeviceContext* context, ID3D11Texture2D*, ID3D11RenderTargetView* rtv) override;
    void on_post_render_vr_framework_dx12(ID3D12GraphicsCommandList* command_list, ID3D12Resource* rt, D3D12_CPU_DESCRIPTOR_HANDLE* rtv) override;
    
    void on_pre_engine_tick(sdk::UGameEngine* engine, float delta) override;
    void on_post_engine_tick(sdk::UGameEngine* engine, float delta) override;
    void on_pre_slate_draw_window(void* renderer, void* command_list, sdk::FViewportInfo* viewport_info) override;
    void on_post_slate_draw_window(void* renderer, void* command_list, sdk::FViewportInfo* viewport_info) override;
    void on_early_calculate_stereo_view_offset(void* stereo_device, const int32_t view_index, Rotator<float>* view_rotation, 
                                            const float world_to_meters, Vector3f* view_location, bool is_double) override;
    void on_pre_calculate_stereo_view_offset(void* stereo_device, const int32_t view_index, Rotator<float>* view_rotation, 
                                             const float world_to_meters, Vector3f* view_location, bool is_double) override;
    void on_post_calculate_stereo_view_offset(void* stereo_device, const int32_t view_index, Rotator<float>* view_rotation, 
                                              const float world_to_meters, Vector3f* view_location, bool is_double) override;
    void on_pre_viewport_client_draw(void* viewport_client, void* viewport, void* canvas) override;
    void on_post_viewport_client_draw(void* viewport_client, void* viewport, void* canvas) override;

    void dispatch_custom_event(const char* event_name, const char* event_data);
    
public:
    void attempt_unload_plugins();
    void reload_plugins();

    /*using UEVR_OnPresentCb = std::function<std::remove_pointer<::UEVR_OnPresentCb>::type>;
    using UEVR_OnDeviceResetCb = std::function<std::remove_pointer<::UEVR_OnDeviceResetCb>::type>;
    using UEVR_OnMessageCb = std::function<std::remove_pointer<::UEVR_OnMessageCb>::type>;
    using UEVR_OnXInputGetStateCb = std::function<std::remove_pointer<::UEVR_OnXInputGetStateCb>::type>;
    using UEVR_OnXInputSetStateCb = std::function<std::remove_pointer<::UEVR_OnXInputSetStateCb>::type>;*/

    /* VR Renderer */
    //using UEVR_OnPostRenderVRFrameworkDX11Cb = std::function<std::remove_pointer<::UEVR_OnPostRenderVRFrameworkDX11Cb>::type>;
    //using UEVR_OnPostRenderVRFrameworkDX12Cb = std::function<std::remove_pointer<::UEVR_OnPostRenderVRFrameworkDX12Cb>::type>;

    /* Engine specific callbacks */
    //using UEVR_Engine_TickCb = std::function<std::remove_pointer<::UEVR_Engine_TickCb>::type>;
    //using UEVR_Slate_DrawWindow_RenderThreadCb = std::function<std::remove_pointer<::UEVR_Slate_DrawWindow_RenderThreadCb>::type>;
    //using UEVR_Stereo_CalculateStereoViewOffsetCb = std::function<std::remove_pointer<::UEVR_Stereo_CalculateStereoViewOffsetCb>::type>;
    //using UEVR_ViewportClient_DrawCb = std::function<std::remove_pointer<::UEVR_ViewportClient_DrawCb>::type>;

    bool add_on_present(UEVR_OnPresentCb cb);
    bool add_on_device_reset(UEVR_OnDeviceResetCb cb);
    bool add_on_message(UEVR_OnMessageCb cb);
    bool add_on_xinput_get_state(UEVR_OnXInputGetStateCb cb);
    bool add_on_xinput_set_state(UEVR_OnXInputSetStateCb cb);
    bool add_on_post_render_vr_framework_dx11(UEVR_OnPostRenderVRFrameworkDX11Cb cb);
    bool add_on_post_render_vr_framework_dx12(UEVR_OnPostRenderVRFrameworkDX12Cb cb);
    bool add_on_custom_event(UEVR_OnCustomEventCb cb);

    bool add_on_pre_engine_tick(UEVR_Engine_TickCb cb);
    bool add_on_post_engine_tick(UEVR_Engine_TickCb cb);
    bool add_on_pre_slate_draw_window_render_thread(UEVR_Slate_DrawWindow_RenderThreadCb cb);
    bool add_on_post_slate_draw_window_render_thread(UEVR_Slate_DrawWindow_RenderThreadCb cb);
    bool add_on_early_calculate_stereo_view_offset(UEVR_Stereo_CalculateStereoViewOffsetCb cb);
    bool add_on_pre_calculate_stereo_view_offset(UEVR_Stereo_CalculateStereoViewOffsetCb cb);
    bool add_on_post_calculate_stereo_view_offset(UEVR_Stereo_CalculateStereoViewOffsetCb cb);
    bool add_on_pre_viewport_client_draw(UEVR_ViewportClient_DrawCb cb);
    bool add_on_post_viewport_client_draw(UEVR_ViewportClient_DrawCb cb);

    bool remove_callback(void* cb) {
        {
            std::unique_lock lock{m_api_cb_mtx};

            for (auto& pcb_list : m_plugin_callback_lists) {
                auto& cb_list = *pcb_list;
                std::erase_if(cb_list, [cb](auto& cb_func) {
                    return cb_func == cb;
                });
            }
        }

        {
            std::unique_lock lock{ m_ufunction_hooks_mtx };

            for (auto& [_, hook] : m_ufunction_hooks) {
                std::scoped_lock _{hook->mux};
                std::erase_if(hook->pre_callbacks, [cb](auto& cb_func) {
                    return (void*)cb_func == cb;
                });
                std::erase_if(hook->post_callbacks, [cb](auto& cb_func) {
                    return (void*)cb_func == cb;
                });
            }
        }

        return true;
    }

    size_t add_inline_hook(safetyhook::InlineHook&& hook) {
        std::scoped_lock lock{m_mux};

        auto state = std::make_unique<InlineHookState>(std::move(hook));
        m_inline_hooks[++m_inline_hook_idx] = std::move(state);

        return m_inline_hook_idx;
    }

    void remove_inline_hook(size_t idx) {
        std::scoped_lock lock{m_mux};

        if (!m_inline_hooks.contains(idx)) {
            return;
        }

        {
            std::scoped_lock _{m_inline_hooks[idx]->mux};
            m_inline_hooks[idx]->hook.reset();
        }

        m_inline_hooks.erase(idx);
    }

    bool hook_ufunction_ptr(UEVR_UFunctionHandle func, UEVR_UFunction_NativePreFn pre, UEVR_UFunction_NativePostFn post);

private:
    std::shared_mutex m_api_cb_mtx;
    std::vector<UEVR_OnPresentCb> m_on_present_cbs{};
    std::vector<UEVR_OnDeviceResetCb> m_on_device_reset_cbs{};
    std::vector<UEVR_OnPostRenderVRFrameworkDX11Cb> m_on_post_render_vr_framework_dx11_cbs{};
    std::vector<UEVR_OnPostRenderVRFrameworkDX12Cb> m_on_post_render_vr_framework_dx12_cbs{};
    std::vector<UEVR_OnMessageCb> m_on_message_cbs{};
    std::vector<UEVR_OnXInputGetStateCb> m_on_xinput_get_state_cbs{};
    std::vector<UEVR_OnXInputSetStateCb> m_on_xinput_set_state_cbs{};
    std::vector<UEVR_OnCustomEventCb> m_on_custom_event_cbs{};

    std::vector<UEVR_Engine_TickCb> m_on_pre_engine_tick_cbs{};
    std::vector<UEVR_Engine_TickCb> m_on_post_engine_tick_cbs{};
    std::vector<UEVR_Slate_DrawWindow_RenderThreadCb> m_on_pre_slate_draw_window_render_thread_cbs{};
    std::vector<UEVR_Slate_DrawWindow_RenderThreadCb> m_on_post_slate_draw_window_render_thread_cbs{};
    std::vector<UEVR_Stereo_CalculateStereoViewOffsetCb> m_on_early_calculate_stereo_view_offset_cbs{};
    std::vector<UEVR_Stereo_CalculateStereoViewOffsetCb> m_on_pre_calculate_stereo_view_offset_cbs{};
    std::vector<UEVR_Stereo_CalculateStereoViewOffsetCb> m_on_post_calculate_stereo_view_offset_cbs{};
    std::vector<UEVR_ViewportClient_DrawCb> m_on_pre_viewport_client_draw_cbs{};
    std::vector<UEVR_ViewportClient_DrawCb> m_on_post_viewport_client_draw_cbs{};

    using generic_std_function = void*;

    std::vector<std::vector<generic_std_function>*> m_plugin_callback_lists{
        // Plugin
        (std::vector<generic_std_function>*)&m_on_present_cbs,
        (std::vector<generic_std_function>*)&m_on_device_reset_cbs,

        // VR Renderer
        (std::vector<generic_std_function>*)&m_on_post_render_vr_framework_dx11_cbs,
        (std::vector<generic_std_function>*)&m_on_post_render_vr_framework_dx12_cbs,

        // Windows CBs
        (std::vector<generic_std_function>*)&m_on_message_cbs,
        (std::vector<generic_std_function>*)&m_on_xinput_get_state_cbs,
        (std::vector<generic_std_function>*)&m_on_xinput_set_state_cbs,

        // Custom
        (std::vector<generic_std_function>*)&m_on_custom_event_cbs,

        // SDK
        (std::vector<generic_std_function>*)&m_on_pre_engine_tick_cbs,
        (std::vector<generic_std_function>*)&m_on_post_engine_tick_cbs,
        (std::vector<generic_std_function>*)&m_on_pre_slate_draw_window_render_thread_cbs,
        (std::vector<generic_std_function>*)&m_on_post_slate_draw_window_render_thread_cbs,
        (std::vector<generic_std_function>*)&m_on_early_calculate_stereo_view_offset_cbs,
        (std::vector<generic_std_function>*)&m_on_pre_calculate_stereo_view_offset_cbs,
        (std::vector<generic_std_function>*)&m_on_post_calculate_stereo_view_offset_cbs,
        (std::vector<generic_std_function>*)&m_on_pre_viewport_client_draw_cbs,
        (std::vector<generic_std_function>*)&m_on_post_viewport_client_draw_cbs
    };

private:
    std::recursive_mutex m_mux{};
    std::map<std::string, HMODULE> m_plugins{};
    std::map<std::string, std::string> m_plugin_load_errors{};
    std::map<std::string, std::string> m_plugin_load_warnings{};

    struct InlineHookState {
        InlineHookState(safetyhook::InlineHook&& hook)
            : hook{std::move(hook)}
        {
        }

        safetyhook::InlineHook hook{};
        std::mutex mux{};
    };

    //std::vector<InlineHookState> m_inline_hooks{};
    std::unordered_map<size_t, std::unique_ptr<InlineHookState>> m_inline_hooks{};
    size_t m_inline_hook_idx{0};

    asmjit::JitRuntime m_jit_runtime{};

    struct UFunctionHookState {
        using Intermediary = void(*)(UEVR_UObjectHandle, void*, void*, sdk::UFunction*);
        Intermediary jitted_pre{};

        std::unique_ptr<PointerHook> hook{};
        std::vector<UEVR_UFunction_NativePreFn> pre_callbacks{};
        std::vector<UEVR_UFunction_NativePostFn> post_callbacks{};
        std::recursive_mutex mux{};

        void remove_callbacks() {
            std::scoped_lock _{mux};

            pre_callbacks.clear();
            post_callbacks.clear();
        }
    };
    std::shared_mutex m_ufunction_hooks_mtx{};
    std::unordered_map<sdk::UFunction*, std::unique_ptr<UFunctionHookState>> m_ufunction_hooks{};
    static void ufunction_hook_intermediary(UEVR_UObjectHandle obj, void* frame, void* result, sdk::UFunction* func);
};

`

---
## ðŸ“ UObjectHook.cpp
**Sti:** $filePath  
**StÃ¸rrelse:** 162344 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include <fstream>
`

### ðŸ—ï¸ Classes
`cpp
class())) {
`

### âš™ï¸ Mod Event Functions
`cpp
on_offset", utility::math::to_json(rotation_offset)
`

### âš™ï¸ General Functions
`cpp
void UObjectHook::MotionControllerStateBase::from_json(const nlohmann::json& data)
`

### ðŸ“„ Komplet Indhold
`cpp
#include <fstream>

#include <utility/Logging.hpp>
#include <utility/String.hpp>
#include <utility/ScopeGuard.hpp>

#include <sdk/UObjectBase.hpp>
#include <sdk/UObjectArray.hpp>
#include <sdk/UClass.hpp>
#include <sdk/FField.hpp>
#include <sdk/FProperty.hpp>
#include <sdk/FEnumProperty.hpp>
#include <sdk/UFunction.hpp>
#include <sdk/AActor.hpp>
#include <sdk/threading/GameThreadWorker.hpp>
#include <sdk/FMalloc.hpp>
#include <sdk/FStructProperty.hpp>
#include <sdk/USceneComponent.hpp>
#include <sdk/UGameplayStatics.hpp>
#include <sdk/APlayerController.hpp>
#include <sdk/APawn.hpp>
#include <sdk/APlayerCameraManager.hpp>
#include <sdk/ScriptVector.hpp>
#include <sdk/FBoolProperty.hpp>
#include <sdk/FObjectProperty.hpp>
#include <sdk/FArrayProperty.hpp>
#include <sdk/UMotionControllerComponent.hpp>

#include "uobjecthook/SDKDumper.hpp"
#include "VR.hpp"

#include "UObjectHook.hpp"

//#define VERBOSE_UOBJECTHOOK

std::shared_ptr<UObjectHook>& UObjectHook::get() {
    static std::shared_ptr<UObjectHook> instance = std::make_shared<UObjectHook>();
    return instance;
}

UObjectHook::MotionControllerState::~MotionControllerState() {
    if (this->adjustment_visualizer != nullptr) {
        GameThreadWorker::get().enqueue([vis = this->adjustment_visualizer]() {
            SPDLOG_INFO("[UObjectHook::MotionControllerState] Destroying adjustment visualizer for component {:x}", (uintptr_t)vis);

            if (!UObjectHook::get()->exists(vis)) {
                return;
            }

            vis->destroy_actor();

            SPDLOG_INFO("[UObjectHook::MotionControllerState] Destroyed adjustment visualizer for component {:x}", (uintptr_t)vis);
        });
    }
}

nlohmann::json UObjectHook::MotionControllerStateBase::to_json() const {
    return {
        {"rotation_offset", utility::math::to_json(rotation_offset)},
        {"location_offset", utility::math::to_json(location_offset)},
        {"hand", hand},
        {"permanent", permanent}
    };
}

void UObjectHook::MotionControllerStateBase::from_json(const nlohmann::json& data) {
    if (data.contains("rotation_offset")) {
        rotation_offset = utility::math::from_json_quat(data["rotation_offset"]);
    }

    if (data.contains("location_offset")) {
        location_offset = utility::math::from_json_vec3(data["location_offset"]);
    }

    if (data.contains("hand")) {
        hand = data["hand"].get<uint8_t>();
        hand = hand % (uint8_t)MotionControllerStateBase::Hand::LAST;
    }

    if (data.contains("permanent") && data["permanent"].is_boolean()) {
        permanent = data["permanent"].get<bool>();
    }
}

void UObjectHook::activate() {
    if (m_hooked) {
        return;
    }

    if (GameThreadWorker::get().is_same_thread()) {
        hook();
        return;
    }

    m_wants_activate = true;
}

void UObjectHook::hook() {
    if (m_hooked) {
        return;
    }

    SPDLOG_INFO("[UObjectHook] Hooking UObjectBase");

    m_hooked = true;
    m_wants_activate = false;

    auto destructor_fn = sdk::UObjectBase::get_destructor();

    if (!destructor_fn) {
        SPDLOG_ERROR("[UObjectHook] Failed to find UObjectBase::destructor, cannot hook UObjectBase");
        return;
    }

    auto add_object_fn = sdk::UObjectBase::get_add_object();

    if (!add_object_fn) {
        SPDLOG_ERROR("[UObjectHook] Failed to find UObjectBase::AddObject, cannot hook UObjectBase");
        return;
    }

    m_destructor_hook = safetyhook::create_inline((void**)destructor_fn.value(), &destructor);

    if (!m_destructor_hook) {
        SPDLOG_ERROR("[UObjectHook] Failed to hook UObjectBase::destructor, cannot hook UObjectBase");
        return;
    }

    m_add_object_hook = safetyhook::create_inline((void**)add_object_fn.value(), &add_object);

    if (!m_add_object_hook) {
        SPDLOG_ERROR("[UObjectHook] Failed to hook UObjectBase::AddObject, cannot hook UObjectBase");
        return;
    }

    SPDLOG_INFO("[UObjectHook] Hooked UObjectBase");

    // Add all the objects that already exist
    auto uobjectarray = sdk::FUObjectArray::get();

    for (auto i = 0; i < uobjectarray->get_object_count(); ++i) {
        auto object = uobjectarray->get_object(i);

        if (object == nullptr || object->object == nullptr) {
            continue;
        }

        add_new_object(object->object);
    }

    SPDLOG_INFO("[UObjectHook] Added {} existing objects", m_objects.size());

    SPDLOG_INFO("[UObjectHook] Deserializing persistent states");
    reload_persistent_states();
    SPDLOG_INFO("[UObjectHook] Deserialized {} persistent states", m_persistent_states.size());

    m_fully_hooked = true;
}

void UObjectHook::hook_process_event() {
    if (m_attempted_hook_process_event) {
        return;
    }

    m_attempted_hook_process_event = true;

    auto uobjectarray = sdk::FUObjectArray::get();

    if (uobjectarray == nullptr) {
        return;
    }

    const auto process_event_index = sdk::UObject::get_process_event_index();

    if (process_event_index == 0) {
        return;
    }

    sdk::UObject* first_obj = nullptr;

    for (auto i = 0; i < uobjectarray->get_object_count(); ++i) {
        const auto object = uobjectarray->get_object(i);

        if (object != nullptr && object->object != nullptr) {
            first_obj = (sdk::UObject*)object->object;
            break;
        }
    }

    if (first_obj != nullptr) {
        const auto vt = *(void***)first_obj;

        if (vt != nullptr) {
            std::scoped_lock _{m_function_mutex};
            auto fn = vt[process_event_index];
            SPDLOG_INFO("[UObjectHook] ProcessEvent {:x}", (uintptr_t)fn);
            m_process_event_hook = safetyhook::create_inline(fn, &process_event_hook);

            if (m_process_event_hook) {
                SPDLOG_INFO("[UObjectHook] Hooked UObject::ProcessEvent");
                m_hooked_process_event = true;
            } else {
                SPDLOG_ERROR("[UObjectHook] Failed to hook UObject::ProcessEvent");
            }
        }
    }
}

void* UObjectHook::process_event_hook(sdk::UObject* obj, sdk::UFunction* func, void* params, void* r9) {
    auto& hook = UObjectHook::get();

    bool do_heavy_data_once = false;

    if (hook->m_process_event_listening) {
        std::scoped_lock _{hook->m_function_mutex};
        
        auto& data = hook->m_called_functions[func];
        ++data.call_count;

        hook->m_most_recent_functions.push_front(func);

        if (hook->m_most_recent_functions.size() > 200) {
            hook->m_most_recent_functions.pop_back();
        }
    }

    auto result = hook->m_process_event_hook.unsafe_call<void*>(obj, func, params, r9);

    if (hook->m_process_event_listening) {
        std::scoped_lock _{hook->m_function_mutex};

        auto& data = hook->m_called_functions[func];

        if (data.heavy_data == nullptr) {
            do_heavy_data_once = true;
            data.heavy_data = std::make_unique<CalledFunctionInfo::HeavyData>();

            const auto ps = func->get_properties_size();
            const auto ma = func->get_min_alignment();
        
            if (ma > 1) {
                data.heavy_data->params.resize(((ps + ma - 1) / ma) * ma);
            } else {
                data.heavy_data->params.resize(ps);
            }

            memset(data.heavy_data->params.data(), 0, data.heavy_data->params.size());
        }

        if ((data.wants_heavy_data || do_heavy_data_once) && !data.heavy_data->params.empty()) {
            auto bak = data.heavy_data->params;

            try {
                memcpy(data.heavy_data->params.data(), params, data.heavy_data->params.size());
            } catch(...) {
                //SPDLOG_ERROR("[UObjectHook] Failed to copy params for function {:x}", (uintptr_t)func);
                return result;
            }
    
            // Go through all properties and look for arrays and upgrade them so we actually own the data
            // We'll do this by copying the data to a new array
            for (auto prop = func->get_child_properties(); prop != nullptr; prop = prop->get_next()) {
                const auto c = prop->get_class();
                if (c == nullptr) {
                    continue;
                }
    
                const auto cname = c->get_name().to_string();
                const auto cname_hash = ::utility::hash(cname);
    
                switch (cname_hash) {
                case L"ArrayProperty"_fnv:
                {
                    using GenericArray = sdk::TArray<void*>;
                    const auto prop_desc = (sdk::FArrayProperty*)prop;

                    size_t inner_size = sizeof(void*);
                    bool supported = false;
    
                    const auto inner = prop_desc->get_inner();
    
                    if (inner != nullptr) {
                        const auto inner_c = inner->get_class();
    
                        if (inner_c != nullptr) {
                            const auto inner_cname = inner_c->get_name().to_string();
                            const auto inner_cname_hash = ::utility::hash(inner_cname);
    
                            // todo... recursive array stuff? good enough for now
                            switch (inner_cname_hash) {
                            case L"StructProperty"_fnv:
                            {
                                const auto s = ((sdk::FStructProperty*)inner)->get_struct();
    
                                if (s == nullptr) {
                                    break;
                                }
    
                                if (s->is_a(sdk::UScriptStruct::static_class())) {
                                    inner_size = s->get_struct_size();
                                } else {
                                    inner_size = s->get_properties_size();
                                }

                                supported = true;

                                break;
                            }
                            case L"ObjectProperty"_fnv:
                            case L"InterfaceProperty"_fnv:
                            {
                                inner_size = sizeof(void*);
                                supported = true;
                                break;
                            }
    
                            default:
                                break;
                            }
                        }
                    }

                    // uh oh
                    if (prop_desc->get_offset() >= data.heavy_data->params.size()) {
                        break;
                    }

                    auto& arr = *(GenericArray*)((uintptr_t)data.heavy_data->params.data() + prop_desc->get_offset());

                    if (!supported) {
                        arr.count = 0;
                        arr.capacity = 0;
                        arr.data = nullptr;
                        break;
                    }
    
                    auto& bak_arr = *(GenericArray*)((uintptr_t)bak.data() + prop_desc->get_offset());
                    auto new_arr = sdk::TArray<void*>{};

                    if (bak_arr.data != nullptr) {
                        new_arr = std::move(bak_arr);
    
                        if (arr.capacity > new_arr.capacity) {
                            new_arr.data = (void**)sdk::FMalloc::get()->realloc(new_arr.data, arr.capacity * inner_size, sizeof(void*));
                            new_arr.capacity = arr.capacity;
                        }
    
                        new_arr.count = arr.count;
                    } else if (arr.capacity > 0) {
                        new_arr.data = (void**)sdk::FMalloc::get()->malloc(arr.capacity * inner_size, sizeof(void*));
                        std::memset(new_arr.data, 0, arr.capacity * inner_size);
                        new_arr.count = arr.count;
                        new_arr.capacity = arr.capacity;
                    }
    
                    if (arr.data != nullptr && arr.count > 0 && arr.capacity >= arr.count && new_arr.data != nullptr && !IsBadReadPtr((void*)arr.data, arr.capacity * inner_size)) {
                        memcpy(new_arr.data, arr.data, arr.count * inner_size);
                        arr.data = nullptr;
                        arr.count = 0;
                        arr.capacity = 0;
                        arr = std::move(new_arr);
                    } else {
                        arr.count = 0;  
                    }
                }
                case L"StrProperty"_fnv:
                {
                    using FString = sdk::TArray<wchar_t>;
    
                    const auto prop_desc = (sdk::FProperty*)prop;

                    // uh oh
                    if (prop_desc->get_offset() >= data.heavy_data->params.size()) {
                        break;
                    }

                    auto& str = *(FString*)((uintptr_t)data.heavy_data->params.data() + prop_desc->get_offset());
                    auto& bak_str = *(FString*)((uintptr_t)bak.data() + prop_desc->get_offset());
    
                    auto new_str = FString{};
    
                    // Same thing but much simpler because we know it's wchar_t
                    if (bak_str.data != nullptr) {
                        new_str = std::move(bak_str);
    
                        if (str.capacity > new_str.capacity) {
                            new_str.data = (wchar_t*)sdk::FMalloc::get()->realloc(new_str.data, str.capacity * sizeof(wchar_t), sizeof(wchar_t));
                            new_str.capacity = str.capacity;
                        }
    
                        new_str.count = str.count;
                    } else if (str.capacity > 0) {
                        new_str.data = (wchar_t*)sdk::FMalloc::get()->malloc(str.capacity * sizeof(wchar_t), sizeof(wchar_t));
                        std::memset(new_str.data, 0, str.capacity * sizeof(wchar_t));
                        new_str.count = str.count;
                        new_str.capacity = str.capacity;
                    }
    
                    if (str.data != nullptr && str.count > 0 && str.capacity >= str.count && new_str.data != nullptr && !IsBadReadPtr((void*)str.data, str.capacity * sizeof(wchar_t))) {
                        memcpy(new_str.data, str.data, str.count * sizeof(wchar_t));
                        str.data = nullptr;
                        str.count = 0;
                        str.capacity = 0;
                        str = std::move(new_str);
                    } else {
                        str.count = 0;
                    }
                }
                    break;
                default:
                    break;
                };
            }
        }
    }

    return result;
}

void UObjectHook::add_new_object(sdk::UObjectBase* object) {
    std::unique_lock _{m_mutex};
    std::unique_ptr<MetaObject> meta_object{};

    /*static const auto prim_comp_t = sdk::find_uobject<sdk::UClass>(L"Class /Script/Engine.PrimitiveComponent");

    if (prim_comp_t != nullptr && object->get_class()->is_a(prim_comp_t)) {
        static const auto bGenerateOverlapEvents = (sdk::FBoolProperty*)prim_comp_t->find_property(L"bGenerateOverlapEvents");

        if (bGenerateOverlapEvents != nullptr) {
            bGenerateOverlapEvents->set_value_in_object(object, true);
        }
    }*/

    const auto c = object->get_class();

    if (c == nullptr) {
        return;
    }

    if (!m_reusable_meta_objects.empty()) {
        meta_object = std::move(m_reusable_meta_objects.back());
        m_reusable_meta_objects.pop_back();
    } else {
        meta_object = std::make_unique<MetaObject>();
        meta_object->super_classes.reserve(16);
    }

    m_objects.insert(object);
    meta_object->super_classes.clear();
    meta_object->full_name = object->get_full_name();
    meta_object->uclass = object->get_class();

    m_most_recent_objects.push_front((sdk::UObject*)object);

    if (m_most_recent_objects.size() > 50) {
        m_most_recent_objects.pop_back();
    }

    for (auto super = (sdk::UStruct*)object->get_class(); super != nullptr; super = super->get_super_struct()) {
        meta_object->super_classes.push_back((sdk::UClass*)super);

        m_objects_by_class[(sdk::UClass*)super].insert(object);

        if (auto it = m_on_creation_add_component_jobs.find((sdk::UClass*)super); it != m_on_creation_add_component_jobs.end()) {
            GameThreadWorker::get().enqueue([object, this]() {
                if (!this->exists(object)) {
                    return;
                }

                for (auto super = (sdk::UStruct*)object->get_class(); super != nullptr; super = super->get_super_struct()) {
                    std::function<void(sdk::UObject*)> job{};

                    {
                        std::shared_lock _{m_mutex};

                        if (auto it = this->m_on_creation_add_component_jobs.find((sdk::UClass*)super); it != this->m_on_creation_add_component_jobs.end()) {
                            job = it->second;
                        }
                    }

                    if (job) {
                        job((sdk::UObject*)object);
                    }
                }
            });
        }
    }

    m_meta_objects[object] = std::move(meta_object);

#ifdef VERBOSE_UOBJECTHOOK
    SPDLOG_INFO("Adding object {:x} {:s}", (uintptr_t)object, utility::narrow(m_meta_objects[object]->full_name));
#endif
}

void UObjectHook::on_config_load(const utility::Config& cfg, bool set_defaults) {
    ZoneScopedN(__FUNCTION__);

    for (IModValue& option : m_options) {
        option.config_load(cfg, set_defaults);
    }

    if (!set_defaults && m_enabled_at_startup->value()) {
        m_wants_activate = true;
    }
}

void UObjectHook::on_config_save(utility::Config& cfg) {
    ZoneScopedN(__FUNCTION__);

    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }
}

void UObjectHook::on_pre_engine_tick(sdk::UGameEngine* engine, float delta) {
    m_last_delta_time = delta;

    if (m_wants_activate) {
        hook();
    }
    
    if (m_fully_hooked) {
        {
            std::shared_lock _{m_mutex};
            const auto ui_active = g_framework->is_drawing_ui();

            for (auto& state : m_motion_controller_attached_components) {
                if (m_overlap_detection_actor == nullptr && state.second->adjusting && ui_active) {
                    state.second->adjusting = false;
                }
            }
        }

        update_persistent_states();
    }
}

const auto quat_converter = glm::quat{Matrix4x4f {
    0, 0, -1, 0,
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 0, 1
}};

// TODO: split this into some functions because its getting a bit massive
void UObjectHook::on_pre_calculate_stereo_view_offset(void* stereo_device, const int32_t view_index, Rotator<float>* view_rotation, 
                                        const float world_to_meters, Vector3f* view_location, bool is_double)
{
    if (!m_fully_hooked) {
        return;
    }

    auto& vr = VR::get();

    if (!vr->is_hmd_active()) {
        return;
    }

    if (m_uobject_hook_disabled) {
        return;
    }

    auto view_d = (Vector3d*)view_location;
    auto rot_d = (Rotator<double>*)view_rotation;

    if (is_double) {
        m_last_camera_location = glm::vec3{*view_d};
    } else {
        m_last_camera_location = *view_location;
    }

    if (m_camera_attach.object != nullptr) {
        if (m_camera_attach.object->is_a(sdk::AActor::static_class())) {
            const auto actor = (sdk::AActor*)m_camera_attach.object;
            const auto location = actor->get_actor_location();
            const auto rotation = actor->get_actor_rotation();

            // Conv rotation to glm quat
            const auto rotation_glm_mat = glm::yawPitchRoll(
                glm::radians(-rotation.y),
                glm::radians(rotation.x),
                glm::radians(-rotation.z));
            const auto rotation_glm_quat = glm::quat{rotation_glm_mat};

            const auto adjusted_loc = location - (quat_converter * (rotation_glm_quat * utility::math::ue4_to_glm(m_camera_attach.offset)));

            if (is_double) {
                *view_d = glm::vec<3, double>{adjusted_loc};
            } else {
                *view_location = adjusted_loc;
            }
        } else if (m_camera_attach.object->is_a(sdk::USceneComponent::static_class())) {
            const auto comp = (sdk::USceneComponent*)m_camera_attach.object;
            const auto location = comp->get_world_location();
            const auto rotation = comp->get_world_rotation();

            // Conv rotation to glm quat
            const auto rotation_glm_mat = glm::yawPitchRoll(
                glm::radians(-rotation.y),
                glm::radians(rotation.x),
                glm::radians(-rotation.z));
            const auto rotation_glm_quat = glm::quat{rotation_glm_mat};

            const auto adjusted_loc = location - (quat_converter * (rotation_glm_quat * utility::math::ue4_to_glm(m_camera_attach.offset)));

            if (is_double) {
                *view_d = glm::vec<3, double>{adjusted_loc};
            } else {
                *view_location = adjusted_loc;
            }
        } // else todo?
    }

    if ((view_index + 1) % 2 == 0) {
        tick_attachments(view_rotation, world_to_meters, view_location, is_double);
    }
}

void UObjectHook::on_post_calculate_stereo_view_offset(void* stereo_device, const int32_t view_index, Rotator<float>* view_rotation, 
                                                    const float world_to_meters, Vector3f* view_location, bool is_double)
{
    if (!m_fully_hooked) {
        return;
    }

    if (!VR::get()->is_hmd_active()) {
        return;
    }

    if (m_uobject_hook_disabled) {
        return;
    }

    std::shared_lock _{m_mutex};
    bool any_adjusting = false;
    for (auto& it : m_motion_controller_attached_components) {
        if (it.second->adjusting) {
            any_adjusting = true;
            break;
        }
    }

    VR::get()->set_aim_allowed(!any_adjusting);
}

void UObjectHook::tick_attachments(Rotator<float>* view_rotation, const float world_to_meters, Vector3f* view_location, bool is_double) {
    if (m_uobject_hook_disabled) {
        return;
    }

    auto& vr = VR::get();
    auto view_d = (Vector3d*)view_location;
    auto rot_d = (Rotator<double>*)view_rotation;

    const auto view_mat_inverse = !is_double ? 
        glm::yawPitchRoll(
            glm::radians(-view_rotation->yaw),
            glm::radians(view_rotation->pitch),
            glm::radians(-view_rotation->roll)) : 
        glm::yawPitchRoll(
            glm::radians(-(float)rot_d->yaw),
            glm::radians((float)rot_d->pitch),
            glm::radians(-(float)rot_d->roll));

    const auto view_quat_inverse = glm::quat {
        view_mat_inverse
    };

    auto vqi_norm = glm::normalize(view_quat_inverse);
    
    // Decoupled Pitch
    if (vr->is_decoupled_pitch_enabled()) {
        vqi_norm = utility::math::flatten(vqi_norm);
    }

    const auto rotation_offset = vr->get_rotation_offset();
    const auto hmd_origin = glm::vec3{vr->get_transform(0)[3]};
    const auto pos = glm::vec3{rotation_offset * (hmd_origin - glm::vec3{vr->get_standing_origin()})};

    const auto adjusted_world_to_meters = world_to_meters * vr->get_world_scale();

    const auto view_quat_inverse_flat = utility::math::flatten(view_quat_inverse);
    const auto offset1 = quat_converter * (glm::normalize(view_quat_inverse_flat) * (pos * adjusted_world_to_meters));

    glm::vec3 final_position{};

    if (is_double) {
        final_position = glm::vec3{*view_d} - offset1;
    } else {
        final_position = *view_location - offset1;
    }

    auto with_mutex = [this](auto fn) {
        std::shared_lock _{m_mutex};
        auto result = fn();

        return result;
    };

    auto comps = with_mutex([this]() { return m_motion_controller_attached_components; });

    const auto is_using_controllers = vr->is_using_controllers();
    const auto has_any_head_components = std::any_of(comps.begin(), comps.end(), [](auto& it) { return it.second->hand == 2; });

    if (!is_using_controllers && !has_any_head_components) {
        return;
    }

    glm::vec3 right_hand_position = vr->get_grip_position(vr->get_right_controller_index());
    glm::quat right_hand_rotation = vr->get_aim_rotation(vr->get_right_controller_index());

    const float lerp_speed = m_attach_lerp_speed->value() * m_last_delta_time;

    if (m_attach_lerp_enabled->value()) {
        auto spherical_distance_right = glm::dot(right_hand_rotation, m_last_right_aim_rotation);

        if (spherical_distance_right < 0.0f) {
            spherical_distance_right = -spherical_distance_right;
        }

        const auto lenr = glm::max(1.0f, glm::length(right_hand_position - m_last_right_grip_location));
        m_last_right_grip_location = glm::lerp(m_last_right_grip_location, right_hand_position, glm::min(1.0f, lerp_speed * lenr));
        m_last_right_aim_rotation = glm::slerp(m_last_right_aim_rotation, right_hand_rotation, lerp_speed * spherical_distance_right);
        right_hand_position = m_last_right_grip_location;
        right_hand_rotation = m_last_right_aim_rotation;
    }

    const auto original_right_hand_rotation = right_hand_rotation;
    const auto original_right_hand_position = right_hand_position - hmd_origin;

    glm::vec3 left_hand_position = vr->get_grip_position(vr->get_left_controller_index());
    glm::quat left_hand_rotation = vr->get_aim_rotation(vr->get_left_controller_index());

    if (m_attach_lerp_enabled->value()) {
        auto spherical_distance_left = glm::dot(left_hand_rotation, m_last_left_aim_rotation);

        if (spherical_distance_left < 0.0f) {
            spherical_distance_left = -spherical_distance_left;
        }

        const auto lenl = glm::max(1.0f, glm::length(left_hand_position - m_last_left_grip_location));
        m_last_left_grip_location = glm::lerp(m_last_left_grip_location, left_hand_position, glm::min(1.0f, lerp_speed * lenl));
        m_last_left_aim_rotation = glm::slerp(m_last_left_aim_rotation, left_hand_rotation, lerp_speed * spherical_distance_left);
        left_hand_position = m_last_left_grip_location;
        left_hand_rotation = m_last_left_aim_rotation;
    }

    right_hand_position = glm::vec3{rotation_offset * (right_hand_position - hmd_origin)};
    left_hand_position = glm::vec3{rotation_offset * (left_hand_position - hmd_origin)};

    right_hand_position = quat_converter * (glm::normalize(view_quat_inverse_flat) * (right_hand_position * adjusted_world_to_meters));
    left_hand_position = quat_converter * (glm::normalize(view_quat_inverse_flat) * (left_hand_position * adjusted_world_to_meters));

    right_hand_position = final_position - right_hand_position;
    left_hand_position = final_position - left_hand_position;

    right_hand_rotation = rotation_offset * right_hand_rotation;
    right_hand_rotation = (glm::normalize(view_quat_inverse_flat) * right_hand_rotation);

    left_hand_rotation = rotation_offset * left_hand_rotation;
    left_hand_rotation = (glm::normalize(view_quat_inverse_flat) * left_hand_rotation);

    //right_hand_rotation = glm::normalize(right_hand_rotation * right_hand_offset_q);
    auto right_hand_euler = glm::degrees(utility::math::euler_angles_from_steamvr(right_hand_rotation));

    //left_hand_rotation = glm::normalize(left_hand_rotation * left_hand_offset_q);
    auto left_hand_euler = glm::degrees(utility::math::euler_angles_from_steamvr(left_hand_rotation));

    const auto head_rotation =  glm::normalize(vqi_norm * (rotation_offset * glm::quat{vr->get_rotation(0)}));
    const auto head_euler = glm::degrees(utility::math::euler_angles_from_steamvr(head_rotation));

    update_motion_controller_components(
        final_position, head_euler,
        left_hand_position, left_hand_euler, 
        right_hand_position, right_hand_euler);

    sdk::TArray<sdk::UPrimitiveComponent*> overlapped_components{};
    sdk::TArray<sdk::UPrimitiveComponent*> overlapped_components_left{};

    // Update overlapped components and overlap actor transform
    if (m_overlap_detection_actor != nullptr && this->exists(m_overlap_detection_actor)) {
        m_overlap_detection_actor->set_actor_location(right_hand_position, false, false);
        m_overlap_detection_actor->set_actor_rotation(right_hand_euler, false);

        if (!g_framework->is_drawing_ui()) {
            overlapped_components = std::move(m_overlap_detection_actor->get_overlapping_components());
        }
    }

    // Update overlapped components and overlap actor transform (left)
    if (m_overlap_detection_actor_left != nullptr && this->exists(m_overlap_detection_actor_left)) {
        m_overlap_detection_actor_left->set_actor_location(left_hand_position, false, false);
        m_overlap_detection_actor_left->set_actor_rotation(left_hand_euler, false);

        if (!g_framework->is_drawing_ui()) {
            overlapped_components_left = std::move(m_overlap_detection_actor_left->get_overlapping_components());
        }
    }

    // Check intuitive attachment for overlapped components
    if (!g_framework->is_drawing_ui() && (!overlapped_components.empty() || !overlapped_components_left.empty())) {
        static bool prev_right_a_pressed = false;
        static bool prev_left_a_pressed = false;
        const auto is_a_down_raw_right = vr->is_action_active_any_joystick(vr->get_action_handle(VR::s_action_a_button_right));
        const auto was_a_pressed_right = !prev_right_a_pressed && is_a_down_raw_right;

        const auto is_a_down_raw_left = vr->is_action_active_any_joystick(vr->get_action_handle(VR::s_action_a_button_left));
        const auto was_a_pressed_left = !prev_left_a_pressed && is_a_down_raw_left;

        prev_right_a_pressed = is_a_down_raw_right;
        prev_left_a_pressed = is_a_down_raw_left;

        // Update existing attached components before moving onto overlapped ones.
        for (auto& it : comps) {
            auto& state = *it.second;

            if (state.hand == (uint8_t)MotionControllerStateBase::Hand::LEFT) {
                if (is_a_down_raw_left) {
                    state.adjusting = true;
                } else if (!is_a_down_raw_left) {
                    state.adjusting = false;
                }
            } else if (state.hand == (uint8_t)MotionControllerStateBase::Hand::RIGHT) {
                if (is_a_down_raw_right) {
                    state.adjusting = true;
                } else if (!is_a_down_raw_right) {
                    state.adjusting = false;
                }
            } else {
                state.adjusting = false;
            }
        }
        
        auto update_overlaps = [&](int32_t hand, const sdk::TArray<sdk::UPrimitiveComponent*>& components) {
            const auto was_pressed = hand == 0 ? was_a_pressed_left : was_a_pressed_right;
            const auto is_pressed = hand == 0 ? is_a_down_raw_left : is_a_down_raw_right;

            for (auto overlap : components) {
                static const auto capsule_component_t = sdk::find_uobject<sdk::UClass>(L"Class /Script/Engine.CapsuleComponent");
                if (overlap->get_class()->is_a(capsule_component_t)) {
                    continue;
                }

                {
                    std::shared_lock _{m_mutex};
                    if (m_spawned_spheres_to_components.contains(overlap)) {
                        overlap = (sdk::UPrimitiveComponent*)m_spawned_spheres_to_components[overlap];
                    }
                }

                const auto owner = overlap->get_owner();
                bool owner_is_adjustment_vis = false;

                if (owner == m_overlap_detection_actor || owner == m_overlap_detection_actor_left) {
                    continue;
                }

                // Make sure we don't try to attach to the adjustment visualizer
                {
                    std::shared_lock _{m_mutex};
                    auto it = std::find_if(m_motion_controller_attached_components.begin(), m_motion_controller_attached_components.end(),
                        [&](auto& it) {
                            if (it.second->adjustment_visualizer != nullptr && this->exists_unsafe(it.second->adjustment_visualizer)) {
                                if (it.second->adjustment_visualizer == owner) {
                                    owner_is_adjustment_vis = true;
                                    return true;
                                }
                            }

                            return false;
                        });
                    
                    if (owner_is_adjustment_vis) {
                        continue;
                    }
                }

                if (was_pressed) {
                    auto state = get_or_add_motion_controller_state(overlap);
                    state->adjusting = true;
                    state->hand = hand;
                } /*else if (!is_pressed) {
                    auto state = get_motion_controller_state(overlap);

                    if (state && (*state)->hand == hand) {
                        (*state)->adjusting = false;
                    }
                }*/
            }
        };

        update_overlaps(0, overlapped_components_left);
        update_overlaps(1, overlapped_components);
    }

    for (auto& it : comps) {
        if (!is_using_controllers && it.second->hand != 2) {
            continue;
        }

        auto comp = it.first;
        if (!this->exists(comp) || it.second == nullptr) {
            continue;
        }
        
        auto& state = *it.second;
        const auto orig_position = comp->get_world_location();
        const auto orig_rotation = comp->get_world_rotation();

        // Convert orig_rotation to quat
        const auto orig_rotation_mat = glm::yawPitchRoll(
            glm::radians(-orig_rotation.y),
            glm::radians(orig_rotation.x),
            glm::radians(-orig_rotation.z));
        const auto orig_rotation_quat = glm::quat{orig_rotation_mat};

        using Hand = MotionControllerStateBase::Hand;
        const auto& hand_rotation = state.hand != Hand::HMD ? (state.hand == Hand::RIGHT ? right_hand_rotation : left_hand_rotation) : head_rotation;
        const auto& hand_position = state.hand != Hand::HMD ? (state.hand == Hand::RIGHT ? right_hand_position : left_hand_position) : final_position;
        const auto& hand_euler = state.hand != Hand::HMD ? (state.hand == Hand::RIGHT ? right_hand_euler : left_hand_euler) : head_euler;

        const auto adjusted_rotation = hand_rotation * glm::inverse(state.rotation_offset);
        const auto adjusted_euler = glm::degrees(utility::math::euler_angles_from_steamvr(adjusted_rotation));
        const auto adjusted_location = hand_position + (quat_converter * (adjusted_rotation * state.location_offset));

        if (state.adjusting) {
            // Create a temporary actor that visualizes how we're adjusting the component
            if (state.adjustment_visualizer == nullptr) {
                auto ugs = sdk::UGameplayStatics::get();
                auto visualizer = ugs->spawn_actor(sdk::UGameEngine::get()->get_world(), sdk::AActor::static_class(), orig_position);

                if (visualizer != nullptr) {
                    auto add_comp = [&](sdk::UClass* c, std::function<void(sdk::UActorComponent*)> fn) -> sdk::UActorComponent* {
                        if (c == nullptr) {
                            SPDLOG_ERROR("[UObjectHook] Cannot add component of null class");
                            return nullptr;
                        }

                        auto new_comp = visualizer->add_component_by_class(c);

                        if (new_comp != nullptr) {
                            fn(new_comp);

                            if (new_comp->is_a(sdk::USceneComponent::static_class())) {
                                auto scene_comp = (sdk::USceneComponent*)new_comp;
                                scene_comp->set_hidden_in_game(false);
                            }

                            visualizer->finish_add_component(new_comp);
                        } else {
                            SPDLOG_ERROR("[UObjectHook] Failed to add component {} to adjustment visualizer", utility::narrow(c->get_full_name()));
                        }
                        
                        return new_comp;
                    };

                    add_comp(sdk::find_uobject<sdk::UClass>(L"Class /Script/Engine.SphereComponent"), [](sdk::UActorComponent* new_comp) {
                        struct SphereRadiusParams {
                            float radius{};
                            bool update_overlaps{false};
                        };

                        auto params = SphereRadiusParams{};
                        params.radius = 10.f;

                        const auto fn = new_comp->get_class()->find_function(L"SetSphereRadius");

                        if (fn != nullptr) {
                            new_comp->process_event(fn, &params);
                        }
                    });

                    /*add_comp(sdk::find_uobject<sdk::UClass>(L"Class /Script/Engine.ArrowComponent"), [](sdk::UActorComponent* new_comp) {
                        struct {
                            float color[4]{1.0f, 0.0f, 0.0f, 1.0f};
                        } params{};

                        const auto fn = new_comp->get_class()->find_function(L"SetArrowColor");

                        if (fn != nullptr) {
                            new_comp->process_event(fn, &params);
                        }
                    });*/

                    // Ghetto way of making a "mesh" out of the box components
                    for (auto j = 0; j < 3; ++j) {
                        for (auto i = 1; i < 25; ++i) {
                            add_comp(sdk::find_uobject<sdk::UClass>(L"Class /Script/Engine.BoxComponent"), [i, j](sdk::UActorComponent* new_comp) {
                                auto color = (uint8_t*)new_comp->get_property_data(L"ShapeColor");

                                if (color != nullptr) {
                                    color[0] = 255;
                                    color[1] = 255;
                                    color[2] = 0;
                                    color[3] = 0;
                                }

                                auto extent = (void*)new_comp->get_property_data(L"BoxExtent");

                                if (extent != nullptr) {
                                    static const bool is_ue5 = sdk::ScriptVector::static_struct()->get_struct_size() == sizeof(glm::vec<3, double>);

                                    const auto ratio = (float)i / 100.0f;

                                    const auto x = j == 0 ? ratio * 25.0f : 25.0f;
                                    const auto y = j == 1 ? ratio * 5.0f : 5.0f;
                                    const auto z = j == 2 ? ratio * 5.0f : 5.0f;

                                    glm::vec3 wanted_ext = glm::vec3{x, y, z};

                                    if (is_ue5) {
                                        *((glm::vec<3, double>*)extent) = wanted_ext;
                                    } else {
                                        *((glm::vec3*)extent) = wanted_ext;
                                    }
                                }
                            });
                        }
                    }

                    //ugs->finish_spawning_actor(visualizer, orig_position);

                    std::unique_lock _{m_mutex};
                    state.adjustment_visualizer = visualizer;
                } else {
                    SPDLOG_ERROR("[UObjectHook] Failed to spawn actor for adjustment visualizer");
                }
            } else {
                state.adjustment_visualizer->set_actor_location(hand_position, false, false);
                state.adjustment_visualizer->set_actor_rotation(hand_euler, false);
            }

            std::unique_lock _{m_mutex};
            const auto mat_inverse = 
                glm::yawPitchRoll(
                    glm::radians(-orig_rotation.y),
                    glm::radians(orig_rotation.x),
                    glm::radians(-orig_rotation.z));

            const auto mq = glm::quat{mat_inverse};
            const auto mqi = glm::inverse(mq);

            state.rotation_offset = mqi * hand_rotation;
            state.location_offset = mqi * utility::math::ue4_to_glm(hand_position - orig_position);
        } else {
            if (state.adjustment_visualizer != nullptr) {
                state.adjustment_visualizer->destroy_actor();

                std::unique_lock _{m_mutex};
                state.adjustment_visualizer = nullptr;
            }

            comp->set_world_location(adjusted_location, false, false);
            comp->set_world_rotation(adjusted_euler, false, false);
        }

        if (!state.permanent) {
            GameThreadWorker::get().enqueue([this, comp, orig_position, orig_rotation]() {
                if (!this->exists(comp)) {
                    return;
                }

                comp->set_world_location(orig_position, false, false);
                comp->set_world_rotation(orig_rotation, false, false);
            });
        }
    }
}

void UObjectHook::spawn_overlapper(uint32_t hand) {
    GameThreadWorker::get().enqueue([this, hand]() {
        auto ugs = sdk::UGameplayStatics::get();
        auto world = sdk::UGameEngine::get()->get_world();

        if (ugs == nullptr || world == nullptr) {
            return;
        }

        auto overlapper = ugs->spawn_actor(world, sdk::AActor::static_class(), glm::vec3{0, 0, 0});

        if (hand == 1) {
            if (m_overlap_detection_actor != nullptr && this->exists(m_overlap_detection_actor)) {
                m_overlap_detection_actor->destroy_actor();
            }

            m_overlap_detection_actor = overlapper;
        } else {
            if (m_overlap_detection_actor_left != nullptr && this->exists(m_overlap_detection_actor_left)) {
                m_overlap_detection_actor_left->destroy_actor();
            }

            m_overlap_detection_actor_left = overlapper;
        }

        if (overlapper != nullptr) {
            auto add_comp = [&](sdk::AActor* target, sdk::UClass* c, std::function<void(sdk::UActorComponent*)> fn) -> sdk::UActorComponent* {
                if (c == nullptr) {
                    SPDLOG_ERROR("[UObjectHook] Cannot add component of null class");
                    return nullptr;
                }

                auto new_comp = target->add_component_by_class(c);

                if (new_comp != nullptr) {
                    fn(new_comp);

                    if (new_comp->is_a(sdk::USceneComponent::static_class())) {
                        auto scene_comp = (sdk::USceneComponent*)new_comp;
                        scene_comp->set_hidden_in_game(false);
                    }

                    target->finish_add_component(new_comp);
                } else {
                    SPDLOG_ERROR("[UObjectHook] Failed to add component {} to target", utility::narrow(c->get_full_name()));
                }
                
                return new_comp;
            };

            const auto sphere_t = sdk::find_uobject<sdk::UClass>(L"Class /Script/Engine.SphereComponent");

            add_comp(overlapper, sphere_t, [](sdk::UActorComponent* new_comp) {
                struct SphereRadiusParams {
                    float radius{};
                    bool update_overlaps{true};
                } params{};

                params.radius = 10.0f;

                const auto fn = new_comp->get_class()->find_function(L"SetSphereRadius");

                if (fn != nullptr) {
                    new_comp->process_event(fn, &params);
                }
            });

            const auto skeletal_mesh_t = sdk::find_uobject<sdk::UClass>(L"Class /Script/Engine.SkeletalMeshComponent");
            const auto meshes = get_objects_by_class(skeletal_mesh_t);
            
            for (auto obj : meshes) {
                if (!this->exists(obj)) {
                    continue;
                }

                // Dont add the same one twice
                if (m_components_with_spheres.contains((sdk::USceneComponent*)obj)) {
                    continue;
                }

                const auto default_obj = obj->get_class()->get_class_default_object();

                if (obj == default_obj) {
                    continue;
                }

                SPDLOG_INFO("[UObjectHook] Spawning sphere for skeletal mesh {}", utility::narrow(obj->get_full_name()));

                auto mesh = (sdk::USceneComponent*)obj;
                auto owner = mesh->get_owner();

                if (owner == nullptr) {
                    continue;
                }

                const auto owner_default = owner->get_class()->get_class_default_object();

                if (owner == owner_default) {
                    continue;
                }

                SPDLOG_INFO("[UObjectHook] Owner of skeletal mesh is {}", utility::narrow(owner->get_full_name()));

                auto new_sphere = (sdk::USceneComponent*)add_comp(owner, sphere_t, [](sdk::UActorComponent* new_comp) {
                    struct SphereRadiusParams {
                        float radius{};
                        bool update_overlaps{true};
                    } params{};

                    params.radius = 10.0f;

                    const auto fn = new_comp->get_class()->find_function(L"SetSphereRadius");

                    if (fn != nullptr) {
                        new_comp->process_event(fn, &params);
                    }
                });

                if (new_sphere != nullptr) {
                    new_sphere->attach_to(mesh, L"None", 0, true);
                    new_sphere->set_local_transform(glm::vec3{}, glm::vec4{0, 0, 0, 1}, glm::vec3{1, 1, 1});

                    std::unique_lock _{m_mutex};
                    m_spawned_spheres.insert(new_sphere);
                    m_spawned_spheres_to_components[new_sphere] = mesh;
                    m_components_with_spheres.insert(mesh);
                }
            }
        } else {
            SPDLOG_ERROR("[UObjectHook] Failed to spawn actor for overlapper");
        }
    });
}

void UObjectHook::destroy_overlapper() {
    GameThreadWorker::get().enqueue([this]() {
        // Destroy all spawned spheres
        auto spheres = get_spawned_spheres();

        for (auto sphere : spheres) {
            if (!this->exists(sphere)) {
                continue;
            }

            sphere->destroy_component();
        }

        {
            std::unique_lock _{m_mutex};
            m_spawned_spheres.clear();
            m_spawned_spheres_to_components.clear();
            m_components_with_spheres.clear();
        }

        if (m_overlap_detection_actor != nullptr && this->exists(m_overlap_detection_actor)) {
            m_overlap_detection_actor->destroy_actor();
            m_overlap_detection_actor = nullptr;
        }

        if (m_overlap_detection_actor_left != nullptr && this->exists(m_overlap_detection_actor_left)) {
            m_overlap_detection_actor_left->destroy_actor();
            m_overlap_detection_actor_left = nullptr;
        }
    });
}

std::filesystem::path UObjectHook::get_persistent_dir() {
    const auto base_dir = Framework::get_persistent_dir();
    const auto uobjecthook_dir = base_dir / "uobjecthook";

    try {
        if (!std::filesystem::exists(uobjecthook_dir)) {
            std::filesystem::create_directories(uobjecthook_dir);
        }
    } catch (const std::exception& e) {
        SPDLOG_ERROR("[UObjectHook] Failed to create persistent directory: {}", e.what());
    } catch (...) {
        SPDLOG_ERROR("[UObjectHook] Failed to create persistent directory");
    }

    return uobjecthook_dir;
}

nlohmann::json UObjectHook::serialize_mc_state(const std::vector<std::string>& path, const std::shared_ptr<MotionControllerState>& state) {
    nlohmann::json result{};

    result["path"] = path;
    result["state"] = state->to_json();
    result["type"] = "motion_controller";

    return result;
}

nlohmann::json UObjectHook::serialize_camera(const std::vector<std::string>& path) {
    nlohmann::json result{};

    result["path"] = path;
    result["offset"] = utility::math::to_json(m_camera_attach.offset);
    result["type"] = "camera";

    // todo: adjustments/offsets, etc...? all it needs is the camera object which is fine

    return result;
}

void UObjectHook::save_camera_state(const std::vector<std::string>& path) {
    auto json = serialize_camera(path);

    const auto wanted_dir = UObjectHook::get_persistent_dir() / "camera_state.json";

    // Create dir if necessary
    try {
        std::filesystem::create_directories(wanted_dir.parent_path());

        if (std::filesystem::exists(wanted_dir.parent_path())) {
            std::ofstream file{wanted_dir};
            file << json.dump(4);
            file.close();

            m_persistent_camera_state = deserialize_camera_state();
        }
    } catch (const std::exception& e) {
        SPDLOG_ERROR("[UObjectHook] Failed to save camera state: {}", e.what());
    } catch (...) {
        SPDLOG_ERROR("[UObjectHook] Failed to save camera state");
    }
}

std::optional<UObjectHook::StatePath> UObjectHook::deserialize_path(const nlohmann::json& data) {
    if (!data.contains("path")) {
        SPDLOG_ERROR("[UObjectHook] Malfomed JSON file (missing path)");
        return std::nullopt;
    }

    // make sure path is an array
    if (!data["path"].is_array()) {
        SPDLOG_ERROR("[UObjectHook] Malfomed JSON file (path is not an array)");
        return std::nullopt;
    }

    const auto path = data["path"].get<std::vector<std::string>>();

    if (path.empty()) {
        SPDLOG_ERROR("[UObjectHook] Malfomed JSON file (path is empty)");
        return std::nullopt;
    }

    return StatePath{path};
}

std::shared_ptr<UObjectHook::PersistentState> UObjectHook::deserialize_mc_state(nlohmann::json& data) {
    SPDLOG_INFO("[UObjectHook] inside deserialize_mc_state");

    if (!data.contains("path") || !data.contains("state")) {
        SPDLOG_ERROR("[UObjectHook] Malfomed JSON file (missing path or state)");
        return nullptr;
    }

    // make sure path is an array
    if (!data["path"].is_array()) {
        SPDLOG_ERROR("[UObjectHook] Malfomed JSON file (path is not an array)");
        return nullptr;
    }

    // make sure state is an object
    if (!data["state"].is_object()) {
        SPDLOG_ERROR("[UObjectHook] Malfomed JSON file (state is not an object)");
        return nullptr;
    }

    if (data.contains("type") && data["type"].is_string()) {
        const auto type = data["type"].get<std::string>();

        if (type != "motion_controller") {
            SPDLOG_ERROR("[UObjectHook] Malfomed JSON file (type is not motion_controller)");
            return nullptr;
        }
    }

    SPDLOG_INFO("[UObjectHook] Deserializing state path...");
    auto path = data["path"].get<std::vector<std::string>>();

    auto persistent_state = std::make_shared<PersistentState>();
    persistent_state->path = path;

    SPDLOG_INFO("[UObjectHook] Deserializing state...");
    persistent_state->state.from_json(data["state"]);

    return persistent_state;
}

std::shared_ptr<UObjectHook::PersistentState> UObjectHook::deserialize_mc_state(std::filesystem::path json_path) {
    if (!std::filesystem::exists(json_path)) {
        return nullptr;
    }

    try {
        auto f = std::ifstream{json_path};

        if (f.is_open()) {
            // Log the file data to make sure we're getting it correctly...
            const auto file_contents = std::string{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};

            SPDLOG_INFO("[UObjectHook] JSON file contents:");
            SPDLOG_INFO("{}", file_contents);

            nlohmann::json data = nlohmann::json::parse(file_contents);

            return deserialize_mc_state(data);
        }

        SPDLOG_ERROR("[UObjectHook] Failed to open JSON file {}", json_path.string());
        return nullptr;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("[UObjectHook] Failed to parse JSON file {}: {}", json_path.string(), e.what());
    } catch (...) {
        SPDLOG_ERROR("[UObjectHook] Failed to parse JSON file {}", json_path.string());
    }

    return nullptr;
}

std::vector<std::shared_ptr<UObjectHook::PersistentState>> UObjectHook::deserialize_all_mc_states() try {
    const auto uobjecthook_dir = get_persistent_dir();

    if (!std::filesystem::exists(uobjecthook_dir)) {
        return {};
    }
    
    // Gather all .json files in this directory
    std::vector<std::filesystem::path> json_files{};
    for (const auto& p : std::filesystem::directory_iterator(uobjecthook_dir)) {
        if (p.path().extension() == ".json") {
            json_files.push_back(p.path());
        }
    }

    std::vector<std::shared_ptr<PersistentState>> result{};
    for (const auto& json_file : json_files) {
        auto state = deserialize_mc_state(json_file);

        if (state != nullptr) {
            state->path_to_json = json_file;
            result.push_back(state);
        }
    }

    return result;
} catch (const std::exception& e) {
    SPDLOG_ERROR("[UObjectHook] Failed to deserialize all motion controller states: {}", e.what());
    return {};
} catch (...) {
    SPDLOG_ERROR("[UObjectHook] Failed to deserialize all motion controller states");
    return {};
}

std::shared_ptr<UObjectHook::PersistentCameraState> UObjectHook::deserialize_camera(const nlohmann::json& data) {
    const auto path = deserialize_path(data);

    if (!path.has_value()) {
        SPDLOG_ERROR("[UObjectHook] Failed to deserialize camera path");
        return nullptr;
    }

    if (!data.contains("type") || !data["type"].is_string()) {
        SPDLOG_ERROR("[UObjectHook] Malfomed JSON file (missing type)");
        return nullptr;
    }

    if (data["type"].get<std::string>() != "camera") {
        SPDLOG_ERROR("[UObjectHook] Malfomed JSON file (type is not camera)");
        return nullptr;
    }

    auto persistent_state = std::make_shared<PersistentCameraState>();
    persistent_state->path = path.value();

    if (data.contains("offset") && data["offset"].is_object()) {
        persistent_state->offset = utility::math::from_json_vec3(data["offset"]);
    }

    return persistent_state;
}

std::shared_ptr<UObjectHook::PersistentCameraState> UObjectHook::deserialize_camera_state() {
    // Look for camera_state.json
    const auto uobjecthook_dir = get_persistent_dir();
    const auto camera_state_path = uobjecthook_dir / "camera_state.json";

    if (!std::filesystem::exists(camera_state_path)) {
        SPDLOG_ERROR("[UObjectHook] Failed to find camera_state.json");
        return nullptr;
    }

    try {
        auto f = std::ifstream{camera_state_path};

        if (f.is_open()) {
            // Log the file data to make sure we're getting it correctly...
            const auto file_contents = std::string{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};

            SPDLOG_INFO("[UObjectHook] JSON file contents:");
            SPDLOG_INFO("{}", file_contents);

            nlohmann::json data = nlohmann::json::parse(file_contents);

            auto result = deserialize_camera(data);

            if (result != nullptr) {
                result->path_to_json = camera_state_path;
            }

            return result;
        }

        SPDLOG_ERROR("[UObjectHook] Failed to open JSON file {}", camera_state_path.string());
        return nullptr;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("[UObjectHook] Failed to parse JSON file {}: {}", camera_state_path.string(), e.what());
    } catch (...) {
        SPDLOG_ERROR("[UObjectHook] Failed to parse JSON file {}", camera_state_path.string());
    }

    return nullptr;
}

void UObjectHook::update_persistent_states() {
    if (m_uobject_hook_disabled && m_fixed_visibilities) {
        return;
    }

    // For when we disable UObjectHook
    utility::ScopeGuard ___{[this]() {
        m_fixed_visibilities = true;
    }};

    // Camera state
    if (m_persistent_camera_state != nullptr) {
        auto obj = m_persistent_camera_state->path.resolve();

        if (obj != nullptr) {
            m_camera_attach.object = obj;
            m_camera_attach.offset = m_persistent_camera_state->offset;
        }
    }

    // Motion controller states
    if (!m_persistent_states.empty()) {
        for (const auto& state : m_persistent_states) {
            if (state == nullptr) {
                continue;
            }

            auto obj = state->path.resolve();

            if (obj == nullptr) {
                continue;
            }

            static const auto scene_component_t = sdk::USceneComponent::static_class();

            // TODO? will need some reworking to support properties from arbitrary objects
            if (!obj.definition->is_a(scene_component_t)) {
                continue;
            }

            // Destroy the existing mc state if it exists
            // This can cause issues if the previous object still exists
            // so we need to detach the old one
            if (state->last_object != nullptr && state->last_object != obj) {
                remove_motion_controller_state(state->last_object);
            }

            auto mc_state = get_or_add_motion_controller_state(obj.as<sdk::USceneComponent*>());

            if (mc_state == nullptr) {
                continue;
            }

            if (mc_state->adjusting) {
                state->state = *mc_state;
            } else {
                *mc_state = state->state;
            }

            state->last_object = obj.as<sdk::USceneComponent*>();
        }
    }

    // Persistent properties
    if (!m_persistent_properties.empty()) {
        const auto scene_comp_t = sdk::USceneComponent::static_class();
        const auto primitive_comp_t = sdk::UPrimitiveComponent::static_class();
        for (const auto& prop_base : m_persistent_properties) {
            if (prop_base == nullptr) {
                continue;
            }

            auto obj = prop_base->path.resolve();

            if (obj == nullptr) {
                continue;
            }

            if (prop_base->hide) {
                if (obj.definition->is_a(primitive_comp_t)) {
                    auto obj_primcomp = obj.as<sdk::UPrimitiveComponent*>();

                    if (m_uobject_hook_disabled) {
                        obj_primcomp->set_overall_visibility(true, true);
                    } else {
                        obj_primcomp->set_overall_visibility(false, prop_base->hide_legacy);
                    }
                } else if (obj.definition->is_a(scene_comp_t)) {
                    auto obj_scenecomp = obj.as<sdk::USceneComponent*>();

                    if (m_uobject_hook_disabled) {
                        obj_scenecomp->set_visibility(true, false);
                    } else {
                        obj_scenecomp->set_visibility(false, false);
                    }
                }
            }

            for (const auto& prop_state : prop_base->properties) {
                const auto prop_desc = obj.definition->find_property(prop_state->name);
            
                if (prop_desc == nullptr) {
                    continue;
                }

                const auto prop_t = prop_desc->get_class();

                if (prop_t == nullptr) {
                    continue;
                }

                const auto prop_t_name = prop_t->get_name().to_string();

                switch (utility::hash(utility::narrow(prop_t_name))) {
                case "FloatProperty"_fnv:
                    {
                        auto& value = *(float*)(obj.as<uintptr_t>() + ((sdk::FProperty*)prop_desc)->get_offset());
                        value = prop_state->data.f;
                    }
                    break;
                case "DoubleProperty"_fnv:
                    {
                        auto& value = *(double*)(obj.as<uintptr_t>() + ((sdk::FProperty*)prop_desc)->get_offset());
                        value = prop_state->data.d;
                    }
                    break;
                case "UInt32Property"_fnv:
                case "IntProperty"_fnv:
                    {
                        auto& value = *(int32_t*)(obj.as<uintptr_t>() + ((sdk::FProperty*)prop_desc)->get_offset());
                        value = prop_state->data.i;
                    }
                    break;
                case "BoolProperty"_fnv:
                    {
                        auto boolprop = (sdk::FBoolProperty*)prop_desc;
                        boolprop->set_value_in_object(obj, prop_state->data.b);
                    }
                    break;
                case "ByteProperty"_fnv:
                    {
                        auto& value = *(uint8_t*)(obj.as<uintptr_t>() + ((sdk::FProperty*)prop_desc)->get_offset());
                        value = prop_state->data.u8;
                    }
                    break;
                case "UInt16Property"_fnv:
                    {
                        auto& value = *(uint16_t*)(obj.as<uintptr_t>() + ((sdk::FProperty*)prop_desc)->get_offset());
                        value = prop_state->data.u16;
                    }
                    break;
                default:
                    // OH NO!!!!! anyways
                    break;
                };
            }
        }
    }
}

void UObjectHook::update_motion_controller_components(
    const glm::vec3& hmd_location, const glm::vec3& hmd_euler,
    const glm::vec3& left_hand_location, const glm::vec3& left_hand_euler,
    const glm::vec3& right_hand_location, const glm::vec3& right_hand_euler) 
{
    const auto mc_c = sdk::UMotionControllerComponent::static_class();

    if (mc_c == nullptr) {
        SPDLOG_ERROR_ONCE("[UObjectHook] Failed to find MotionControllerComponent class, cannot update motion controller components");
        return;
    }

    const auto motion_controllers = this->get_objects_by_class(mc_c);

    if (motion_controllers.empty()) {
        return;
    }

    for (auto obj : motion_controllers) {
        auto mc = (sdk::UMotionControllerComponent*)obj;
        if (!this->exists(mc)) {
            continue;
        }

        if (mc == mc_c->get_class_default_object()) {
            continue;
        }

        if (mc->get_outer() == nullptr || !this->exists(mc->get_outer())) {
            continue;
        }

        if (!sdk::UObjectReference{mc}.valid()) {
            continue;
        }

        if (mc->get_player_index() > 0) {
            continue;
        }

        if (mc->has_motion_source()) {
            const auto& motion_source = mc->get_motion_source();
            const auto motion_source_str = motion_source.to_string();

            if (motion_source_str == L"Left") {
                mc->set_world_location(left_hand_location, false, false);
                mc->set_world_rotation(left_hand_euler, false);
            } else if (motion_source_str == L"Right") {
                mc->set_world_location(right_hand_location, false, false);
                mc->set_world_rotation(right_hand_euler, false);
            } else if (motion_source_str == L"Head" || motion_source_str == L"HMD") {
                mc->set_world_location(hmd_location, false, false);
                mc->set_world_rotation(hmd_euler, false);
            }
        } else {
            if (mc->get_hand() == sdk::EControllerHand::Left) {
                mc->set_world_location(left_hand_location, false, false);
                mc->set_world_rotation(left_hand_euler, false);
            } else if (mc->get_hand() == sdk::EControllerHand::Right) {
                mc->set_world_location(right_hand_location, false, false);
                mc->set_world_rotation(right_hand_euler, false);
            }
        }
    }
}

sdk::UObject* UObjectHook::StatePath::resolve_base_object() const {
    if (!this->has_valid_base()) {
        return nullptr;
    }

    auto engine = sdk::UGameEngine::get();
    if (engine == nullptr) {
        return nullptr;
    }

    // TODO: Convert these into an enum or something when we initially parse the JSON file.
    switch (utility::hash(m_path.front())) {
    case "Acknowledged Pawn"_fnv:
    {
        auto world = engine->get_world();
        if (world == nullptr) {
            return nullptr;
        }

        auto player_controller = sdk::UGameplayStatics::get()->get_player_controller(world, 0);

        if (player_controller == nullptr) {
            return nullptr;
        }
        
        return player_controller->get_acknowledged_pawn();
        break;
    }

    case "Player Controller"_fnv:
    {
        auto world = engine->get_world();
        if (world == nullptr) {
            return nullptr;
        }

        return sdk::UGameplayStatics::get()->get_player_controller(world, 0);
        break;
    }

    case "Camera Manager"_fnv:
    {      
        auto world = engine->get_world();
        if (world == nullptr) {
            return nullptr;
        }

        auto player_controller = sdk::UGameplayStatics::get()->get_player_controller(world, 0);

        if (player_controller == nullptr) {
            return nullptr;
        }
        
        return player_controller->get_player_camera_manager();
        break;
    }

    case "World"_fnv:
    {
        return engine->get_world();
        break;
    }

    default:
        break;
    };

    return nullptr;
}

UObjectHook::ResolvedObject UObjectHook::StatePath::resolve() const {
    const auto base = resolve_base_object();

    if (base == nullptr) {
        return nullptr;
    }

    void* previous_data = base;
    sdk::UStruct* previous_data_desc = base->get_class();

    if (previous_data_desc == nullptr) {
        return nullptr;
    }

    for (auto it = m_path.begin() + 1; it != m_path.end(); ++it) {
        if (previous_data == nullptr || previous_data_desc == nullptr) {
            return nullptr;
        }

        switch (utility::hash(*it)) {
        case "Outer"_fnv:
        {
            static const auto object_t = sdk::UObject::static_class();

            if (!previous_data_desc->is_a(object_t)) {
                return nullptr;
            }

            previous_data = ((sdk::UObject*)previous_data)->get_outer();
            if (previous_data != nullptr) {
                previous_data_desc = ((sdk::UObject*)previous_data)->get_class();
            } else {
                previous_data_desc = nullptr;
            }

            break;
        }
        case "Components"_fnv:
        {
            // Make sure the base is an AActor
            static const auto actor_t = sdk::AActor::static_class();

            if (!previous_data_desc->is_a(actor_t)) {
                return nullptr;
            }

            const auto components = ((sdk::AActor*)previous_data)->get_all_components();

            if (components.empty()) {
                return nullptr;
            }

            auto next_it = it + 1;

            if (next_it == m_path.end()) {
                return nullptr;
            }

            for (auto comp : components) {
                const auto& comp_fname = comp->get_fname();
                const auto comp_name = comp_fname.to_string_remove_numbers();
                const auto comp_ends_with_number = comp_fname.get_number() != 0;

                const auto comp_expanded_name = utility::narrow(comp->get_class()->get_fname().to_string() + L" " + comp_name);
                const auto is_match = comp_ends_with_number ? next_it->starts_with(comp_expanded_name) 
                                                            : *next_it == comp_expanded_name;

                if (is_match) {
                    previous_data = comp;
                    previous_data_desc = comp->get_class();
                    ++it;
                    break;
                }
            }

            break;
        }
        case "Properties"_fnv:
        {
            auto next_it = it + 1;

            if (next_it == m_path.end()) {
                return nullptr;
            }

            const auto prop_name = *next_it;
            const auto prop_desc = previous_data_desc->find_property(utility::widen(prop_name));

            if (prop_desc == nullptr) {
                return nullptr;
            }

            const auto prop_t = prop_desc->get_class();

            if  (prop_t == nullptr) {
                return nullptr;
            }

            const auto prop_t_name = prop_t->get_name().to_string();
            switch (utility::hash(utility::narrow(prop_t_name))) {
            case "InterfaceProperty"_fnv:
            case "ObjectProperty"_fnv:
            {
                const auto obj_ptr = prop_desc->get_data<sdk::UObject*>(previous_data);
                const auto obj = obj_ptr != nullptr ? *obj_ptr : nullptr;

                if (obj == nullptr) {
                    return nullptr;
                }

                previous_data = obj;
                previous_data_desc = obj->get_class();
                ++it;
                break;
            }
            case "StructProperty"_fnv:
            {
                const auto struct_data = prop_desc->get_data<void*>(previous_data);

                previous_data = struct_data;
                previous_data_desc = ((sdk::FStructProperty*)prop_desc)->get_struct();

                if (previous_data_desc == nullptr || previous_data == nullptr) {
                    return nullptr;
                }

                // ++it because we are examining the properties
                ++it;
                break;
            }
            case "ArrayProperty"_fnv:
            {
                const auto inner = ((sdk::FArrayProperty*)prop_desc)->get_inner();

                if (inner == nullptr) {
                    return nullptr;
                }
                
                const auto inner_c = inner->get_class();

                if (inner_c == nullptr) {
                    return nullptr;
                }

                const auto inner_c_type = utility::narrow(inner_c->get_name().to_string());

                // only support ObjectProperty for now
                if (inner_c_type != "ObjectProperty" && inner_c_type != "InterfaceProperty") {
                    return nullptr;
                }

                const auto array_ptr = prop_desc->get_data<sdk::TArray<sdk::UObject*>>(previous_data);

                if (array_ptr == nullptr) {
                    return nullptr;
                }

                const auto& arr = *array_ptr;

                if (arr.empty() || arr.data == nullptr) {
                    return nullptr;
                }

                auto prop_it = next_it + 1;

                if (prop_it == m_path.end()) {
                    return nullptr;
                }

                bool found = false;

                // Now look for the object in the array
                for (auto obj : arr) {
                    if (obj == nullptr) {
                        continue;
                    }

                    const auto& obj_fname = obj->get_fname();
                    const auto obj_name = obj_fname.to_string_remove_numbers();
                    const auto obj_ends_with_number = obj_fname.get_number() != 0;

                    const auto obj_expanded_name = utility::narrow(obj->get_class()->get_fname().to_string() + L" " + obj_name);
                    const auto is_match = obj_ends_with_number ? prop_it->starts_with(obj_expanded_name) 
                                                               : *prop_it == obj_expanded_name;

                    if (is_match) {
                        found = true;
                        previous_data = obj;
                        previous_data_desc = obj->get_class();
                        ++it;
                        ++it;
                        break;
                    }
                }

                if (!found) {
                    return nullptr;
                }
                break;
            }

            default:
                SPDLOG_ERROR("[UObjectHook] Unsupported persistent property type {}", utility::narrow(prop_t_name));
                break;
            };

            break;
        }

        default:
            return nullptr;
            break;
        };
    }

    if (previous_data == nullptr || previous_data_desc == nullptr) {
        return nullptr;
    }

    return ResolvedObject{(sdk::UObject*)previous_data, previous_data_desc};
}

void UObjectHook::on_frame() {
    if (m_keybind_toggle_uobject_hook->is_key_down_once()) {
        set_disabled(!is_disabled());
    }
}

void UObjectHook::on_draw_ui() {
    activate();

    if (!m_fully_hooked) {
        ImGui::Text("Waiting for UObjectBase to be hooked...");
        return;
    }

    std::shared_lock _{m_mutex};
    std::scoped_lock __{m_function_mutex};

    if (m_uobject_hook_disabled) {
        ImGui::TextColored(ImVec4{1.0f, 0.0f, 0.0f, 1.0f}, "UObjectHook is disabled");
        if (ImGui::Button("Re-enable")) {
            m_uobject_hook_disabled = false;
        }
        return;
    }

    if (ImGui::Button("Reload Persistent States")) {
        reload_persistent_states();
    }

    ImGui::SameLine();

    if (ImGui::Button("Destroy Persistent States")) {
        reset_persistent_states();

        const auto uobjecthook_dir = get_persistent_dir();

        if (std::filesystem::exists(uobjecthook_dir)) {
            for (const auto& p : std::filesystem::directory_iterator(uobjecthook_dir)) {
                if (p.path().extension() == ".json") {
                    std::filesystem::remove(p.path());
                }
            }
        }
    }
}

void UObjectHook::on_draw_sidebar_entry(std::string_view in_entry) {
    on_draw_ui();
    ImGui::Separator();

    if (in_entry == "Main") {
        draw_main();
    } else if (in_entry == "Config") {
        draw_config();
    } else if (in_entry == "Developer") {
        draw_developer();
    }
}

void UObjectHook::draw_config() {
    m_enabled_at_startup->draw("Enabled at Startup");
    m_attach_lerp_enabled->draw("Enable Attach Lerp");
    m_attach_lerp_speed->draw("Attach Lerp Speed");

    ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
    if (ImGui::TreeNode("UObjectHook Keybinds")) {
        m_keybind_toggle_uobject_hook->draw("Disable UObjectHook Key");

        ImGui::TreePop();
    }
}

void UObjectHook::draw_developer() {
    if (ImGui::Button("Dump SDK")) {
        SDKDumper::dump();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
    if (ImGui::TreeNode("Debug Stats")) {
        // uint64_t
        ImGui::Text("Constructor calls: %llu", m_debug.constructor_calls);
        ImGui::Text("Destructor calls: %llu", m_debug.destructor_calls);

        ImGui::TreePop();
    }

    ImGui::Separator();

    if (!m_attempted_hook_process_event) {
        if (ImGui::Button("Create ProcessEvent hook")) {
            GameThreadWorker::get().enqueue([this]() {
                hook_process_event();
            });
        }
    } else if (m_hooked_process_event) {
        ImGui::Checkbox("ProcessEvent Listener", &m_process_event_listening);

        if (m_process_event_listening) {
            if (ImGui::Button("Clear Ignored Functions")) {
                m_ignored_recent_functions.clear();
            }

            ImGui::SameLine();

            std::scoped_lock __{m_function_mutex};

            if (ImGui::Button("Clear Called Functions")) {
                m_called_functions.clear();
                m_most_recent_functions.clear();
            }

            if (ImGui::Button("Ignore All Called Functions")) {
                m_ignored_recent_functions.clear();

                for (auto& [ufunc, data] : m_called_functions) {
                    m_ignored_recent_functions.insert(ufunc);
                }
            }

            ImGui::Text("Called functions: %llu", m_called_functions.size());

            std::vector<sdk::UFunction*> functions_to_cleanup{};

            if (ImGui::TreeNode("Recent Functions")) {
                for (auto ufunc : m_most_recent_functions) {
                    if (ufunc == nullptr) {
                        continue;
                    }
                    
                    if (m_ignored_recent_functions.contains(ufunc)) {
                        continue;
                    }

                    if (!this->exists(ufunc)) {
                        functions_to_cleanup.push_back(ufunc);
                        continue;
                    }

                    ImGui::PushID(ufunc);

                    utility::ScopeGuard ___{[]() {
                        ImGui::PopID();
                    }};

                    if (ImGui::Button("Ignore")) {
                        m_ignored_recent_functions.insert(ufunc);
                    }

                    ImGui::SameLine();

                    ImGui::Text("%s", utility::narrow(ufunc->get_full_name()).c_str());
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("All Called Functions")) {
                ImGui::SliderInt("Max Calls", &m_process_event_search.max_calls, 0, 10000);
                ImGui::InputText("Search", m_process_event_search.buffer.data(), m_process_event_search.buffer.size());

                std::string_view search{m_process_event_search.buffer.data()};
                std::vector<sdk::UFunction*> functions_sorted_by_call_count{};
                
                for (auto& [ufunc, data] : m_called_functions) {
                    if (ufunc == nullptr) {
                        continue;
                    }

                    if (!this->exists(ufunc)) {
                        functions_to_cleanup.push_back(ufunc);
                        continue;
                    }

                    if (m_process_event_search.max_calls > 0 && data.call_count > m_process_event_search.max_calls) {
                        continue;
                    }

                    // maybe a TODO here for optimization
                    if (!search.empty() && !utility::narrow(ufunc->get_full_name()).contains(search)) {
                        continue;
                    }

                    functions_sorted_by_call_count.push_back(ufunc);
                }

                std::sort(functions_sorted_by_call_count.begin(), functions_sorted_by_call_count.end(), [this](sdk::UFunction* a, sdk::UFunction* b) {
                    return m_called_functions[a].call_count > m_called_functions[b].call_count;
                });

                for (auto& ufunc : functions_sorted_by_call_count) {
                    if (m_ignored_recent_functions.contains(ufunc)) {
                        continue;
                    }

                    if (!this->exists(ufunc)) {
                        functions_to_cleanup.push_back(ufunc);
                        continue;
                    }

                    ImGui::PushID(ufunc);

                    utility::ScopeGuard ___{[]() {
                        ImGui::PopID();
                    }};

                    if (ImGui::Button("Ignore")) {
                        m_ignored_recent_functions.insert(ufunc);
                    }

                    ImGui::SameLine();
                    
                    const auto made = ImGui::TreeNode(utility::narrow(ufunc->get_full_name()).c_str());

                    ImGui::SameLine();
                    ImGui::Text(" (%llu)", m_called_functions[ufunc].call_count);

                    if (made) {
                        auto& data = m_called_functions[ufunc];
                        data.wants_heavy_data = true;

                        if (data.heavy_data != nullptr) {
                            // Param inspector.
                            ui_handle_struct(data.heavy_data->params.data(), ufunc);
                        }

                        ImGui::TreePop();
                    }
                }

                ImGui::TreePop();
            }

            if (!functions_to_cleanup.empty()) {
                spdlog::info("[UObjectHook] Cleaning up {} functions", functions_to_cleanup.size());

                for (auto& ufunc : functions_to_cleanup) {
                    std::erase_if(m_most_recent_functions, [ufunc](auto& it) {
                        return it == ufunc;
                    });

                    m_called_functions.erase(ufunc);
                }
            }
        }
    } else {
        ImGui::Text("Failed to hook ProcessEvent!");
    }

    ImGui::Separator();

    static std::array<char, 512> address_buffer{};
    ImGui::InputText("Address Lookup", address_buffer.data(), address_buffer.size());

    // Try-catch block around this because it's possible the user could enter invalid input
    // also hex->int conversion can throw
    try {
        auto obj = (sdk::UObject*)std::stoull(address_buffer.data(), nullptr, 16);

        if (obj != nullptr && this->exists(obj)) {
            ImGui::PushID(obj);
            if (ImGui::TreeNode(utility::narrow(obj->get_full_name()).c_str())) {
                ui_handle_object(obj);
                ImGui::TreePop();
            }

            ImGui::PopID();
        }
    } catch (...) {
        // ignore
    }
}

void UObjectHook::draw_main() {
    if (!m_motion_controller_attached_components.empty()) {
        const auto made = ImGui::TreeNode("Attached Components");

        if (made) {
            if (ImGui::Button("Detach all")) {
                m_motion_controller_attached_components.clear();

                for (auto persistent_state : m_persistent_states) {
                    if (persistent_state != nullptr) {
                        persistent_state->erase_json_file();
                    }
                }

                m_persistent_states.clear();
            }

            // make a copy because the user could press the detach button while iterating
            auto attached = m_motion_controller_attached_components;

            for (auto& it : attached) {
                if (!this->exists_unsafe(it.first) || it.second == nullptr) {
                    continue;
                }

                auto comp = it.first;
                std::wstring comp_name = comp->get_class()->get_fname().to_string() + L" " + comp->get_fname().to_string();

                ImGui::PushID(comp);
                if (ImGui::TreeNode(utility::narrow(comp_name).data())) {
                    ui_handle_object(comp);

                    ImGui::TreePop();
                }
                ImGui::PopID();
            }

            ImGui::TreePop();
        }
    }

    const auto made2 = m_camera_attach.object != nullptr && ImGui::TreeNode("Attached Camera Object");

    if (made2) {
        if (ImGui::Button("Detach Camera")) {
            m_camera_attach.object = nullptr;
            m_camera_attach.offset = glm::vec3{0.0f, 0.0f, 0.0f};

            if (m_persistent_camera_state != nullptr) {
                m_persistent_camera_state->erase_json_file();
            }

            m_persistent_camera_state.reset();
        }

        ui_handle_object(m_camera_attach.object);

        ImGui::TreePop();
    }

    if (m_overlap_detection_actor == nullptr) {
        if (ImGui::Button("Spawn Overlapper")) {
            spawn_overlapper(0);
            spawn_overlapper(1);
        }
    } else if (!this->exists_unsafe(m_overlap_detection_actor)) {
        m_overlap_detection_actor = nullptr;
    } else {
        ImGui::SetNextItemOpen(true, ImGuiCond_Once);
        const auto made = ImGui::TreeNode("Overlapped Objects");

        if (made) {
            if (ImGui::Button("Destroy Overlapper")) {
                destroy_overlapper();
            }

            ImGui::SameLine();
            bool attach_all = false;
            if (ImGui::Button("Attach all")) {
                attach_all = true;
            }

            auto overlapped_components = m_overlap_detection_actor->get_overlapping_components();

            for (auto& it : overlapped_components) {
                auto comp = (sdk::USceneComponent*)it;
                if (!this->exists_unsafe(comp)) {
                    continue;
                }

                if (m_spawned_spheres.contains(comp) && m_spawned_spheres_to_components.contains(comp)) {
                    comp = m_spawned_spheres_to_components[comp];
                }

                if (attach_all){ 
                    if (!m_motion_controller_attached_components.contains(comp)) {
                        m_motion_controller_attached_components[comp] = std::make_shared<MotionControllerState>();
                    }
                }

                std::wstring comp_name = comp->get_class()->get_fname().to_string() + L" " + comp->get_fname().to_string();

                if (ImGui::TreeNode(utility::narrow(comp_name).data())) {
                    ui_handle_object(comp);
                    ImGui::TreePop();
                }
            }

            ImGui::TreePop();
        }
    }

    ImGui::Text("Objects: %zu (%zu actual)", m_objects.size(), sdk::FUObjectArray::get()->get_object_count());

    if (ImGui::TreeNode("Recent Objects")) {
        for (auto& object : m_most_recent_objects) {
            if (!this->exists_unsafe(object)) {
                continue;
            }

            if (ImGui::TreeNode(utility::narrow(object->get_full_name()).data())) {
                ui_handle_object(object);
                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }

    // Display common objects like things related to the player
    if (ImGui::TreeNode("Common Objects")) {
        auto engine = sdk::UGameEngine::get();
        auto world = engine != nullptr ? engine->get_world() : nullptr;

        if (world != nullptr) {
            if (ImGui::TreeNode("PlayerController")) {
                auto scope = m_path.enter_clean("Player Controller");
                auto player_controller = sdk::UGameplayStatics::get()->get_player_controller(world, 0);

                if (player_controller != nullptr) {
                    ui_handle_object(player_controller);
                } else {
                    ImGui::Text("No player controller");
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Acknowledged Pawn")) {
                auto scope = m_path.enter_clean("Acknowledged Pawn");
                auto player_controller = sdk::UGameplayStatics::get()->get_player_controller(world, 0);

                if (player_controller != nullptr) {
                    auto pawn = player_controller->get_acknowledged_pawn();

                    if (pawn != nullptr) {
                        ui_handle_object(pawn);
                    } else {
                        ImGui::Text("No pawn");
                    }
                } else {
                    ImGui::Text("No player controller");
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("Camera Manager")) {
                auto scope = m_path.enter_clean("Camera Manager");
                auto player_controller = sdk::UGameplayStatics::get()->get_player_controller(world, 0);

                if (player_controller != nullptr) {
                    auto camera_manager = player_controller->get_player_camera_manager();

                    if (camera_manager != nullptr) {
                        ui_handle_object((sdk::UObject*)camera_manager);
                    } else {
                        ImGui::Text("No camera manager");
                    }
                } else {
                    ImGui::Text("No player controller");
                }

                ImGui::TreePop();
            }

            if (ImGui::TreeNode("World")) {
                auto scope = m_path.enter_clean("World");
                ui_handle_object(world);
                ImGui::TreePop();
            }
        } else {
            ImGui::Text("No world");
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Objects by class")) {
        ImGui::Checkbox("Hide Default Classes", &m_hide_default_classes);

        static char filter[256]{};
        ImGui::InputText("Filter", filter, sizeof(filter));

        const bool filter_empty = std::string_view{filter}.empty();

        const auto now = std::chrono::steady_clock::now();
        bool needs_sort = true;

        if (m_sorting_task.valid()) {
            // Check if the sorting task is finished
            if (m_sorting_task.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
                // Do something if needed when sorting is done
                m_sorted_classes = m_sorting_task.get();
                needs_sort = true;
            } else {
                needs_sort = false;
            }
        } else {
            needs_sort = true;
        }

        if (needs_sort) {
            auto sort_classes = [this](std::vector<sdk::UClass*> classes) {
                std::sort(classes.begin(), classes.end(), [this](sdk::UClass* a, sdk::UClass* b) {
                    std::shared_lock _{m_mutex};
                    if (!m_objects.contains(a) || !m_objects.contains(b)) {
                        return false;
                    }

                    return m_meta_objects[a]->full_name < m_meta_objects[b]->full_name;
                });

                return classes;
            };

            auto unsorted_classes = std::vector<sdk::UClass*>{};

            for (auto& [c, set]: m_objects_by_class) {
                unsorted_classes.push_back(c);
            }

            // Launch sorting in a separate thread
            m_sorting_task = std::async(std::launch::async, sort_classes, unsorted_classes);
            m_last_sort_time = now;
        }

        const auto wide_filter = utility::widen(filter);
        const bool made_child = ImGui::BeginChild("Objects by class entries", ImVec2(0, 0), true, ImGuiWindowFlags_::ImGuiWindowFlags_HorizontalScrollbar);

        utility::ScopeGuard sg{[made_child]() {
            //if (made_child) {
                // well apparently BeginChild doesn't care about if it returned true or not so...
                // TODO: check this every time we update imgui?
                ImGui::EndChild();
            //}
        }};

        for (auto uclass : m_sorted_classes) {
            const auto& objects_ref = m_objects_by_class[uclass];

            if (objects_ref.empty()) {
                continue;
            }

            if (!m_meta_objects.contains(uclass)) {
                continue;
            }

            if (objects_ref.size() == 1 && m_hide_default_classes) {
                auto first = *objects_ref.begin();

                if (m_meta_objects.contains(first)) {
                    auto fc = first != nullptr ? m_meta_objects[first]->uclass : nullptr;

                    if (fc != nullptr && m_meta_objects.contains(fc) && fc->get_class_default_object() == first) {
                        continue;
                    }
                }
            }

            const auto uclass_name = utility::narrow(m_meta_objects[uclass]->full_name);
            bool valid = true;

            if (!filter_empty) {
                valid = false;

                for (auto super = (sdk::UStruct*)uclass; super; super = super->get_super_struct()) {
                    if (auto it = m_meta_objects.find(super); it != m_meta_objects.end()) {
                        if (it->second->full_name.find(wide_filter) != std::wstring::npos) {
                            valid = true;
                            break;
                        }
                    }
                }
            }

            if (!valid) {
                continue;
            }

            if (ImGui::TreeNode(uclass_name.data())) {
                ui_standard_object_context_menu(uclass);

                std::vector<sdk::UObjectBase*> objects{};

                for (auto object : objects_ref) {
                    if (m_hide_default_classes) {
                        if (auto c = m_meta_objects[object]->uclass; c != nullptr && m_meta_objects.contains(c) && c->get_class_default_object() != object) {
                            objects.push_back(object);
                        }
                    } else {
                        objects.push_back(object);   
                    }
                }

                std::sort(objects.begin(), objects.end(), [this](sdk::UObjectBase* a, sdk::UObjectBase* b) {
                    return m_meta_objects[a]->full_name < m_meta_objects[b]->full_name;
                });

                if (uclass->is_a(sdk::AActor::static_class())) {
                    static char component_add_name[256]{};

                    if (ImGui::InputText("Add Component Permanently", component_add_name, sizeof(component_add_name), ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue)) {
                        const auto component_c = sdk::find_uobject<sdk::UClass>(utility::widen(component_add_name));

                        if (component_c != nullptr) {
                            m_on_creation_add_component_jobs[uclass] = [this, component_c](sdk::UObject* object) {
                                if (!this->exists(object)) {
                                    return;
                                }

                                if (object == object->get_class()->get_class_default_object()) {
                                    return;
                                }

                                auto actor = (sdk::AActor*)object;
                                auto component = (sdk::UObject*)actor->add_component_by_class(component_c);

                                if (component != nullptr) {
                                    if (component->get_class()->is_a(sdk::find_uobject<sdk::UClass>(L"Class /Script/Engine.SphereComponent"))) {
                                        struct SphereRadiusParams {
                                            float radius{};
                                        };

                                        auto params = SphereRadiusParams{};
                                        params.radius = 100.f;

                                        const auto fn = component->get_class()->find_function(L"SetSphereRadius");

                                        if (fn != nullptr) {
                                            component->process_event(fn, &params);
                                        }
                                    }

                                    struct {
                                        bool hidden{false};
                                        bool propagate{true};
                                    } set_hidden_params{};

                                    const auto fn = component->get_class()->find_function(L"SetHiddenInGame");

                                    if (fn != nullptr) {
                                        component->process_event(fn, &set_hidden_params);
                                    }

                                    actor->finish_add_component(component);

                                    // Set component_add_name to empty
                                    component_add_name[0] = '\0';
                                } else {
                                    component_add_name[0] = 'e';
                                    component_add_name[1] = 'r';
                                    component_add_name[2] = 'r';
                                    component_add_name[3] = '\0';
                                }
                            };
                        } else {
                            strcpy_s(component_add_name, "Nonexistent component");
                        }
                    }
                }

                for (const auto& object : objects) {
                    const auto made = ImGui::TreeNode(utility::narrow(m_meta_objects[object]->full_name).data());

                    if (made) {
                        ui_handle_object((sdk::UObject*)object);
                        
                        ImGui::TreePop();
                    } else {
                        ui_standard_object_context_menu(object);
                    }
                }

                ImGui::TreePop();
            } else {
                ui_standard_object_context_menu(uclass);
            }
        }

        ImGui::TreePop();
    }
}

void UObjectHook::ui_standard_object_context_menu(sdk::UObjectBase* object) {
    if (ImGui::BeginPopupContextItem()) {
        auto sc = [](const std::string& text) {
            if (OpenClipboard(NULL)) {
                EmptyClipboard();
                HGLOBAL hcd = GlobalAlloc(GMEM_DDESHARE, text.size() + 1);
                char* data = (char*)GlobalLock(hcd);
                strcpy(data, text.c_str());
                GlobalUnlock(hcd);
                SetClipboardData(CF_TEXT, hcd);
                CloseClipboard();
            }
        };

        if (ImGui::Button("Copy Name")) {
            sc(utility::narrow(m_meta_objects[object]->full_name));
        }

        if (ImGui::Button("Copy Address")) {
            const auto hex = (std::stringstream{} << std::hex << (uintptr_t)object).str();
            sc(hex);
        }

        ImGui::EndPopup();
    }
}

void UObjectHook::ui_handle_object(sdk::UObject* object) {
    if (object == nullptr) {
        ImGui::Text("nullptr");
        return;
    }

    if (!this->exists_unsafe(object)) {
        ImGui::Text("Invalid object");
        return;
    }

    ui_standard_object_context_menu(object);

    const auto uclass = object->get_class();

    if (uclass == nullptr) {
        ImGui::Text("null class");
        return;
    }


    if (!this->exists_unsafe(uclass)) {
        ImGui::Text("Invalid class");
        return;
    }

    if (object->is_a(sdk::UClass::static_class())) {
        if (ImGui::TreeNode("Default Object")) {
            auto def = ((sdk::UClass*)object)->get_class_default_object();

            if (def != nullptr) {
                ui_handle_object(def);
            } else {
                ImGui::Text("Null default object");
            }

            ImGui::TreePop();
        }
    }

    ImGui::Text("%s", utility::narrow(object->get_full_name()).data());

    if (ImGui::TreeNode("Outer")) {
        auto outer_scope = m_path.enter("Outer");
        ui_handle_object(object->get_outer());
        ImGui::TreePop();
    }

    static const auto material_t = sdk::find_uobject<sdk::UClass>(L"Class /Script/Engine.MaterialInterface");

    if (uclass->is_a(material_t)) {
        ui_handle_material_interface(object);
    }

    static const auto widget_component_t = sdk::find_uobject<sdk::UClass>(L"Class /Script/UMG.WidgetComponent");

    if (uclass->is_a(widget_component_t)) {
        if (ImGui::Button("Set to Screen Space")) {
            static const auto set_widget_space_fn = uclass->find_function(L"SetWidgetSpace");

            if (set_widget_space_fn != nullptr) {
                struct {
                    uint32_t space{1};
                } params{};

                object->process_event(set_widget_space_fn, &params);
            }
        }
    }

    if (uclass->is_a(sdk::UActorComponent::static_class())) {
        /*if (ImGui::Button("Destroy Component")) {
            auto comp = (sdk::UActorComponent*)object;

            comp->destroy_component();
        }*/
    }

    if (uclass->is_a(sdk::USceneComponent::static_class())) {
        ui_handle_scene_component((sdk::USceneComponent*)object);
    }

    if (uclass->is_a(sdk::AActor::static_class())) {
        ui_handle_actor(object);
    }

    ui_handle_struct(object, uclass);
}

void UObjectHook::ui_handle_scene_component(sdk::USceneComponent* comp) {
    if (comp == nullptr) {
        return;
    }

    bool attached = m_motion_controller_attached_components.contains(comp);

    if (attached) {
        if (ImGui::Button("Detach")) {
            m_motion_controller_attached_components.erase(comp);

            auto existing = std::find_if(m_persistent_states.begin(), m_persistent_states.end(), [&](const auto& state2) {
                return state2 != nullptr && state2->path.resolve() == comp;
            });

            if (existing != m_persistent_states.end()) {
                (*existing)->erase_json_file();
                m_persistent_states.erase(existing);
            }
        }

        if (m_motion_controller_attached_components.contains(comp)) {
            ImGui::SameLine();
            auto& state = m_motion_controller_attached_components[comp];

            if (ImGui::Checkbox("Adjust", &state->adjusting)) {
                if (state->adjusting && m_overlap_detection_actor == nullptr) {
                    VR::get()->set_aim_allowed(false);
                    g_framework->set_draw_ui(false);
                }
            }

            ImGui::SameLine();

            if (ImGui::Checkbox("Permanent Change", &state->permanent)) {
                // Locate the existing persistent state if it exists
                auto existing = std::find_if(m_persistent_states.begin(), m_persistent_states.end(), [&](const auto& state2) {
                    return state2 != nullptr && state2->path.resolve() == comp;
                });

                if (existing != m_persistent_states.end()) {
                    (*existing)->state.permanent = state->permanent;
                }
            }

            auto existing = std::find_if(m_persistent_states.begin(), m_persistent_states.end(), [&](const auto& state2) {
                return state2 != nullptr && state2->path.resolve() == comp;
            });

            // Finetuning of the controller rotation offset
            // Convert to pitch/yaw/roll first.
            auto euler = utility::math::euler_angles_from_steamvr(state->rotation_offset);
            if (ImGui::DragFloat3("RotationOffset", &euler.x, 0.01f)) {
                // Convert back to quaternion
                state->rotation_offset = glm::quat{glm::yawPitchRoll(-euler.y, euler.x, -euler.z)};

                if (existing != m_persistent_states.end()) {
                    (*existing)->state.rotation_offset = state->rotation_offset;
                }
            }

            // Finetuning of the controller position offset.
            if (ImGui::DragFloat3("PositionOffset", &state->location_offset.x, 0.01f)) {
                if (existing != m_persistent_states.end()) {
                    (*existing)->state.location_offset = state->location_offset;
                }
            }

            auto save_state_logic = [&](const std::vector<std::string>& path) {
                auto json = serialize_mc_state(path, state);

                // Concat the entire path together and hash it to get a unique name
                std::string concat_path{};
                for (const auto& p : path) {
                    concat_path += p;
                }

                const auto hash_str = std::to_string(utility::hash(concat_path)) + "_mc_state.json";
                auto wanted_path = UObjectHook::get_persistent_dir() / hash_str;

                // Use the one this was originally loaded from instead.
                if (existing != m_persistent_states.end() && (*existing)->path_to_json.has_value()) {
                    wanted_path = (*existing)->path_to_json.value();
                }

                // Create dir if necessary
                try {
                    std::filesystem::create_directories(wanted_path.parent_path());

                    if (std::filesystem::exists(wanted_path.parent_path())) {
                        std::ofstream file{wanted_path};
                        file << json.dump(4);
                        file.close();

                        m_persistent_states = deserialize_all_mc_states();
                    }
                } catch (const std::exception& e) {
                    SPDLOG_ERROR("[UObjectHook] Failed to save motion controller state: {}", e.what());
                } catch (...) {
                    SPDLOG_ERROR("[UObjectHook] Failed to save motion controller state");
                }
            };

            // Save state stuff
            // First one is for checking whether we already have an existing persistent state
            // with its own path.
            if (existing != m_persistent_states.end()) {
                if (ImGui::Button("Save state")) {
                    save_state_logic((*existing)->path.path());
                }
            } else if (m_path.has_valid_base()) {
                if (ImGui::Button("Save state")) {
                    save_state_logic(m_path.path());
                }
            } else {
                if (auto path = try_get_path(comp); path.has_value()) {
                    if (ImGui::Button("Save state")) {
                        save_state_logic(path->path());
                    }
                } else {
                    ImGui::Text("Can't save, did not start from a valid base or none of the allowed bases can reach this component");
                }
            }
        }
    } else {
        if (m_camera_attach.object != comp) {
            if (ImGui::Button("Attach left")) {
                m_motion_controller_attached_components[comp] = std::make_shared<MotionControllerState>();
                m_motion_controller_attached_components[comp]->hand = 0;
            }

            ImGui::SameLine();

            if (ImGui::Button("Attach right")) {
                m_motion_controller_attached_components[comp] = std::make_shared<MotionControllerState>();
                m_motion_controller_attached_components[comp]->hand = 1;
            }

            ImGui::SameLine();

            if (ImGui::Button("Attach HMD")) {
                m_motion_controller_attached_components[comp] = std::make_shared<MotionControllerState>();
                m_motion_controller_attached_components[comp]->hand = 2;
            }

            if (ImGui::Button("Attach Camera to")) {
                m_camera_attach.object = comp;
                m_camera_attach.offset = glm::vec3{0.0f, 0.0f, 0.0f};
            }

            ImGui::SameLine();

            if (ImGui::Button("Attach Camera to (Relative)")) {
                m_camera_attach.object = comp;
                m_camera_attach.offset = glm::vec3{0.0f, 0.0f, m_last_camera_location.z - comp->get_world_location().z};
            }
        } else {
            if (ImGui::Button("Detach")) {
                m_camera_attach.object = nullptr;
                m_camera_attach.offset = glm::vec3{0.0f, 0.0f, 0.0f};

                if (m_persistent_camera_state != nullptr) {
                    m_persistent_camera_state->erase_json_file();
                }

                m_persistent_camera_state.reset();
            }

            ImGui::SameLine();

            if (m_persistent_camera_state != nullptr && m_persistent_camera_state->path.resolve() == comp) {
                if (ImGui::Button("Save state")) {
                    save_camera_state(m_persistent_camera_state->path.path());
                }
            } else if (m_path.has_valid_base()) {
                if (ImGui::Button("Save state")) {
                    save_camera_state(m_path.path());
                }
            } else if (auto path = try_get_path(comp); path.has_value()) {
                if (ImGui::Button("Save state")) {
                    save_camera_state(path->path());
                }
            } else {
                ImGui::Text("Can't save, did not start from a valid base or none of the allowed bases can reach this component");
            }

            if (ImGui::DragFloat3("Camera Offset", &m_camera_attach.offset.x, 0.1f)) {
                if (m_persistent_camera_state != nullptr) {
                    m_persistent_camera_state->offset = m_camera_attach.offset;
                }
            }
        }
    }

    const auto prim_comp_t = sdk::UPrimitiveComponent::static_class();
    auto prim_comp = (sdk::UPrimitiveComponent*)comp;

    bool visible = comp->is_a(prim_comp_t) ? prim_comp->is_rendering_in_main_pass() : comp->is_visible();
    bool legacy_visible = comp->is_visible();

    auto visible_checkbox = ImGui::Checkbox("Visible", &visible);
    ImGui::SameLine();

    const auto legacy_visible_changed = ImGui::Checkbox("Legacy", &legacy_visible);
    visible_checkbox |= legacy_visible_changed;

    if (visible_checkbox) {
        if (legacy_visible_changed) {
            visible = legacy_visible;
        }

        if (comp->is_a(prim_comp_t)) {
            prim_comp->set_overall_visibility(visible, legacy_visible_changed || (visible == true && !legacy_visible));
        } else {
            comp->set_visibility(visible, false);
        }

        if (visible) {
            // Check if we have a persistent property for this component
            std::shared_ptr<PersistentProperties> props{};

            for (const auto& existing_prop : m_persistent_properties) {
                if (existing_prop->path.resolve() == comp) {
                    props = existing_prop;
                    break;
                }
            }

            if (props != nullptr && props->hide) {
                props->hide = false;
                props->save_to_file();
            }
        }

        if (legacy_visible) {
            // Check if we have a persistent property for this component
            std::shared_ptr<PersistentProperties> props{};

            for (const auto& existing_prop : m_persistent_properties) {
                if (existing_prop->path.resolve() == comp) {
                    props = existing_prop;
                    break;
                }
            }

            if (props != nullptr && props->hide_legacy) {
                props->hide_legacy = false;
                props->save_to_file();
            }
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Save Visibility State")) {
        std::shared_ptr<PersistentProperties> props{};

        // Find existing one if possible
        for (const auto& existing_prop : m_persistent_properties) {
            if (existing_prop->path.resolve() == comp) {
                props = existing_prop;
                break;
            }
        }

        // Add new one if necessary
        if (props == nullptr) {
            if (m_path.has_valid_base()) {
                props = std::make_shared<PersistentProperties>();
                props->path = m_path;
            } else if (auto path = try_get_path(comp); path.has_value()) {
                props = std::make_shared<PersistentProperties>();
                props->path = path.value();
            } else {
                SPDLOG_ERROR("[UObjectHook] Can't save visibility state, did not start from a valid base or none of the allowed bases can reach this component");
                return;
            }
            
            if (props != nullptr) {
                m_persistent_properties.push_back(props);
            }
        }

        if (props != nullptr) {
            props->hide = !visible;
            props->hide_legacy = !legacy_visible;
            props->save_to_file();
        }
    }

    if (ImGui::TreeNode("Sockets")) {
        const auto socket_names = comp->get_all_socket_names();

        for (auto& name : socket_names) {
            //ImGui::Text("%s", utility::narrow(name.to_string()).data());
            if (ImGui::TreeNode(utility::narrow(name.to_string()).data())) {
                auto location = comp->get_socket_location(name.to_string());
                auto rotation = comp->get_socket_rotation(name.to_string());

                if (ImGui::DragFloat3("Location", &location.x, 0.1f)) {
                    //comp->set_socket_location(name, location);
                }

                if (ImGui::DragFloat3("Rotation", &rotation.x, 0.1f)) {
                    //comp->set_socket_rotation(name, rotation);
                }

                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }
}

std::optional<UObjectHook::StatePath> UObjectHook::try_get_path(sdk::UObject* target) const {
    const auto component_name = utility::narrow(target->get_class()->get_fname().to_string() + L" " + target->get_fname().to_string());

    // Check if any of our bases can reach this component through their components list.
    for (const auto& allowed_base : s_allowed_bases) {
        const auto possible_path = std::vector<std::string>{allowed_base, "Components", component_name};
        const auto path = StatePath{possible_path};
        const auto resolved = path.resolve();

        if (resolved == target) {
            return path;
        }
    }

    // Try to look through the objects' properties instead now.
    for (const auto& allowed_base : s_allowed_bases) {
        const auto base_obj = StatePath{{allowed_base}}.resolve_base_object();

        if (base_obj == nullptr) {
            continue;
        }

        for (auto field = base_obj->get_class()->get_child_properties(); field != nullptr; field = field->get_next()) {
            if (field->get_class()->get_name().to_string() != L"ObjectProperty") {
                continue;
            }

            const auto prop = (sdk::FObjectProperty*)field;
            const auto obj_ptr = prop->get_data<sdk::UObject*>(base_obj);

            if (obj_ptr == nullptr || *obj_ptr == nullptr) {
                continue;
            }

            const auto obj = *obj_ptr;

            if (obj == target) {
                const auto possible_path = std::vector<std::string>{allowed_base, "Properties", utility::narrow(field->get_field_name().to_string())};
                const auto path = StatePath{possible_path};
                const auto resolved = path.resolve();

                if (resolved == target) {
                    return path;
                }

                break;
            }

            // Traverse that object's components now and see if we can find it there.
            if (obj->get_class()->is_a(sdk::AActor::static_class())) {
                const auto actor = (sdk::AActor*)*obj_ptr;

                for (auto actor_comp : actor->get_all_components()) {
                    if (actor_comp != target) {
                        continue;
                    }

                    const auto possible_path = std::vector<std::string>{allowed_base, "Properties", utility::narrow(field->get_field_name().to_string()), "Components", component_name};
                    const auto path = StatePath{possible_path};
                    const auto resolved = path.resolve();

                    if (resolved == target) {
                        return path;
                    }
                }
            }
        }
    }

    return std::nullopt;
}

void UObjectHook::ui_handle_material_interface(sdk::UObject* object) {
    if (object == nullptr) {
        return;
    }

    const auto uclass = object->get_class();

    if (uclass == nullptr) {
        return;
    }

    if (ImGui::Button("Apply to all actors")) {
        static const auto mesh_component_t = sdk::find_uobject<sdk::UClass>(L"Class /Script/Engine.StaticMeshComponent");
        static const auto create_dynamic_mat = mesh_component_t->find_function(L"CreateDynamicMaterialInstance");
        static const auto set_material_fn = mesh_component_t->find_function(L"SetMaterial");
        static const auto get_num_materials_fn = mesh_component_t->find_function(L"GetNumMaterials");

        //const auto actors = m_objects_by_class[sdk::AActor::static_class()];
        const auto components = m_objects_by_class[mesh_component_t];

        //auto components = actor->get_all_components();

        for (auto comp_obj : components) {
            auto comp = (sdk::UActorComponent*)comp_obj;
            if (comp->is_a(mesh_component_t)) {
                if (comp == comp->get_class()->get_class_default_object()) {
                    continue;
                }

                auto owner = comp->get_owner();

                if (owner == nullptr || owner->get_class()->get_class_default_object() == owner) {
                    continue;
                }

                struct {
                    int32_t num{};
                } get_num_materials_params{};

                comp->process_event(get_num_materials_fn, &get_num_materials_params);

                for (int i = 0; i < get_num_materials_params.num; ++i) {
                    GameThreadWorker::get().enqueue([this, i, object, comp]() {
                        if (!this->exists(comp) || !this->exists(object)) {
                            return;
                        }

                        /*struct {
                            int32_t index{};
                            sdk::UObject* material{};
                        } params{};

                        params.index = i;
                        params.material = object;

                        comp->process_event(set_material_fn, &params);*/

                        struct {
                            int32_t index{};
                            sdk::UObject* material{};
                            sdk::FName name{L"None"};
                            sdk::UObject* ret{};
                        } params{};

                        params.index = i;
                        params.material = object;

                        comp->process_event(create_dynamic_mat, &params);

                        if (params.ret != nullptr && object->get_full_name().find(L"GizmoMaterial") != std::wstring::npos) {
                            const auto c = params.ret->get_class();
                            static const auto set_vector_param_fn = c->find_function(L"SetVectorParameterValue");

                            struct {
                                sdk::FName name{L"GizmoColor"};
                                glm::vec4 color{};
                            } set_vector_param_params{};

                            set_vector_param_params.color.x = 1.0f;
                            set_vector_param_params.color.y = 0.0f;
                            set_vector_param_params.color.z = 0.0f;
                            set_vector_param_params.color.w = 1.0f;

                            params.ret->process_event(set_vector_param_fn, &set_vector_param_params);
                        }

                        if (params.ret != nullptr && object->get_full_name().find(L"BasicShapeMaterial") != std::wstring::npos) {
                            const auto c = params.ret->get_class();
                            static const auto set_vector_param_fn = c->find_function(L"SetVectorParameterValue");

                            struct {
                                sdk::FName name{L"Color"};
                                glm::vec4 color{};
                            } set_vector_param_params{};

                            set_vector_param_params.color.x = 1.0f;
                            set_vector_param_params.color.y = 0.0f;
                            set_vector_param_params.color.z = 0.0f;
                            set_vector_param_params.color.w = 1.0f;

                            params.ret->process_event(set_vector_param_fn, &set_vector_param_params);
                        }
                    });
                }
            }
        }
    }
}

void UObjectHook::ui_handle_actor(sdk::UObject* object) {
    if (object == nullptr) {
        return;
    }

    const auto uclass = object->get_class();

    if (uclass == nullptr) {
        return;
    }

    auto actor = (sdk::AActor*)object;

    if (m_camera_attach.object != object ){
        if (ImGui::Button("Attach Camera to")) {
            m_camera_attach.object = object;
            m_camera_attach.offset = glm::vec3{0.0f, 0.0f, 0.0f};
        }

        ImGui::SameLine();

        if (ImGui::Button("Attach Camera to (Relative)")) {
            m_camera_attach.object = object;
            m_camera_attach.offset = glm::vec3{0.0f, 0.0f, m_last_camera_location.z - actor->get_actor_location().z};
        }
    } else {
        if (ImGui::Button("Detach")) {
            m_camera_attach.object = nullptr;
            m_camera_attach.offset = glm::vec3{0.0f, 0.0f, 0.0f};

            if (m_persistent_camera_state != nullptr) {
                m_persistent_camera_state->erase_json_file();
            }

            m_persistent_camera_state.reset();
        }

        if (m_persistent_camera_state != nullptr && m_persistent_camera_state->path.resolve() == object) {
            if (ImGui::Button("Save state")) {
                save_camera_state(m_persistent_camera_state->path.path());
            }
        } else if (m_path.has_valid_base()) {
            if (ImGui::Button("Save state")) {
                save_camera_state(m_path.path());
            }
        } else if (auto path = try_get_path(object); path.has_value()) {
            if (ImGui::Button("Save state")) {
                save_camera_state(path->path());
            }
        } else {
            ImGui::Text("Can't save, did not start from a valid base or none of the allowed bases can reach this object");
        }

        if (ImGui::DragFloat3("Camera Offset", &m_camera_attach.offset.x, 0.1f)) {
            if (m_persistent_camera_state != nullptr) {
                m_persistent_camera_state->offset = m_camera_attach.offset;
            }
        }
    }

    static char component_add_name[256]{};

    if (ImGui::InputText("Add Component", component_add_name, sizeof(component_add_name), ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue)) {
        const auto component_c = sdk::find_uobject<sdk::UClass>(utility::widen(component_add_name));

        if (component_c != nullptr) {
            GameThreadWorker::get().enqueue([=, this]() {
                if (!this->exists(object)) {
                    return;
                }

                auto component = (sdk::UObject*)actor->add_component_by_class(component_c);

                if (component != nullptr) {
                    if (component->get_class()->is_a(sdk::find_uobject<sdk::UClass>(L"Class /Script/Engine.SphereComponent"))) {
                        struct SphereRadiusParams {
                            float radius{};
                        };

                        auto params = SphereRadiusParams{};
                        params.radius = 100.f;

                        const auto fn = component->get_class()->find_function(L"SetSphereRadius");

                        if (fn != nullptr) {
                            component->process_event(fn, &params);
                        }
                    }

                    struct {
                        bool hidden{false};
                        bool propagate{true};
                    } set_hidden_params{};

                    const auto fn = component->get_class()->find_function(L"SetHiddenInGame");

                    if (fn != nullptr) {
                        component->process_event(fn, &set_hidden_params);
                    }

                    actor->finish_add_component(component);

                    // Set component_add_name to empty
                    component_add_name[0] = '\0';
                } else {
                    component_add_name[0] = 'e';
                    component_add_name[1] = 'r';
                    component_add_name[2] = 'r';
                    component_add_name[3] = '\0';
                }
            });
        } else {
            strcpy_s(component_add_name, "Nonexistent component");
        }
    }

    if (ImGui::TreeNode("Components")) {
        auto scope = m_path.enter("Components");
        auto components = actor->get_all_components();

        std::sort(components.begin(), components.end(), [](sdk::UObject* a, sdk::UObject* b) {
            return a->get_full_name() < b->get_full_name();
        });

        for (auto comp : components) {
            auto comp_obj = (sdk::UObject*)comp;

            ImGui::PushID(comp_obj);
            // not using full_name because its HUGE
            std::wstring comp_name = comp->get_class()->get_fname().to_string() + L" " + comp->get_fname().to_string();
            const auto made = ImGui::TreeNode(utility::narrow(comp_name).data());

            if (made) {
                auto scope2 = m_path.enter(utility::narrow(comp_name));
                ui_handle_object(comp_obj);
                ImGui::TreePop();
            }

            ImGui::PopID();
        }

        ImGui::TreePop();
    }
}

void UObjectHook::ui_handle_functions(void* object, sdk::UStruct* uclass) {
    if (uclass == nullptr) {
        return;
    }

    const bool is_real_object = object != nullptr && m_objects.contains((sdk::UObject*)object);
    auto object_real = (sdk::UObject*)object;

    std::vector<sdk::UFunction*> sorted_functions{};
    static const auto ufunction_t = sdk::UFunction::static_class();

    for (auto super = (sdk::UStruct*)uclass; super != nullptr; super = super->get_super_struct()) {
        auto funcs = super->get_children();

        for (auto func = funcs; func != nullptr; func = func->get_next()) {
            if (func->get_class()->is_a(ufunction_t)) {
                sorted_functions.push_back((sdk::UFunction*)func);
            }
        }
    }

    std::sort(sorted_functions.begin(), sorted_functions.end(), [](sdk::UFunction* a, sdk::UFunction* b) {
        return a->get_fname().to_string() < b->get_fname().to_string();
    });

    for (auto func : sorted_functions) {
        ImGui::PushID((void*)func);

        utility::ScopeGuard pop_guard{[]() {
            ImGui::PopID();
        }};

        const auto made = ImGui::TreeNode(utility::narrow(func->get_fname().to_string()).data());

        ui_standard_object_context_menu(func);

        if (m_called_functions.contains(func)) {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4{0.0f, 1.0f, 0.0f, 1.0f}, "[Called]");
        }

        if (is_real_object && made) {
            auto parameters = func->get_child_properties();

            if (parameters == nullptr || (parameters->get_next() == nullptr && parameters->get_field_name().to_string() == L"ReturnValue")) {
                if (ImGui::Button("Call")) {
                    struct {
                        char poop[1024]{};
                    } params{};

                    object_real->process_event(func, &params);
                }
            } else if (parameters->get_next() == nullptr) {
                switch (utility::hash(utility::narrow(parameters->get_class()->get_name().to_string()))) {
                case "BoolProperty"_fnv:
                    {
                        if (ImGui::Button("Enable")) {
                            struct {
                                bool enabled{true};
                                char padding[0x10];
                            } params{};

                            object_real->process_event(func, &params);
                        }

                        ImGui::SameLine();

                        if (ImGui::Button("Disable")) {
                            struct {
                                bool enabled{false};
                                char padding[0x10];
                            } params{};

                            object_real->process_event(func, &params);
                        }
                    }
                    break;
                case "StrProperty"_fnv:
                {
                    if (ImGui::Button("Call")) {
                        struct {
                            sdk::TArrayLite<wchar_t> str{};
                            char padding[0x10];
                        } params{};

                        params.str.data = (wchar_t*)L"Hello world!";
                        params.str.count = wcslen(params.str.data);
                        params.str.capacity = params.str.count + 1;

                        object_real->process_event(func, &params);
                    }
                }
                    break;
                case "UInt32Property"_fnv:
                case "IntProperty"_fnv:
                case "EnumProperty"_fnv:
                    {
                        static int value = 0;
                        ImGui::InputInt("Value", &value);

                        if (ImGui::Button("Call")) {
                            struct {
                                int value{};
                                char padding[0x10];
                            } params{};

                            params.value = value;

                            object_real->process_event(func, &params);
                        }
                    }
                    break;

                default:
                    break;
                };
            }

            for (auto param = parameters; param != nullptr; param = param->get_next()) {
                const auto cname = utility::narrow(param->get_class()->get_name().to_string());
                ImGui::Text("%s %s", cname.data(), utility::narrow(param->get_field_name().to_string()).data());

                if (cname.contains("Property")) {
                    const auto prop = (sdk::FProperty*)param;

                    if (prop->is_out_param()) {
                        ImGui::SameLine();
                        ImGui::TextColored(ImVec4{0.0f, 1.0f, 0.0f, 1.0f}, "[Out]");
                    }

                    // Display full name of StructProperty
                    if (cname == "StructProperty") {
                        const auto prop = (sdk::FStructProperty*)param;
                        const auto s = prop->get_struct();

                        if (s != nullptr) {
                            const auto struct_name = utility::narrow(s->get_full_name());
                            constexpr auto r = (float)78.0f / 255.0f;
                            constexpr auto g = (float)201.0f / 255.0f;
                            constexpr auto b = (float)176.0f / 255.0f;

                            ImGui::SameLine();
                            ImGui::TextColored(ImVec4{r, g, b, 1.0f}, "[%s]", struct_name.data());
                        }
                    }
                }
            }

            ImGui::TreePop();
        }
    }
}

void UObjectHook::ui_handle_properties(void* object, sdk::UStruct* uclass) {
    auto previous_path = m_path;

    auto scope = m_path.enter("Properties");

    if (uclass == nullptr) {
        return;
    }

    const bool is_real_object = object != nullptr && m_objects.contains((sdk::UObject*)object);

    std::vector<sdk::FField*> sorted_fields{};

    for (auto super = (sdk::UStruct*)uclass; super != nullptr; super = super->get_super_struct()) {
        auto props = super->get_child_properties();

        for (auto prop = props; prop != nullptr; prop = prop->get_next()) {
            sorted_fields.push_back(prop);
        }
    }

    std::sort(sorted_fields.begin(), sorted_fields.end(), [](sdk::FField* a, sdk::FField* b) {
        return a->get_field_name().to_string() < b->get_field_name().to_string();
    });

    for (auto prop : sorted_fields) {
        auto propc = prop->get_class();
        const auto propc_type = propc->get_name().to_string();

        if (object == nullptr) {
            const auto name = utility::narrow(propc->get_name().to_string());
            const auto field_name = utility::narrow(prop->get_field_name().to_string());
            ImGui::Text("%s %s", name.data(), field_name.data());

            continue;
        }

        auto hash_type = utility::hash(propc_type);

        if (hash_type == L"EnumProperty"_fnv) {
            auto enum_prop = (sdk::FEnumProperty*)prop;
            if (auto numeric_prop = (sdk::FProperty*)enum_prop->get_underlying_prop(); numeric_prop != nullptr) try {
                if (auto nc = numeric_prop->get_class(); nc != nullptr) {
                    const auto nc_name = nc->get_name().to_string_no_numbers(); // Calling this variant so we don't cause a crash if the name is bad.

                    // This check is just in-case we get handed back some bad memory.
                    if (nc_name.contains(L"Property")) {
                        hash_type = utility::hash(nc_name);
                    }
                }
            } catch (...) {
                // Can happen because we haven't rigorously mapped out FEnumProperty yet for older UE versions.
            }
        }

        // Right-click lambda for supported properties, usually for saving.
        auto display_context = [&](auto value) {
            if (!ImGui::BeginPopupContextItem()) {
                return;
            }

            if (!previous_path.has_valid_base()) {
                ImGui::Text("Can't save, did not start from a valid base");
                ImGui::EndPopup();
                return;
            }

            auto save_logic = [&](bool unsave = false) {
                const auto field_name = prop->get_field_name().to_string();
                std::shared_ptr<PersistentProperties> props{};

                // Find existing one if possible
                for (const auto& existing_prop : m_persistent_properties) {
                    if (existing_prop->path.resolve() == object) {
                        props = existing_prop;
                        break;
                    }
                }

                // Add new one if necessary
                if (props == nullptr) {
                    props = std::make_shared<PersistentProperties>();
                    props->path = StatePath{previous_path.path()};
                    m_persistent_properties.push_back(props);
                }

                // Add property to list if needed
                std::shared_ptr<PersistentProperties::PropertyState> state{};

                for (const auto& existing_state : props->properties) {
                    if (existing_state->name == prop->get_field_name().to_string()) {
                        state = existing_state;
                        break;
                    }
                }

                // Add new one if necessary
                if (state == nullptr) {
                    state = std::make_shared<PersistentProperties::PropertyState>();
                    state->name = prop->get_field_name().to_string();
                    props->properties.push_back(state);
                }

                memcpy(&state->data, &value, sizeof(value));

                if (unsave) {
                    props->properties.erase(
                        std::remove(props->properties.begin(), props->properties.end(), state), 
                        props->properties.end()
                    );
                }
                
                // Concat the entire path together and hash it to get a unique name
                std::string concat_path{};
                for (const auto& p : previous_path.path()) {
                    concat_path += p;
                }

                const auto hash_str = std::to_string(utility::hash(concat_path)) + "_props.json";
                auto wanted_path = UObjectHook::get_persistent_dir() / hash_str;

                if (props->path_to_json.has_value()) {
                    wanted_path = props->path_to_json.value();
                }

                // Create dir if necessary
                try {
                    std::filesystem::create_directories(wanted_path.parent_path());

                    if (props->properties.empty()) {
                        // Delete the file if it exists. Happens if we unsave.
                        if (std::filesystem::exists(wanted_path)) {
                            std::filesystem::remove(wanted_path);
                        }

                        // Delete the property entry from m_peristent_properties.
                        m_persistent_properties.erase(
                            std::remove(m_persistent_properties.begin(), m_persistent_properties.end(), props), 
                            m_persistent_properties.end()
                        );

                        return;
                    }

                    props->save_to_file(wanted_path);
                } catch (const std::exception& e) {
                    SPDLOG_ERROR("[UObjectHook] Failed to save persistent properties: {}", e.what());
                } catch (...) {
                    SPDLOG_ERROR("[UObjectHook] Failed to save persistent properties");
                }
            };

            if (ImGui::Button("Save Property")) {
                save_logic();
            }

            if (ImGui::Button("Unsave Property")) {
                save_logic(true);
            }

            ImGui::EndPopup();
        };

        switch (hash_type) {
        case L"FloatProperty"_fnv:
            {
                auto& value = *(float*)((uintptr_t)object + ((sdk::FProperty*)prop)->get_offset());
                ImGui::DragFloat(utility::narrow(prop->get_field_name().to_string()).data(), &value, 0.01f);
                display_context(value);
            }
            break;
        case L"DoubleProperty"_fnv:
            {
                auto& value = *(double*)((uintptr_t)object + ((sdk::FProperty*)prop)->get_offset());
                float casted_value = (float)value;
                if (ImGui::DragFloat(utility::narrow(prop->get_field_name().to_string()).data(), (float*)&casted_value, 0.01f)) {
                    value = (double)casted_value;
                }
                display_context(value);
            }
            break;
        case L"UInt16Property"_fnv:
            {
                auto& value = *(uint16_t*)((uintptr_t)object + ((sdk::FProperty*)prop)->get_offset());
                int converted = (int)value;
                if (ImGui::SliderInt(utility::narrow(prop->get_field_name().to_string()).data(), (int*)&converted, 0, 65535)) {
                    value = (uint16_t)converted;
                }
                display_context(value);
            }
            break;
        case L"UInt32Property"_fnv:
        case L"IntProperty"_fnv:
            {
                auto& value = *(int32_t*)((uintptr_t)object + ((sdk::FProperty*)prop)->get_offset());
                ImGui::DragInt(utility::narrow(prop->get_field_name().to_string()).data(), &value, 1);
                display_context(value);
            }
            break;
        case L"UInt64Property"_fnv:
            {
                auto& value = *(uint64_t*)((uintptr_t)object + ((sdk::FProperty*)prop)->get_offset());
                ImGui::DragScalar(utility::narrow(prop->get_field_name().to_string()).data(), ImGuiDataType_U64, &value, 1);
                display_context(value);
            }
            break;
        case L"BoolProperty"_fnv:
            {
                auto boolprop = (sdk::FBoolProperty*)prop;
                auto value = boolprop->get_value_from_object(object);
                if (ImGui::Checkbox(utility::narrow(prop->get_field_name().to_string()).data(), &value)) {
                    boolprop->set_value_in_object(object, value);
                }
                display_context(value);
            }
            break;
        case L"ByteProperty"_fnv:
            {
                auto& value = *(uint8_t*)((uintptr_t)object + ((sdk::FProperty*)prop)->get_offset());
                int converted = (int)value;
                if (ImGui::SliderInt(utility::narrow(prop->get_field_name().to_string()).data(), &converted, 0, 255)) {
                    value = (uint8_t)converted;
                }
                display_context(value);
            }
            break;
        case L"InterfaceProperty"_fnv:
        case L"ObjectProperty"_fnv:
        case L"ClassProperty"_fnv:
            {
                auto& value = *(sdk::UObject**)((uintptr_t)object + ((sdk::FProperty*)prop)->get_offset());
                
                if (ImGui::TreeNode(utility::narrow(prop->get_field_name().to_string()).data())) {
                    auto scope2 = m_path.enter(utility::narrow(prop->get_field_name().to_string()));
                    ui_handle_object(value);
                    ImGui::TreePop();
                }
            }
            break;
        case L"StructProperty"_fnv:
            {
                void* addr = (void*)((uintptr_t)object + ((sdk::FProperty*)prop)->get_offset());

                const auto made = ImGui::TreeNode(utility::narrow(prop->get_field_name().to_string()).data());

                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::Button("Copy Address")) {
                        const auto hex = (std::stringstream{} << std::hex << (uintptr_t)addr).str();

                        if (OpenClipboard(NULL)) {
                            EmptyClipboard();
                            HGLOBAL hcd = GlobalAlloc(GMEM_DDESHARE, hex.size() + 1);
                            char* data = (char*)GlobalLock(hcd);
                            strcpy(data, hex.c_str());
                            GlobalUnlock(hcd);
                            SetClipboardData(CF_TEXT, hcd);
                            CloseClipboard();
                        }
                    }

                    ImGui::EndPopup();
                }

                if (made) {
                    auto scope2 = m_path.enter(utility::narrow(prop->get_field_name().to_string()));
                    ui_handle_struct(addr, ((sdk::FStructProperty*)prop)->get_struct());
                    ImGui::TreePop();
                }
            }
            break;
        case L"Function"_fnv:
            break;
        case L"ArrayProperty"_fnv:
            if (ImGui::TreeNode(utility::narrow(prop->get_field_name().to_string()).data())) {
                auto scope2 = m_path.enter(utility::narrow(prop->get_field_name().to_string()));
                ui_handle_array_property(object, (sdk::FArrayProperty*)prop);
                ImGui::TreePop();
            }

            break;
        case L"NameProperty"_fnv:
            {
                const auto& value = *(sdk::FName*)((uintptr_t)object + ((sdk::FProperty*)prop)->get_offset());
                const auto wstr = value.to_string();
                const auto str = utility::narrow(wstr);

                ImGui::Text("%s: ", utility::narrow(prop->get_field_name().to_string()).data());
                ImGui::SameLine(0.0f, 0.0f);
                ImGui::TextColored(ImVec4{3.0f / 255.0f, 232.0f / 255.0f, 252.0f / 255.0f, 1.0f}, "%s", str.data());
            }
            break;
        case L"StrProperty"_fnv:
        {
            using FString = sdk::TArray<wchar_t>;
            const auto& value = *(FString*)((uintptr_t)object + ((sdk::FProperty*)prop)->get_offset());

            if (value.data != nullptr && value.count > 0) {
                const auto str = std::wstring_view{value.data, (size_t)value.count};
                const auto narrow_str = utility::narrow(str);

                ImGui::Text("%s: ", utility::narrow(prop->get_field_name().to_string()).data());
                ImGui::SameLine(0.0f, 0.0f);
                ImGui::TextColored(ImVec4{3.0f / 255.0f, 232.0f / 255.0f, 252.0f / 255.0f, 1.0f}, "%s", narrow_str.data());
            } else {
                ImGui::Text("%s: ", utility::narrow(prop->get_field_name().to_string()).data());
                ImGui::SameLine(0.0f, 0.0f);
                ImGui::TextColored(ImVec4{3.0f / 255.0f, 232.0f / 255.0f, 252.0f / 255.0f, 1.0f}, "[empty string]");
            }
            break;
        }
        default:
            {
                const auto name = utility::narrow(propc->get_name().to_string());
                const auto field_name = utility::narrow(prop->get_field_name().to_string());
                ImGui::Text("%s %s", name.data(), field_name.data());
            }
            break;
        };
    }
}

void UObjectHook::ui_handle_array_property(void* addr, sdk::FArrayProperty* prop) {
    if (addr == nullptr || prop == nullptr) {
        return;
    }

    const auto& array_generic = *(sdk::TArray<void*>*)((uintptr_t)addr + prop->get_offset());

    if (array_generic.data == nullptr || array_generic.count == 0) {
        ImGui::Text("Empty array");
        return;
    }

    const auto inner = prop->get_inner();

    if (inner == nullptr) {
        ImGui::Text("Failed to get inner property");
        return;
    }
    
    const auto inner_c = inner->get_class();

    if (inner_c == nullptr) {
        ImGui::Text("Failed to get inner property class");
        return;
    }

    const auto inner_c_type = utility::narrow(inner_c->get_name().to_string());

    switch (utility::hash(inner_c_type)) {
    case "InterfaceProperty"_fnv:
    case "ObjectProperty"_fnv:
    {
        const auto& array_obj = *(sdk::TArray<sdk::UObject*>*)((uintptr_t)addr + prop->get_offset());

        for (auto obj : array_obj) {
            std::wstring name = obj->get_class()->get_fname().to_string() + L" " + obj->get_fname().to_string();
            const auto narrow_name = utility::narrow(name);

            if (ImGui::TreeNode(narrow_name.data())) {
                auto scope = m_path.enter(narrow_name);
                ui_handle_object(obj);
                ImGui::TreePop();
            }
        }

        break;
    }
    case "StructProperty"_fnv:
    {
        // Not really an array of void* but we will skip over individual elements
        // using pointer arithmetic.
        const auto& array_obj = *(sdk::TArray<void*>*)((uintptr_t)addr + prop->get_offset());

        if (array_obj.data == nullptr || array_obj.count == 0) {
            ImGui::Text("Empty array");
            return;
        }

        const auto struct_prop = (sdk::FStructProperty*)inner;
        const auto strukt = struct_prop->get_struct();

        if (strukt == nullptr) {
            ImGui::Text("Cannot determine struct type");
            return;
        }
        
        auto element_size = strukt->get_struct_size();

        if (element_size == 0) {
            element_size = strukt->get_properties_size();

            if (element_size == 0) {
                ImGui::Text("Cannot determine struct size");
                return;
            }
        }

        for (size_t i = 0; i < array_obj.count; ++i) try {
            auto element = (void*)((uintptr_t)array_obj.data + (i * element_size));

            if (element != nullptr) {
                if (ImGui::TreeNode((void*)element, "Element %d", i)) {
                    // TODO: Figure out if this can be used with persistent states?
                    //auto scope = m_path.enter("Element " + std::to_string(i));
                    ui_handle_struct(element, strukt);
                    ImGui::TreePop();
                }
            }
        } catch(...) {
            ImGui::Text("Failed to display element %d", i);
        }

        break;
    }
    default:
        ImGui::Text("Array of %s (unsupported)", inner_c_type.data());
        break;
    };
}

void UObjectHook::ui_handle_struct(void* addr, sdk::UStruct* uclass) {
    if (uclass == nullptr) {
        return;
    }

    if (addr != nullptr && this->exists_unsafe((sdk::UObject*)addr) && uclass->is_a(sdk::UStruct::static_class())) {
        uclass = (sdk::UStruct*)addr;
        addr = nullptr;
    }

    // Display inheritance tree
    if (ImGui::TreeNode("Inheritance")) {
        for (auto super = (sdk::UStruct*)uclass; super != nullptr; super = super->get_super_struct()) {
            if (ImGui::TreeNode(utility::narrow(super->get_full_name()).data())) {
                ui_handle_struct(addr, super);
                ImGui::TreePop();
            }
        }

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Functions")) {
        ui_handle_functions(addr, uclass);
        ImGui::TreePop();
    }

    ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
    if (ImGui::TreeNode("Properties")) {
        ui_handle_properties(addr, uclass);
        ImGui::TreePop();
    }
}

void* UObjectHook::add_object(void* rcx, void* rdx, void* r8, void* r9, void* stack1, void* stack2, void* stack3, void* stack4) {
    auto& hook = UObjectHook::get();
    auto result = hook->m_add_object_hook.unsafe_call<void*>(rcx, rdx, r8, r9, stack1, stack2, stack3, stack4);

    {
        static bool is_rcx = [&]() {
            if (!IsBadReadPtr(rcx, sizeof(void*)) && 
                !IsBadReadPtr(*(void**)rcx, sizeof(void*)) &&
                !IsBadReadPtr(**(void***)rcx, sizeof(void*))) 
            {
                SPDLOG_INFO("[UObjectHook] RCX is UObjectBase*");
                return true;
            } else {
                SPDLOG_INFO("[UObjectHook] RDX is UObjectBase*");
                return false;
            }
        }();

        sdk::UObjectBase* obj = nullptr;

        if (is_rcx) {
            obj = (sdk::UObjectBase*)rcx;
        } else {
            obj = (sdk::UObjectBase*)rdx;
        }

        ++hook->m_debug.constructor_calls;
        hook->add_new_object(obj);
    }

    return result;
}

void* UObjectHook::destructor(sdk::UObjectBase* object, void* rdx, void* r8, void* r9) {
    auto& hook = UObjectHook::get();

    {
        std::unique_lock _{hook->m_mutex};

        if (auto it = hook->m_meta_objects.find(object); it != hook->m_meta_objects.end()) {
            ++hook->m_debug.destructor_calls;

#ifdef VERBOSE_UOBJECTHOOK
            SPDLOG_INFO("Removing object {:x} {:s}", (uintptr_t)object, utility::narrow(it->second->full_name));
#endif
            hook->m_objects.erase(object);
            hook->m_motion_controller_attached_components.erase((sdk::USceneComponent*)object);
            hook->m_spawned_spheres.erase((sdk::USceneComponent*)object);
            hook->m_spawned_spheres_to_components.erase((sdk::USceneComponent*)object);
            hook->m_components_with_spheres.erase((sdk::USceneComponent*)object);

            if (object == hook->m_overlap_detection_actor) {
                hook->m_overlap_detection_actor = nullptr;
            }

            if (object == hook->m_overlap_detection_actor_left) {
                hook->m_overlap_detection_actor_left = nullptr;
            }

            if (object == hook->m_camera_attach.object) {
                hook->m_camera_attach.object = nullptr;
            }

            /*for (auto super = (sdk::UStruct*)it->second->uclass; super != nullptr;) {
                hook->m_objects_by_class[(sdk::UClass*)super].erase(object);

                // Just make sure we don't do any operations on super because it might be invalid...
                if (!hook->m_objects.contains(super)) {
                    SPDLOG_ERROR("Super for {:x} is not valid", (uintptr_t)object);
                    break;
                }

                super = super->get_super_struct();
            }*/

            for (auto super : it->second->super_classes) {
                hook->m_objects_by_class[super].erase(object);
            }

            hook->m_reusable_meta_objects.push_back(std::move(it->second));
            hook->m_meta_objects.erase(object);
        }
    }

    auto result = hook->m_destructor_hook.unsafe_call<void*>(object, rdx, r8, r9);

    return result;
}

void UObjectHook::PersistentProperties::save_to_file(std::optional<std::filesystem::path> path) try {
    if (!path.has_value()) {
        path = path_to_json;
    }

    if (!path.has_value()) {
        std::string concat_path{};
        for (const auto& p : this->path.path()) {
            concat_path += p;
        }

        const auto hash_str = std::to_string(utility::hash(concat_path)) + "_props.json";
        path = UObjectHook::get_persistent_dir() / hash_str;
    }

    std::filesystem::create_directories(path->parent_path());

    if (!std::filesystem::exists(path->parent_path())) {
        SPDLOG_ERROR("[UObjectHook] Failed to create directory {}", path->parent_path().string());
        return;
    }

    this->path_to_json = *path;

    nlohmann::json j = to_json();
    std::ofstream file(*path);
    file << j.dump(4);
} catch (const std::exception& e) {
    SPDLOG_ERROR("[UObjectHook] Failed to save persistent properties: {}", e.what());
} catch (...) {
    SPDLOG_ERROR("[UObjectHook] Failed to save persistent properties");
}

nlohmann::json UObjectHook::PersistentProperties::to_json() const {
    nlohmann::json json{};

    json["path"] = path.path();
    json["properties"] = nlohmann::json::array();
    json["type"] = "properties";
    json["hide"] = hide;
    json["hide_legacy"] = hide_legacy;

    for (const auto& prop : properties) {
        json["properties"].push_back({
            {"name", utility::narrow(prop->name)},
            {"data", prop->data.u64}
        });
    }

    return json;
}

std::shared_ptr<UObjectHook::PersistentProperties> UObjectHook::PersistentProperties::from_json(const nlohmann::json& json) try {
    if (!json.contains("path") || !json.contains("properties") || !json.contains("type")) {
        throw std::runtime_error("Missing path or properties");
    }

    // Make sure we're loading the right type
    if (!json["type"].is_string() || json["type"].get<std::string>() != "properties") {
        throw std::runtime_error("Wrong type");
    }

    auto result = std::make_shared<UObjectHook::PersistentProperties>();

    result->path = StatePath{json["path"].get<std::vector<std::string>>()};
    result->properties.clear();

    for (const auto& prop : json["properties"]) {
        if (!prop.contains("name") || !prop.contains("data")) {
            throw std::runtime_error("Missing name or data");
        }

        if (!prop["data"].is_number_unsigned()) {
            throw std::runtime_error("Data is not unsigned");
        }

        if (!prop["name"].is_string()) {
            throw std::runtime_error("Name is not string");
        }

        auto state = std::make_shared<PropertyState>();
        state->name = utility::widen(prop["name"].get<std::string>());
        state->data.u64 = prop["data"].get<uint64_t>();
        result->properties.push_back(state);
    }

    if (json.contains("hide") && json["hide"].is_boolean()) {
        result->hide = json["hide"].get<bool>();
    } else {
        result->hide = false;
    }

    if (json.contains("hide_legacy") && json["hide_legacy"].is_boolean()) {
        result->hide_legacy = json["hide_legacy"].get<bool>();
    } else {
        result->hide_legacy = false;
    }

    return result;
} catch (const std::exception& e) {
    SPDLOG_ERROR("[UObjectHook] Failed to deserialize persistent properties: {}", e.what());
    return nullptr;
} catch (...) {
    SPDLOG_ERROR("[UObjectHook] Failed to deserialize persistent properties");
    return nullptr;
}

std::shared_ptr<UObjectHook::PersistentProperties> UObjectHook::PersistentProperties::from_json(std::filesystem::path json_path) {
    if (!std::filesystem::exists(json_path)) {
        return nullptr;
    }

    try {
        auto f = std::ifstream{json_path};

        if (f.is_open()) {
            const auto file_contents = std::string{std::istreambuf_iterator<char>(f), std::istreambuf_iterator<char>()};

            nlohmann::json data = nlohmann::json::parse(file_contents);

            return UObjectHook::PersistentProperties::from_json(data);
        }

        SPDLOG_ERROR("[UObjectHook] Failed to open JSON file {}", json_path.string());
        return nullptr;
    } catch (const std::exception& e) {
        SPDLOG_ERROR("[UObjectHook] Failed to parse JSON file {}: {}", json_path.string(), e.what());
    } catch (...) {
        SPDLOG_ERROR("[UObjectHook] Failed to parse JSON file {}", json_path.string());
    }

    return nullptr;
}


std::vector<std::shared_ptr<UObjectHook::PersistentProperties>> UObjectHook::deserialize_all_persistent_properties() const try {
    const auto uobjecthook_dir = get_persistent_dir();

    if (!std::filesystem::exists(uobjecthook_dir)) {
        return {};
    }
    
    // Gather all .json files in this directory
    std::vector<std::filesystem::path> json_files{};
    for (const auto& p : std::filesystem::directory_iterator(uobjecthook_dir)) {
        if (p.path().extension() == ".json") {
            json_files.push_back(p.path());
        }
    }

    std::vector<std::shared_ptr<UObjectHook::PersistentProperties>> result{};
    for (const auto& json_file : json_files) {
        // load file
        auto state = UObjectHook::PersistentProperties::from_json(json_file);

        if (state != nullptr) {
            state->path_to_json = json_file;
            result.push_back(state);
            SPDLOG_INFO("[UObjectHook] Loaded persistent properties from {}", json_file.string());
        } else {
            SPDLOG_ERROR("[UObjectHook] {} does not appear to be a valid persistent properties file", json_file.string());
        }
    }

    return result;
} catch (const std::exception& e) {
    SPDLOG_ERROR("[UObjectHook] Failed to deserialize all persistent properties: {}", e.what());
    return {};
} catch (...) {
    SPDLOG_ERROR("[UObjectHook] Failed to deserialize all persistent properties");
    return {};
}
`

---
## ðŸ“ UObjectHook.hpp
**Sti:** $filePath  
**StÃ¸rrelse:** 17901 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include <filesystem>
`

### ðŸ—ï¸ Classes
`cpp
class UObjectHook : public Mod {
`

### âš™ï¸ Mod Event Functions
`cpp
on_config_load(const utility::Config& cfg, bool set_defaults)
`

### âš™ï¸ General Functions
`cpp
void activate()
`

### ðŸ“„ Komplet Indhold
`cpp
#pragma once

#include <filesystem>
#include <shared_mutex>
#include <unordered_set>
#include <memory>
#include <deque>
#include <future>

#include <nlohmann/json.hpp>

#include <safetyhook.hpp>
#include <utility/PointerHook.hpp>

#include "Mod.hpp"

namespace sdk {
class UObjectBase;
class UObject;
class UClass;
class FFieldClass;
class FStructProperty;
class UScriptStruct;
class USceneComponent;
class UActorComponent;
class AActor;
class FArrayProperty;
}

class UObjectHook : public Mod {
public:
    static std::shared_ptr<UObjectHook>& get();

    std::unordered_set<sdk::UObjectBase*> get_objects_by_class(sdk::UClass* uclass) const {
        std::shared_lock _{m_mutex};
        if (auto it = m_objects_by_class.find(uclass); it != m_objects_by_class.end()) {
            return it->second;
        }

        return {};
    }

    bool exists(sdk::UObjectBase* object) const {
        std::shared_lock _{m_mutex};
        return exists_unsafe(object);
    }

    void activate();

    bool is_disabled() const {
        return m_uobject_hook_disabled;
    }

    void set_disabled(bool disabled) {
        m_uobject_hook_disabled = disabled;
        m_fixed_visibilities = false;
    }
    
    bool is_fully_hooked() const {
        return m_fully_hooked;
    }

protected:
    std::string_view get_name() const override { return "UObjectHook"; };
    bool is_advanced_mod() const override { return true; }

    std::vector<SidebarEntryInfo> get_sidebar_entries() override { 
        return {
            { "Main", true },
            { "Config", false },
            { "Developer", true }
        };
    }

    void on_config_load(const utility::Config& cfg, bool set_defaults) override;
    void on_config_save(utility::Config& cfg) override;

    void on_pre_engine_tick(sdk::UGameEngine* engine, float delta) override;
    void on_frame() override;
    void on_draw_sidebar_entry(std::string_view in_entry) override;
    void on_draw_ui() override;

    void draw_config();
    void draw_developer();
    void draw_main();

    void on_pre_calculate_stereo_view_offset(void* stereo_device, const int32_t view_index, Rotator<float>* view_rotation, 
                                             const float world_to_meters, Vector3f* view_location, bool is_double) override;

    void on_post_calculate_stereo_view_offset(void* stereo_device, const int32_t view_index, Rotator<float>* view_rotation, 
                                                      const float world_to_meters, Vector3f* view_location, bool is_double) override;

public:
    struct MotionControllerStateBase {
        enum Hand : uint8_t {
            LEFT = 0,
            RIGHT,
            HMD,
            LAST
        };

        nlohmann::json to_json() const;
        void from_json(const nlohmann::json& data);

        MotionControllerStateBase& operator=(const MotionControllerStateBase& other) = default;

        // State that can be parsed from disk
        glm::quat rotation_offset{glm::identity<glm::quat>()};
        glm::vec3 location_offset{0.0f, 0.0f, 0.0f};
        uint8_t hand{(uint8_t)Hand::RIGHT}; // 2 == HMD
        bool permanent{false};
    };

    // Assert if MotionControllerStateBase is not trivially copyable
    static_assert(std::is_trivially_copyable_v<MotionControllerStateBase>);
    static_assert(std::is_trivially_destructible_v<MotionControllerStateBase>);
    static_assert(std::is_standard_layout_v<MotionControllerStateBase>);

    struct MotionControllerState final : MotionControllerStateBase {
        ~MotionControllerState();

        MotionControllerState& operator=(const MotionControllerStateBase& other) {
            MotionControllerStateBase::operator=(other);
            return *this;
        }

        operator MotionControllerStateBase&() {
            return *this;
        }

        // In-memory state
        sdk::AActor* adjustment_visualizer{nullptr};
        bool adjusting{false};
    };

    std::shared_ptr<MotionControllerState> get_or_add_motion_controller_state(sdk::USceneComponent* component) {
        {
            std::shared_lock _{m_mutex};
            if (auto it = m_motion_controller_attached_components.find(component); it != m_motion_controller_attached_components.end()) {
                return it->second;
            }
        }

        std::unique_lock _{m_mutex};
        auto result = std::make_shared<MotionControllerState>();
        return m_motion_controller_attached_components[component] = result;

        return result;
    }

    std::optional<std::shared_ptr<MotionControllerState>> get_motion_controller_state(sdk::USceneComponent* component) {
        std::shared_lock _{m_mutex};
        if (auto it = m_motion_controller_attached_components.find(component); it != m_motion_controller_attached_components.end()) {
            return it->second;
        }

        return {};
    }

    void remove_motion_controller_state(sdk::USceneComponent* component) {
        std::unique_lock _{m_mutex};
        m_motion_controller_attached_components.erase(component);
    }

    void remove_all_motion_controller_states() {
        std::unique_lock _{m_mutex};
        m_motion_controller_attached_components.clear();
    }

private:
    struct StatePath;
    struct PersistentState;
    struct PersistentCameraState;
    struct PersistentProperties;

    bool exists_unsafe(sdk::UObjectBase* object) const {
        return m_objects.contains(object);
    }

    void hook();
    void add_new_object(sdk::UObjectBase* object);

    void tick_attachments(
        Rotator<float>* view_rotation, const float world_to_meters, Vector3f* view_location, bool is_double
    );

    void ui_standard_object_context_menu(sdk::UObjectBase* object);
    void ui_handle_object(sdk::UObject* object);
    void ui_handle_properties(void* object, sdk::UStruct* definition);
    void ui_handle_array_property(void* object, sdk::FArrayProperty* definition);
    void ui_handle_functions(void* object, sdk::UStruct* definition);
    void ui_handle_struct(void* addr, sdk::UStruct* definition);

    void ui_handle_scene_component(sdk::USceneComponent* component);
    void ui_handle_material_interface(sdk::UObject* object);
    void ui_handle_actor(sdk::UObject* object);

    void spawn_overlapper(uint32_t hand = 0);
    void destroy_overlapper();

    std::optional<StatePath> try_get_path(sdk::UObject* target) const;

    static inline const std::vector<std::string> s_allowed_bases {
        "Acknowledged Pawn",
        "Player Controller",
        "Camera Manager",
        "World"
    };

    static std::filesystem::path get_persistent_dir();
    nlohmann::json serialize_mc_state(const std::vector<std::string>& path, const std::shared_ptr<MotionControllerState>& state);
    nlohmann::json serialize_camera(const std::vector<std::string>& path);
    void save_camera_state(const std::vector<std::string>& path);
    std::optional<StatePath> deserialize_path(const nlohmann::json& data);
    std::shared_ptr<PersistentState> deserialize_mc_state(nlohmann::json& data);
    std::shared_ptr<PersistentState> deserialize_mc_state(std::filesystem::path json_path);
    std::vector<std::shared_ptr<PersistentState>> deserialize_all_mc_states();
    std::shared_ptr<PersistentCameraState> deserialize_camera(const nlohmann::json& data);
    std::shared_ptr<PersistentCameraState> deserialize_camera_state();
    void update_persistent_states();
    void update_motion_controller_components(
        const glm::vec3& hmd_location, const glm::vec3& hmd_euler,
        const glm::vec3& left_hand_location, const glm::vec3& left_hand_euler,
        const glm::vec3& right_hand_location, const glm::vec3& right_hand_euler);

    static void* add_object(void* rcx, void* rdx, void* r8, void* r9, void* stack1, void* stack2, void* stack3, void* stack4);
    static void* destructor(sdk::UObjectBase* object, void* rdx, void* r8, void* r9);

    bool m_hooked{false};
    bool m_fully_hooked{false};
    bool m_wants_activate{false};
    float m_last_delta_time{1000.0f / 60.0f};

    struct DebugInfo {
        uint64_t constructor_calls{0};
        uint64_t destructor_calls{0};
    } m_debug{};

    glm::vec3 m_last_left_grip_location{};
    glm::vec3 m_last_right_grip_location{};
    glm::quat m_last_left_aim_rotation{glm::identity<glm::quat>()};
    glm::quat m_last_right_aim_rotation{glm::identity<glm::quat>()};

    mutable std::shared_mutex m_mutex{};

    struct MetaObject {
        std::wstring full_name{};
        sdk::UClass* uclass{nullptr};
        std::vector<sdk::UClass*> super_classes{};
    };

    std::unordered_set<sdk::UObjectBase*> m_objects{};
    std::unordered_map<sdk::UObjectBase*, std::unique_ptr<MetaObject>> m_meta_objects{};
    std::unordered_map<sdk::UClass*, std::unordered_set<sdk::UObjectBase*>> m_objects_by_class{};

    std::deque<std::unique_ptr<MetaObject>> m_reusable_meta_objects{};

    SafetyHookInline m_add_object_hook{};
    SafetyHookInline m_destructor_hook{};

    std::chrono::steady_clock::time_point m_last_sort_time{};
    std::vector<sdk::UClass*> m_sorted_classes{};
    std::future<std::vector<sdk::UClass*>> m_sorting_task{};

    std::unordered_map<sdk::UClass*, std::function<void (sdk::UObject*)>> m_on_creation_add_component_jobs{};

    std::deque<sdk::UObject*> m_most_recent_objects{};
    std::unordered_set<sdk::UObject*> m_motion_controller_attached_objects{};

    std::unordered_map<sdk::USceneComponent*, std::shared_ptr<MotionControllerState>> m_motion_controller_attached_components{};
    sdk::AActor* m_overlap_detection_actor{nullptr};
    sdk::AActor* m_overlap_detection_actor_left{nullptr};

    struct CameraState {
        sdk::UObject* object{nullptr};
        glm::vec3 offset{};
    } m_camera_attach{};

    auto get_spawned_spheres() const {
        std::shared_lock _{m_mutex};
        return m_spawned_spheres;
    }
    
    std::unordered_set<sdk::USceneComponent*> m_spawned_spheres{};
    std::unordered_set<sdk::USceneComponent*> m_components_with_spheres{};
    std::unordered_map<sdk::USceneComponent*, sdk::USceneComponent*> m_spawned_spheres_to_components{};

    struct ResolvedObject {
    public:
        ResolvedObject() = default;
        ResolvedObject(void* data, sdk::UStruct* definition) : data{data}, definition{definition} {}
        ResolvedObject(std::nullptr_t) : data{nullptr}, definition{nullptr} {}

        operator sdk::UObject*() const noexcept {
            return object;
        }

        operator void*() const noexcept {
            return data;
        }

        bool operator==(void* other) const noexcept {
            return data == other;
        }

        bool operator==(sdk::UObject* other) const noexcept {
            return object == other;
        }

        bool operator==(std::nullptr_t) const noexcept {
            return data == nullptr;
        }

        bool operator!=(std::nullptr_t) const noexcept {
            return data != nullptr;
        }

        template<typename T>
        T as() const noexcept {
            return (T)data;
        }

        template<typename T>
        T as() noexcept {
            return (T)data;
        }

    public:
        union {
            void* data{nullptr};
            sdk::UObject* object;
        };

        sdk::UStruct* definition{nullptr};
        bool is_object{false};
    };

    class StatePath {
    public:
        struct PathScope {
            PathScope(const PathScope&) = delete;
            PathScope& operator=(const PathScope&) = delete;

            PathScope(PathScope&& other) noexcept : m_path(other.m_path) {
                other.m_moved = true;
            }

            PathScope& operator=(PathScope&& other) noexcept {
                if (this != &other) {
                    m_path = other.m_path;
                    other.m_moved = true;
                }
                return *this;
            }

            PathScope(StatePath& path, const std::string& name) : m_path{path} {
                m_path.push(name);
            }

            ~PathScope() {
                if (!m_moved) {
                    m_path.m_path.pop_back();
                }
            }

        private:
            StatePath& m_path;
            bool m_moved{false};
        };

        StatePath() = default;
        StatePath(const std::vector<std::string>& path) : m_path{path} {}

        StatePath& operator=(const std::vector<std::string>& path) {
            m_path = path;
            return *this;
        }

        const auto& path() const {
            return m_path;
        }

        PathScope enter(const std::string& name) {
            return PathScope{*this, name};
        }

        PathScope enter_clean(const std::string& name) {
            clear();
            return PathScope{*this, name};
        }

        bool has_valid_base() const {
            if (m_path.empty()) {
                return false;
            }

            return std::find(s_allowed_bases.begin(), s_allowed_bases.end(), m_path[0]) != s_allowed_bases.end();
        }

        sdk::UObject* resolve_base_object() const;
        ResolvedObject resolve()  const;

    private:
        void clear() {
            m_path.clear();
        }

        void push(const std::string& name) {
            m_path.push_back(name);
        }

        void pop() {
            m_path.pop_back();
        }

        std::vector<std::string> m_path{};
    } m_path;

    struct JsonAssociation {
        std::optional<std::filesystem::path> path_to_json{};
        void erase_json_file() const {
            if (path_to_json.has_value() && std::filesystem::exists(*path_to_json)) {
                std::filesystem::remove(*path_to_json);
            }
        }
    };

    struct PersistentState : JsonAssociation {
        StatePath path{};
        MotionControllerStateBase state{};
        sdk::USceneComponent* last_object{nullptr};
    };

    struct PersistentCameraState : JsonAssociation {
        StatePath path{};
        glm::vec3 offset{};
    };

    struct PersistentProperties : JsonAssociation {
        void save_to_file(std::optional<std::filesystem::path> path = std::nullopt);
        nlohmann::json to_json() const;
        static std::shared_ptr<PersistentProperties> from_json(std::filesystem::path json_path);
        static std::shared_ptr<PersistentProperties> from_json(const nlohmann::json& j);
        
        StatePath path{};

        struct PropertyState {
            std::wstring name{};
            union {
                uint64_t u64;
                double d;
                float f;
                int32_t i;
                uint8_t u8;
                uint16_t u16;
                bool b;
            } data;
        };

        std::vector<std::shared_ptr<PropertyState>> properties{};
        bool hide{false};
        bool hide_legacy{false};
    };

    glm::vec3 m_last_camera_location{};

    std::shared_ptr<PersistentCameraState> m_persistent_camera_state{};
    std::vector<std::shared_ptr<PersistentState>> m_persistent_states{};
    std::vector<std::shared_ptr<PersistentProperties>> m_persistent_properties{};

    void reload_persistent_states() {
        m_persistent_states = deserialize_all_mc_states();
        m_persistent_camera_state = deserialize_camera_state();
        m_persistent_properties = deserialize_all_persistent_properties();
    }

    void reset_persistent_states() {
        m_persistent_states.clear();
        m_persistent_properties.clear();
        m_persistent_camera_state.reset();
    }

    std::vector<std::shared_ptr<PersistentProperties>> deserialize_all_persistent_properties() const;

private:
    ModToggle::Ptr m_enabled_at_startup{ModToggle::create(generate_name("EnabledAtStartup"), false)};
    ModToggle::Ptr m_attach_lerp_enabled{ModToggle::create(generate_name("AttachLerpEnabled"), true)};
    ModSlider::Ptr m_attach_lerp_speed{ModSlider::create(generate_name("AttachLerpSpeed"), 0.01f, 30.0f, 15.0f)};

    ModKey::Ptr m_keybind_toggle_uobject_hook{ModKey::create(generate_name("ToggleUObjectHookKey"))};
    bool m_uobject_hook_disabled{false};
    bool m_fixed_visibilities{false};
    bool m_hide_default_classes{false};

    safetyhook::InlineHook m_process_event_hook{};
    bool m_process_event_listening{true};
    bool m_attempted_hook_process_event{false};
    bool m_hooked_process_event{false};
    void hook_process_event();
    static void* process_event_hook(sdk::UObject* obj, sdk::UFunction* func, void* params, void* r9);

    std::recursive_mutex m_function_mutex{};

    struct CalledFunctionInfo {
        size_t call_count{0};

        struct HeavyData {
            std::vector<uint8_t> params{};
        };

        std::unique_ptr<HeavyData> heavy_data{nullptr};
        bool wants_heavy_data{false};
    };

    std::unordered_map<sdk::UFunction*, CalledFunctionInfo> m_called_functions{};
    std::deque<sdk::UFunction*> m_most_recent_functions{};
    std::unordered_set<sdk::UFunction*> m_ignored_recent_functions{};

    struct {
        int32_t max_calls{0};
        std::array<char, 512> buffer{0};
    } m_process_event_search{};

public:
    UObjectHook() {
        m_options = {
            *m_enabled_at_startup,
            *m_attach_lerp_enabled,
            *m_attach_lerp_speed,
            *m_keybind_toggle_uobject_hook
        };
    }

private:
};
`

---
## ðŸ“ VR.cpp
**Sti:** $filePath  
**StÃ¸rrelse:** 123031 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include <fstream>
`

### ðŸ—ï¸ Classes
`cpp

`

### âš™ï¸ Mod Event Functions
`cpp
on_initialize()
`

### âš™ï¸ General Functions
`cpp
void VR::on_xinput_get_state(uint32_t* retval, uint32_t user_index, XINPUT_STATE* state)
`

### ðŸ“„ Komplet Indhold
`cpp
#define NOMINMAX

#include <fstream>

#include <windows.h>
#include <dbt.h>

#include <imgui.h>
#include <utility/Module.hpp>
#include <utility/Registry.hpp>
#include <utility/ScopeGuard.hpp>

#include <sdk/Globals.hpp>
#include <sdk/CVar.hpp>
#include <sdk/threading/GameThreadWorker.hpp>
#include <sdk/UGameplayStatics.hpp>
#include <sdk/APlayerController.hpp>

#include <tracy/Tracy.hpp>

#include "Framework.hpp"
#include "frameworkConfig.hpp"

#include "utility/Logging.hpp"

#include "VR.hpp"

std::shared_ptr<VR>& VR::get() {
    static std::shared_ptr<VR> instance = std::make_shared<VR>();
    return instance;
}

// Called when the mod is initialized
std::optional<std::string> VR::clean_initialize() try {
    ZoneScopedN(__FUNCTION__);

    auto openvr_error = initialize_openvr();

    if (openvr_error || !m_openvr->loaded) {
        if (m_openvr->error) {
            spdlog::info("OpenVR failed to load: {}", *m_openvr->error);
        }

        m_openvr->is_hmd_active = false;
        m_openvr->was_hmd_active = false;
        m_openvr->needs_pose_update = false;

        // Attempt to load OpenXR instead
        auto openxr_error = initialize_openxr();

        if (openxr_error || !m_openxr->loaded) {
            m_openxr->needs_pose_update = false;
        }
    } else {
        m_openxr->error = "OpenVR loaded first.";
    }

    if (!get_runtime()->loaded) {
        // this is okay. we're not going to fail the whole thing entirely
        // so we're just going to return OK, but
        // when the VR mod draws its menu, it'll say "VR is not available"
        return Mod::on_initialize();
    }

    // Check whether the user has Hardware accelerated GPU scheduling enabled
    const auto hw_schedule_value = utility::get_registry_dword(
        HKEY_LOCAL_MACHINE,
        "SYSTEM\\CurrentControlSet\\Control\\GraphicsDrivers",
        "HwSchMode");

    if (hw_schedule_value) {
        m_has_hw_scheduling = *hw_schedule_value == 2;
    }

    m_init_finished = true;

    // all OK
    return Mod::on_initialize();
} catch(...) {
    spdlog::error("Exception occurred in VR::on_initialize()");

    m_runtime->error = "Exception occurred in VR::on_initialize()";
    m_openxr->dll_missing = false;
    m_openvr->dll_missing = false;
    m_openxr->error = "Exception occurred in VR::on_initialize()";
    m_openvr->error = "Exception occurred in VR::on_initialize()";
    m_openvr->loaded = false;
    m_openvr->is_hmd_active = false;
    m_openxr->loaded = false;
    m_init_finished = false;

    return Mod::on_initialize();
}

std::optional<std::string> VR::initialize_openvr() {
    ZoneScopedN(__FUNCTION__);

    spdlog::info("Attempting to load OpenVR");

    m_openvr = std::make_shared<runtimes::OpenVR>();
    m_openvr->loaded = false;

    const auto wants_openxr = m_requested_runtime_name->value() == "openxr_loader.dll";

    SPDLOG_INFO("[VR] Requested runtime: {}", m_requested_runtime_name->value());

    if (wants_openxr && GetModuleHandleW(L"openxr_loader.dll") != nullptr) {
        // pre-injected
        m_openvr->dll_missing = true;
        m_openvr->error = "OpenXR already loaded";
        return Mod::on_initialize();
    }

    if (GetModuleHandleW(L"openvr_api.dll") == nullptr) {
        // pre-injected
        if (GetModuleHandleW(L"openxr_loader.dll") != nullptr) {
            m_openvr->dll_missing = true;
            m_openvr->error = "OpenXR already loaded";
            return Mod::on_initialize();
        }


        if (utility::load_module_from_current_directory(L"openvr_api.dll") == nullptr) {
            spdlog::info("[VR] Could not load openvr_api.dll");

            m_openvr->dll_missing = true;
            m_openvr->error = "Could not load openvr_api.dll";
            return Mod::on_initialize();
        }
    }

    if (g_framework->is_dx12()) {
        m_d3d12.on_reset(this);
    } else {
        m_d3d11.on_reset(this);
    }

    m_openvr->needs_pose_update = true;
    m_openvr->got_first_poses = false;
    m_openvr->is_hmd_active = true;
    m_openvr->was_hmd_active = true;

    spdlog::info("Attempting to call vr::VR_Init");

    auto error = vr::VRInitError_None;
	m_openvr->hmd = vr::VR_Init(&error, vr::VRApplication_Scene);

    // check if error
    if (error != vr::VRInitError_None) {
        m_openvr->error = "VR_Init failed: " + std::string{vr::VR_GetVRInitErrorAsEnglishDescription(error)};
        return Mod::on_initialize();
    }

    if (m_openvr->hmd == nullptr) {
        m_openvr->error = "VR_Init failed: HMD is null";
        return Mod::on_initialize();
    }

    // get render target size
    m_openvr->update_render_target_size();

    if (vr::VRCompositor() == nullptr) {
        m_openvr->error = "VRCompositor failed to initialize.";
        return Mod::on_initialize();
    }

    auto input_error = initialize_openvr_input();

    if (input_error) {
        m_openvr->error = *input_error;
        return Mod::on_initialize();
    }

    auto overlay_error = m_overlay_component.on_initialize_openvr();

    if (overlay_error) {
        m_openvr->error = *overlay_error;
        return Mod::on_initialize();
    }
    
    m_openvr->loaded = true;
    m_openvr->error = std::nullopt;
    m_runtime = m_openvr;

    return Mod::on_initialize();
}

std::optional<std::string> VR::initialize_openvr_input() {
    ZoneScopedN(__FUNCTION__);

    const auto module_directory = Framework::get_persistent_dir();

    // write default actions and bindings with the static strings we have
    for (auto& it : m_binding_files) {
        spdlog::info("Writing default binding file {}", it.first);

        std::ofstream file{ module_directory / it.first };
        file << it.second;
    }

    const auto actions_path = module_directory / "actions.json";
    auto input_error = vr::VRInput()->SetActionManifestPath(actions_path.string().c_str());

    if (input_error != vr::VRInputError_None) {
        return "VRInput failed to set action manifest path: " + std::to_string((uint32_t)input_error);
    }

    // get action set
    auto action_set_error = vr::VRInput()->GetActionSetHandle("/actions/default", &m_action_set);

    if (action_set_error != vr::VRInputError_None) {
        return "VRInput failed to get action set: " + std::to_string((uint32_t)action_set_error);
    }

    if (m_action_set == vr::k_ulInvalidActionSetHandle) {
        return "VRInput failed to get action set handle.";
    }

    for (auto& it : m_action_handles) {
        auto error = vr::VRInput()->GetActionHandle(it.first.c_str(), &it.second.get());

        if (error != vr::VRInputError_None) {
            return "VRInput failed to get action handle: (" + it.first + "): " + std::to_string((uint32_t)error);
        }

        if (it.second == vr::k_ulInvalidActionHandle) {
            return "VRInput failed to get action handle: (" + it.first + ")";
        }
    }

    m_active_action_set.ulActionSet = m_action_set;
    m_active_action_set.ulRestrictedToDevice = vr::k_ulInvalidInputValueHandle;
    m_active_action_set.nPriority = 0;

    m_openvr->pose_action = m_action_pose;
    m_openvr->grip_pose_action = m_action_grip_pose;

    detect_controllers();

    return std::nullopt;
}

std::optional<std::string> VR::initialize_openxr() {
    ZoneScopedN(__FUNCTION__);

    m_openxr.reset();
    m_openxr = std::make_shared<runtimes::OpenXR>();

    spdlog::info("[VR] Initializing OpenXR");

    if (GetModuleHandleW(L"openxr_loader.dll") == nullptr) {
        if (utility::load_module_from_current_directory(L"openxr_loader.dll") == nullptr) {
            spdlog::info("[VR] Could not load openxr_loader.dll");

            m_openxr->loaded = false;
            m_openxr->error = "Could not load openxr_loader.dll";

            return std::nullopt;
        }
    }

    if (g_framework->is_dx12()) {
        m_d3d12.on_reset(this);
    } else {
        m_d3d11.on_reset(this);
    }

    m_openxr->needs_pose_update = true;
    m_openxr->got_first_poses = false;

    // Step 1: Create an instance
    spdlog::info("[VR] Creating OpenXR instance");

    XrResult result{XR_SUCCESS};

    // We may just be restarting OpenXR, so try to find an existing instance first
    if (m_openxr->instance == XR_NULL_HANDLE) {
        std::vector<const char*> extensions{};

        if (g_framework->is_dx12()) {
            extensions.push_back(XR_KHR_D3D12_ENABLE_EXTENSION_NAME);
        } else {
            extensions.push_back(XR_KHR_D3D11_ENABLE_EXTENSION_NAME);
        }

        // Enumerate available extensions and enable depth extension if available
        uint32_t extension_count{};
        result = xrEnumerateInstanceExtensionProperties(nullptr, 0, &extension_count, nullptr);

        std::vector<XrExtensionProperties> extension_properties(extension_count, {XR_TYPE_EXTENSION_PROPERTIES});

        if (!XR_FAILED(result)) try {
            result = xrEnumerateInstanceExtensionProperties(nullptr, extension_count, &extension_count, extension_properties.data());

            if (!XR_FAILED(result)) {
                for (const auto& extension_property : extension_properties) {
                    spdlog::info("[VR] Found OpenXR extension: {}", extension_property.extensionName);
                }

                const std::unordered_set<std::string> wanted_extensions {
                    XR_KHR_COMPOSITION_LAYER_DEPTH_EXTENSION_NAME,
                    XR_KHR_COMPOSITION_LAYER_CYLINDER_EXTENSION_NAME
                    // To be seen if we need more!
                };

                for (const auto& extension_property : extension_properties) {
                    if (wanted_extensions.contains(extension_property.extensionName)) {
                        spdlog::info("[VR] Enabling {} extension", extension_property.extensionName);
                        m_openxr->enabled_extensions.insert(extension_property.extensionName);
                        extensions.push_back(extension_property.extensionName);
                    }
                }
            }
        } catch(...) {
            spdlog::error("[VR] Unknown error while enumerating OpenXR extensions");
        }

        XrInstanceCreateInfo instance_create_info{XR_TYPE_INSTANCE_CREATE_INFO};
        instance_create_info.next = nullptr;
        instance_create_info.enabledExtensionCount = (uint32_t)extensions.size();
        instance_create_info.enabledExtensionNames = extensions.data();

        std::string application_name{"UEVR"};

        // Append the current executable name to the application base name
        {
            const auto exe = utility::get_executable();
            const auto full_path = utility::get_module_pathw(exe);

            if (full_path) {
                const auto fs_path = std::filesystem::path(*full_path);
                const auto filename = fs_path.stem().string();

                application_name += "_" + filename;

                // Trim the name to 127 characters
                if (application_name.length() >= XR_MAX_APPLICATION_NAME_SIZE) {
                    application_name = application_name.substr(0, XR_MAX_APPLICATION_NAME_SIZE - 1);
                }
            }
        }

        spdlog::info("[VR] Application name: {}", application_name);

        strcpy(instance_create_info.applicationInfo.applicationName, application_name.c_str());
        instance_create_info.applicationInfo.applicationName[XR_MAX_APPLICATION_NAME_SIZE - 1] = '\0';
        instance_create_info.applicationInfo.apiVersion = XR_CURRENT_API_VERSION;
        
        result = xrCreateInstance(&instance_create_info, &m_openxr->instance);

        // we can't convert the result to a string here
        // because the function requires the instance to be valid
        if (result != XR_SUCCESS) {
            m_openxr->error = "Could not create openxr instance: " + std::to_string((int32_t)result);
            if (result == XR_ERROR_LIMIT_REACHED) {
                m_openxr->error = "Could not create openxr instance: XR_ERROR_LIMIT_REACHED\n"
                    "Ensure that the OpenXR plugin has been renamed or deleted from the game's binaries folder.";
            }
            spdlog::error("[VR] {}", m_openxr->error.value());

            return std::nullopt;
        }
    } else {
        spdlog::info("[VR] Found existing openxr instance");
    }
    
    // Step 2: Create a system
    spdlog::info("[VR] Creating OpenXR system");

    // We may just be restarting OpenXR, so try to find an existing system first
    if (m_openxr->system == XR_NULL_SYSTEM_ID) {
        XrSystemGetInfo system_info{XR_TYPE_SYSTEM_GET_INFO};
        system_info.formFactor = m_openxr->form_factor;

        result = xrGetSystem(m_openxr->instance, &system_info, &m_openxr->system);

        if (result != XR_SUCCESS) {
            m_openxr->error = "Could not create openxr system: " + m_openxr->get_result_string(result);
            spdlog::error("[VR] {}", m_openxr->error.value());

            return std::nullopt;
        }
    } else {
        spdlog::info("[VR] Found existing openxr system");
    }

    // Step 3: Create a session
    spdlog::info("[VR] Initializing graphics info");

    XrSessionCreateInfo session_create_info{XR_TYPE_SESSION_CREATE_INFO};

    if (g_framework->is_dx12()) {
        m_d3d12.openxr().initialize(session_create_info);
    } else {
        m_d3d11.openxr().initialize(session_create_info);
    }

    spdlog::info("[VR] Creating OpenXR session");
    session_create_info.systemId = m_openxr->system;
    result = xrCreateSession(m_openxr->instance, &session_create_info, &m_openxr->session);

    if (result != XR_SUCCESS) {
        m_openxr->error = "Could not create openxr session: " + m_openxr->get_result_string(result);
        spdlog::error("[VR] {}", m_openxr->error.value());

        return std::nullopt;
    }

    // Step 4: Create a space
    spdlog::info("[VR] Creating OpenXR space");

    // We may just be restarting OpenXR, so try to find an existing space first

    if (m_openxr->stage_space == XR_NULL_HANDLE) {
        XrReferenceSpaceCreateInfo space_create_info{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
        space_create_info.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
        space_create_info.poseInReferenceSpace = {};
        space_create_info.poseInReferenceSpace.orientation.w = 1.0f;

        result = xrCreateReferenceSpace(m_openxr->session, &space_create_info, &m_openxr->stage_space);

        if (result != XR_SUCCESS) {
            m_openxr->error = "Could not create openxr stage space: " + m_openxr->get_result_string(result);
            spdlog::error("[VR] {}", m_openxr->error.value());

            return std::nullopt;
        }
    }

    if (m_openxr->view_space == XR_NULL_HANDLE) {
        XrReferenceSpaceCreateInfo space_create_info{XR_TYPE_REFERENCE_SPACE_CREATE_INFO};
        space_create_info.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_VIEW;
        space_create_info.poseInReferenceSpace = {};
        space_create_info.poseInReferenceSpace.orientation.w = 1.0f;

        result = xrCreateReferenceSpace(m_openxr->session, &space_create_info, &m_openxr->view_space);

        if (result != XR_SUCCESS) {
            m_openxr->error = "Could not create openxr view space: " + m_openxr->get_result_string(result);
            spdlog::error("[VR] {}", m_openxr->error.value());

            return std::nullopt;
        }
    }

    // Step 5: Get the system properties
    spdlog::info("[VR] Getting OpenXR system properties");

    XrSystemProperties system_properties{XR_TYPE_SYSTEM_PROPERTIES};
    result = xrGetSystemProperties(m_openxr->instance, m_openxr->system, &system_properties);

    if (result != XR_SUCCESS) {
        m_openxr->error = "Could not get system properties: " + m_openxr->get_result_string(result);
        spdlog::error("[VR] {}", m_openxr->error.value());

        return std::nullopt;
    }

    m_openxr->on_system_properties_acquired(system_properties);

    // Step 6: Get the view configuration properties
    m_openxr->update_render_target_size();

    // Step 7: Create a view
    if (!m_openxr->view_configs.empty()){
        m_openxr->views.resize(m_openxr->view_configs.size(), {XR_TYPE_VIEW});
        m_openxr->stage_views.resize(m_openxr->view_configs.size(), {XR_TYPE_VIEW});
    }

    if (m_openxr->view_configs.empty()) {
        m_openxr->error = "No view configurations found";
        spdlog::error("[VR] {}", m_openxr->error.value());

        return std::nullopt;
    }

    m_openxr->loaded = true;
    m_runtime = m_openxr;

    if (auto err = initialize_openxr_input()) {
        m_openxr->error = err.value();
        m_openxr->loaded = false;
        spdlog::error("[VR] {}", m_openxr->error.value());

        return std::nullopt;
    }

    detect_controllers();

    if (m_init_finished) {
        // This is usually done in on_config_load
        // but the runtime can be reinitialized, so we do it here instead
        initialize_openxr_swapchains();
    }

    return std::nullopt;
}

std::optional<std::string> VR::initialize_openxr_input() {
    ZoneScopedN(__FUNCTION__);

    if (auto err = m_openxr->initialize_actions(VR::actions_json)) {
        m_openxr->error = err.value();
        spdlog::error("[VR] {}", m_openxr->error.value());

        return std::nullopt;
    }
    
    for (auto& it : m_action_handles) {
        auto openxr_action_name = m_openxr->translate_openvr_action_name(it.first);

        if (m_openxr->action_set.action_map.contains(openxr_action_name)) {
            it.second.get() = (decltype(it.second)::type)m_openxr->action_set.action_map[openxr_action_name];
            spdlog::info("[VR] Successfully mapped action {} to {}", it.first, openxr_action_name);
        }
    }

    m_left_joystick = (decltype(m_left_joystick))VRRuntime::Hand::LEFT;
    m_right_joystick = (decltype(m_right_joystick))VRRuntime::Hand::RIGHT;

    return std::nullopt;
}

std::optional<std::string> VR::initialize_openxr_swapchains() {
    ZoneScopedN(__FUNCTION__);

    // This depends on the config being loaded.
    if (!m_init_finished) {
        return std::nullopt;
    }

    spdlog::info("[VR] Creating OpenXR swapchain");

    const auto supported_swapchain_formats = m_openxr->get_supported_swapchain_formats();

    // Log
    for (auto f : supported_swapchain_formats) {
        spdlog::info("[VR] Supported swapchain format: {}", f);
    }

    if (g_framework->is_dx12()) {
        auto err = m_d3d12.openxr().create_swapchains();

        if (err) {
            m_openxr->error = err.value();
            m_openxr->loaded = false;
            spdlog::error("[VR] {}", m_openxr->error.value());

            return m_openxr->error;
        }
    } else {
        auto err = m_d3d11.openxr().create_swapchains();

        if (err) {
            m_openxr->error = err.value();
            m_openxr->loaded = false;
            spdlog::error("[VR] {}", m_openxr->error.value());
            return m_openxr->error;
        }
    }

    return std::nullopt;
}

bool VR::detect_controllers() {
    ZoneScopedN(__FUNCTION__);

    // already detected
    if (!m_controllers.empty()) {
        return true;
    }

    if (get_runtime()->is_openvr()) {
        auto left_joystick_origin_error = vr::EVRInputError::VRInputError_None;
        auto right_joystick_origin_error = vr::EVRInputError::VRInputError_None;

        vr::InputOriginInfo_t left_joystick_origin_info{};
        vr::InputOriginInfo_t right_joystick_origin_info{};

        // Get input origin info for the joysticks
        // get the source input device handles for the joysticks
        auto left_joystick_error = vr::VRInput()->GetInputSourceHandle("/user/hand/left", &m_left_joystick);

        if (left_joystick_error != vr::VRInputError_None) {
            return false;
        }

        auto right_joystick_error = vr::VRInput()->GetInputSourceHandle("/user/hand/right", &m_right_joystick);

        if (right_joystick_error != vr::VRInputError_None) {
            return false;
        }

        m_openvr->left_controller_handle = m_left_joystick;
        m_openvr->right_controller_handle = m_right_joystick;

        left_joystick_origin_info = {};
        right_joystick_origin_info = {};

        left_joystick_origin_error = vr::VRInput()->GetOriginTrackedDeviceInfo(m_left_joystick, &left_joystick_origin_info, sizeof(left_joystick_origin_info));
        right_joystick_origin_error = vr::VRInput()->GetOriginTrackedDeviceInfo(m_right_joystick, &right_joystick_origin_info, sizeof(right_joystick_origin_info));
        if (left_joystick_origin_error != vr::EVRInputError::VRInputError_None || right_joystick_origin_error != vr::EVRInputError::VRInputError_None) {
            return false;
        }

        // Instead of manually going through the devices,
        // We do this. The order of the devices isn't always guaranteed to be
        // Left, and then right. Using the input state handles will always
        // Get us the correct device indices.
        m_controllers.push_back(left_joystick_origin_info.trackedDeviceIndex);
        m_controllers.push_back(right_joystick_origin_info.trackedDeviceIndex);
        m_controllers_set.insert(left_joystick_origin_info.trackedDeviceIndex);
        m_controllers_set.insert(right_joystick_origin_info.trackedDeviceIndex);

        spdlog::info("Left Hand: {}", left_joystick_origin_info.trackedDeviceIndex);
        spdlog::info("Right Hand: {}", right_joystick_origin_info.trackedDeviceIndex);

        m_openvr->left_controller_index = left_joystick_origin_info.trackedDeviceIndex;
        m_openvr->right_controller_index = right_joystick_origin_info.trackedDeviceIndex;
    } else if (get_runtime()->is_openxr()) {
        // ezpz
        m_controllers.push_back(1);
        m_controllers.push_back(2);
        m_controllers_set.insert(1);
        m_controllers_set.insert(2);

        spdlog::info("Left Hand: {}", 1);
        spdlog::info("Right Hand: {}", 2);
    }


    return true;
}

bool VR::is_any_action_down() {
    ZoneScopedN(__FUNCTION__);

    if (!m_runtime->ready()) {
        return false;
    }

    const auto left_axis = get_left_stick_axis();
    const auto right_axis = get_right_stick_axis();

    if (glm::length(left_axis) >= m_joystick_deadzone->value()) {
        return true;
    }

    if (glm::length(right_axis) >= m_joystick_deadzone->value()) {
        return true;
    }

    const auto left_joystick = get_left_joystick();
    const auto right_joystick = get_right_joystick();

    for (auto& it : m_action_handles) {
        // These are too easy to trigger
        if (it.second == m_action_thumbrest_touch_left || it.second == m_action_thumbrest_touch_right) {
            continue;
        }

        if (it.second == m_action_a_button_touch_left || it.second == m_action_a_button_touch_right) {
            continue;
        }

        if (it.second == m_action_b_button_touch_left || it.second == m_action_b_button_touch_right) {
            continue;
        }

        if (is_action_active(it.second, left_joystick) || is_action_active(it.second, right_joystick)) {
            return true;
        }
    }

    return false;
}

bool VR::on_message(HWND wnd, UINT message, WPARAM w_param, LPARAM l_param) {
    ZoneScopedN(__FUNCTION__);

    if (message == WM_DEVICECHANGE && !m_spoofed_gamepad_connection) {
        spdlog::info("[VR] Received WM_DEVICECHANGE");
        m_last_xinput_spoof_sent = std::chrono::steady_clock::now();
    }

    return true;
}

void VR::on_xinput_get_state(uint32_t* retval, uint32_t user_index, XINPUT_STATE* state) {
    ZoneScopedN(__FUNCTION__);

    if (std::chrono::steady_clock::now() - m_last_engine_tick > std::chrono::seconds(1)) {
        SPDLOG_INFO_EVERY_N_SEC(1, "[VR] XInputGetState called, but engine tick hasn't been called in over a second. Is the game loading?");
        update_action_states();
    }

    if (*retval == ERROR_SUCCESS) {
        // Once here for normal gamepads, and once for the spoofed gamepad at the end
        update_imgui_state_from_xinput_state(*state, false);
        gamepad_snapturn(*state);
    }

    const auto now = std::chrono::steady_clock::now();

    if (now - m_last_xinput_update > std::chrono::seconds(2)) {
        m_lowest_xinput_user_index = user_index;
    }

    if (user_index < m_lowest_xinput_user_index) {
        m_lowest_xinput_user_index = user_index;
        spdlog::info("[VR] Changed lowest XInput user index to {}", user_index);
    }

    if (user_index != m_lowest_xinput_user_index) {
        if (!m_spoofed_gamepad_connection && is_using_controllers()) {
            spdlog::info("[VR] XInputGetState called, but user index is {}", user_index);
        }

        return;
    }

    if (!m_spoofed_gamepad_connection) {
        spdlog::info("[VR] Successfully spoofed gamepad connection @ {}", user_index);
    }
    
    m_last_xinput_update = now;
    m_spoofed_gamepad_connection = true;

    auto runtime = get_runtime();

    auto do_pause_select = [&]() {
        if (!runtime->ready()) {
            return;
        }

        if (runtime->handle_pause) {
            // Spoof the start button being pressed
            state->Gamepad.wButtons |= XINPUT_GAMEPAD_START;
            *retval = ERROR_SUCCESS;
            runtime->handle_pause = false;
            runtime->handle_select_button = false;
        }

        if (runtime->handle_select_button) {
            // Spoof the back button being pressed
            state->Gamepad.wButtons |= XINPUT_GAMEPAD_BACK;
            *retval = ERROR_SUCCESS;
            runtime->handle_select_button = false;
            runtime->handle_pause = false;
        }
    };

    do_pause_select();

    if (is_using_controllers_within(std::chrono::minutes(5))) {
        *retval = ERROR_SUCCESS;
    }

    if (!is_using_controllers()) {
        return;
    }

    // Clear button state for VR controllers
    if (is_using_controllers_within(std::chrono::seconds(5))) {
        state->Gamepad.wButtons = 0;
        state->Gamepad.bLeftTrigger = 0;
        state->Gamepad.bRightTrigger = 0;
        state->Gamepad.sThumbLX = 0;
        state->Gamepad.sThumbLY = 0;
        state->Gamepad.sThumbRX = 0;
        state->Gamepad.sThumbRY = 0;
    }

    const auto left_joystick = get_left_joystick();
    const auto right_joystick = get_right_joystick();
    const auto wants_swap = m_swap_controllers->value();

    runtime->handle_pause_select(is_action_active_any_joystick(m_action_system_button));
    do_pause_select();

    const auto& a_button_left = !wants_swap ? m_action_a_button_left : m_action_a_button_right;
    const auto& a_button_right = !wants_swap ? m_action_a_button_right : m_action_a_button_left;

    const auto is_right_a_button_down = is_action_active_any_joystick(a_button_right);
    const auto is_left_a_button_down = is_action_active_any_joystick(a_button_left);

    if (is_right_a_button_down) {
        state->Gamepad.wButtons |= XINPUT_GAMEPAD_A;
    }

    if (is_left_a_button_down) {
        state->Gamepad.wButtons |= XINPUT_GAMEPAD_B;
    }

    const auto& b_button_left = !wants_swap ? m_action_b_button_left : m_action_b_button_right;
    const auto& b_button_right = !wants_swap ? m_action_b_button_right : m_action_b_button_left;

    const auto is_right_b_button_down = is_action_active_any_joystick(b_button_right);
    const auto is_left_b_button_down = is_action_active_any_joystick(b_button_left);

    if (is_right_b_button_down) {
        state->Gamepad.wButtons |= XINPUT_GAMEPAD_X;
    }

    if (is_left_b_button_down) {
        state->Gamepad.wButtons |= XINPUT_GAMEPAD_Y;
    }

    const auto is_left_joystick_click_down = is_action_active(m_action_joystick_click, left_joystick);
    const auto is_right_joystick_click_down = is_action_active(m_action_joystick_click, right_joystick);

    if (is_left_joystick_click_down) {
        state->Gamepad.wButtons |= XINPUT_GAMEPAD_LEFT_THUMB;
    }

    if (is_right_joystick_click_down) {
        state->Gamepad.wButtons |= XINPUT_GAMEPAD_RIGHT_THUMB;
    }

    const auto is_left_trigger_down = is_action_active(m_action_trigger, left_joystick);
    const auto is_right_trigger_down = is_action_active(m_action_trigger, right_joystick);

    if (is_left_trigger_down) {
        state->Gamepad.bLeftTrigger = 255;
    }

    if (is_right_trigger_down) {
        state->Gamepad.bRightTrigger = 255;
    }

    const auto is_right_grip_down = is_action_active(m_action_grip, right_joystick);
    const auto is_left_grip_down = is_action_active(m_action_grip, left_joystick);

    if (is_right_grip_down) {
        state->Gamepad.wButtons |= XINPUT_GAMEPAD_RIGHT_SHOULDER;
    }

    if (is_left_grip_down) {
        state->Gamepad.wButtons |= XINPUT_GAMEPAD_LEFT_SHOULDER;
    }

    const auto is_dpad_up_down = is_action_active_any_joystick(m_action_dpad_up);

    if (is_dpad_up_down) {
        state->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_UP;
    }

    const auto is_dpad_right_down = is_action_active_any_joystick(m_action_dpad_right);

    if (is_dpad_right_down) {
        state->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_RIGHT;
    }

    const auto is_dpad_down_down = is_action_active_any_joystick(m_action_dpad_down);

    if (is_dpad_down_down) {
        state->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_DOWN;
    }

    const auto is_dpad_left_down = is_action_active_any_joystick(m_action_dpad_left);

    if (is_dpad_left_down) {
        state->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_LEFT;
    }

    const auto left_joystick_axis = get_joystick_axis(left_joystick);
    const auto right_joystick_axis = get_joystick_axis(right_joystick);

    const auto true_left_joystick_axis = get_joystick_axis(m_left_joystick);
    const auto true_right_joystick_axis = get_joystick_axis(m_right_joystick);

    state->Gamepad.sThumbLX = (int16_t)std::clamp<float>(((float)state->Gamepad.sThumbLX + left_joystick_axis.x * 32767.0f), -32767.0f, 32767.0f);
    state->Gamepad.sThumbLY = (int16_t)std::clamp<float>(((float)state->Gamepad.sThumbLY + left_joystick_axis.y * 32767.0f), -32767.0f, 32767.0f);

    state->Gamepad.sThumbRX = (int16_t)std::clamp<float>(((float)state->Gamepad.sThumbRX + right_joystick_axis.x * 32767.0f), -32767.0f, 32767.0f);
    state->Gamepad.sThumbRY = (int16_t)std::clamp<float>(((float)state->Gamepad.sThumbRY + right_joystick_axis.y * 32767.0f), -32767.0f, 32767.0f);

    bool already_dpad_shifted{false};

    if (m_dpad_gesture_state.direction != DPadGestureState::Direction::NONE) {
        already_dpad_shifted = true;

        if ((m_dpad_gesture_state.direction & DPadGestureState::Direction::UP) != 0) {
            state->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_UP;
        }

        if ((m_dpad_gesture_state.direction & DPadGestureState::Direction::RIGHT) != 0) {
            state->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_RIGHT;
        }

        if ((m_dpad_gesture_state.direction & DPadGestureState::Direction::DOWN) != 0) {
            state->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_DOWN;
        }

        if ((m_dpad_gesture_state.direction & DPadGestureState::Direction::LEFT) != 0) {
            state->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_LEFT;
        }

        // Zero out the thumbstick values
        state->Gamepad.sThumbLX = 0;
        state->Gamepad.sThumbLY = 0;

        std::scoped_lock _{m_dpad_gesture_state.mtx};
        m_dpad_gesture_state.direction = DPadGestureState::Direction::NONE;
    }

    // Touching the thumbrest allows us to use the thumbstick as a dpad.  Additional options are for controllers without capacitives/games that rely solely on DPad
    if (!already_dpad_shifted && m_dpad_shifting->value()) {
        bool button_touch_inactive{true};
        bool thumbrest_check{false};

        DPadMethod dpad_method = get_dpad_method();
        if (dpad_method == DPadMethod::RIGHT_TOUCH) {
            thumbrest_check = is_action_active_any_joystick(m_action_thumbrest_touch_right);
            button_touch_inactive = !is_action_active_any_joystick(m_action_a_button_touch_right) && !is_action_active_any_joystick(m_action_b_button_touch_right);
        }
        if (dpad_method == DPadMethod::LEFT_TOUCH) {
            thumbrest_check = is_action_active_any_joystick(m_action_thumbrest_touch_left);
            button_touch_inactive = !is_action_active_any_joystick(m_action_a_button_touch_left) && !is_action_active_any_joystick(m_action_b_button_touch_left);
        }

        const auto dpad_active = (button_touch_inactive && thumbrest_check) || dpad_method == DPadMethod::LEFT_JOYSTICK || dpad_method == DPadMethod::RIGHT_JOYSTICK;

        if (dpad_active) {
            float ty{0.0f};
            float tx{0.0f};
            //SHORT ThumbY{0};
            //SHORT ThumbX{0};
            // If someone is accidentally touching both thumbrests while also moving a joystick, this will default to left joystick.
            if (dpad_method == DPadMethod::RIGHT_TOUCH || dpad_method == DPadMethod::LEFT_JOYSTICK) {
                //ThumbY = state->Gamepad.sThumbLY;
                //ThumbX = state->Gamepad.sThumbLX;
                ty = true_left_joystick_axis.y;
                tx = true_left_joystick_axis.x;
            }
            else if (dpad_method == DPadMethod::LEFT_TOUCH || dpad_method == DPadMethod::RIGHT_JOYSTICK) {
                //ThumbY = state->Gamepad.sThumbRY;
                //ThumbX = state->Gamepad.sThumbRX;
                ty = true_right_joystick_axis.y;
                tx = true_right_joystick_axis.x;
            }
            
            if (ty >= 0.5f) {
                state->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_UP;
            }

            if (ty <= -0.5f) {
                state->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_DOWN;
            }

            if (tx >= 0.5f) {
                state->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_RIGHT;
            }

            if (tx <= -0.5f) {
                state->Gamepad.wButtons |= XINPUT_GAMEPAD_DPAD_LEFT;
            }

            if (dpad_method == DPadMethod::RIGHT_TOUCH || dpad_method == DPadMethod::LEFT_JOYSTICK) {
                if (!wants_swap) {
                    state->Gamepad.sThumbLY = 0;
                    state->Gamepad.sThumbLX = 0;
                } else {
                    state->Gamepad.sThumbRY = 0;
                    state->Gamepad.sThumbRX = 0;
                }
            }
            else if (dpad_method == DPadMethod::LEFT_TOUCH || dpad_method == DPadMethod::RIGHT_JOYSTICK) {
                if (!wants_swap) {
                    state->Gamepad.sThumbRY = 0;
                    state->Gamepad.sThumbRX = 0;
                } else {
                    state->Gamepad.sThumbLY = 0;
                    state->Gamepad.sThumbLX = 0;
                }
            }
        }
    }

    // Determine if snapturn should be run on frame
    if (m_snapturn->value()) {
        DPadMethod dpad_method = get_dpad_method();
        const auto snapturn_deadzone = get_snapturn_js_deadzone();
        float stick_axis{};

        if (!m_was_snapturn_run_on_input) {
            if (dpad_method == RIGHT_JOYSTICK) {
                stick_axis = true_left_joystick_axis.x;
                if (glm::abs(stick_axis) >= snapturn_deadzone) {
                    if (stick_axis < 0) {
                        m_snapturn_left = true;
                    }
                    m_snapturn_on_frame = true;
                    m_was_snapturn_run_on_input = true;
                }
            }
            else {
                stick_axis = right_joystick_axis.x;
                const auto& thumbrest_touch_left = !wants_swap ? m_action_thumbrest_touch_left : m_action_thumbrest_touch_right;
                if (glm::abs(stick_axis) >= snapturn_deadzone && !(dpad_method == DPadMethod::LEFT_TOUCH && is_action_active_any_joystick(thumbrest_touch_left))) {
                    if (stick_axis < 0) {
                        m_snapturn_left = true;
                    }
                    m_snapturn_on_frame = true;
                    m_was_snapturn_run_on_input = true;
                }
            }
        }
        else {
            if (dpad_method == RIGHT_JOYSTICK) {
                if (glm::abs(true_left_joystick_axis.x) < snapturn_deadzone) {
                    m_was_snapturn_run_on_input = false;
                } else {
                    state->Gamepad.sThumbLY = 0;
                    state->Gamepad.sThumbLX = 0;
                }
            }
            else {
                if (glm::abs(right_joystick_axis.x) < snapturn_deadzone) {
                    m_was_snapturn_run_on_input = false;
                } else {
                    state->Gamepad.sThumbRY = 0;
                    state->Gamepad.sThumbRX = 0;
                }
            }
        }
    }
    
    // Do it again after all the VR buttons have been spoofed
    update_imgui_state_from_xinput_state(*state, true);
}

void VR::on_xinput_set_state(uint32_t* retval, uint32_t user_index, XINPUT_VIBRATION* vibration) {
    ZoneScopedN(__FUNCTION__);

    if (user_index != m_lowest_xinput_user_index) {
        return;
    }

    if (!is_using_controllers()) {
        return;
    }

    const auto left_amplitude = ((float)vibration->wLeftMotorSpeed / 65535.0f) * 5.0f;
    const auto right_amplitude = ((float)vibration->wRightMotorSpeed / 65535.0f) * 5.0f;

    if (left_amplitude > 0.0f) {
        trigger_haptic_vibration(0.0f, 0.1f, 1.0f, left_amplitude, get_left_joystick());
    }

    if (right_amplitude > 0.0f) {
        trigger_haptic_vibration(0.0f, 0.1f, 1.0f, right_amplitude, get_right_joystick());
    }
}

// Allows imgui navigation to work with the controllers
void VR::update_imgui_state_from_xinput_state(XINPUT_STATE& state, bool is_vr_controller) {
    ZoneScopedN(__FUNCTION__);

    bool is_using_this_controller = true;

    const auto is_using_vr_controller_recently = is_using_controllers_within(std::chrono::seconds(1));
    const auto is_gamepad = !is_vr_controller;

    if (is_vr_controller && !is_using_vr_controller_recently) {
        is_using_this_controller = false;
    } else if (is_gamepad && is_using_vr_controller_recently) { // dont allow gamepad navigation if using vr controllers
        is_using_this_controller = false;
    }

    // L3 + R3 to open the menu
    if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0 && (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0) {
        if (!FrameworkConfig::get()->is_enable_l3_r3_toggle()) {
            return;
        }

        bool should_open = true;

        const auto now = std::chrono::steady_clock::now();

        if (FrameworkConfig::get()->is_l3_r3_long_press() && !g_framework->is_drawing_ui()) {
            if (!m_xinput_context.menu_longpress_begin_held) {
                m_xinput_context.menu_longpress_begin = now;
            }

            m_xinput_context.menu_longpress_begin_held = true;
            should_open = (now - m_xinput_context.menu_longpress_begin) >= std::chrono::seconds(1);
        } else {
            m_xinput_context.menu_longpress_begin_held = false;
        }

        if (should_open && now - m_last_xinput_l3_r3_menu_open >= std::chrono::seconds(1)) {
            m_last_xinput_l3_r3_menu_open = std::chrono::steady_clock::now();
            g_framework->set_draw_ui(!g_framework->is_drawing_ui());

            state.Gamepad.wButtons &= ~(XINPUT_GAMEPAD_LEFT_THUMB | XINPUT_GAMEPAD_RIGHT_THUMB); // so input doesn't go through to the game
        }
    } else if (is_using_this_controller) {
        m_xinput_context.headlocked_begin_held = false;
        m_xinput_context.menu_longpress_begin_held = false;
    }

    // We need to adjust the stick values based on the selected movement orientation value if the user wants to do this
    // It will either need to be adjusted by the HMD rotation or one of the controllers.
    if (is_using_this_controller && m_movement_orientation->value() != VR::AimMethod::GAME && m_movement_orientation->value() != m_aim_method->value()) {
        const auto left_stick_og = glm::vec2((float)state.Gamepad.sThumbLX, (float)state.Gamepad.sThumbLY );
        const auto left_stick_magnitude = glm::clamp(glm::length(left_stick_og), -32767.0f, 32767.0f);
        const auto left_stick = glm::normalize(left_stick_og);
        const auto left_stick_angle = glm::atan2(left_stick.y, left_stick.x);

        if (this->is_controller_movement_enabled() && is_vr_controller) {
            const auto controller_index = this->get_movement_orientation() == VR::AimMethod::LEFT_CONTROLLER ? get_left_controller_index() : get_right_controller_index();
            const auto controller_rotation = utility::math::flatten(m_rotation_offset * glm::quat{get_rotation(controller_index)});
            const auto controller_forward = controller_rotation * glm::vec3(0.0f, 0.0f, 1.0f);
            const auto controller_angle = glm::atan2(controller_forward.x, controller_forward.z);

            // Normalize angles to [0, 2Ï€]
            const auto normalized_left_stick_angle = left_stick_angle < 0 ? left_stick_angle + 2 * glm::pi<float>() : left_stick_angle;
            const auto normalized_controller_angle = controller_angle < 0 ? controller_angle + 2 * glm::pi<float>() : controller_angle;

            // Add the angles together
            const auto new_left_stick_angle = utility::math::fix_angle(normalized_left_stick_angle + normalized_controller_angle);
            const auto new_left_stick = glm::vec2(glm::cos(new_left_stick_angle), glm::sin(new_left_stick_angle)) * left_stick_magnitude;

            state.Gamepad.sThumbLX = (int16_t)new_left_stick.x;
            state.Gamepad.sThumbLY = (int16_t)new_left_stick.y;
        } else { // Fallback to head aim
            // Rotate the left stick by the HMD rotation
            const auto hmd_rotation = utility::math::flatten(m_rotation_offset * glm::quat{get_rotation(0)});
            const auto hmd_forward = hmd_rotation * glm::vec3(0.0f, 0.0f, 1.0f);
            const auto hmd_angle = glm::atan2(hmd_forward.x, hmd_forward.z);

            // Normalize angles to [0, 2Ï€]
            const auto normalized_left_stick_angle = left_stick_angle < 0 ? left_stick_angle + 2 * glm::pi<float>() : left_stick_angle;
            const auto normalized_hmd_angle = hmd_angle < 0 ? hmd_angle + 2 * glm::pi<float>() : hmd_angle;

            // Add the angles together
            const auto new_left_stick_angle = utility::math::fix_angle(normalized_left_stick_angle + normalized_hmd_angle);
            const auto new_left_stick = glm::vec2{glm::cos(new_left_stick_angle), glm::sin(new_left_stick_angle)} * left_stick_magnitude;

            state.Gamepad.sThumbLX = (int16_t)new_left_stick.x;
            state.Gamepad.sThumbLY = (int16_t)new_left_stick.y;
        }
    }

    if (!g_framework->is_drawing_ui()) {
        m_rt_modifier.draw = false;
        return;
    }

    if (!is_using_this_controller) {
        return;
    }

    // Gamepad navigation when the menu is open
    m_xinput_context.enqueue(is_vr_controller, state, [this](const XINPUT_STATE& state, bool is_vr_controller){
        static auto last_time = std::chrono::high_resolution_clock::now();

        const auto delta = std::chrono::duration<float>((std::chrono::high_resolution_clock::now() - last_time)).count();
        last_time = std::chrono::high_resolution_clock::now();

        auto& io = ImGui::GetIO();
        auto& gamepad = state.Gamepad;

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.BackendFlags |= ImGuiBackendFlags_HasGamepad;

        // Headlocked aim toggle
        if (!FrameworkConfig::get()->is_l3_r3_long_press()) {
            if ((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) != 0 && (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) != 0) {
                if (!m_xinput_context.headlocked_begin_held) {
                    m_xinput_context.headlocked_begin = std::chrono::steady_clock::now();
                    m_xinput_context.headlocked_begin_held = true;
                }
            } else {
                m_xinput_context.headlocked_begin_held = false;
            }
        }

        // Now that we're drawing the UI, check for special button combos the user can use as shortcuts
        // like recenter view, set standing origin, camera offset modification, etc.
        m_rt_modifier.draw = gamepad.bRightTrigger >= 128;

        if (!m_rt_modifier.draw) {
            m_rt_modifier.page = 0;
            m_rt_modifier.was_moving_left = false;
            m_rt_modifier.was_moving_right = false;
        }

        // If user holding down RT with menu open...
        if (m_rt_modifier.draw) {
            // Camera offset modification
            const auto right_ratio = (float)gamepad.sThumbLX / 32767.0f;
            const auto forward_ratio = (float)gamepad.sThumbLY / 32767.0f;
            const auto up_ratio = (float)gamepad.sThumbRY / 32767.0f;

            if (right_ratio <= -0.25f || right_ratio >= 0.25f) {
                const auto right_offset = right_ratio * delta * 150.0f;
                m_camera_right_offset->value() += right_offset;
            }

            if (forward_ratio <= -0.25f || forward_ratio >= 0.25f) {
                const auto forward_offset = forward_ratio * delta * 150.0f;
                m_camera_forward_offset->value() += forward_offset;
            }

            if (up_ratio <= -0.25f || up_ratio >= 0.25f) {
                const auto up_offset = up_ratio * delta * 150.0f;
                m_camera_up_offset->value() += up_offset;
            }

            if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) {
                if (!m_rt_modifier.was_moving_left) {
                    if (m_rt_modifier.page > 0) {
                        m_rt_modifier.page--;
                    } else {
                        m_rt_modifier.page = m_rt_modifier.num_pages - 1;
                    }

                    m_rt_modifier.was_moving_left = true;
                }
            } else {
                m_rt_modifier.was_moving_left = false;
            }

            if (gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) {
                if (!m_rt_modifier.was_moving_right) {
                    if (m_rt_modifier.page < m_rt_modifier.num_pages - 1) {
                        m_rt_modifier.page++;
                    } else {
                        m_rt_modifier.page = 0;
                    }

                    m_rt_modifier.was_moving_right = true;
                }
            } else {
                m_rt_modifier.was_moving_right = false;
            }

            // Reset camera offset
            switch (m_rt_modifier.page) {
            case 2:
                if (gamepad.wButtons & XINPUT_GAMEPAD_B) {
                    save_camera(2);
                }

                // Recenter
                if (gamepad.wButtons & XINPUT_GAMEPAD_Y) {
                    save_camera(1);
                }

                // Reset standing origin
                if (gamepad.wButtons & XINPUT_GAMEPAD_X) {
                    save_camera(0);
                }
                break;
            
            case 1:
                if (gamepad.wButtons & XINPUT_GAMEPAD_B) {
                    load_camera(2);
                }

                // Recenter
                if (gamepad.wButtons & XINPUT_GAMEPAD_Y) {
                    load_camera(1);
                }

                // Reset standing origin
                if (gamepad.wButtons & XINPUT_GAMEPAD_X) {
                    load_camera(0);
                }

                break; 
            case 0:
            default:
                if (gamepad.wButtons & XINPUT_GAMEPAD_B) {
                    m_camera_right_offset->value() = 0.0f;
                    m_camera_forward_offset->value() = 0.0f;
                    m_camera_up_offset->value() = 0.0f;
                }

                // Recenter
                if (gamepad.wButtons & XINPUT_GAMEPAD_Y) {
                    this->recenter_view();
                }

                // Reset standing origin
                if (gamepad.wButtons & XINPUT_GAMEPAD_X) {
                    this->set_standing_origin(this->get_position(0));
                }
                
                break;
            }

            // ignore everything else
            return;
        }

        // From imgui_impl_win32.cpp
        #define IM_SATURATE(V)                      (V < 0.0f ? 0.0f : V > 1.0f ? 1.0f : V)
        #define MAP_BUTTON(KEY_NO, BUTTON_ENUM)     { io.AddKeyEvent(KEY_NO, (gamepad.wButtons & BUTTON_ENUM) != 0); }
        #define MAP_ANALOG(KEY_NO, VALUE, V0, V1)   { float vn = (float)(VALUE - V0) / (float)(V1 - V0); io.AddKeyAnalogEvent(KEY_NO, vn > 0.10f, IM_SATURATE(vn)); }

        MAP_BUTTON(ImGuiKey_GamepadStart,           XINPUT_GAMEPAD_START);
        MAP_BUTTON(ImGuiKey_GamepadBack,            XINPUT_GAMEPAD_BACK);
        MAP_BUTTON(ImGuiKey_GamepadFaceLeft,        XINPUT_GAMEPAD_X);
        MAP_BUTTON(ImGuiKey_GamepadFaceRight,       XINPUT_GAMEPAD_B);
        MAP_BUTTON(ImGuiKey_GamepadFaceUp,          XINPUT_GAMEPAD_Y);
        MAP_BUTTON(ImGuiKey_GamepadFaceDown,        XINPUT_GAMEPAD_A);
        MAP_BUTTON(ImGuiKey_GamepadDpadLeft,        XINPUT_GAMEPAD_DPAD_LEFT);
        MAP_BUTTON(ImGuiKey_GamepadDpadRight,       XINPUT_GAMEPAD_DPAD_RIGHT);
        MAP_BUTTON(ImGuiKey_GamepadDpadUp,          XINPUT_GAMEPAD_DPAD_UP);
        MAP_BUTTON(ImGuiKey_GamepadDpadDown,        XINPUT_GAMEPAD_DPAD_DOWN);
        MAP_ANALOG(ImGuiKey_GamepadL2,              gamepad.bLeftTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD, 255);
        MAP_ANALOG(ImGuiKey_GamepadR2,              gamepad.bRightTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD, 255);
        MAP_BUTTON(ImGuiKey_GamepadL3,              XINPUT_GAMEPAD_LEFT_THUMB);
        MAP_BUTTON(ImGuiKey_GamepadR3,              XINPUT_GAMEPAD_RIGHT_THUMB);

        if (!is_vr_controller) {
            MAP_ANALOG(ImGuiKey_GamepadLStickLeft,      gamepad.sThumbLX, -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, -32768);
            MAP_ANALOG(ImGuiKey_GamepadLStickRight,     gamepad.sThumbLX, +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, +32767);
            MAP_ANALOG(ImGuiKey_GamepadLStickUp,        gamepad.sThumbLY, +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, +32767);
            MAP_ANALOG(ImGuiKey_GamepadLStickDown,      gamepad.sThumbLY, -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, -32768);
            MAP_BUTTON(ImGuiKey_GamepadL1,              XINPUT_GAMEPAD_LEFT_SHOULDER);
            MAP_BUTTON(ImGuiKey_GamepadR1,              XINPUT_GAMEPAD_RIGHT_SHOULDER);
        } else {
            // Map it to the dpad
            const auto left_stick_left = gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * 2;
            if (m_xinput_context.vr.left_stick_left.was_pressed(left_stick_left)) {
                io.AddKeyEvent(ImGuiKey_GamepadDpadLeft, true);
            } else if (!left_stick_left) {
                io.AddKeyEvent(ImGuiKey_GamepadDpadLeft, false);
            }

            const auto left_stick_right = gamepad.sThumbLX > +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * 2;
            if (m_xinput_context.vr.left_stick_right.was_pressed(left_stick_right)) {
                io.AddKeyEvent(ImGuiKey_GamepadDpadRight, true);
            } else if (!left_stick_right) {
                io.AddKeyEvent(ImGuiKey_GamepadDpadRight, false);
            }

            const auto left_stick_up = gamepad.sThumbLY > +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * 2;
            if (m_xinput_context.vr.left_stick_up.was_pressed(left_stick_up)) {
                io.AddKeyEvent(ImGuiKey_GamepadDpadUp, true);
            } else if (!left_stick_up) {
                io.AddKeyEvent(ImGuiKey_GamepadDpadUp, false);
            }

            const auto left_stick_down = gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE * 2;
            if (m_xinput_context.vr.left_stick_down.was_pressed(left_stick_down)) {
                io.AddKeyEvent(ImGuiKey_GamepadDpadDown, true);
            } else if (!left_stick_down) {
                io.AddKeyEvent(ImGuiKey_GamepadDpadDown, false);
            }
        }

        MAP_ANALOG(ImGuiKey_GamepadRStickLeft,      gamepad.sThumbRX, -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, -32768);
        MAP_ANALOG(ImGuiKey_GamepadRStickRight,     gamepad.sThumbRX, +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, +32767);
        MAP_ANALOG(ImGuiKey_GamepadRStickUp,        gamepad.sThumbRY, +XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, +32767);
        MAP_ANALOG(ImGuiKey_GamepadRStickDown,      gamepad.sThumbRY, -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE, -32768);
    });

    // Zero out the state so we don't send input to the game.
    ZeroMemory(&state.Gamepad, sizeof(XINPUT_GAMEPAD));
}

void VR::on_pre_engine_tick(sdk::UGameEngine* engine, float delta) {
    ZoneScopedN(__FUNCTION__);

    m_cvar_manager->on_pre_engine_tick(engine, delta);
    m_last_engine_tick = std::chrono::steady_clock::now();

    if (!get_runtime()->loaded || !is_hmd_active()) {
        return;
    }

    SPDLOG_INFO_ONCE("VR: Pre-engine tick");

    m_render_target_pool_hook->on_pre_engine_tick(engine, delta);

    update_statistics_overlay(engine);

    // Dont update action states on AFR frames
    // TODO: fix this for actual AFR, but we dont really care about pure AFR since synced beats it most of the time
    if (m_fake_stereo_hook != nullptr && !m_fake_stereo_hook->is_ignoring_next_viewport_draw()) {
        update_action_states();
    }
}

void VR::on_pre_calculate_stereo_view_offset(void* stereo_device, const int32_t view_index, Rotator<float>* view_rotation, 
                                             const float world_to_meters, Vector3f* view_location, bool is_double)
{
    if (!is_hmd_active()) {
        m_camera_freeze.position_wants_freeze = false;
        m_camera_freeze.rotation_wants_freeze = false;
        return;
    }

    const auto now = std::chrono::high_resolution_clock::now();
    const auto delta = std::chrono::duration<float, std::chrono::seconds::period>(now - m_last_lerp_update).count();

    Rotator<double>* view_rotation_double = (Rotator<double>*)view_rotation;
    Vector3d* view_location_double = (Vector3d*)view_location;

    glm::vec3 target_rotation = is_double ? glm::vec3{*(glm::vec<3, double>*)view_rotation_double} : *(glm::vec<3, float>*)view_rotation;

    const auto should_lerp_pitch = m_lerp_camera_pitch->value();
    const auto should_lerp_yaw = m_lerp_camera_yaw->value();
    const auto should_lerp_roll = m_lerp_camera_roll->value();

    auto lerp_angle = [](auto a, auto b, auto t) {
        const auto diff = b - a;
        if constexpr (std::is_same_v<decltype(a), double>) {
            if (diff > 180.0) {
                b -= 360.0;
            } else if (diff < -180.0) {
                b += 360.0;
            }
        } else {
            if (diff > 180.0f) {
                b -= 360.0f;
            } else if (diff < -180.0f) {
                b += 360.0f;
            }
        }

        return glm::lerp(a, b, t);
    };

    const auto lerp_t = m_lerp_camera_speed->value() * delta;

    if (should_lerp_pitch) {
        if (is_double) {
            view_rotation_double->pitch = lerp_angle((double)m_camera_lerp.last_rotation.x, (double)target_rotation.x, (double)lerp_t);
        } else {
            view_rotation->pitch = lerp_angle(m_camera_lerp.last_rotation.x, target_rotation.x, lerp_t);
        }
    }

    if (should_lerp_yaw) {
        if (is_double) {
            view_rotation_double->yaw = lerp_angle((double)m_camera_lerp.last_rotation.y, (double)target_rotation.y, (double)lerp_t);
        } else {
            view_rotation->yaw = lerp_angle(m_camera_lerp.last_rotation.y, target_rotation.y, lerp_t);
        }
    }

    if (should_lerp_roll) {
        if (is_double) {
            view_rotation_double->roll = lerp_angle((double)m_camera_lerp.last_rotation.z, (double)target_rotation.z, (double)lerp_t);
        } else {
            view_rotation->roll = lerp_angle(m_camera_lerp.last_rotation.z, target_rotation.z, lerp_t);
        }
    }

    if (is_double) {
        m_camera_lerp.last_rotation = glm::vec3{ (float)view_rotation_double->pitch, (float)view_rotation_double->yaw, (float)view_rotation_double->roll };
    } else {
        m_camera_lerp.last_rotation = glm::vec3{ view_rotation->pitch, view_rotation->yaw, view_rotation->roll };
    }

    m_last_lerp_update = std::chrono::high_resolution_clock::now();

    if (m_camera_freeze.position_wants_freeze) {
        if (is_double) {
            m_camera_freeze.position = glm::vec3{ (float)view_location_double->x, (float)view_location_double->y, (float)view_location_double->z };
        } else {
            m_camera_freeze.position = glm::vec3{ view_location->x, view_location->y, view_location->z };
        }

        m_camera_freeze.position_wants_freeze = false;
        m_camera_freeze.position_frozen = true;
    }

    if (m_camera_freeze.rotation_wants_freeze) {
        if (is_double) {
            m_camera_freeze.rotation = glm::vec3{ (float)view_rotation_double->pitch, (float)view_rotation_double->yaw, (float)view_rotation_double->roll };
        } else {
            m_camera_freeze.rotation = glm::vec3{ view_rotation->pitch, view_rotation->yaw, view_rotation->roll };
        }

        m_camera_freeze.rotation_wants_freeze = false;
        m_camera_freeze.rotation_frozen = true;
    }

    if (m_camera_freeze.position_frozen) {
        if (is_double) {
            view_location_double->x = m_camera_freeze.position.x;
            view_location_double->y = m_camera_freeze.position.y;
            view_location_double->z = m_camera_freeze.position.z;
        } else {
            view_location->x = m_camera_freeze.position.x;
            view_location->y = m_camera_freeze.position.y;
            view_location->z = m_camera_freeze.position.z;
        }
    }

    if (m_camera_freeze.rotation_frozen) {
        if (is_double) {
            view_rotation_double->pitch = m_camera_freeze.rotation.x;
            view_rotation_double->yaw = m_camera_freeze.rotation.y;
            view_rotation_double->roll = m_camera_freeze.rotation.z;
        } else {
            view_rotation->pitch = m_camera_freeze.rotation.x;
            view_rotation->yaw = m_camera_freeze.rotation.y;
            view_rotation->roll = m_camera_freeze.rotation.z;
        }
    }
}

void VR::on_pre_viewport_client_draw(void* viewport_client, void* viewport, void* canvas){
    ZoneScopedN(__FUNCTION__);

    if (m_custom_z_near_enabled->value()) {
        SPDLOG_INFO_ONCE("Attempting to set custom z near");
        sdk::globals::get_near_clipping_plane() = m_custom_z_near->value();
    }
}

void VR::update_hmd_state(bool from_view_extensions, uint32_t frame_count) {
    ZoneScopedN(__FUNCTION__);

    std::scoped_lock _{m_reinitialize_mtx};

    auto runtime = get_runtime();
    if (m_uncap_framerate->value()) {
        sdk::set_cvar_data_float(L"Engine", L"t.MaxFPS", 500.0f);
    }

    // Allows games running in HDR mode to not have a black UI overlay
    if (m_disable_hdr_compositing->value()) {
        sdk::set_cvar_data_int(L"SlateRHIRenderer", L"r.HDR.UI.CompositeMode", 0);
    }

    if (m_disable_blur_widgets->value()) {
        if (auto val = sdk::get_cvar_int(L"Slate", L"Slate.AllowBackgroundBlurWidgets"); val && *val != 0) {
            sdk::set_cvar_int(L"Slate", L"Slate.AllowBackgroundBlurWidgets", 0);
        }
    }

    if (!is_using_afr()) {
        const auto is_hzbo_frozen_by_cvm = m_cvar_manager != nullptr && m_cvar_manager->is_hzbo_frozen_and_enabled();

        // Forcefully disable r.HZBOcclusion, it doesn't work with native stereo mode (sometimes)
        // Except when the user sets it to 1 with the CVar Manager, we need to respect that
        if (m_disable_hzbocclusion->value() && !is_hzbo_frozen_by_cvm) {
            const auto r_hzb_occlusion_value = sdk::get_cvar_int(L"Renderer", L"r.HZBOcclusion");

            // Only set it once, otherwise we'll be spamming a Set call every frame
            if (r_hzb_occlusion_value && *r_hzb_occlusion_value != 0) {
                sdk::set_cvar_int(L"Renderer", L"r.HZBOcclusion", 0);
            }
        }

        if (m_disable_instance_culling->value()) {
            const auto r_instance_culling_value = sdk::get_cvar_int(L"Renderer", L"r.InstanceCulling.OcclusionCull");

            if (r_instance_culling_value && *r_instance_culling_value != 0) {
                sdk::set_cvar_int(L"Renderer", L"r.InstanceCulling.OcclusionCull", 0);
            }
        }
    }

    if (frame_count != 0 && is_using_afr() && frame_count % 2 == 0) {
        if (runtime->is_openxr()) {
            std::scoped_lock __{ m_openxr->sync_assignment_mtx };

            const auto last_frame = (frame_count - 1) % runtimes::OpenXR::QUEUE_SIZE;
            const auto now_frame = frame_count % runtimes::OpenXR::QUEUE_SIZE;
            m_openxr->pipeline_states[now_frame] = m_openxr->pipeline_states[last_frame];
            m_openxr->pipeline_states[now_frame].frame_count = now_frame;
        } else {
            const auto last_frame = (frame_count - 1) % m_openvr->pose_queue.size();
            const auto now_frame = frame_count % m_openvr->pose_queue.size();
            m_openvr->pose_queue[now_frame] = m_openvr->pose_queue[last_frame];
        }

        // Forcefully disable motion blur because it freaks out with AFR
        sdk::set_cvar_data_int(L"Engine", L"r.DefaultFeature.MotionBlur", 0);
        return;
    }
    
    runtime->update_poses(from_view_extensions, frame_count);

    // Update the poses used for the game
    // If we used the data directly from the WaitGetPoses call, we would have to lock a different mutex and wait a long time
    // This is because the WaitGetPoses call is blocking, and we don't want to block any game logic
    if (runtime->wants_reset_origin && runtime->ready() && runtime->got_first_valid_poses) {
        std::unique_lock _{ runtime->pose_mtx };
        set_rotation_offset(glm::identity<glm::quat>());
        m_standing_origin = get_position_unsafe(vr::k_unTrackedDeviceIndex_Hmd);

        runtime->wants_reset_origin = false;
    }

    runtime->update_matrices(m_nearz, m_farz);

    runtime->got_first_poses = true;
}

void VR::update_action_states() {
    ZoneScopedN(__FUNCTION__);

    std::scoped_lock _{m_actions_mtx};

    auto runtime = get_runtime();

    if (runtime == nullptr || runtime->wants_reinitialize) {
        return;
    }

    static bool once = true;

    if (once) {
        spdlog::info("VR: Updating action states");
        once = false;
    }


    if (runtime->is_openvr()) {
        const auto start_time = std::chrono::high_resolution_clock::now();

        auto error = vr::VRInput()->UpdateActionState(&m_active_action_set, sizeof(m_active_action_set), 1);

        if (error != vr::VRInputError_None) {
            spdlog::error("VRInput failed to update action state: {}", (uint32_t)error);
        }

        const auto end_time = std::chrono::high_resolution_clock::now();
        const auto time_delta = end_time - start_time;

        m_last_input_delay = time_delta;
        m_avg_input_delay = (m_avg_input_delay + time_delta) / 2;

        if ((end_time - start_time) >= std::chrono::milliseconds(30)) {
            spdlog::warn("VRInput update action state took too long: {}ms", std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count());

            //reinitialize_openvr();
            runtime->wants_reinitialize = true;
        }   
    } else {
        get_runtime()->update_input();
    }

    bool actively_using_controller = false;

    if (is_any_action_down()) {
        m_last_controller_update = std::chrono::steady_clock::now();
        actively_using_controller = true;
    }

    const auto last_xinput_update_is_late = std::chrono::steady_clock::now() - m_last_xinput_update >= std::chrono::seconds(2);
    const auto should_be_spoofing = (actively_using_controller || get_runtime()->handle_pause);

    if (m_spoofed_gamepad_connection && last_xinput_update_is_late && should_be_spoofing) {
        m_spoofed_gamepad_connection = false;
    }

    if (!m_spoofed_gamepad_connection && last_xinput_update_is_late && should_be_spoofing) {
        spdlog::info("[VR] Attempting to spoof gamepad connection");
        g_framework->post_message(WM_DEVICECHANGE, 0, 0);
        g_framework->activate_window();

        m_last_xinput_spoof_sent = std::chrono::steady_clock::now();
    }

    /*if (m_recenter_view_key->is_key_down_once()) {
        recenter_view();
    }

    if (m_set_standing_key->is_key_down_once()) {
        set_standing_origin(get_position(0));
    }*/

    static bool once2 = true;

    if (once2) {
        spdlog::info("VR: Updated action states");
        once2 = false;
    }

    update_dpad_gestures();
}

void VR::update_dpad_gestures() {
    if (!is_hmd_active()) {
        return;
    }

    const auto dpad_method = get_dpad_method();
    if (dpad_method != DPadMethod::GESTURE_HEAD && dpad_method != DPadMethod::GESTURE_HEAD_RIGHT) {
        return;
    }

    const auto wanted_index = dpad_method == DPadMethod::GESTURE_HEAD ? get_left_controller_index() : get_right_controller_index();

    const auto controller_pos = glm::vec3{get_position(wanted_index)};
    const auto hmd_transform = get_hmd_transform(m_frame_count);

    // Check if controller is near HMD
    const auto dist = glm::length(controller_pos - glm::vec3{hmd_transform[3]});

    if (dist > 0.2f) {
        return;
    }

    const auto dir_to_left = glm::normalize(controller_pos - glm::vec3{hmd_transform[3]});
    const auto hmd_dir = glm::quat{glm::extractMatrixRotation(hmd_transform)} * glm::vec3{0.0f, 0.0f, 1.0f};

    const auto angle = glm::acos(glm::dot(dir_to_left, hmd_dir));

    constexpr float threshold = glm::radians(120.0f);

    if (angle > threshold) {
        return;
    }

    // Make sure the angle is to the left/right of the HMD
    if (dpad_method == DPadMethod::GESTURE_HEAD_RIGHT) {
        if (glm::cross(dir_to_left, hmd_dir).y > 0.0f) {
            return;
        }
    } else if (glm::cross(dir_to_left, hmd_dir).y < 0.0f) {
        return;
    }

    // Send a vibration pulse to the controller
    const auto chosen_joystick = dpad_method == DPadMethod::GESTURE_HEAD ? m_left_joystick : m_right_joystick;
    trigger_haptic_vibration(0.0f, 0.1f, 1.0f, 5.0f, chosen_joystick);

    std::scoped_lock _{m_dpad_gesture_state.mtx};

    const auto left_joystick_axis = get_joystick_axis(chosen_joystick);

    if (left_joystick_axis.x < -0.5f) {
        m_dpad_gesture_state.direction |= DPadGestureState::Direction::LEFT;
    } else if (left_joystick_axis.x > 0.5f) {
        m_dpad_gesture_state.direction |= DPadGestureState::Direction::RIGHT;
    } 
    
    if (left_joystick_axis.y < -0.5f) {
        m_dpad_gesture_state.direction |= DPadGestureState::Direction::DOWN;
    } else if (left_joystick_axis.y > 0.5f) {
        m_dpad_gesture_state.direction |= DPadGestureState::Direction::UP;
    }
}

void VR::on_config_load(const utility::Config& cfg, bool set_defaults) {
    ZoneScopedN(__FUNCTION__);

    for (IModValue& option : m_options) {
        option.config_load(cfg, set_defaults);
    }

    if (get_runtime() != nullptr && get_runtime()->loaded) {
        get_runtime()->on_config_load(cfg, set_defaults);

        // Run the rest of OpenXR initialization code here that depends on config values
        if (m_first_config_load) {
            m_first_config_load = false; // because the frontend can request config reloads

            if (get_runtime()->is_openxr()) {
                spdlog::info("[VR] Finishing up OpenXR initialization");
                initialize_openxr_swapchains();
            }
        }
    }

    if (m_fake_stereo_hook != nullptr) {
        m_fake_stereo_hook->on_config_load(cfg, set_defaults);
    }

    m_overlay_component.on_config_load(cfg, set_defaults);

    if (m_cvar_manager != nullptr) {
        m_cvar_manager->on_config_load(cfg, set_defaults);   
    }

    // Load camera offsets
    load_cameras();
}

void VR::on_config_save(utility::Config& cfg) {
    ZoneScopedN(__FUNCTION__);

    for (IModValue& option : m_options) {
        option.config_save(cfg);
    }

    if (m_fake_stereo_hook != nullptr) {
        m_fake_stereo_hook->on_config_save(cfg);
    }

    if (get_runtime()->loaded) {
        get_runtime()->on_config_save(cfg);
    }

    m_overlay_component.on_config_save(cfg);

    // Save camera offsets
    save_cameras();
}

void VR::load_cameras() try {
    ZoneScopedN(__FUNCTION__);

    const auto cameras_txt = Framework::get_persistent_dir("cameras.txt");

    if (std::filesystem::exists(cameras_txt)) {
        spdlog::info("[VR] Loading camera offsets from {}", cameras_txt.string());

        utility::Config cfg{cameras_txt.string()};

        for (auto i = 0; i < m_camera_datas.size(); i++) {
            auto& data = m_camera_datas[i];

            if (auto offs = cfg.get<float>(std::format("camera_right_offset{}", i))) {
                data.offset.x = *offs;
            }

            if (auto offs = cfg.get<float>(std::format("camera_up_offset{}", i))) {
                data.offset.y = *offs;
            }

            if (auto offs = cfg.get<float>(std::format("camera_forward_offset{}", i))) {
                data.offset.z = *offs;
            }

            if (auto scale = cfg.get<float>(std::format("world_scale{}", i))) {
                data.world_scale = *scale;
            }

            if (auto decoupled_pitch = cfg.get<bool>(std::format("decoupled_pitch{}", i))) {
                data.decoupled_pitch = *decoupled_pitch;
            }

            if (auto decoupled_pitch_ui_adjust = cfg.get<bool>(std::format("decoupled_pitch_ui_adjust{}", i))) {
                data.decoupled_pitch_ui_adjust = *decoupled_pitch_ui_adjust;
            }
        }
    }
} catch(...) {
    spdlog::error("[VR] Failed to load camera offsets");
}

void VR::load_camera(int index) {
    ZoneScopedN(__FUNCTION__);

    if (index < 0 || index >= m_camera_datas.size()) {
        return;
    }

    const auto& data = m_camera_datas[index];

    m_camera_right_offset->value() = data.offset.x;
    m_camera_up_offset->value() = data.offset.y;
    m_camera_forward_offset->value() = data.offset.z;
    m_world_scale->value() = data.world_scale;
    m_decoupled_pitch->value() = data.decoupled_pitch;
    m_decoupled_pitch_ui_adjust->value() = data.decoupled_pitch_ui_adjust;
}

void VR::save_camera(int index) {
    ZoneScopedN(__FUNCTION__);

    if (index < 0 || index >= m_camera_datas.size()) {
        return;
    }

    auto& data = m_camera_datas[index];

    data.offset = {
        m_camera_right_offset->value(),
        m_camera_up_offset->value(),
        m_camera_forward_offset->value()
    };

    data.world_scale = m_world_scale->value();
    data.decoupled_pitch = m_decoupled_pitch->value();
    data.decoupled_pitch_ui_adjust = m_decoupled_pitch_ui_adjust->value();

    save_cameras();
}

void VR::save_cameras() try {
    ZoneScopedN(__FUNCTION__);

    const auto cameras_txt = Framework::get_persistent_dir("cameras.txt");

    spdlog::info("[VR] Saving camera offsets to {}", cameras_txt.string());

    utility::Config cfg{cameras_txt.string()};

    for (auto i = 0; i < m_camera_datas.size(); i++) {
        const auto& data = m_camera_datas[i];
        cfg.set<float>(std::format("camera_right_offset{}", i), data.offset.x);
        cfg.set<float>(std::format("camera_up_offset{}", i), data.offset.y);
        cfg.set<float>(std::format("camera_forward_offset{}", i), data.offset.z);
        cfg.set<float>(std::format("world_scale{}", i), m_camera_datas[i].world_scale);
        cfg.set<bool>(std::format("decoupled_pitch{}", i), m_camera_datas[i].decoupled_pitch);
        cfg.set<bool>(std::format("decoupled_pitch_ui_adjust{}", i), m_camera_datas[i].decoupled_pitch_ui_adjust);
    }

    cfg.save(cameras_txt.string());
} catch(...) {
    spdlog::error("[VR] Failed to save camera offsets");
}


void VR::on_pre_imgui_frame() {
    ZoneScopedN(__FUNCTION__);

    m_xinput_context.update();

    if (!get_runtime()->ready()) {
        return;
    }

    if (!m_disable_overlay) {
        m_overlay_component.on_pre_imgui_frame();
    }
}

void VR::handle_keybinds() {
    ZoneScopedN(__FUNCTION__);

    if (m_keybind_recenter->is_key_down_once()) {
        recenter_view();
    }

    if (m_keybind_recenter_horizon->is_key_down_once()) {
        recenter_horizon();
    }

    if (m_keybind_load_camera_0->is_key_down_once()) {
        load_camera(0);
    }

    if (m_keybind_load_camera_1->is_key_down_once()) {
        load_camera(1);
    }

    if (m_keybind_load_camera_2->is_key_down_once()) {
        load_camera(2);
    }

    if (m_keybind_set_standing_origin->is_key_down_once()) {
        m_standing_origin = get_position(0);
    }

    if (m_keybind_toggle_2d_screen->is_key_down_once()) {
        m_2d_screen_mode->toggle();
    }

    if (m_keybind_disable_vr->is_key_down_once()) {
        m_disable_vr = !m_disable_vr; // definitely should not be persistent
    }

    // The Slate UI
    if (m_keybind_toggle_gui->is_key_down_once()) {
        m_enable_gui->toggle();
    }
}

void VR::on_frame() {
    ZoneScopedN(__FUNCTION__);

    m_cvar_manager->on_frame();
    handle_keybinds();

    if (!get_runtime()->ready()) {
        return;
    }

    const auto now = std::chrono::steady_clock::now();
    const auto is_allowed_draw_window = now - m_last_xinput_update < std::chrono::seconds(2);

    if (!is_allowed_draw_window) {
        m_rt_modifier.draw = false;
    }

    if (is_allowed_draw_window && m_xinput_context.headlocked_begin_held && !FrameworkConfig::get()->is_l3_r3_long_press()) {
        const auto rt_size = g_framework->get_rt_size();

        ImGui::Begin("AimMethod Notification", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav);

        ImGui::Text("Continue holding down L3 + R3 to toggle aim method");

        if (std::chrono::steady_clock::now() - m_xinput_context.headlocked_begin >= std::chrono::seconds(1)) {
            if (m_aim_method->value() == VR::AimMethod::GAME) {
                m_aim_method->value() = m_previous_aim_method;
            } else {
                m_aim_method->value() = VR::AimMethod::GAME; // turns it off
            }

            m_xinput_context.headlocked_begin_held = false;
        } else {
            if (m_aim_method->value() != VR::AimMethod::GAME) {
                m_previous_aim_method = (VR::AimMethod)m_aim_method->value();
            } else if (m_previous_aim_method == VR::AimMethod::GAME) {
                m_previous_aim_method = VR::AimMethod::HEAD; // so it will at least be something
            }
        }

        const auto window_size = ImGui::GetWindowSize();

        const auto centered_x = (rt_size.x / 2) - (window_size.x / 2);
        const auto centered_y = (rt_size.y / 2) - (window_size.y / 2);
        ImGui::SetWindowPos(ImVec2(centered_x, centered_y), ImGuiCond_Always);

        ImGui::End();
    }

    if (m_rt_modifier.draw) {
        const auto rt_size = g_framework->get_rt_size();

        ImGui::Begin("RT Modifier Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoNav);
        
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "RT + Left Stick: Camera left/right/forward/back");
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "RT + Right Stick: Camera up/down");
        
        ImGui::Text("Page: %d", m_rt_modifier.page + 1);
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "DPad Left: Previous page | DPad Right: Next page");

        switch (m_rt_modifier.page) {
        case 2:
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "RT + B: Save Camera 2");
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "RT + Y: Save Camera 1");
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "RT + X: Save Camera 0");
            break;

        case 1:
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "RT + B: Load Camera 2");
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "RT + Y: Load Camera 1");
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "RT + X: Load Camera 0");
            break;

        case 0:
        default:
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "RT + B: Reset camera offset");
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "RT + Y: Recenter view");
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "RT + X: Reset standing origin");
            m_rt_modifier.page = 0;
            break;
        }

        const auto window_size = ImGui::GetWindowSize();

        const auto centered_x = (rt_size.x / 2) - (window_size.x / 2);
        const auto centered_y = (rt_size.y / 2) - (window_size.y / 2);
        ImGui::SetWindowPos(ImVec2(centered_x, centered_y), ImGuiCond_Always);
        ImGui::End();
    }
}

void VR::on_present() {
    ZoneScopedN(__FUNCTION__);

    m_present_thread_id = GetCurrentThreadId();

    utility::ScopeGuard _guard {[&]() {
        if (!is_using_afr() || (m_render_frame_count + 1) % 2 == m_left_eye_interval) {
            SetEvent(m_present_finished_event);
        }

        m_last_frame_count = m_render_frame_count;
    }};

    m_frame_count = get_runtime()->internal_render_frame_count;

    if (!is_using_afr() || m_render_frame_count % 2 == m_left_eye_interval) {
        ResetEvent(m_present_finished_event);
    }

    auto runtime = get_runtime();

    if (!runtime->loaded) {
        m_fake_stereo_hook->on_frame(); // Just let all the hooks engage, whatever.
        return;
    }

    runtime->consume_events(nullptr);

    m_fake_stereo_hook->on_frame();

    auto openvr = get_runtime<runtimes::OpenVR>();

    if (runtime->is_openvr()) {
        if (openvr->got_first_poses) {
            const auto hmd_activity = openvr->hmd->GetTrackedDeviceActivityLevel(vr::k_unTrackedDeviceIndex_Hmd);
            auto hmd_active = hmd_activity == vr::k_EDeviceActivityLevel_UserInteraction || hmd_activity == vr::k_EDeviceActivityLevel_UserInteraction_Timeout;

            if (hmd_active) {
                openvr->last_hmd_active_time = std::chrono::system_clock::now();
            }

            const auto now = std::chrono::system_clock::now();

            if (now - openvr->last_hmd_active_time <= std::chrono::seconds(5)) {
                hmd_active = true;
            }

            openvr->is_hmd_active = hmd_active;

            // upon headset re-entry, reinitialize OpenVR
            if (openvr->is_hmd_active && !openvr->was_hmd_active) {
                openvr->wants_reinitialize = true;
            }

            openvr->was_hmd_active = openvr->is_hmd_active;

            if (!is_hmd_active()) {
                return;
            }
        } else {
            openvr->is_hmd_active = true; // We need to force out an initial WaitGetPoses call
            openvr->was_hmd_active = true;
        }
    }

    // attempt to fix crash when reinitializing openvr
    std::scoped_lock _{m_openvr_mtx};
    m_submitted = false;

    const auto renderer = g_framework->get_renderer_type();
    vr::EVRCompositorError e = vr::EVRCompositorError::VRCompositorError_None;

    const auto is_left_eye_frame = is_using_afr() ? (m_render_frame_count % 2 == m_left_eye_interval) : true;

    if (is_left_eye_frame && runtime->get_synchronize_stage() == VRRuntime::SynchronizeStage::LATE) {
        const auto had_sync = runtime->got_first_sync;
        runtime->synchronize_frame();

        if (!runtime->got_first_poses || !had_sync) {
            update_hmd_state();
        }
    }

    if (renderer == Framework::RendererType::D3D11) {
        // if we don't do this then D3D11 OpenXR freezes for some reason.
        if (!runtime->got_first_sync) {
            SPDLOG_INFO_EVERY_N_SEC(1, "Attempting to sync!");
            if (runtime->get_synchronize_stage() == VRRuntime::SynchronizeStage::LATE) {
                runtime->synchronize_frame();
            }

            update_hmd_state();
        }

        m_is_d3d12 = false;
        e = m_d3d11.on_frame(this);
    } else if (renderer == Framework::RendererType::D3D12) {
        m_is_d3d12 = true;
        e = m_d3d12.on_frame(this);
    }

    // force a waitgetposes call to fix this...
    if (e == vr::EVRCompositorError::VRCompositorError_AlreadySubmitted && runtime->is_openvr()) {
        openvr->got_first_poses = false;
        openvr->needs_pose_update = true;
    }

    if (m_submitted) {
        if (m_submitted) {
            if (!m_disable_overlay) {
                m_overlay_component.on_post_compositor_submit();
            }

            if (runtime->is_openvr()) {
                //vr::VRCompositor()->SetExplicitTimingMode(vr::VRCompositorTimingMode_Explicit_ApplicationPerformsPostPresentHandoff);
                //vr::VRCompositor()->PostPresentHandoff();
            }
        }

        //runtime->needs_pose_update = true;
        m_submitted = false;

        // On the first ever submit, we need to activate the window and set the mouse to the center
        // so the user doesn't have to click on the window to get input.
        if (m_first_submit) {
            m_first_submit = false;

            // for some reason this doesn't work if called directly from here
            // so we have to do it in a separate thread
            std::thread worker([]() {
                g_framework->activate_window();
                g_framework->set_mouse_to_center();
                spdlog::info("Finished first submit from worker thread!");
            });
            worker.detach();
        }
    }
}

void VR::on_post_present() {
    FrameMarkNamed("Present");
    ZoneScopedN(__FUNCTION__);

    const auto is_same_frame = m_render_frame_count > 0 && m_render_frame_count == m_frame_count;

    m_render_frame_count = m_frame_count;

    auto runtime = get_runtime();

    if (!get_runtime()->loaded) {
        return;
    }

    std::scoped_lock _{m_openvr_mtx};

    if (!m_is_d3d12) {
        m_d3d11.on_post_present(this);
    } else {
        m_d3d12.on_post_present(this);
    }

    detect_controllers();

    const auto is_left_eye_frame = is_using_afr() ? (is_same_frame || (m_render_frame_count % 2 == m_left_eye_interval)) : true;

    if (is_left_eye_frame) {
        if (runtime->get_synchronize_stage() == VRRuntime::SynchronizeStage::VERY_LATE || !runtime->got_first_sync) {
            const auto had_sync = runtime->got_first_sync;
            runtime->synchronize_frame();

            if (!runtime->got_first_poses || !had_sync) {
                update_hmd_state();
            }
        }

        if (runtime->is_openxr() && runtime->ready() && runtime->get_synchronize_stage() > VRRuntime::SynchronizeStage::EARLY) {
            if (!m_openxr->frame_began) {
                m_openxr->begin_frame();
            }
        }
    }

    if (runtime->wants_reinitialize) {
        std::scoped_lock _{m_reinitialize_mtx};

        if (runtime->is_openvr()) {
            m_openvr->wants_reinitialize = false;
            reinitialize_openvr();
        } else if (runtime->is_openxr()) {
            m_openxr->wants_reinitialize = false;
            reinitialize_openxr();
        }
    }
}

uint32_t VR::get_hmd_width() const {
    if (m_2d_screen_mode->value()) {
        if (get_runtime()->is_openxr()) {
            return g_framework->get_rt_size().x * m_openxr->resolution_scale->value();
        }

        return g_framework->get_rt_size().x;
    }

    if (m_extreme_compat_mode->value()) {
        return g_framework->get_rt_size().x;
    }

    return std::max<uint32_t>(get_runtime()->get_width(), 128);
}

uint32_t VR::get_hmd_height() const {
    if (m_2d_screen_mode->value()) {
        if (get_runtime()->is_openxr()) {
            return g_framework->get_rt_size().y * m_openxr->resolution_scale->value();
        }

        return g_framework->get_rt_size().y;
    }

    if (m_extreme_compat_mode->value()) {
        return g_framework->get_rt_size().y;
    }

    return std::max<uint32_t>(get_runtime()->get_height(), 128);
}

void VR::on_draw_sidebar_entry(std::string_view name) {
    const auto hash = utility::hash(name.data());

    // Draw the ui thats always drawn first.
    on_draw_ui();

    /*const auto made_child = ImGui::BeginChild("VRChild", ImVec2(0, 0), true, ImGuiWindowFlags_::ImGuiWindowFlags_NavFlattened);

    utility::ScopeGuard sg([made_child]() {
        if (made_child) {
            ImGui::EndChild();
        }
    });*/

    enum SelectedPage {
        PAGE_RUNTIME,
        PAGE_UNREAL,
        PAGE_INPUT,
        PAGE_CAMERA,
        PAGE_KEYBINDS,
        PAGE_CONSOLE,
        PAGE_COMPATIBILITY,
        PAGE_DEBUG,
    };

    SelectedPage selected_page = PAGE_RUNTIME;

    /*ImGui::BeginTable("VRTable", 2, ImGuiTableFlags_::ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_::ImGuiTableFlags_BordersOuterV | ImGuiTableFlags_::ImGuiTableFlags_SizingFixedFit);
    ImGui::TableSetupColumn("LeftPane", ImGuiTableColumnFlags_WidthFixed, 150.0f);
    ImGui::TableSetupColumn("RightPane", ImGuiTableColumnFlags_WidthStretch);

    // Draw left pane
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0); // Set to the first column

        ImGui::BeginGroup();

        auto dcs = [&](const char* label, SelectedPage page_value) -> bool {
            ImGui::PushStyleVar(ImGuiStyleVar_SelectableTextAlign, ImVec2(0.5f, 0.5f));
            utility::ScopeGuard sg3([]() {
                ImGui::PopStyleVar();
            });
            if (ImGui::Selectable(label, selected_page == page_value)) {
                selected_page = page_value;
                return true;
            }
            return false;
        };

        dcs("Runtime", PAGE_RUNTIME);
        dcs("Unreal", PAGE_UNREAL);
        dcs("Input", PAGE_INPUT);
        dcs("Camera", PAGE_CAMERA);
        dcs("Console/CVars", PAGE_CONSOLE);
        dcs("Compatibility", PAGE_COMPATIBILITY);
        dcs("Debug", PAGE_DEBUG);

        ImGui::EndGroup();
    }

    ImGui::TableNextColumn(); // Move to the next column (right)
    ImGui::BeginGroup();*/

    switch (hash) {
    case "Runtime"_fnv:
        selected_page = PAGE_RUNTIME;
        break;
    case "Unreal"_fnv:
        selected_page = PAGE_UNREAL;
        break;
    case "Input"_fnv:
        selected_page = PAGE_INPUT;
        break;
    case "Camera"_fnv:
        selected_page = PAGE_CAMERA;
        break;
    case "Keybinds"_fnv:
        selected_page = PAGE_KEYBINDS;
        break;
    case "Console/CVars"_fnv:
        selected_page = PAGE_CONSOLE;
        break;
    case "Compatibility"_fnv:
        selected_page = PAGE_COMPATIBILITY;
        break;
    case "Debug"_fnv:
        selected_page = PAGE_DEBUG;
        break;
    default:
        ImGui::Text("Unknown page selected");
        break;
    }

    if (selected_page == PAGE_RUNTIME) {
        if (m_has_hw_scheduling) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::TextWrapped("WARNING: Hardware-accelerated GPU scheduling is enabled. This may cause the game to run slower.");
            ImGui::TextWrapped("Go into your Windows Graphics settings and disable \"Hardware-accelerated GPU scheduling\"");
            ImGui::PopStyleColor();
            ImGui::TextWrapped("Note: This is only necessary if you are experiencing performance issues.");
        }

        if (GetModuleHandleW(L"nvngx_dlssg.dll") != nullptr) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            ImGui::TextWrapped("WARNING: DLSS Frame Generation has been detected. Make sure it is disabled within in-game settings.");
            ImGui::PopStyleColor();
        }

        ImGui::Text((std::string{"Runtime Information ("} + get_runtime()->name().data() + ")").c_str());

        m_desktop_fix->draw("Desktop Spectator View");
        ImGui::SameLine();
        m_2d_screen_mode->draw("2D Screen Mode");

        ImGui::TextWrapped("Render Resolution (per-eye): %d x %d", get_runtime()->get_width(), get_runtime()->get_height());
        ImGui::TextWrapped("Total Render Resolution: %d x %d", get_runtime()->get_width() * 2, get_runtime()->get_height());

        if (get_runtime()->is_openvr()) {
            ImGui::TextWrapped("Resolution can be changed in SteamVR");
        }

        get_runtime()->on_draw_ui();

        m_overlay_component.on_draw_ui();

        ImGui::TreePop();
    }

    if (selected_page == PAGE_UNREAL) {
        m_rendering_method->draw("Rendering Method");
        m_synced_afr_method->draw("Synced Sequential Method");

        m_world_scale->draw("World Scale");
        m_depth_scale->draw("Depth Scale");

        m_disable_hzbocclusion->draw("Disable HZBOcclusion");
        m_disable_instance_culling->draw("Disable Instance Culling");
        m_disable_hdr_compositing->draw("Disable HDR Composition");
        m_disable_blur_widgets->draw("Disable Blur Widgets");
        m_uncap_framerate->draw("Uncap Framerate");
        m_enable_gui->draw("Enable GUI");
        m_enable_depth->draw("Enable Depth-based Latency Reduction");
        m_load_blueprint_code->draw("Load Blueprint Code");
        m_ghosting_fix->draw("Ghosting Fix");

        ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
        if (ImGui::TreeNode("Native Stereo Fix")) {
            m_native_stereo_fix->draw("Enabled");
            m_native_stereo_fix_same_pass->draw("Use Same Stereo Pass");
            ImGui::TreePop();
        }

        ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
        if (ImGui::TreeNode("Near Clip Plane")) {
            m_custom_z_near_enabled->draw("Enable");

            if (m_custom_z_near_enabled->value()) {
                m_custom_z_near->draw("Value");

                if (m_custom_z_near->value() <= 0.0f) {
                    m_custom_z_near->value() = 0.01f;
                }
            }

            ImGui::TreePop();
        }
    }

    if (selected_page == PAGE_INPUT) {
        ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
        if (ImGui::TreeNode("Controller")) {
            m_joystick_deadzone->draw("VR Joystick Deadzone");
            m_controller_pitch_offset->draw("Controller Pitch Offset");

            m_dpad_shifting->draw("DPad Shifting");
            ImGui::SameLine();
            m_swap_controllers->draw("Left-handed Controller Inputs");
            m_dpad_shifting_method->draw("DPad Shifting Method");

            ImGui::TreePop();
        }

        ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
        if (ImGui::TreeNode("Aim Method")) {
            ImGui::TextWrapped("Some games may not work with this enabled.");
            if (m_aim_method->draw("Type")) {
                m_previous_aim_method = (AimMethod)m_aim_method->value();
            }

            m_aim_speed->draw("Speed");
            m_aim_interp->draw("Smoothing");

            m_aim_modify_player_control_rotation->draw("Modify Player Control Rotation");
            ImGui::SameLine();
            m_aim_use_pawn_control_rotation->draw("Use Pawn Control Rotation");

            m_aim_multiplayer_support->draw("Multiplayer Support");

            ImGui::TreePop();
        }

        ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
        if (ImGui::TreeNode("Snap Turn")) {
            m_snapturn->draw("Enabled");
            m_snapturn_angle->draw("Angle");
            m_snapturn_joystick_deadzone->draw("Deadzone");
        
            ImGui::TreePop();
        }

        ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
        if (ImGui::TreeNode("Movement Orientation")) {
            m_movement_orientation->draw("Type");

            ImGui::TreePop();
        }

        ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
        if (ImGui::TreeNode("Roomscale Movement")) {
            m_roomscale_movement->draw("Enabled");

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("When enabled, headset movement will affect the movement of the player character.");
            }

            ImGui::SameLine();
            m_roomscale_sweep->draw("Sweep Movement");
            // Draw description of option
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("When enabled, roomscale movement will use a sweep to prevent the player from moving through walls.\nThis also allows physics objects to interact with the player, like doors.");
            }

            ImGui::TreePop();
        }
    }

    if (selected_page == PAGE_CAMERA) {
        ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
        if (ImGui::TreeNode("Camera Freeze")) {
            float camera_offset[] = {m_camera_forward_offset->value(), m_camera_right_offset->value(), m_camera_up_offset->value()};
            if (ImGui::SliderFloat3("Camera Offset", camera_offset, -4000.0f, 4000.0f)) {
                m_camera_forward_offset->value() = camera_offset[0];
                m_camera_right_offset->value() = camera_offset[1];
                m_camera_up_offset->value() = camera_offset[2];
            }

            for (auto i = 0; i < m_camera_datas.size(); ++i) {
                auto& data = m_camera_datas[i];

                if (ImGui::Button(std::format("Save Camera {}", i).data())) {
                    save_camera(i);
                }

                ImGui::SameLine();

                if (ImGui::Button(std::format("Load Camera {}", i).data())) {
                    load_camera(i);
                }
            }

            bool pos_freeze = m_camera_freeze.position_frozen || m_camera_freeze.position_wants_freeze;
            if (ImGui::Checkbox("Freeze Position", &pos_freeze)) {
                if (pos_freeze) {
                    m_camera_freeze.position_wants_freeze = true;
                } else {
                    m_camera_freeze.position_frozen = false;
                }
            }

            ImGui::SameLine();
            bool rot_freeze = m_camera_freeze.rotation_frozen || m_camera_freeze.rotation_wants_freeze;
            if (ImGui::Checkbox("Freeze Rotation", &rot_freeze)) {
                if (rot_freeze) {
                    m_camera_freeze.rotation_wants_freeze = true;
                } else {
                    m_camera_freeze.rotation_frozen = false;
                }
            }

            ImGui::TreePop();
        }

        ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
        if (ImGui::TreeNode("Camera Lerp")) {
            m_lerp_camera_pitch->draw("Lerp Pitch");
            ImGui::SameLine();
            m_lerp_camera_yaw->draw("Lerp Yaw");
            ImGui::SameLine();
            m_lerp_camera_roll->draw("Lerp Roll");
            m_lerp_camera_speed->draw("Lerp Speed");

            ImGui::TreePop();
        }

        ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
        if (ImGui::TreeNode("Decoupled Pitch")) {
            m_decoupled_pitch->draw("Enabled");
            m_decoupled_pitch_ui_adjust->draw("Auto Adjust UI");

            ImGui::TreePop();
        }
    }

    if (selected_page == PAGE_KEYBINDS) {
        ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
        if (ImGui::TreeNode("Playspace Keys")) {
            m_keybind_recenter->draw("Recenter View Key");
            m_keybind_recenter_horizon->draw("Recenter Horizon Key");
            m_keybind_set_standing_origin->draw("Set Standing Origin Key");

            ImGui::TreePop();
        }

        ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
        if (ImGui::TreeNode("Camera Keys")) {
            m_keybind_load_camera_0->draw("Load Camera 0 Key");
            m_keybind_load_camera_1->draw("Load Camera 1 Key");
            m_keybind_load_camera_2->draw("Load Camera 2 Key");

            ImGui::TreePop();
        }

        ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
        if (ImGui::TreeNode("Overlay/Runtime Keys")) {
            m_keybind_toggle_2d_screen->draw("Toggle 2D Screen Mode Key");
            m_keybind_toggle_gui->draw("Toggle In-Game UI Key");
            m_keybind_disable_vr->draw("Disable VR Key");

            ImGui::TreePop();
        }
    }

    if (selected_page == PAGE_CONSOLE) {
        m_cvar_manager->on_draw_ui();
    }

    if (selected_page == PAGE_COMPATIBILITY) {
        ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
        if (ImGui::TreeNode("Compatibility Options")) {
            m_compatibility_ahud->draw("AHUD UI Compatibility");
            m_compatibility_skip_uobjectarray_init->draw("Skip UObjectArray Init");
            m_compatibility_skip_pip->draw("Skip PostInitProperties");
            m_sceneview_compatibility_mode->draw("SceneView Compatibility Mode");
            m_extreme_compat_mode->draw("Extreme Compatibility Mode");

            // changes to any of these options should trigger a regeneration of the eye projection matrices
            const auto horizontal_projection_changed = m_horizontal_projection_override->draw("Horizontal Projection");
            const auto vertical_projection_changed = m_vertical_projection_override->draw("Vertical Projection");
            const auto scale_render = m_grow_rectangle_for_projection_cropping->draw("Scale Render Target");
            const auto scale_render_changed = get_runtime()->is_modifying_eye_texture_scale != scale_render;
            get_runtime()->is_modifying_eye_texture_scale = scale_render;
            get_runtime()->should_recalculate_eye_projections = horizontal_projection_changed || vertical_projection_changed || scale_render_changed;

            ImGui::TreePop();
        }

        ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
        if (ImGui::TreeNode("Splitscreen Compatibility")) {
            m_splitscreen_compatibility_mode->draw("Enabled");
            m_splitscreen_view_index->draw("Index");
            ImGui::TreePop();
        }
    }
    
    if (selected_page == PAGE_DEBUG) {
        if (m_fake_stereo_hook != nullptr) {
            m_fake_stereo_hook->on_draw_ui();
        }

        ImGui::Combo("Sync Mode", (int*)&get_runtime()->custom_stage, "Early\0Late\0Very Late\0");
        ImGui::DragFloat4("Right Bounds", (float*)&m_right_bounds, 0.005f, -2.0f, 2.0f);
        ImGui::DragFloat4("Left Bounds", (float*)&m_left_bounds, 0.005f, -2.0f, 2.0f);
        ImGui::Checkbox("Disable Projection Matrix Override", &m_disable_projection_matrix_override);
        ImGui::Checkbox("Disable View Matrix Override", &m_disable_view_matrix_override);
        ImGui::Checkbox("Disable Backbuffer Size Override", &m_disable_backbuffer_size_override);
        ImGui::Checkbox("Disable VR Overlay", &m_disable_overlay);
        ImGui::Checkbox("Disable VR Entirely", &m_disable_vr);
        ImGui::Checkbox("Stereo Emulation Mode", &m_stereo_emulation_mode);
        ImGui::Checkbox("Wait for Present", &m_wait_for_present);
        m_controllers_allowed->draw("Controllers allowed");
        ImGui::Checkbox("Controller test mode", &m_controller_test_mode);
        m_show_fps->draw("Show FPS");
        m_show_statistics->draw("Show Engine Statistics");

        const double min_ = 0.0;
        const double max_ = 25.0;
        ImGui::SliderScalar("Prediction Scale", ImGuiDataType_Double, &m_openxr->prediction_scale, &min_, &max_);

        ImGui::DragFloat4("Raw Left", (float*)&m_raw_projections[0], 0.01f, -100.0f, 100.0f);
        ImGui::DragFloat4("Raw Right", (float*)&m_raw_projections[1], 0.01f, -100.0f, 100.0f);

        const auto left_stick_axis = get_left_stick_axis();
        const auto right_stick_axis = get_right_stick_axis();

        ImGui::DragFloat2("Left Stick", (float*)&left_stick_axis, 0.01f, -1.0f, 1.0f);
        ImGui::DragFloat2("Right Stick", (float*)&right_stick_axis, 0.01f, -1.0f, 1.0f);

        ImGui::TextWrapped("Hardware scheduling: %s", m_has_hw_scheduling ? "Enabled" : "Disabled");
    }

    ImGui::EndGroup();
    //ImGui::EndTable();
}

void VR::on_draw_ui() {
    ZoneScopedN(__FUNCTION__);

    // create VR tree entry in menu (imgui)
    ImGui::PushID("VR");
    ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
    if (!m_fake_stereo_hook->has_attempted_to_hook_engine() || !m_fake_stereo_hook->has_attempted_to_hook_slate()) {
        std::string adjusted_name = get_name().data();
        adjusted_name += " (Loading...)";

        /*if (!ImGui::CollapsingHeader(adjusted_name.data())) {
            ImGui::PopID();
            return;
        }*/

        ImGui::TextWrapped("Loading...");
    } else {
        /*if (!ImGui::CollapsingHeader(get_name().data())) {
            ImGui::PopID();
            return;
        }*/
    }
    ImGui::PopID();

    auto display_error = [](auto& runtime, std::string dll_name) {
        if (runtime == nullptr || !runtime->error && runtime->loaded) {
            return;
        }

        if (runtime->error && runtime->dll_missing) {
            ImGui::TextWrapped("%s not loaded: %s not found", runtime->name().data(), dll_name.data());
            ImGui::TextWrapped("Please select %s from the loader if you want to use %s", runtime->name().data(), runtime->name().data());
        } else if (runtime->error) {
            ImGui::TextWrapped("%s not loaded: %s", runtime->name().data(), runtime->error->c_str());
        } else {
            ImGui::TextWrapped("%s not loaded: Unknown error", runtime->name().data());
        }

        ImGui::Separator();
    };

    if (!get_runtime()->loaded || get_runtime()->error) {
        display_error(m_openxr, "openxr_loader.dll");
        display_error(m_openvr, "openvr_api.dll");
    }

    if (!get_runtime()->loaded) {
        ImGui::TextWrapped("No runtime loaded.");

        if (ImGui::Button("Attempt to reinitialize")) {
            clean_initialize();
        }

        return;
    }

    if (ImGui::Button("Set Standing Height")) {
        m_standing_origin.y = get_position(0).y;
    }

    ImGui::SameLine();

    if (ImGui::Button("Set Standing Origin")) {
        m_standing_origin = get_position(0);
    }

    ImGui::SameLine();

    if (ImGui::Button("Recenter View")) {
        recenter_view();
    }

    ImGui::SameLine();

    if (ImGui::Button("Recenter Horizon")) {
        recenter_horizon();
    }

    if (ImGui::Button("Reinitialize Runtime")) {
        get_runtime()->wants_reinitialize = true;
    }
}

Vector4f VR::get_position(uint32_t index, bool grip) const {
    return get_transform(index, grip)[3];
}

Vector4f VR::get_velocity(uint32_t index) const {
    if (index >= vr::k_unMaxTrackedDeviceCount) {
        return Vector4f{};
    }

    std::shared_lock _{ get_runtime()->pose_mtx };

    return get_velocity_unsafe(index);
}

Vector4f VR::get_angular_velocity(uint32_t index) const {
    if (index >= vr::k_unMaxTrackedDeviceCount) {
        return Vector4f{};
    }

    std::shared_lock _{ get_runtime()->pose_mtx };

    return get_angular_velocity_unsafe(index);
}

Vector4f VR::get_position_unsafe(uint32_t index) const {
    if (get_runtime()->is_openvr()) {
        if (index >= vr::k_unMaxTrackedDeviceCount) {
            return Vector4f{};
        }

        if (index == vr::k_unTrackedDeviceIndex_Hmd) {
            const auto pose = m_openvr->get_current_hmd_pose();
            auto matrix = Matrix4x4f{ *(Matrix3x4f*)&pose };
            auto result = glm::rowMajor4(matrix)[3];
            result.w = 1.0f;

            return result;
        }

        if (index == get_left_controller_index()) {
            return m_openvr->grip_matrices[VRRuntime::Hand::LEFT][3];
        }

        if (index == get_right_controller_index()) {
            return m_openvr->grip_matrices[VRRuntime::Hand::RIGHT][3];
        }

        auto& pose = get_openvr_poses()[index];
        auto matrix = Matrix4x4f{ *(Matrix3x4f*)&pose.mDeviceToAbsoluteTracking };
        auto result = glm::rowMajor4(matrix)[3];
        result.w = 1.0f;

        return result;
    } else if (get_runtime()->is_openxr()) {
        if (index >= 3) {
            return Vector4f{};
        }

        // HMD position
        if (index == 0 && !m_openxr->stage_views.empty()) {
            const auto vspl = m_openxr->get_current_view_space_location();
            return Vector4f{ *(Vector3f*)&vspl.pose.position, 1.0f };
        } else if (index > 0) {
            if (index == get_left_controller_index()) {
                return m_openxr->grip_matrices[VRRuntime::Hand::LEFT][3];
            } else if (index == get_right_controller_index()) {
                return m_openxr->grip_matrices[VRRuntime::Hand::RIGHT][3];
            }
        }

        return Vector4f{};
    } 

    return Vector4f{};
}

Vector4f VR::get_velocity_unsafe(uint32_t index) const {
    if (get_runtime()->is_openvr()) {
        if (index >= vr::k_unMaxTrackedDeviceCount) {
            return Vector4f{};
        }

        const auto& pose = get_openvr_poses()[index];
        const auto& velocity = pose.vVelocity;

        return Vector4f{ velocity.v[0], velocity.v[1], velocity.v[2], 0.0f };
    } else if (get_runtime()->is_openxr()) {
        if (index >= 3) {
            return Vector4f{};
        }

        // todo: implement HMD velocity
        if (index == 0) {
            return Vector4f{};
        }

        return Vector4f{ *(Vector3f*)&m_openxr->hands[index-1].grip_velocity.linearVelocity, 0.0f };
    }

    return Vector4f{};
}

Vector4f VR::get_angular_velocity_unsafe(uint32_t index) const {
    if (get_runtime()->is_openvr()) {
        if (index >= vr::k_unMaxTrackedDeviceCount) {
            return Vector4f{};
        }

        const auto& pose = get_openvr_poses()[index];
        const auto& angular_velocity = pose.vAngularVelocity;

        return Vector4f{ angular_velocity.v[0], angular_velocity.v[1], angular_velocity.v[2], 0.0f };
    } else if (get_runtime()->is_openxr()) {
        if (index >= 3) {
            return Vector4f{};
        }

        // todo: implement HMD velocity
        if (index == 0) {
            return Vector4f{};
        }
    
        return Vector4f{ *(Vector3f*)&m_openxr->hands[index-1].grip_velocity.angularVelocity, 0.0f };
    }

    return Vector4f{};
}

Matrix4x4f VR::get_hmd_rotation(uint32_t frame_count) const {
    return glm::extractMatrixRotation(get_hmd_transform(frame_count));
}

Matrix4x4f VR::get_hmd_transform(uint32_t frame_count) const {
    ZoneScopedN(__FUNCTION__);

    if (get_runtime()->is_openvr()) {
        std::shared_lock _{ get_runtime()->pose_mtx };

        const auto pose = m_openvr->get_hmd_pose(frame_count);
        const auto matrix = Matrix4x4f{ *(Matrix3x4f*)&pose };
        return glm::rowMajor4(matrix);
    } else if (get_runtime()->is_openxr()) {
        std::shared_lock __{ get_runtime()->eyes_mtx };

        const auto vspl = m_openxr->get_view_space_location(frame_count);
        auto mat = Matrix4x4f{runtimes::OpenXR::to_glm(vspl.pose.orientation)};
        mat[3] = Vector4f{*(Vector3f*)&vspl.pose.position, 1.0f};

        return mat;
    }

    return glm::identity<Matrix4x4f>();
}

Matrix4x4f VR::get_rotation(uint32_t index, bool grip) const {
    return glm::extractMatrixRotation(get_transform(index, grip));
}

Matrix4x4f VR::get_transform(uint32_t index, bool grip) const {
    ZoneScopedN(__FUNCTION__);

    if (get_runtime()->is_openvr()) {
        if (index >= vr::k_unMaxTrackedDeviceCount) {
            return glm::identity<Matrix4x4f>();
        }

        std::shared_lock _{ get_runtime()->pose_mtx };

        if (index == vr::k_unTrackedDeviceIndex_Hmd) {
            const auto pose = m_openvr->get_current_hmd_pose();
            const auto matrix = Matrix4x4f{ *(Matrix3x4f*)&pose };
            return glm::rowMajor4(matrix);
        }

        if (index == get_left_controller_index()) {
            return grip ? m_openvr->grip_matrices[VRRuntime::Hand::LEFT] : m_openvr->aim_matrices[VRRuntime::Hand::LEFT];
        } else if (index == get_right_controller_index()) {
            return grip ? m_openvr->grip_matrices[VRRuntime::Hand::RIGHT] : m_openvr->aim_matrices[VRRuntime::Hand::RIGHT];
        }

        const auto& pose = get_openvr_poses()[index];
        const auto matrix = Matrix4x4f{ *(Matrix3x4f*)&pose.mDeviceToAbsoluteTracking };
        return glm::rowMajor4(matrix);
    } else if (get_runtime()->is_openxr()) {
        // HMD rotation
        if (index == 0 && !m_openxr->stage_views.empty()) {
            const auto vspl = m_openxr->get_current_view_space_location();
            auto mat = Matrix4x4f{runtimes::OpenXR::to_glm(vspl.pose.orientation)};
            mat[3] = Vector4f{*(Vector3f*)&vspl.pose.position, 1.0f};
            return mat;
        } else if (index > 0) {
            if (index == get_left_controller_index()) {
                return grip ? m_openxr->grip_matrices[VRRuntime::Hand::LEFT] : m_openxr->aim_matrices[VRRuntime::Hand::LEFT];
            } else if (index == get_right_controller_index()) {
                return grip ? m_openxr->grip_matrices[VRRuntime::Hand::RIGHT] : m_openxr->aim_matrices[VRRuntime::Hand::RIGHT];
            }
        }
    }

    return glm::identity<Matrix4x4f>();
}

Matrix4x4f VR::get_grip_transform(uint32_t index) const {
    return get_transform(index);
}

Matrix4x4f VR::get_aim_transform(uint32_t index) const {
    return get_transform(index, false);
}

vr::HmdMatrix34_t VR::get_raw_transform(uint32_t index) const {
    if (get_runtime()->is_openvr()) {
        if (index >= vr::k_unMaxTrackedDeviceCount) {
            return vr::HmdMatrix34_t{};
        }

        std::shared_lock _{ get_runtime()->pose_mtx };

        if (index == vr::k_unTrackedDeviceIndex_Hmd) {
            return m_openvr->get_current_hmd_pose();
        }

        auto& pose = get_openvr_poses()[index];
        return pose.mDeviceToAbsoluteTracking;
    } else {
        spdlog::error("VR: get_raw_transform: not implemented for {}", get_runtime()->name());
        return vr::HmdMatrix34_t{};
    }
}

Vector4f VR::get_eye_offset(VRRuntime::Eye eye) const {
    ZoneScopedN(__FUNCTION__);

    if (!is_hmd_active()) {
        return Vector4f{};
    }

    std::shared_lock _{ get_runtime()->eyes_mtx };

    if (eye == VRRuntime::Eye::LEFT) {
        return get_runtime()->eyes[vr::Eye_Left][3];
    }
    
    return get_runtime()->eyes[vr::Eye_Right][3];
}

Vector4f VR::get_current_offset() {
    if (!is_hmd_active()) {
        return Vector4f{};
    }

    std::shared_lock _{ get_runtime()->eyes_mtx };

    if (m_frame_count % 2 == m_left_eye_interval) {
        //return Vector4f{m_eye_distance * -1.0f, 0.0f, 0.0f, 0.0f};
        return get_runtime()->eyes[vr::Eye_Left][3];
    }
    
    return get_runtime()->eyes[vr::Eye_Right][3];
    //return Vector4f{m_eye_distance, 0.0f, 0.0f, 0.0f};
}

Matrix4x4f VR::get_eye_transform(uint32_t index) {
    ZoneScopedN(__FUNCTION__);

    if (!is_hmd_active() || index > 2) {
        return glm::identity<Matrix4x4f>();
    }

    std::shared_lock _{get_runtime()->eyes_mtx};

    return get_runtime()->eyes[index];
}

Matrix4x4f VR::get_current_eye_transform(bool flip) {
    if (!is_hmd_active()) {
        return glm::identity<Matrix4x4f>();
    }

    std::shared_lock _{get_runtime()->eyes_mtx};

    auto mod_count = flip ? m_right_eye_interval : m_left_eye_interval;

    if (m_frame_count % 2 == mod_count) {
        return get_runtime()->eyes[vr::Eye_Left];
    }

    return get_runtime()->eyes[vr::Eye_Right];
}

Matrix4x4f VR::get_projection_matrix(VRRuntime::Eye eye, bool flip) {
    ZoneScopedN(__FUNCTION__);

    if (!is_hmd_active()) {
        return glm::identity<Matrix4x4f>();
    }

    std::shared_lock _{get_runtime()->eyes_mtx};

    if ((eye == VRRuntime::Eye::LEFT && !flip) || (eye == VRRuntime::Eye::RIGHT && flip)) {
        return get_runtime()->projections[(uint32_t)VRRuntime::Eye::LEFT];
    }

    return get_runtime()->projections[(uint32_t)VRRuntime::Eye::RIGHT];
}

Matrix4x4f VR::get_current_projection_matrix(bool flip) {
    if (!is_hmd_active()) {
        return glm::identity<Matrix4x4f>();
    }

    std::shared_lock _{get_runtime()->eyes_mtx};

    auto mod_count = flip ? m_right_eye_interval : m_left_eye_interval;

    if (m_frame_count % 2 == mod_count) {
        return get_runtime()->projections[(uint32_t)VRRuntime::Eye::LEFT];
    }

    return get_runtime()->projections[(uint32_t)VRRuntime::Eye::RIGHT];
}

bool VR::is_action_active(vr::VRActionHandle_t action, vr::VRInputValueHandle_t source) const {
    ZoneScopedN(__FUNCTION__);

    if (!get_runtime()->loaded) {
        return false;
    }

    if (action == vr::k_ulInvalidActionHandle) {
        return false;
    }
    
    bool active = false;

    if (get_runtime()->is_openvr()) {
        vr::InputDigitalActionData_t data{};
        vr::VRInput()->GetDigitalActionData(action, &data, sizeof(data), source);

        active = data.bActive && data.bState;
    } else if (get_runtime()->is_openxr()) {
        active = m_openxr->is_action_active((XrAction)action, (VRRuntime::Hand)source);
    }

    return active;
}

Vector2f VR::get_joystick_axis(vr::VRInputValueHandle_t handle) const {
    ZoneScopedN(__FUNCTION__);

    if (!get_runtime()->loaded) {
        return Vector2f{};
    }

    if (get_runtime()->is_openvr()) {
        vr::InputAnalogActionData_t data{};
        vr::VRInput()->GetAnalogActionData(m_action_joystick, &data, sizeof(data), handle);

        const auto deadzone = m_joystick_deadzone->value();
        auto out = Vector2f{ data.x, data.y };

        //return glm::length(out) > deadzone ? out : Vector2f{};
        if (glm::abs(out.x) < deadzone) {
            out.x = 0.0f;
        }

        if (glm::abs(out.y) < deadzone) {
            out.y = 0.0f;
        }

        return out;
    } else if (get_runtime()->is_openxr()) {
        // Not using get_left/right_joystick here because it flips the controllers
        if (handle == m_left_joystick) {
            auto out = m_openxr->get_left_stick_axis();
            //return glm::length(out) > m_joystick_deadzone->value() ? out : Vector2f{};
            // okay.. instead of that actually clamp x/y to the proper deadzone
            if (glm::abs(out.x) < m_joystick_deadzone->value()) {
                out.x = 0.0f;
            }

            if (glm::abs(out.y) < m_joystick_deadzone->value()) {
                out.y = 0.0f;
            }

            return out;
        } else if (handle == m_right_joystick) {
            auto out = m_openxr->get_right_stick_axis();
            //return glm::length(out) > m_joystick_deadzone->value() ? out : Vector2f{};

            if (glm::abs(out.x) < m_joystick_deadzone->value()) {
                out.x = 0.0f;
            }

            if (glm::abs(out.y) < m_joystick_deadzone->value()) {
                out.y = 0.0f;
            }

            return out;
        }
    }

    return Vector2f{};
}

Vector2f VR::get_left_stick_axis() const {
    return get_joystick_axis(get_left_joystick());
}

Vector2f VR::get_right_stick_axis() const {
    return get_joystick_axis(get_right_joystick());
}

void VR::trigger_haptic_vibration(float seconds_from_now, float duration, float frequency, float amplitude, vr::VRInputValueHandle_t source) {
    ZoneScopedN(__FUNCTION__);

    if (!get_runtime()->loaded || !is_using_controllers()) {
        return;
    }

    if (get_runtime()->is_openvr()) {
        vr::VRInput()->TriggerHapticVibrationAction(m_action_haptic, seconds_from_now, duration, frequency, amplitude, source);
    } else if (get_runtime()->is_openxr()) {
        m_openxr->trigger_haptic_vibration(duration, frequency, amplitude, (VRRuntime::Hand)source);
    }
}

float VR::get_standing_height() {
    ZoneScopedN(__FUNCTION__);

    std::shared_lock _{ get_runtime()->pose_mtx };

    return m_standing_origin.y;
}

Vector4f VR::get_standing_origin() {
    ZoneScopedN(__FUNCTION__);

    std::shared_lock _{ get_runtime()->pose_mtx };

    return m_standing_origin;
}

void VR::set_standing_origin(const Vector4f& origin) {
    ZoneScopedN(__FUNCTION__);

    std::unique_lock _{ get_runtime()->pose_mtx };
    
    m_standing_origin = origin;
}

glm::quat VR::get_rotation_offset() {
    ZoneScopedN(__FUNCTION__);

    std::shared_lock _{ m_rotation_mtx };

    return m_rotation_offset;
}

void VR::set_rotation_offset(const glm::quat& offset) {
    ZoneScopedN(__FUNCTION__);

    std::unique_lock _{ m_rotation_mtx };

    m_rotation_offset = offset;
}

void VR::recenter_view() {
    ZoneScopedN(__FUNCTION__);

    const auto new_rotation_offset = glm::normalize(glm::inverse(utility::math::flatten(glm::quat{get_rotation(0)})));

    set_rotation_offset(new_rotation_offset);
}

void VR::recenter_horizon() {
    ZoneScopedN(__FUNCTION__);

    const auto new_rotation_offset = glm::normalize(glm::inverse(glm::quat{get_rotation(0)}));

    set_rotation_offset(new_rotation_offset);
}

void VR::gamepad_snapturn(XINPUT_STATE& state) {
    if (!m_snapturn->value()) {
        return;
    }

    if (!is_hmd_active()) {
        return;
    }

    const auto stick_axis = (float)state.Gamepad.sThumbRX / (float)std::numeric_limits<SHORT>::max();

    if (!m_was_snapturn_run_on_input) {
        if (glm::abs(stick_axis) > m_snapturn_joystick_deadzone->value()) {
            m_snapturn_left = stick_axis < 0.0f;
            m_snapturn_on_frame = true;
            m_was_snapturn_run_on_input = true;
            state.Gamepad.sThumbRX = 0;
        }
    } else {
        if (glm::abs(stick_axis) < m_snapturn_joystick_deadzone->value()) {
            m_was_snapturn_run_on_input = false;
        } else {
            state.Gamepad.sThumbRX = 0;
        }
    }
}

void VR::process_snapturn() {
    if (!m_snapturn_on_frame) {
        return;
    }

    const auto engine = sdk::UEngine::get();

    if (engine == nullptr) {
        return;
    }

    const auto world = engine->get_world();

    if (world == nullptr) {
        return;
    }

    if (const auto controller = sdk::UGameplayStatics::get()->get_player_controller(world, 0); controller != nullptr) {
        auto controller_rot = controller->get_control_rotation();
        auto turn_degrees = get_snapturn_angle();
        
        if (m_snapturn_left) {
            turn_degrees = -turn_degrees;
            m_snapturn_left = false;
        }

        controller_rot.y += turn_degrees;
        controller->set_control_rotation(controller_rot);
    }
        
    m_snapturn_on_frame = false;
}

void VR::update_statistics_overlay(sdk::UGameEngine* engine) {
    if (engine == nullptr) {
        return;
    }
    
    if (m_show_fps_state != m_show_fps->value()) {
        engine->exec(L"stat fps");
        m_show_fps_state = m_show_fps->value();
    }
    
    if (m_show_statistics_state != m_show_statistics->value()) {
        engine->exec(L"stat unit");
        m_show_statistics_state = m_show_statistics->value();
    }
}

`

---
## ðŸ“ VR.hpp
**Sti:** $filePath  
**StÃ¸rrelse:** 46505 bytes  
**Sidst Ã¦ndret:** 2025-08-19 11:20:13

### ðŸ”— Includes
`cpp
#include <memory>
`

### ðŸ—ï¸ Classes
`cpp
class VR : public Mod {
`

### âš™ï¸ Mod Event Functions
`cpp
on_initialize_d3d_thread()
`

### âš™ï¸ General Functions
`cpp
void on_config_load(const utility::Config& cfg, bool set_defaults)
`

### ðŸ“„ Komplet Indhold
`cpp
#pragma once

#define NOMINMAX

#include <memory>
#include <string>

#include <sdk/Math.hpp>

#include "vr/runtimes/OpenVR.hpp"
#include "vr/runtimes/OpenXR.hpp"

#include "vr/D3D11Component.hpp"
#include "vr/D3D12Component.hpp"
#include "vr/OverlayComponent.hpp"

#include "vr/FFakeStereoRenderingHook.hpp"
#include "vr/RenderTargetPoolHook.hpp"
#include "vr/CVarManager.hpp"

#include "Mod.hpp"

#undef max
#include <tracy/Tracy.hpp>

class VR : public Mod {
public:
    enum RenderingMethod {
        NATIVE_STEREO = 0,
        SYNCHRONIZED = 1,
        ALTERNATING = 2,
    };

    enum SyncedSequentialMethod {
        SKIP_TICK = 0,
        SKIP_DRAW = 1,
    };

    enum AimMethod : int32_t {
        GAME,
        HEAD,
        RIGHT_CONTROLLER,
        LEFT_CONTROLLER,
        TWO_HANDED_RIGHT,
        TWO_HANDED_LEFT,
    };

    enum DPadMethod : int32_t {
        RIGHT_TOUCH,
        LEFT_TOUCH,
        LEFT_JOYSTICK,
        RIGHT_JOYSTICK,
        GESTURE_HEAD,
        GESTURE_HEAD_RIGHT,
    };

    enum HORIZONTAL_PROJECTION_OVERRIDE : int32_t {
        HORIZONTAL_DEFAULT,
        HORIZONTAL_SYMMETRIC,
        HORIZONTAL_MIRROR
    };

    enum VERTICAL_PROJECTION_OVERRIDE : int32_t {
        VERTICAL_DEFAULT,
        VERTICAL_SYMMETRIC,
        VERTICAL_MATCHED
    };

    static const inline std::string s_action_pose = "/actions/default/in/Pose";
    static const inline std::string s_action_grip_pose = "/actions/default/in/GripPose";
    static const inline std::string s_action_trigger = "/actions/default/in/Trigger";
    static const inline std::string s_action_grip = "/actions/default/in/Grip";
    static const inline std::string s_action_joystick = "/actions/default/in/Joystick";
    static const inline std::string s_action_joystick_click = "/actions/default/in/JoystickClick";

    static const inline std::string s_action_a_button_left = "/actions/default/in/AButtonLeft";
    static const inline std::string s_action_b_button_left = "/actions/default/in/BButtonLeft";
    static const inline std::string s_action_a_button_touch_left = "/actions/default/in/AButtonTouchLeft";
    static const inline std::string s_action_b_button_touch_left = "/actions/default/in/BButtonTouchLeft";

    static const inline std::string s_action_a_button_right = "/actions/default/in/AButtonRight";
    static const inline std::string s_action_b_button_right = "/actions/default/in/BButtonRight";
    static const inline std::string s_action_a_button_touch_right = "/actions/default/in/AButtonTouchRight";
    static const inline std::string s_action_b_button_touch_right = "/actions/default/in/BButtonTouchRight";

    static const inline std::string s_action_dpad_up = "/actions/default/in/DPad_Up";
    static const inline std::string s_action_dpad_right = "/actions/default/in/DPad_Right";
    static const inline std::string s_action_dpad_down = "/actions/default/in/DPad_Down";
    static const inline std::string s_action_dpad_left = "/actions/default/in/DPad_Left";
    static const inline std::string s_action_system_button = "/actions/default/in/SystemButton";
    static const inline std::string s_action_thumbrest_touch_left = "/actions/default/in/ThumbrestTouchLeft";
    static const inline std::string s_action_thumbrest_touch_right = "/actions/default/in/ThumbrestTouchRight";

public:
    static std::shared_ptr<VR>& get();

    std::string_view get_name() const override { return "VR"; }

    std::optional<std::string> clean_initialize();
    std::optional<std::string> on_initialize_d3d_thread() {
        return clean_initialize();
    }

    std::vector<SidebarEntryInfo> get_sidebar_entries() override {
        return {
            {"Runtime", false},
            {"Unreal", false},
            {"Input", false},
            {"Camera", false},
            {"Keybinds", false},
            {"Console/CVars", true},
            {"Compatibility", true},
            {"Debug", true},
        };
    }

    // texture bounds to tell OpenVR which parts of the submitted texture to render (default - use the whole texture).
    // Will be modified to accommodate forced symmetrical eye projection
    vr::VRTextureBounds_t m_right_bounds{0.0f, 0.0f, 1.0f, 1.0f};
    vr::VRTextureBounds_t m_left_bounds{0.0f, 0.0f, 1.0f, 1.0f};

    void on_config_load(const utility::Config& cfg, bool set_defaults) override;
    void on_config_save(utility::Config& cfg) override;
    
    void on_draw_ui() override;
    void on_draw_sidebar_entry(std::string_view name) override;
    void on_pre_imgui_frame() override;

    void handle_keybinds();
    void on_frame() override;

    void on_present() override;
    void on_post_present() override;

    void on_device_reset() override {
        get_runtime()->on_device_reset();

        if (m_fake_stereo_hook != nullptr) {
            m_fake_stereo_hook->on_device_reset();
        }

        if (m_is_d3d12) {
            m_d3d12.on_reset(this);
        } else {
            m_d3d11.on_reset(this);
        }
    }

    bool on_message(HWND wnd, UINT message, WPARAM w_param, LPARAM l_param) override;
    void on_xinput_get_state(uint32_t* retval, uint32_t user_index, XINPUT_STATE* state) override;
    void on_xinput_set_state(uint32_t* retval, uint32_t user_index, XINPUT_VIBRATION* vibration) override;
    void update_imgui_state_from_xinput_state(XINPUT_STATE& state, bool is_vr_controller);

    void on_pre_engine_tick(sdk::UGameEngine* engine, float delta) override;
    void on_pre_calculate_stereo_view_offset(void* stereo_device, const int32_t view_index, Rotator<float>* view_rotation, 
                                             const float world_to_meters, Vector3f* view_location, bool is_double) override;
    void on_pre_viewport_client_draw(void* viewport_client, void* viewport, void* canvas) override;

    void update_hmd_state(bool from_view_extensions = false, uint32_t frame_count = 0);
    void update_action_states();
    void update_dpad_gestures();

    void reinitialize_renderer() {
        if (m_is_d3d12) {
            m_d3d12.force_reset();
        } else {
            m_d3d11.force_reset();
        }
    }


    Vector4f get_position(uint32_t index, bool grip = true)  const;
    Vector4f get_velocity(uint32_t index)  const;
    Vector4f get_angular_velocity(uint32_t index)  const;
    Matrix4x4f get_hmd_rotation(uint32_t frame_count) const;
    Matrix4x4f get_hmd_transform(uint32_t frame_count) const;
    Matrix4x4f get_rotation(uint32_t index, bool grip = true)  const;
    Matrix4x4f get_transform(uint32_t index, bool grip = true) const;
    vr::HmdMatrix34_t get_raw_transform(uint32_t index) const;

    Vector4f get_grip_position(uint32_t index) const {
        return get_position(index, true);
    }

    Vector4f get_aim_position(uint32_t index) const {
        return get_position(index, false);
    }

    Matrix4x4f get_grip_rotation(uint32_t index) const {
        return get_rotation(index, true);
    }

    Matrix4x4f get_aim_rotation(uint32_t index) const {
        return get_rotation(index, false);
    }

    Matrix4x4f get_grip_transform(uint32_t hand_index) const;
    Matrix4x4f get_aim_transform(uint32_t hand_index) const;

    Vector4f get_eye_offset(VRRuntime::Eye eye) const;
    Vector4f get_current_offset();
    
    Matrix4x4f get_eye_transform(uint32_t index);
    Matrix4x4f get_current_eye_transform(bool flip = false);
    Matrix4x4f get_projection_matrix(VRRuntime::Eye eye, bool flip = false);
    Matrix4x4f get_current_projection_matrix(bool flip = false);

    bool is_action_active(vr::VRActionHandle_t action, vr::VRInputValueHandle_t source = vr::k_ulInvalidInputValueHandle) const;

    bool is_action_active_any_joystick(vr::VRActionHandle_t action) const {
        if (is_action_active(action, m_left_joystick)) {
            return true;
        }

        if (is_action_active(action, m_right_joystick)) {
            return true;
        }

        return false;
    }
    Vector2f get_joystick_axis(vr::VRInputValueHandle_t handle) const;

    vr::VRActionHandle_t get_action_handle(std::string_view action_path) {
        if (auto it = m_action_handles.find(action_path.data()); it != m_action_handles.end()) {
            return it->second;
        }

        return vr::k_ulInvalidActionHandle;
    }

    Vector2f get_left_stick_axis() const;
    Vector2f get_right_stick_axis() const;

    void trigger_haptic_vibration(float seconds_from_now, float duration, float frequency, float amplitude, vr::VRInputValueHandle_t source = vr::k_ulInvalidInputValueHandle);
    
    float get_standing_height();
    Vector4f get_standing_origin();
    void set_standing_origin(const Vector4f& origin);

    glm::quat get_rotation_offset();
    void set_rotation_offset(const glm::quat& offset);
    void recenter_view();
    void recenter_horizon();


    template<typename T = VRRuntime>
    T* get_runtime() const {
        return (T*)m_runtime.get();
    }

    runtimes::OpenXR* get_openxr_runtime() const {
        return m_openxr.get();
    }

    runtimes::OpenVR* get_openvr_runtime() const {
        return m_openvr.get();
    }

    bool is_hmd_active() const {
        if (m_disable_vr) {
            return false;
        }

        auto runtime = get_runtime();

        if (runtime == nullptr) {
            return false;
        }

        return runtime->ready() || (m_stereo_emulation_mode && runtime->loaded);
    }

    auto get_hmd() const {
        return m_openvr->hmd;
    }

    auto& get_openvr_poses() const {
        return m_openvr->render_poses;
    }

    auto& get_overlay_component() {
        return m_overlay_component;
    }

    uint32_t get_hmd_width() const;
    uint32_t get_hmd_height() const;

    const auto& get_eyes() const {
        return get_runtime()->eyes;
    }

    auto get_frame_count() const {
        return m_frame_count;
    }

    auto& get_controllers() const {
        return m_controllers;
    }

    bool is_using_controllers() const {
        return m_controller_test_mode || (m_controllers_allowed->value() &&
        is_hmd_active() && !m_controllers.empty() && (std::chrono::steady_clock::now() - m_last_controller_update) <= std::chrono::seconds((int32_t)m_motion_controls_inactivity_timer->value()));
    }

    bool is_using_controllers_within(std::chrono::seconds seconds) const {
        return m_controllers_allowed->value() && is_hmd_active() && !m_controllers.empty() && (std::chrono::steady_clock::now() - m_last_controller_update) <= seconds;
    }

    int get_hmd_index() const {
        return 0;
    }

    int get_left_controller_index() const {
        const auto wants_swap = m_swap_controllers->value();

        if (m_runtime->is_openxr()) {
            return wants_swap ? 2 : 1;
        } else if (m_runtime->is_openvr()) {
            return !m_controllers.empty() ? (wants_swap ? m_controllers[1] : m_controllers[0]) : -1;
        }

        return -1;
    }

    int get_right_controller_index() const {
        const auto wants_swap = m_swap_controllers->value();

        if (m_runtime->is_openxr()) {
            return wants_swap ? 1 : 2;
        } else if (m_runtime->is_openvr()) {
            return !m_controllers.empty() ? (wants_swap ? m_controllers[0] : m_controllers[1]) : -1;
        }

        return -1;
    }

    auto get_left_joystick() const {
        if (!m_swap_controllers->value()) {
            return m_left_joystick;
        }

        return m_right_joystick;
    }

    auto get_right_joystick() const {
        if (!m_swap_controllers->value()) {
            return m_right_joystick;
        }

        return m_left_joystick;
    }

    bool is_gui_enabled() const {
        return m_enable_gui->value();
    }

    auto get_camera_forward_offset() const {
        return m_camera_forward_offset->value();
    }

    auto get_camera_right_offset() const {
        return m_camera_right_offset->value();
    }

    auto get_camera_up_offset() const {
        return m_camera_up_offset->value();
    }

    auto get_world_scale() const {
        return m_world_scale->value();
    }

    auto is_stereo_emulation_enabled() const {
        return m_stereo_emulation_mode;
    }

    void reset_present_event() {
        ResetEvent(m_present_finished_event);
    }

    void wait_for_present() {
        if (!m_wait_for_present) {
            return;
        }

        if (m_frame_count <= m_game_frame_count) {
            //return;
        }

        if (WaitForSingleObject(m_present_finished_event, 11) == WAIT_TIMEOUT) {
            //timed_out = true;
        }

        m_game_frame_count = m_frame_count;
        //ResetEvent(m_present_finished_event);
    }

    auto& get_vr_mutex() {
        return m_openvr_mtx;
    }

    bool is_using_afr() const {
        return m_rendering_method->value() == RenderingMethod::ALTERNATING || 
               m_rendering_method->value() == RenderingMethod::SYNCHRONIZED ||
               m_extreme_compat_mode->value() == true;
    }

    bool is_using_synchronized_afr() const {
        return m_rendering_method->value() == RenderingMethod::SYNCHRONIZED ||
               (m_extreme_compat_mode->value() && m_rendering_method->value() == RenderingMethod::NATIVE_STEREO);
    }

    SyncedSequentialMethod get_synced_sequential_method() const {
        return (SyncedSequentialMethod)m_synced_afr_method->value();
    }

    uint32_t get_lowest_xinput_index() const {
        return m_lowest_xinput_user_index;
    }

    auto& get_render_target_pool_hook() const {
        return m_render_target_pool_hook;
    }

    void set_world_to_meters(float value) {
        m_world_to_meters = value;
    }

    float get_world_to_meters() const {
        return m_world_to_meters * m_world_scale->value();
    }

    float get_depth_scale() const {
        return m_depth_scale->value();
    }

    bool is_depth_enabled() const {
        return m_enable_depth->value();
    }

    bool is_decoupled_pitch_enabled() const {
        return m_decoupled_pitch->value();
    }

    bool is_decoupled_pitch_ui_adjust_enabled() const {
        return m_decoupled_pitch_ui_adjust->value();
    }

    void set_decoupled_pitch(bool value) {
        m_decoupled_pitch->value() = value;
    }

    void set_aim_allowed(bool value) {
        m_aim_temp_disabled = !value;
    }

    bool is_aim_allowed() const {
        return !m_aim_temp_disabled;
    }

    AimMethod get_aim_method() const {
        if (m_aim_temp_disabled) {
            return AimMethod::GAME;
        }

        return (AimMethod)m_aim_method->value();
    }

    void set_aim_method(AimMethod method) {
        if ((size_t)method >= s_aim_method_names.size()) {
            method = AimMethod::GAME;
        }

        m_aim_method->value() = method;
    }

    AimMethod get_movement_orientation() const {
        return (AimMethod)m_movement_orientation->value();
    }

    float get_aim_speed() const {
        return m_aim_speed->value();
    }
    
    bool is_aim_multiplayer_support_enabled() const {
        return m_aim_multiplayer_support->value();
    }

    bool is_aim_pawn_control_rotation_enabled() const {
        return m_aim_use_pawn_control_rotation->value();
    }

    bool is_aim_modify_player_control_rotation_enabled() const {
        return m_aim_modify_player_control_rotation->value();
    }

    bool is_aim_interpolation_enabled() const {
        return m_aim_interp->value();
    }
    
    bool is_any_aim_method_active() const {
        return m_aim_method->value() > AimMethod::GAME && !m_aim_temp_disabled;
    }

    bool is_headlocked_aim_enabled() const {
        return m_aim_method->value() == AimMethod::HEAD && !m_aim_temp_disabled;
    }

    bool is_controller_aim_enabled() const {
        const auto value = m_aim_method->value();
        return !m_aim_temp_disabled && (value == AimMethod::LEFT_CONTROLLER || value == AimMethod::RIGHT_CONTROLLER || value == AimMethod::TWO_HANDED_LEFT || value == AimMethod::TWO_HANDED_RIGHT);
    }

    bool is_controller_movement_enabled() const {
        const auto value = m_movement_orientation->value();
        return value == AimMethod::LEFT_CONTROLLER || value == AimMethod::RIGHT_CONTROLLER || value == AimMethod::TWO_HANDED_LEFT || value == AimMethod::TWO_HANDED_RIGHT;
    }

    bool wants_blueprint_load() const {
        return m_load_blueprint_code->value();
    }

    bool is_splitscreen_compatibility_enabled() const {
        return m_splitscreen_compatibility_mode->value();
    }

    uint32_t get_requested_splitscreen_index() const {
        return m_splitscreen_view_index->value();
    }

    bool is_sceneview_compatibility_enabled() const {
        return m_sceneview_compatibility_mode->value();
    }

    bool is_native_stereo_fix_enabled() const {
        return m_native_stereo_fix->value() && !is_using_afr();
    }

    bool is_native_stereo_fix_same_pass_enabled() const {
        return m_native_stereo_fix_same_pass->value();
    }

    bool is_ahud_compatibility_enabled() const {
        return m_compatibility_ahud->value();
    }

    bool is_ghosting_fix_enabled() const {
        return m_ghosting_fix->value();
    }

    auto& get_fake_stereo_hook() {
        return m_fake_stereo_hook;
    }

    void set_pre_flattened_rotation(const glm::quat& rot) {
        std::unique_lock _{m_decoupled_pitch_data.mtx};
        m_decoupled_pitch_data.pre_flattened_rotation = rot;
    }

    auto get_pre_flattened_rotation() const {
        std::shared_lock _{m_decoupled_pitch_data.mtx};
        return m_decoupled_pitch_data.pre_flattened_rotation;
    }

    bool is_using_2d_screen() const {
        return m_2d_screen_mode->value();
    }

    bool is_roomscale_enabled() const {
        return m_roomscale_movement->value() && !m_aim_temp_disabled;
    }

    bool is_roomscale_sweep_enabled() const {
        return m_roomscale_sweep->value();
    }

    bool is_dpad_shifting_enabled() const {
        return m_dpad_shifting->value();
    }

    DPadMethod get_dpad_method() const {
        return (DPadMethod)m_dpad_shifting_method->value();
    }

    bool is_snapturn_enabled() const {
        return m_snapturn->value();
    }

    void set_snapturn_enabled(bool value) {
        m_snapturn->value() = value;
    }

    float get_snapturn_js_deadzone() const {
        return m_snapturn_joystick_deadzone->value();
    }

    int get_snapturn_angle() const {
        return m_snapturn_angle->value();
    }

    float get_controller_pitch_offset() const {
        return m_controller_pitch_offset->value();
    }

    bool should_skip_post_init_properties() const {
        return m_compatibility_skip_pip->value();
    }
    
    bool should_skip_uobjectarray_init() const {
        return m_compatibility_skip_uobjectarray_init->value();
    }

    bool is_extreme_compatibility_mode_enabled() const {
        return m_extreme_compat_mode->value();
    }

    auto get_horizontal_projection_override() const {
        return m_horizontal_projection_override->value();
    }

    auto get_vertical_projection_override() const {
        return m_vertical_projection_override->value();
    }

    bool should_grow_rectangle_for_projection_cropping() const {
        return m_grow_rectangle_for_projection_cropping->value();
    }

    vrmod::D3D11Component& d3d11() {
        return m_d3d11;
    }

    vrmod::D3D12Component& d3d12() {
        return m_d3d12;
    }

    uint32_t get_present_thread_id() const {
        return m_present_thread_id;
    }

private:
    Vector4f get_position_unsafe(uint32_t index) const;
    Vector4f get_velocity_unsafe(uint32_t index) const;
    Vector4f get_angular_velocity_unsafe(uint32_t index) const;

private:
    std::optional<std::string> initialize_openvr();
    std::optional<std::string> initialize_openvr_input();
    std::optional<std::string> initialize_openxr();
    std::optional<std::string> initialize_openxr_input();
    std::optional<std::string> initialize_openxr_swapchains();

    bool detect_controllers();
    bool is_any_action_down();

    std::optional<std::string> reinitialize_openvr() {
        spdlog::info("Reinitializing OpenVR");
        std::scoped_lock _{m_openvr_mtx};

        m_runtime.reset();
        m_runtime = std::make_shared<VRRuntime>();
        m_openvr.reset();

        // Reinitialize openvr input, hopefully this fixes the issue
        m_controllers.clear();
        m_controllers_set.clear();

        auto e = initialize_openvr();

        if (e) {
            spdlog::error("Failed to reinitialize OpenVR: {}", *e);
        }

        return e;
    }

    std::optional<std::string> reinitialize_openxr() {
        spdlog::info("Reinitializing OpenXR");
        std::scoped_lock _{m_openvr_mtx};

        if (m_is_d3d12) {
            m_d3d12.openxr().destroy_swapchains();
        } else {
            m_d3d11.openxr().destroy_swapchains();
        }

        m_openxr.reset();
        m_runtime.reset();
        m_runtime = std::make_shared<VRRuntime>();
        
        m_controllers.clear();
        m_controllers_set.clear();

        auto e = initialize_openxr();

        if (e) {
            spdlog::error("Failed to reinitialize OpenXR: {}", *e);
        }

        return e;
    }

    float m_nearz{ 0.1f };
    float m_farz{ 3000.0f };
    float m_world_to_meters{1.0f}; // Placeholder, it gets set later in a hook

    std::unique_ptr<FFakeStereoRenderingHook> m_fake_stereo_hook{ std::make_unique<FFakeStereoRenderingHook>() };
    std::unique_ptr<RenderTargetPoolHook> m_render_target_pool_hook{ std::make_unique<RenderTargetPoolHook>() };
    std::unique_ptr<CVarManager> m_cvar_manager{ std::make_unique<CVarManager>() };

    void add_components_vr() {
        m_components = {
            m_fake_stereo_hook.get(),
            m_render_target_pool_hook.get(),
            m_cvar_manager.get(),
            &m_overlay_component
        };
    }

    std::shared_ptr<VRRuntime> m_runtime{std::make_shared<VRRuntime>()}; // will point to the real runtime if it exists
    std::shared_ptr<runtimes::OpenVR> m_openvr{std::make_shared<runtimes::OpenVR>()};
    std::shared_ptr<runtimes::OpenXR> m_openxr{std::make_shared<runtimes::OpenXR>()};

    mutable TracyLockable(std::recursive_mutex, m_openvr_mtx);
    mutable TracyLockable(std::recursive_mutex, m_reinitialize_mtx);
    mutable TracyLockable(std::recursive_mutex, m_actions_mtx);
    mutable std::shared_mutex m_rotation_mtx{};

    std::vector<int32_t> m_controllers{};
    std::unordered_set<int32_t> m_controllers_set{};

    glm::vec3 m_overlay_rotation{-1.550f, 0.0f, -1.330f};
    glm::vec4 m_overlay_position{0.0f, 0.06f, -0.07f, 1.0f};
    
    Vector4f m_standing_origin{ 0.0f, 1.5f, 0.0f, 0.0f };
    glm::quat m_rotation_offset{ glm::identity<glm::quat>() };

    HANDLE m_present_finished_event{CreateEvent(nullptr, TRUE, FALSE, nullptr)};

    Vector4f m_raw_projections[2]{};

    vrmod::D3D11Component m_d3d11{};
    vrmod::D3D12Component m_d3d12{};
    vrmod::OverlayComponent m_overlay_component;
    bool m_disable_overlay{false};

    // Action set handles
    vr::VRActionSetHandle_t m_action_set{};
    vr::VRActiveActionSet_t m_active_action_set{};

    // Action handles
    vr::VRActionHandle_t m_action_pose{ };
    vr::VRActionHandle_t m_action_trigger{ };
    vr::VRActionHandle_t m_action_grip{ };
    vr::VRActionHandle_t m_action_grip_pose{ };
    vr::VRActionHandle_t m_action_joystick{};
    vr::VRActionHandle_t m_action_joystick_click{};

    vr::VRActionHandle_t m_action_a_button_right{};
    vr::VRActionHandle_t m_action_a_button_touch_right{};
    vr::VRActionHandle_t m_action_b_button_right{};
    vr::VRActionHandle_t m_action_b_button_touch_right{};

    vr::VRActionHandle_t m_action_a_button_left{};
    vr::VRActionHandle_t m_action_a_button_touch_left{};
    vr::VRActionHandle_t m_action_b_button_left{};
    vr::VRActionHandle_t m_action_b_button_touch_left{};

    vr::VRActionHandle_t m_action_dpad_up{};
    vr::VRActionHandle_t m_action_dpad_right{};
    vr::VRActionHandle_t m_action_dpad_down{};
    vr::VRActionHandle_t m_action_dpad_left{};

    vr::VRActionHandle_t m_action_system_button{};
    vr::VRActionHandle_t m_action_haptic{};
    vr::VRActionHandle_t m_action_thumbrest_touch_left{};
    vr::VRActionHandle_t m_action_thumbrest_touch_right{};

    std::unordered_map<std::string, std::reference_wrapper<vr::VRActionHandle_t>> m_action_handles {
        { s_action_pose, m_action_pose },
        { s_action_grip_pose, m_action_grip_pose },
        { s_action_trigger, m_action_trigger },
        { s_action_grip, m_action_grip },
        { s_action_joystick, m_action_joystick },
        { s_action_joystick_click, m_action_joystick_click },

        { s_action_a_button_left, m_action_a_button_left },
        { s_action_b_button_left, m_action_b_button_left },
        { s_action_a_button_touch_left, m_action_a_button_touch_left },
        { s_action_b_button_touch_left, m_action_b_button_touch_left },

        { s_action_a_button_right, m_action_a_button_right },
        { s_action_b_button_right, m_action_b_button_right },
        { s_action_a_button_touch_right, m_action_a_button_touch_right },
        { s_action_b_button_touch_right, m_action_b_button_touch_right },

        { s_action_dpad_up, m_action_dpad_up },
        { s_action_dpad_right, m_action_dpad_right },
        { s_action_dpad_down, m_action_dpad_down },
        { s_action_dpad_left, m_action_dpad_left },

        { s_action_system_button, m_action_system_button },
        { s_action_thumbrest_touch_left, m_action_thumbrest_touch_left },
        { s_action_thumbrest_touch_right, m_action_thumbrest_touch_right },

        // Out
        { "/actions/default/out/Haptic", m_action_haptic },
    };

    // Input sources
    vr::VRInputValueHandle_t m_left_joystick{};
    vr::VRInputValueHandle_t m_right_joystick{};

    std::chrono::steady_clock::time_point m_last_controller_update{};
    std::chrono::steady_clock::time_point m_last_xinput_update{};
    std::chrono::steady_clock::time_point m_last_xinput_spoof_sent{};
    std::chrono::steady_clock::time_point m_last_xinput_l3_r3_menu_open{};
    std::chrono::steady_clock::time_point m_last_interaction_display{};
    std::chrono::steady_clock::time_point m_last_engine_tick{};

    uint32_t m_lowest_xinput_user_index{};

    std::chrono::nanoseconds m_last_input_delay{};
    std::chrono::nanoseconds m_avg_input_delay{};

    static const inline std::vector<std::string> s_rendering_method_names {
        "Native Stereo",
        "Synchronized Sequential",
        "Alternating/AFR",
    };

    static const inline std::vector<std::string> s_synced_afr_method_names {
        "Skip Tick",
        "Skip Draw",
    };

    static const inline std::vector<std::string> s_aim_method_names {
        "Game",
        "Head/HMD",
        "Right Controller",
        "Left Controller",
        "Two Handed (Right)",
        "Two Handed (Left)",
    };

    static const inline std::vector<std::string> s_dpad_method_names {
        "Right Thumbrest + Left Joystick",
        "Left Thumbrest + Right Joystick",
        "Left Joystick (Disables Standard Joystick Input)",
        "Right Joystick (Disables Standard Joystick Input)",
        "Gesture (Head) + Left Joystick",
        "Gesture (Head) + Right Joystick",
    };

    static const inline std::vector<std::string> s_horizontal_projection_override_names{
        "Raw / default",
        "Symmetrical",
        "Mirrored",
    };

    static const inline std::vector<std::string> s_vertical_projection_override_names{
        "Raw / default",
        "Symmetrical",
        "Matched",
    };

    const ModCombo::Ptr m_rendering_method{ ModCombo::create(generate_name("RenderingMethod"), s_rendering_method_names) };
    const ModCombo::Ptr m_synced_afr_method{ ModCombo::create(generate_name("SyncedSequentialMethod"), s_synced_afr_method_names, 1) };
    const ModToggle::Ptr m_extreme_compat_mode{ ModToggle::create(generate_name("ExtremeCompatibilityMode"), false, true) };
    const ModToggle::Ptr m_uncap_framerate{ ModToggle::create(generate_name("UncapFramerate"), true) };
    const ModToggle::Ptr m_disable_blur_widgets{ ModToggle::create(generate_name("DisableBlurWidgets"), true) };
    const ModToggle::Ptr m_disable_hdr_compositing{ ModToggle::create(generate_name("DisableHDRCompositing"), true, true) };
    const ModToggle::Ptr m_disable_hzbocclusion{ ModToggle::create(generate_name("DisableHZBOcclusion"), true, true) };
    const ModToggle::Ptr m_disable_instance_culling{ ModToggle::create(generate_name("DisableInstanceCulling"), true, true) };
    const ModToggle::Ptr m_desktop_fix{ ModToggle::create(generate_name("DesktopRecordingFix_V2"), true) };
    const ModToggle::Ptr m_enable_gui{ ModToggle::create(generate_name("EnableGUI"), true) };
    const ModToggle::Ptr m_enable_depth{ ModToggle::create(generate_name("PassDepthToRuntime"), false, true) };
    const ModToggle::Ptr m_decoupled_pitch{ ModToggle::create(generate_name("DecoupledPitch"), false) };
    const ModToggle::Ptr m_decoupled_pitch_ui_adjust{ ModToggle::create(generate_name("DecoupledPitchUIAdjust"), true) };
    const ModToggle::Ptr m_load_blueprint_code{ ModToggle::create(generate_name("LoadBlueprintCode"), false, true) };
    const ModToggle::Ptr m_2d_screen_mode{ ModToggle::create(generate_name("2DScreenMode"), false) };
    const ModToggle::Ptr m_roomscale_movement{ ModToggle::create(generate_name("RoomscaleMovement"), false) };
    const ModToggle::Ptr m_roomscale_sweep{ ModToggle::create(generate_name("RoomscaleMovementSweep"), true) };
    const ModToggle::Ptr m_swap_controllers{ ModToggle::create(generate_name("SwapControllerInputs"), false) };
    const ModCombo::Ptr m_horizontal_projection_override{ModCombo::create(generate_name("HorizontalProjectionOverride"), s_horizontal_projection_override_names)};
    const ModCombo::Ptr m_vertical_projection_override{ModCombo::create(generate_name("VerticalProjectionOverride"), s_vertical_projection_override_names)};
    const ModToggle::Ptr m_grow_rectangle_for_projection_cropping{ModToggle::create(generate_name("GrowRectangleForProjectionCropping"), false)};

    // Snap turn settings and globals
    void gamepad_snapturn(XINPUT_STATE& state);
    void process_snapturn();
    
    const ModToggle::Ptr m_snapturn{ ModToggle::create(generate_name("SnapTurn"), false) };
    const ModSlider::Ptr m_snapturn_joystick_deadzone{ ModSlider::create(generate_name("SnapturnJoystickDeadzone"), 0.01f, 0.99f, 0.2f) };
    const ModInt32::Ptr m_snapturn_angle{ ModSliderInt32::create(generate_name("SnapturnTurnAngle"), 1, 359, 45) };
    bool m_snapturn_on_frame{false};
    bool m_snapturn_left{false};
    bool m_was_snapturn_run_on_input{false};

    const ModSlider::Ptr m_controller_pitch_offset{ ModSlider::create(generate_name("ControllerPitchOffset"), -90.0f, 90.0f, 0.0f) };

    // Aim method and movement orientation are not the same thing, but they can both have the same options
    const ModCombo::Ptr m_aim_method{ ModCombo::create(generate_name("AimMethod"), s_aim_method_names, AimMethod::GAME) };
    const ModCombo::Ptr m_movement_orientation{ ModCombo::create(generate_name("MovementOrientation"), s_aim_method_names, AimMethod::GAME) };
    AimMethod m_previous_aim_method{ AimMethod::GAME };
    const ModToggle::Ptr m_aim_use_pawn_control_rotation{ ModToggle::create(generate_name("AimUsePawnControlRotation"), false) };
    const ModToggle::Ptr m_aim_modify_player_control_rotation{ ModToggle::create(generate_name("AimModifyPlayerControlRotation"), false) };
    const ModToggle::Ptr m_aim_multiplayer_support{ ModToggle::create(generate_name("AimMPSupport"), false) };
    const ModToggle::Ptr m_aim_interp{ ModToggle::create(generate_name("AimInterp"), true, true) };
    const ModSlider::Ptr m_aim_speed{ ModSlider::create(generate_name("AimSpeed"), 0.01f, 25.0f, 15.0f) };
    const ModToggle::Ptr m_dpad_shifting{ ModToggle::create(generate_name("DPadShifting"), true) };
    const ModCombo::Ptr m_dpad_shifting_method{ ModCombo::create(generate_name("DPadShiftingMethod"), s_dpad_method_names, DPadMethod::RIGHT_TOUCH) };
    
    struct DPadGestureState {
        std::recursive_mutex mtx{};
        enum Direction : uint8_t {
            NONE,
            UP = 1 << 0,
            RIGHT = 1 << 1,
            DOWN = 1 << 2,
            LEFT = 1 << 3,
        };
        uint8_t direction{NONE};
    } m_dpad_gesture_state{};

    //const ModToggle::Ptr m_headlocked_aim{ ModToggle::create(generate_name("HeadLockedAim"), false) };
    //const ModToggle::Ptr m_headlocked_aim_controller_based{ ModToggle::create(generate_name("HeadLockedAimControllerBased"), false) };
    const ModSlider::Ptr m_motion_controls_inactivity_timer{ ModSlider::create(generate_name("MotionControlsInactivityTimer"), 30.0f, 100.0f, 30.0f) };
    const ModSlider::Ptr m_joystick_deadzone{ ModSlider::create(generate_name("JoystickDeadzone"), 0.01f, 0.9f, 0.2f) };
    const ModSlider::Ptr m_camera_forward_offset{ ModSlider::create(generate_name("CameraForwardOffset"), -4000.0f, 4000.0f, 0.0f) };
    const ModSlider::Ptr m_camera_right_offset{ ModSlider::create(generate_name("CameraRightOffset"), -4000.0f, 4000.0f, 0.0f) };
    const ModSlider::Ptr m_camera_up_offset{ ModSlider::create(generate_name("CameraUpOffset"), -4000.0f, 4000.0f, 0.0f) };
    const ModSlider::Ptr m_camera_fov_distance_multiplier{ ModSlider::create(generate_name("CameraFOVDistanceMultiplier"), 0.00f, 1000.0f, 0.0f) };
    const ModSlider::Ptr m_world_scale{ ModSlider::create(generate_name("WorldScale"), 0.01f, 10.0f, 1.0f) };
    const ModSlider::Ptr m_depth_scale{ ModSlider::create(generate_name("DepthScale"), 0.01f, 1.0f, 1.0f) };

    const ModToggle::Ptr m_ghosting_fix{ ModToggle::create(generate_name("GhostingFix"), false) };
    const ModToggle::Ptr m_native_stereo_fix{ ModToggle::create(generate_name("NativeStereoFix"), false) };
    const ModToggle::Ptr m_native_stereo_fix_same_pass{ ModToggle::create(generate_name("NativeStereoFixSamePass"), true) };

    const ModSlider::Ptr m_custom_z_near{ ModSlider::create(generate_name("CustomZNear"), 0.001f, 100.0f, 0.01f, true) };
    const ModToggle::Ptr m_custom_z_near_enabled{ ModToggle::create(generate_name("EnableCustomZNear"), false, true) };

    const ModToggle::Ptr m_splitscreen_compatibility_mode{ ModToggle::create(generate_name("Compatibility_SplitScreen"), false, true) };
    const ModInt32::Ptr m_splitscreen_view_index{ ModInt32::create(generate_name("SplitscreenViewIndex"), 0, true) };

    const ModToggle::Ptr m_sceneview_compatibility_mode{ ModToggle::create(generate_name("Compatibility_SceneView"), false, true) };

    const ModToggle::Ptr m_compatibility_skip_pip{ ModToggle::create(generate_name("Compatibility_SkipPostInitProperties"), false, true) };
    const ModToggle::Ptr m_compatibility_skip_uobjectarray_init{ ModToggle::create(generate_name("Compatibility_SkipUObjectArrayInit"), false, true) };

    const ModToggle::Ptr m_compatibility_ahud{ ModToggle::create(generate_name("Compatibility_AHUD"), false, true) };

    // Keybinds
    const ModKey::Ptr m_keybind_recenter{ ModKey::create(generate_name("RecenterViewKey")) };
    const ModKey::Ptr m_keybind_recenter_horizon{ ModKey::create(generate_name("RecenterHorizonKey")) };
    const ModKey::Ptr m_keybind_set_standing_origin{ ModKey::create(generate_name("ResetStandingOriginKey")) };

    const ModKey::Ptr m_keybind_load_camera_0{ ModKey::create(generate_name("LoadCamera0Key")) };
    const ModKey::Ptr m_keybind_load_camera_1{ ModKey::create(generate_name("LoadCamera1Key")) };
    const ModKey::Ptr m_keybind_load_camera_2{ ModKey::create(generate_name("LoadCamera2Key")) };

    const ModKey::Ptr m_keybind_toggle_2d_screen{ ModKey::create(generate_name("Toggle2DScreenKey")) };
    const ModKey::Ptr m_keybind_disable_vr{ ModKey::create(generate_name("DisableVRKey")) };
    bool m_disable_vr{false}; // definitely should not be persistent

    const ModKey::Ptr m_keybind_toggle_gui{ ModKey::create(generate_name("ToggleSlateGUIKey")) };
    
    const ModString::Ptr m_requested_runtime_name{ ModString::create("Frontend_RequestedRuntime", "unset") };

    const ModToggle::Ptr m_lerp_camera_pitch{ ModToggle::create(generate_name("LerpCameraPitch"), false) };
    const ModToggle::Ptr m_lerp_camera_yaw{ ModToggle::create(generate_name("LerpCameraYaw"), false) };
    const ModToggle::Ptr m_lerp_camera_roll{ ModToggle::create(generate_name("LerpCameraRoll"), false) };
    const ModSlider::Ptr m_lerp_camera_speed{ ModSlider::create(generate_name("LerpCameraSpeed"), 0.01f, 10.0f, 1.0f) };

    std::chrono::high_resolution_clock::time_point m_last_lerp_update{};

    struct DecoupledPitchData {
        mutable std::shared_mutex mtx{};
        glm::quat pre_flattened_rotation{};
    } m_decoupled_pitch_data{};

    struct CameraFreeze {
        glm::vec3 position{};
        glm::vec3 rotation{}; // euler
        bool position_frozen{false};
        bool rotation_frozen{false};

        bool position_wants_freeze{false};
        bool rotation_wants_freeze{false};
    } m_camera_freeze{};

    struct CameraLerp {
        glm::vec3 last_position{};
        glm::vec3 last_rotation{};
    } m_camera_lerp{};

    struct CameraData {
        glm::vec3 offset{};
        float world_scale{1.0f};
        bool decoupled_pitch{false};
        bool decoupled_pitch_ui_adjust{true};
    };
    std::array<CameraData, 3> m_camera_datas{};
    void save_cameras();
    void load_cameras();
    void load_camera(int index);
    void save_camera(int index);

public:
    VR() {
        m_options = {
            *m_rendering_method,
            *m_synced_afr_method,
            *m_extreme_compat_mode,
            *m_uncap_framerate,
            *m_disable_hdr_compositing,
            *m_disable_hzbocclusion,
            *m_disable_instance_culling,
            *m_desktop_fix,
            *m_enable_gui,
            *m_enable_depth,
            *m_decoupled_pitch,
            *m_decoupled_pitch_ui_adjust,
            *m_load_blueprint_code,
            *m_2d_screen_mode,
            *m_roomscale_movement,
            *m_roomscale_sweep,
            *m_swap_controllers,
            *m_horizontal_projection_override,
            *m_vertical_projection_override,
            *m_grow_rectangle_for_projection_cropping,
            *m_snapturn,
            *m_snapturn_joystick_deadzone,
            *m_snapturn_angle,
            *m_controller_pitch_offset,
            *m_aim_method,
            *m_movement_orientation,
            *m_aim_use_pawn_control_rotation,
            *m_aim_modify_player_control_rotation,
            *m_aim_multiplayer_support,
            *m_aim_speed,
            *m_aim_interp,
            *m_dpad_shifting,
            *m_dpad_shifting_method,
            *m_motion_controls_inactivity_timer,
            *m_joystick_deadzone,
            *m_camera_forward_offset,
            *m_camera_right_offset,
            *m_camera_up_offset,
            *m_world_scale,
            *m_depth_scale,
            *m_custom_z_near,
            *m_custom_z_near_enabled,
            *m_ghosting_fix,
            *m_native_stereo_fix,
            *m_native_stereo_fix_same_pass,
            *m_splitscreen_compatibility_mode,
            *m_splitscreen_view_index,
            *m_compatibility_skip_pip,
            *m_compatibility_skip_uobjectarray_init,
            *m_compatibility_ahud,
            *m_sceneview_compatibility_mode,
            *m_keybind_recenter,
            *m_keybind_recenter_horizon,
            *m_keybind_set_standing_origin,
            *m_keybind_load_camera_0,
            *m_keybind_load_camera_1,
            *m_keybind_load_camera_2,
            *m_keybind_toggle_2d_screen,
            *m_keybind_disable_vr,
            *m_keybind_toggle_gui,
            *m_requested_runtime_name,
            *m_show_fps,
            *m_show_statistics,
            *m_controllers_allowed,
            *m_lerp_camera_pitch,
            *m_lerp_camera_yaw,
            *m_lerp_camera_roll,
            *m_lerp_camera_speed,
        };

        add_components_vr();
    }

private:
    bool m_stereo_emulation_mode{false}; // not a good config option, just for debugging
    bool m_wait_for_present{true};
    const ModToggle::Ptr m_controllers_allowed{ ModToggle::create(generate_name("ControllersAllowed"), true) };
    bool m_controller_test_mode{false};
    
    const ModToggle::Ptr m_show_fps{ ModToggle::create(generate_name("ShowFPSOverlay"), false) };
    bool m_show_fps_state{false};

    const ModToggle::Ptr m_show_statistics{ ModToggle::create(generate_name("ShowStatsOverlay"), false) };
    bool m_show_statistics_state{false};

    void update_statistics_overlay(sdk::UGameEngine* engine);

    int m_game_frame_count{};
    int m_frame_count{};
    int m_render_frame_count{};
    int m_last_frame_count{-1};
    int m_left_eye_frame_count{0};
    int m_right_eye_frame_count{0};

    bool m_submitted{false};

    // == 1 or == 0
    uint8_t m_left_eye_interval{0};
    uint8_t m_right_eye_interval{1};

    bool m_first_config_load{true};
    bool m_first_submit{true};
    bool m_is_d3d12{false};
    bool m_backbuffer_inconsistency{false};
    bool m_init_finished{false};
    bool m_has_hw_scheduling{false}; // hardware accelerated GPU scheduling
    bool m_spoofed_gamepad_connection{false};
    bool m_aim_temp_disabled{false};

    struct {
        bool draw{false};
        bool was_moving_left{false};
        bool was_moving_right{false};
        uint8_t page{0};
        uint8_t num_pages{3};
    } m_rt_modifier{};

    bool m_disable_projection_matrix_override{ false };
    bool m_disable_view_matrix_override{false};
    bool m_disable_backbuffer_size_override{false};

    uint32_t m_present_thread_id{};

    struct XInputContext {
        struct PadContext {
            using Func = std::function<void(const XINPUT_STATE&, bool is_vr_controller)>;
            std::optional<Func> update{};
            XINPUT_STATE state{};
        };

        PadContext gamepad{};
        PadContext vr_controller{};
        
        TracyLockable(std::recursive_mutex, mtx);

        struct VRState {
            class StickState {
            public:
                bool was_pressed(bool current_state) {
                    if (!current_state) {
                        is_pressed = false;
                        return false;
                    }

                    const auto now = std::chrono::steady_clock::now();
                    if (is_pressed && now - initial_press > std::chrono::milliseconds(500)) {
                        return true;
                    }

                    if (!is_pressed) {
                        initial_press = now;
                        is_pressed = true;
                        return true;
                    }

                    return false;
                } 
            
            private:
                std::chrono::steady_clock::time_point initial_press{};
                bool is_pressed{false};
            };

            StickState left_stick_up{};
            StickState left_stick_down{};
            StickState left_stick_left{};
            StickState left_stick_right{};
        } vr;

        void enqueue(bool is_vr_controller, const XINPUT_STATE& in_state, PadContext::Func func) {
            ZoneScopedN(__FUNCTION__);

            std::scoped_lock _{mtx};
            if (is_vr_controller) {
                vr_controller.update = func;
                vr_controller.state = in_state;
            } else {
                gamepad.update = func;
                gamepad.state = in_state;
            }
        }

        void update() {
            ZoneScopedN(__FUNCTION__);

            std::scoped_lock _{mtx};

            if (vr_controller.update) {
                (*vr_controller.update)(vr_controller.state, true);
                vr_controller.update.reset();
            }

            if (gamepad.update) {
                (*gamepad.update)(gamepad.state, false);
                gamepad.update.reset();
            }
        }

        bool headlocked_begin_held{false};
        bool menu_longpress_begin_held{false};
        std::chrono::steady_clock::time_point headlocked_begin{};
        std::chrono::steady_clock::time_point menu_longpress_begin{};
    } m_xinput_context{};

    static std::string actions_json;
    static std::string binding_rift_json;
    static std::string bindings_oculus_touch_json;
    static std::string binding_vive;
    static std::string bindings_vive_controller;
    static std::string bindings_knuckles;

    const std::unordered_map<std::string, std::string> m_binding_files {
        { "actions.json", actions_json },
        { "binding_rift.json", binding_rift_json },
        { "bindings_oculus_touch.json", bindings_oculus_touch_json },
        { "binding_vive.json", binding_vive },
        { "bindings_vive_controller.json", bindings_vive_controller },
        { "bindings_knuckles.json", bindings_knuckles }
    };

    friend class vrmod::D3D11Component;
    friend class vrmod::D3D12Component;
    friend class vrmod::OverlayComponent;
    friend class FFakeStereoRenderingHook;
};

`

---
