/*
 * UEVR ImGui Theme Helpers
 * 
 * Comprehensive theme management system for ImGui
 * 
 * Copyright (c) 2024 UEVR Project
 * Licensed under MIT License
 */

#pragma once

#include <imgui.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>

namespace uevr {
namespace ImGui {

// Color scheme definitions
struct ColorScheme {
    std::string name;
    std::string description;
    
    // Core colors
    ImVec4 text;
    ImVec4 text_disabled;
    ImVec4 window_bg;
    ImVec4 child_bg;
    ImVec4 popup_bg;
    ImVec4 border;
    ImVec4 border_shadow;
    
    // Widget colors
    ImVec4 frame_bg;
    ImVec4 frame_bg_hovered;
    ImVec4 frame_bg_active;
    ImVec4 title_bg;
    ImVec4 title_bg_active;
    ImVec4 title_bg_collapsed;
    
    // Button colors
    ImVec4 button;
    ImVec4 button_hovered;
    ImVec4 button_active;
    
    // Header colors
    ImVec4 header;
    ImVec4 header_hovered;
    ImVec4 header_active;
    
    // Selection colors
    ImVec4 selection;
    ImVec4 selection_hovered;
    
    // Slider colors
    ImVec4 slider_grab;
    ImVec4 slider_grab_active;
    
    // Scrollbar colors
    ImVec4 scrollbar_bg;
    ImVec4 scrollbar_grab;
    ImVec4 scrollbar_grab_hovered;
    ImVec4 scrollbar_grab_active;
    
    // Tab colors
    ImVec4 tab;
    ImVec4 tab_hovered;
    ImVec4 tab_active;
    
    // Plot colors
    ImVec4 plot_lines;
    ImVec4 plot_lines_hovered;
    ImVec4 plot_histogram;
    ImVec4 plot_histogram_hovered;
    
    // Table colors
    ImVec4 table_header_bg;
    ImVec4 table_border_light;
    ImVec4 table_border_strong;
    ImVec4 table_row_bg;
    ImVec4 table_row_bg_alt;
    
    // Drag and drop colors
    ImVec4 drag_drop_target;
    ImVec4 nav_highlight;
    ImVec4 nav_windowing_highlight;
    ImVec4 nav_windowing_dim_bg;
    
    // Modal colors
    ImVec4 modal_window_dim_bg;
};

// Style configuration
struct StyleConfig {
    std::string name;
    std::string description;
    
    // Spacing
    float window_padding_x;
    float window_padding_y;
    float window_title_align_x;
    float window_title_align_y;
    float window_rounding;
    float window_border_size;
    
    // Child windows
    float child_rounding;
    float child_border_size;
    float popup_rounding;
    float popup_border_size;
    
    // Frame padding
    float frame_padding_x;
    float frame_padding_y;
    float frame_rounding;
    float frame_border_size;
    
    // Item spacing
    float item_spacing_x;
    float item_spacing_y;
    float item_inner_spacing_x;
    float item_inner_spacing_y;
    
    // Indent
    float indent_spacing;
    float columns_min_spacing;
    
    // Scrollbar
    float scrollbar_size;
    float scrollbar_rounding;
    float grab_min_size;
    float grab_rounding;
    
    // Tab
    float tab_rounding;
    float tab_border_size;
    float tab_indent;
    float tab_min_width_for_close_button;
    
    // Button
    float button_text_align_x;
    float button_text_align_y;
    
    // Selectable
    float selectable_text_align_x;
    float selectable_text_align_y;
    
    // Display
    float display_safe_area_padding_x;
    float display_safe_area_padding_y;
    
    // Mouse cursor
    bool mouse_cursor_scale;
    bool anti_aliased_lines;
    bool anti_aliased_lines_use_tex;
    bool anti_aliased_fill;
    bool curve_tessellation_tol;
    float circle_tessellation_max_error;
};

// Theme preset
struct ThemePreset {
    std::string name;
    std::string description;
    std::string author;
    std::string version;
    ColorScheme colors;
    StyleConfig style;
    std::vector<std::string> tags;
    std::unordered_map<std::string, std::string> metadata;
};

// Theme manager class
class ThemeManager {
public:
    // Singleton access
    static ThemeManager& getInstance();
    
    // Constructor and destructor
    ThemeManager();
    ~ThemeManager();
    
    // Copy and move operations
    ThemeManager(const ThemeManager&) = delete;
    ThemeManager& operator=(const ThemeManager&) = delete;
    ThemeManager(ThemeManager&&) = delete;
    ThemeManager& operator=(ThemeManager&&) = delete;
    
    // Theme management
    bool loadTheme(const std::string& theme_name);
    bool saveTheme(const std::string& theme_name);
    bool createTheme(const std::string& theme_name, const ThemePreset& preset);
    bool deleteTheme(const std::string& theme_name);
    
    // Theme application
    void applyTheme(const std::string& theme_name);
    void applyTheme(const ThemePreset& preset);
    void applyColorScheme(const ColorScheme& colors);
    void applyStyleConfig(const StyleConfig& style);
    
    // Theme information
    std::vector<std::string> getAvailableThemes() const;
    ThemePreset getTheme(const std::string& theme_name) const;
    std::string getCurrentTheme() const;
    
    // Built-in themes
    void applyDefaultTheme();
    void applyDarkTheme();
    void applyLightTheme();
    void applyClassicTheme();
    void applyCorporateTheme();
    void applyGamingTheme();
    void applyMinimalTheme();
    void applyColorfulTheme();
    
    // Customization
    void setCustomColor(const std::string& color_name, const ImVec4& color);
    void setCustomStyle(const std::string& style_name, float value);
    void resetToDefault();
    
    // Theme export/import
    bool exportTheme(const std::string& theme_name, const std::string& file_path);
    bool importTheme(const std::string& file_path);
    
    // Theme validation
    bool validateTheme(const ThemePreset& preset) const;
    std::vector<std::string> getThemeIssues(const ThemePreset& preset) const;
    
    // Dynamic theme switching
    void enableDynamicThemes(bool enable);
    bool isDynamicThemesEnabled() const;
    void setThemeTransitionDuration(float duration);
    
    // Theme callbacks
    using ThemeChangeCallback = std::function<void(const std::string&, const ThemePreset&)>;
    void setThemeChangeCallback(ThemeChangeCallback callback);
    void clearThemeChangeCallback();

protected:
    // Internal theme management
    bool loadThemeInternal(const std::string& theme_name);
    bool saveThemeInternal(const std::string& theme_name);
    void notifyThemeChange(const std::string& theme_name, const ThemePreset& preset);
    
    // Theme file operations
    bool loadThemeFromFile(const std::string& file_path, ThemePreset& preset);
    bool saveThemeToFile(const std::string& file_path, const ThemePreset& preset);
    
    // Built-in theme creation
    ThemePreset createDefaultTheme();
    ThemePreset createDarkTheme();
    ThemePreset createLightTheme();
    ThemePreset createClassicTheme();
    ThemePreset createCorporateTheme();
    ThemePreset createGamingTheme();
    ThemePreset createMinimalTheme();
    ThemePreset createColorfulTheme();

private:
    // Theme storage
    std::unordered_map<std::string, ThemePreset> m_themes;
    std::string m_current_theme;
    
    // Configuration
    bool m_dynamic_themes_enabled;
    float m_theme_transition_duration;
    
    // Callbacks
    ThemeChangeCallback m_theme_change_callback;
    
    // Internal state
    bool m_initialized;
};

// Utility functions
namespace ThemeUtils {
    
    // Color manipulation
    ImVec4 adjustBrightness(const ImVec4& color, float factor);
    ImVec4 adjustSaturation(const ImVec4& color, float factor);
    ImVec4 adjustContrast(const ImVec4& color, float factor);
    ImVec4 blendColors(const ImVec4& color1, const ImVec4& color2, float blend_factor);
    ImVec4 createAccentColor(const ImVec4& base_color);
    
    // Style manipulation
    float adjustSpacing(float base_spacing, float factor);
    float adjustRounding(float base_rounding, float factor);
    float adjustBorderSize(float base_border_size, float factor);
    
    // Theme generation
    ThemePreset generateThemeFromImage(const std::string& image_path);
    ThemePreset generateThemeFromColors(const std::vector<ImVec4>& colors);
    ThemePreset generateThemeFromPalette(const std::string& palette_name);
    
    // Theme analysis
    float calculateThemeContrast(const ThemePreset& theme);
    float calculateThemeBrightness(const ThemePreset& theme);
    float calculateThemeSaturation(const ThemePreset& theme);
    std::vector<std::string> getThemeColorNames(const ThemePreset& theme);
    
    // Accessibility
    bool isThemeAccessible(const ThemePreset& theme);
    ThemePreset makeThemeAccessible(const ThemePreset& theme);
    std::vector<std::string> getAccessibilityIssues(const ThemePreset& theme);
    
    // Export formats
    std::string exportThemeToCSS(const ThemePreset& theme);
    std::string exportThemeToJSON(const ThemePreset& theme);
    std::string exportThemeToXML(const ThemePreset& theme);
    
    // Import formats
    bool importThemeFromCSS(const std::string& css_content, ThemePreset& theme);
    bool importThemeFromJSON(const std::string& json_content, ThemePreset& theme);
    bool importThemeFromXML(const std::string& xml_content, ThemePreset& theme);
}

// Quick theme application functions
void applyTheme(const std::string& theme_name);
void applyDarkTheme();
void applyLightTheme();
void applyClassicTheme();
void applyCorporateTheme();
void applyGamingTheme();
void applyMinimalTheme();
void applyColorfulTheme();

// Theme presets
extern const ThemePreset DEFAULT_THEME;
extern const ThemePreset DARK_THEME;
extern const ThemePreset LIGHT_THEME;
extern const ThemePreset CLASSIC_THEME;
extern const ThemePreset CORPORATE_THEME;
extern const ThemePreset GAMING_THEME;
extern const ThemePreset MINIMAL_THEME;
extern const ThemePreset COLORFUL_THEME;

} // namespace ImGui
} // namespace uevr
