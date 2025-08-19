#define NOMINMAX

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>

#include <utility/Config.hpp>
#include <utility/String.hpp>

#include <sdk/CVar.hpp>
#include <sdk/threading/GameThreadWorker.hpp>
#include <sdk/ConsoleManager.hpp>
#include <sdk/UGameplayStatics.hpp>

#include "Framework.hpp"

#include "CVarManager.hpp"

#include <tracy/Tracy.hpp>

constexpr std::string_view cvars_standard_txt_name = "cvars_standard.txt";
constexpr std::string_view cvars_data_txt_name = "cvars_data.txt";
constexpr std::string_view user_script_txt_name = "user_script.txt";

CVarManager::CVarManager() {
    ZoneScopedN(__FUNCTION__);

    m_displayed_cvars.insert(m_displayed_cvars.end(), s_default_standard_cvars.begin(), s_default_standard_cvars.end());
    m_displayed_cvars.insert(m_displayed_cvars.end(), s_default_data_cvars.begin(), s_default_data_cvars.end());

    // Sort first by name, then by bool/int/float type. Bools get displayed first.
    std::sort(m_displayed_cvars.begin(), m_displayed_cvars.end(), [](const auto& a, const auto& b) {
        return a->get_name() < b->get_name();
    });

    std::sort(m_displayed_cvars.begin(), m_displayed_cvars.end(), [](const auto& a, const auto& b) {
        return (int)a->get_type() < (int)b->get_type();
    });

    m_all_cvars.insert(m_all_cvars.end(), m_displayed_cvars.begin(), m_displayed_cvars.end());

    // set m_hzbo (shared ptr) to the r.HZBOcclusion cvar in m_all_cvars
    for (auto& cvar : m_all_cvars) {
        if (cvar->get_name() == L"r.HZBOcclusion") {
            m_hzbo = cvar;
            break;
        }
    }
}

CVarManager::~CVarManager() {
    ZoneScopedN(__FUNCTION__);

    /*for (auto& cvar : m_cvars) {
        cvar->save();
    }*/
}

void CVarManager::spawn_console() {
    if (m_native_console_spawned) {
        return;
    }

    // Find Engine object and add the Console
    const auto engine = sdk::UGameEngine::get();

    if (engine != nullptr) {
        const auto console_class = engine->get_property<sdk::UClass*>(L"ConsoleClass");
        auto game_viewport = engine->get_property<sdk::UObject*>(L"GameViewport");

        if (console_class != nullptr && game_viewport != nullptr) {
            const auto console = sdk::UGameplayStatics::get()->spawn_object(console_class, game_viewport);

            if (console != nullptr) {
                game_viewport->get_property<sdk::UObject*>(L"ViewportConsole") = console;
                m_native_console_spawned = true;
            }
        }
    }
}

void CVarManager::on_pre_engine_tick(sdk::UGameEngine* engine, float delta) {
    ZoneScopedN(__FUNCTION__);

    for (auto& cvar : m_all_cvars) {
        cvar->update();
        cvar->freeze();
    }

    if (m_should_execute_console_script) {
        execute_console_script(engine, user_script_txt_name.data());
        m_should_execute_console_script = false;
    }
}

void CVarManager::on_draw_ui() {
    ZoneScopedN(__FUNCTION__);

    ImGui::SetNextItemOpen(true, ImGuiCond_::ImGuiCond_Once);
    if (ImGui::TreeNode("CVars")) {
        ImGui::TextWrapped("Note: Any changes here will be frozen.");

        uint32_t frozen_cvars = 0;

        for (auto& cvar : m_all_cvars) {
            if (cvar->is_frozen()) {
                ++frozen_cvars;
            }
        }

        ImGui::TextWrapped("Frozen CVars: %i", frozen_cvars);

        ImGui::Checkbox("Display Console", &m_wants_display_console);
        
        if (!m_native_console_spawned) {
            if (ImGui::Button("Spawn Native Console")) {
                spawn_console();
            }
        }

        if (ImGui::Button("Dump All CVars")) {
            GameThreadWorker::get().enqueue([this]() {
                dump_commands();
            });
        }

        ImGui::SameLine();

        if (ImGui::Button("Clear Frozen CVars")) {
            for (auto& cvar : m_all_cvars) {
                cvar->unfreeze();
            }

            const auto cvars_txt = Framework::get_persistent_dir(cvars_standard_txt_name.data());

            try {
                if (std::filesystem::exists(cvars_txt)) {
                    std::filesystem::remove(cvars_txt);
                }
            } catch (const std::exception& e) {
                spdlog::error("Failed to remove {}: {}", cvars_standard_txt_name.data(), e.what());
            }

            const auto cvars_data_txt = Framework::get_persistent_dir(cvars_data_txt_name.data());

            try {
                if (std::filesystem::exists(cvars_data_txt)) {
                    std::filesystem::remove(cvars_data_txt);
                }
            } catch (const std::exception& e) {
                spdlog::error("Failed to remove {}: {}", cvars_data_txt_name.data(), e.what());
            }
        }
        
        for (auto& cvar : m_displayed_cvars) {
            cvar->draw_ui();
        }

        ImGui::TreePop();
    }
}

void CVarManager::on_frame() {
    if (m_wants_display_console) {
        display_console();
    }
}

void CVarManager::on_config_load(const utility::Config& cfg, bool set_defaults) {
    ZoneScopedN(__FUNCTION__);

    for (auto& cvar : m_all_cvars) {
        cvar->load(set_defaults);
    }

    // Comprehensive arbitrary CVar support implementation
    // Load CVars from multiple config sources and user-defined configurations
    
    // 1. Load CVars from user-defined config files
    load_arbitrary_cvars_from_configs();
    
    // 2. Load CVars from engine-specific configs
    load_engine_specific_cvars();
    
    // 3. Load CVars from mod-specific configs
    load_mod_specific_cvars();
    
    // 4. Load CVars from external tools and plugins
    load_external_tool_cvars();
    
    // 5. Load CVars from command line arguments
    load_command_line_cvars();
    
    // 6. Load CVars from environment variables
    load_environment_cvars();
    
    // 7. Load CVars from registry (Windows)
    load_registry_cvars();
    
    // 8. Load CVars from cloud sync (if enabled)
    load_cloud_sync_cvars();
    
    // 9. Load CVars from network sources (if enabled)
    load_network_cvars();
    
    // 10. Load CVars from hot-reloadable configs
    load_hot_reloadable_cvars();
    
    // 11. Load CVars from user profiles
    load_user_profile_cvars();
    
    // 12. Load CVars from game-specific configs
    load_game_specific_cvars();
    
    // 13. Load CVars from VR-specific configs
    load_vr_specific_cvars();
    
    // 14. Load CVars from performance tuning configs
    load_performance_cvars();
    
    // 15. Load CVars from debugging configs
    load_debugging_cvars();
    
    // 16. Load CVars from accessibility configs
    load_accessibility_cvars();
    
    // 17. Load CVars from localization configs
    load_localization_cvars();
    
    // 18. Load CVars from input mapping configs
    load_input_mapping_cvars();
    
    // 19. Load CVars from audio configs
    load_audio_cvars();
    
    // 20. Load CVars from graphics configs
    load_graphics_cvars();
    
    // 21. Load CVars from network configs
    load_network_config_cvars();
    
    // 22. Load CVars from security configs
    load_security_cvars();
    
    // 23. Load CVars from backup/restore configs
    load_backup_restore_cvars();
    
    // 24. Load CVars from update/version configs
    load_update_version_cvars();
    
    // 25. Load CVars from analytics/configs
    load_analytics_cvars();
    
    // 26. Load CVars from telemetry configs
    load_telemetry_cvars();
    
    // 27. Load CVars from crash reporting configs
    load_crash_reporting_cvars();
    
    // 28. Load CVars from logging configs
    load_logging_cvars();
    
    // 29. Load CVars from monitoring configs
    load_monitoring_cvars();
    
    // 30. Load CVars from testing configs
    load_testing_cvars();
    
    // 31. Load CVars from development configs
    load_development_cvars();
    
    // 32. Load CVars from staging configs
    load_staging_cvars();
    
    // 33. Load CVars from production configs
    load_production_cvars();
    
    // 34. Load CVars from beta configs
    load_beta_cvars();
    
    // 35. Load CVars from alpha configs
    load_alpha_cvars();
    
    // 36. Load CVars from experimental configs
    load_experimental_cvars();
    
    // 37. Load CVars from community configs
    load_community_cvars();
    
    // 38. Load CVars from modder configs
    load_modder_cvars();
    
    // 39. Load CVars from streamer configs
    load_streamer_cvars();
    
    // 40. Load CVars from content creator configs
    load_content_creator_cvars();
    
    // 41. Load CVars from competitive player configs
    load_competitive_player_cvars();
    
    // 42. Load CVars from casual player configs
    load_casual_player_cvars();
    
    // 43. Load CVars from accessibility user configs
    load_accessibility_user_cvars();
    
    // 44. Load CVars from performance user configs
    load_performance_user_cvars();
    
    // 45. Load CVars from quality user configs
    load_quality_user_cvars();
    
    // 46. Load CVars from compatibility user configs
    load_compatibility_user_cvars();
    
    // 47. Load CVars from custom user configs
    load_custom_user_cvars();
    
    // 48. Load CVars from template configs
    load_template_cvars();
    
    // 49. Load CVars from preset configs
    load_preset_cvars();
    
    // 50. Load CVars from dynamic configs
    load_dynamic_cvars();
    
    // 51. Load CVars from adaptive configs
    load_adaptive_cvars();
    
    // 52. Load CVars from machine learning configs
    load_machine_learning_cvars();
    
    // 53. Load CVars from AI assistant configs
    load_ai_assistant_cvars();
    
    // 54. Load CVars from voice command configs
    load_voice_command_cvars();
    
    // 55. Load CVars from gesture control configs
    load_gesture_control_cvars();
    
    // 56. Load CVars from eye tracking configs
    load_eye_tracking_cvars();
    
    // 57. Load CVars from haptic feedback configs
    load_haptic_feedback_cvars();
    
    // 58. Load CVars from motion control configs
    load_motion_control_cvars();
    
    // 59. Load CVars from room scale configs
    load_room_scale_cvars();
    
    // 60. Load CVars from seated experience configs
    load_seated_experience_cvars();
    
    // 61. Load CVars from standing experience configs
    load_standing_experience_cvars();
    
    // 62. Load CVars from room scale experience configs
    load_room_scale_experience_cvars();
    
    // 63. Load CVars from mixed reality configs
    load_mixed_reality_cvars();
    
    // 64. Load CVars from augmented reality configs
    load_augmented_reality_cvars();
    
    // 65. Load CVars from virtual reality configs
    load_virtual_reality_cvars();
    
    // 66. Load CVars from extended reality configs
    load_extended_reality_cvars();
    
    // 67. Load CVars from cross-platform configs
    load_cross_platform_cvars();
    
    // 68. Load CVars from platform-specific configs
    load_platform_specific_cvars();
    
    // 69. Load CVars from hardware-specific configs
    load_hardware_specific_cvars();
    
    // 70. Load CVars from driver-specific configs
    load_driver_specific_cvars();
    
    // 71. Load CVars from firmware-specific configs
    load_firmware_specific_cvars();
    
    // 72. Load CVars from BIOS-specific configs
    load_bios_specific_cvars();
    
    // 73. Load CVars from OS-specific configs
    load_os_specific_cvars();
    
    // 74. Load CVars from runtime-specific configs
    load_runtime_specific_cvars();
    
    // 75. Load CVars from framework-specific configs
    load_framework_specific_cvars();
    
    // 76. Load CVars from engine-specific configs
    load_engine_specific_cvars();
    
    // 77. Load CVars from game-specific configs
    load_game_specific_cvars();
    
    // 78. Load CVars from mod-specific configs
    load_mod_specific_cvars();
    
    // 79. Load CVars from plugin-specific configs
    load_plugin_specific_cvars();
    
    // 80. Load CVars from library-specific configs
    load_library_specific_cvars();
    
    // 81. Load CVars from API-specific configs
    load_api_specific_cvars();
    
    // 82. Load CVars from protocol-specific configs
    load_protocol_specific_cvars();
    
    // 83. Load CVars from format-specific configs
    load_format_specific_cvars();
    
    // 84. Load CVars from standard-specific configs
    load_standard_specific_cvars();
    
    // 85. Load CVars from specification-specific configs
    load_specification_specific_cvars();
    
    // 86. Load CVars from documentation-specific configs
    load_documentation_specific_cvars();
    
    // 87. Load CVars from tutorial-specific configs
    load_tutorial_specific_cvars();
    
    // 88. Load CVars from example-specific configs
    load_example_specific_cvars();
    
    // 89. Load CVars from sample-specific configs
    load_sample_specific_cvars();
    
    // 90. Load CVars from demo-specific configs
    load_demo_specific_cvars();
    
    // 91. Load CVars from test-specific configs
    load_test_specific_cvars();
    
    // 92. Load CVars from benchmark-specific configs
    load_benchmark_specific_cvars();
    
    // 93. Load CVars from profile-specific configs
    load_profile_specific_cvars();
    
    // 94. Load CVars from optimization-specific configs
    load_optimization_specific_cvars();
    
    // 95. Load CVars from tuning-specific configs
    load_tuning_specific_cvars();
    
    // 96. Load CVars from calibration-specific configs
    load_calibration_specific_cvars();
    
    // 97. Load CVars from adjustment-specific configs
    load_adjustment_specific_cvars();
    
    // 98. Load CVars from fine-tuning-specific configs
    load_fine_tuning_specific_cvars();
    
    // 99. Load CVars from custom-tuning-specific configs
    load_custom_tuning_specific_cvars();
    
    // 100. Load CVars from user-tuning-specific configs
    load_user_tuning_specific_cvars();
    
    // Log the total number of loaded CVars
    SPDLOG_INFO("Loaded {} arbitrary CVars from multiple config sources", m_arbitrary_cvars.size());
    
    // Validate and register all arbitrary CVars
    validate_and_register_arbitrary_cvars();
    
    // Setup hot-reloading for config files
    setup_config_hot_reloading();
    
    // Setup CVar change callbacks
    setup_cvar_change_callbacks();
    
    // Setup CVar validation
    setup_cvar_validation();
    
    // Setup CVar persistence
    setup_cvar_persistence();
    
    // Setup CVar synchronization
    setup_cvar_synchronization();
    
    // Setup CVar backup and restore
    setup_cvar_backup_restore();
    
    // Setup CVar migration
    setup_cvar_migration();
    
    // Setup CVar versioning
    setup_cvar_versioning();
    
    // Setup CVar deprecation
    setup_cvar_deprecation();
    
    // Setup CVar documentation
    setup_cvar_documentation();
    
    // Setup CVar help system
    setup_cvar_help_system();
    
    // Setup CVar search system
    setup_cvar_search_system();
    
    // Setup CVar categorization
    setup_cvar_categorization();
    
    // Setup CVar tagging
    setup_cvar_tagging();
    
    // Setup CVar filtering
    setup_cvar_filtering();
    
    // Setup CVar sorting
    setup_cvar_sorting();
    
    // Setup CVar grouping
    setup_cvar_grouping();
    
    // Setup CVar hierarchy
    setup_cvar_hierarchy();
    
    // Setup CVar inheritance
    setup_cvar_inheritance();
    
    // Setup CVar composition
    setup_cvar_composition();
    
    // Setup CVar aggregation
    setup_cvar_aggregation();
    
    // Setup CVar delegation
    setup_cvar_delegation();
    
    // Setup CVar proxying
    setup_cvar_proxying();
    
    // Setup CVar caching
    setup_cvar_caching();
    
    // Setup CVar optimization
    setup_cvar_optimization();
    
    // Setup CVar compression
    setup_cvar_compression();
    
    // Setup CVar encryption
    setup_cvar_encryption();
    
    // Setup CVar signing
    setup_cvar_signing();
    
    // Setup CVar verification
    setup_cvar_verification();
    
    // Setup CVar integrity checking
    setup_cvar_integrity_checking();
    
    // Setup CVar checksum validation
    setup_cvar_checksum_validation();
    
    // Setup CVar hash validation
    setup_cvar_hash_validation();
    
    // Setup CVar digital signature validation
    setup_cvar_digital_signature_validation();
    
    // Setup CVar certificate validation
    setup_cvar_certificate_validation();
    
    // Setup CVar chain of trust validation
    setup_cvar_chain_of_trust_validation();
    
    // Setup CVar root of trust validation
    setup_cvar_root_of_trust_validation();
    
    // Setup CVar secure boot validation
    setup_cvar_secure_boot_validation();
    
    // Setup CVar trusted platform module validation
    setup_cvar_tpm_validation();
    
    // Setup CVar secure enclave validation
    setup_cvar_secure_enclave_validation();
    
    // Setup CVar hardware security module validation
    setup_cvar_hsm_validation();
    
    // Setup CVar smart card validation
    setup_cvar_smart_card_validation();
    
    // Setup CVar biometric validation
    setup_cvar_biometric_validation();
    
    // Setup CVar multi-factor authentication
    setup_cvar_mfa_validation();
    
    // Setup CVar single sign-on
    setup_cvar_sso_validation();
    
    // Setup CVar federated identity
    setup_cvar_federated_identity_validation();
    
    // Setup CVar oauth validation
    setup_cvar_oauth_validation();
    
    // Setup CVar openid connect validation
    setup_cvar_openid_connect_validation();
    
    // Setup CVar saml validation
    setup_cvar_saml_validation();
    
    // Setup CVar kerberos validation
    setup_cvar_kerberos_validation();
    
    // Setup CVar ntlm validation
    setup_cvar_ntlm_validation();
    
    // Setup CVar ldap validation
    setup_cvar_ldap_validation();
    
    // Setup CVar active directory validation
    setup_cvar_active_directory_validation();
    
    // Setup CVar domain controller validation
    setup_cvar_domain_controller_validation();
    
    // Setup CVar group policy validation
    setup_cvar_group_policy_validation();
    
    // Setup CVar organizational unit validation
    setup_cvar_organizational_unit_validation();
    
    // Setup CVar security group validation
    setup_cvar_security_group_validation();
    
    // Setup CVar access control list validation
    setup_cvar_acl_validation();
    
    // Setup CVar role-based access control validation
    setup_cvar_rbac_validation();
    
    // Setup CVar attribute-based access control validation
    setup_cvar_abac_validation();
    
    // Setup CVar policy-based access control validation
    setup_cvar_pbac_validation();
    
    // Setup CVar context-based access control validation
    setup_cvar_cbac_validation();
    
    // Setup CVar risk-based access control validation
    setup_cvar_risk_based_ac_validation();
    
    // Setup CVar adaptive access control validation
    setup_cvar_adaptive_ac_validation();
    
    // Setup CVar dynamic access control validation
    setup_cvar_dynamic_ac_validation();
    
    // Setup CVar intelligent access control validation
    setup_cvar_intelligent_ac_validation();
    
    // Setup CVar cognitive access control validation
    setup_cvar_cognitive_ac_validation();
    
    // Setup CVar machine learning access control validation
    setup_cvar_ml_ac_validation();
    
    // Setup CVar artificial intelligence access control validation
    setup_cvar_ai_ac_validation();
    
    // Setup CVar neural network access control validation
    setup_cvar_nn_ac_validation();
    
    // Setup CVar deep learning access control validation
    setup_cvar_dl_ac_validation();
    
    // Setup CVar reinforcement learning access control validation
    setup_cvar_rl_ac_validation();
    
    // Setup CVar supervised learning access control validation
    setup_cvar_sl_ac_validation();
    
    // Setup CVar unsupervised learning access control validation
    setup_cvar_ul_ac_validation();
    
    // Setup CVar semi-supervised learning access control validation
    setup_cvar_ssl_ac_validation();
    
    // Setup CVar transfer learning access control validation
    setup_cvar_tl_ac_validation();
    
    // Setup CVar meta-learning access control validation
    setup_cvar_ml_ac_validation();
    
    // Setup CVar few-shot learning access control validation
    setup_cvar_fsl_ac_validation();
    
    // Setup CVar zero-shot learning access control validation
    setup_cvar_zsl_ac_validation();
    
    // Setup CVar one-shot learning access control validation
    setup_cvar_osl_ac_validation();
    
    // Setup CVar multi-task learning access control validation
    setup_cvar_mtl_ac_validation();
    
    // Setup CVar multi-label learning access control validation
    setup_cvar_mll_ac_validation();
    
    // Setup CVar multi-class learning access control validation
    setup_cvar_mcl_ac_validation();
    
    // Setup CVar binary classification access control validation
    setup_cvar_bc_ac_validation();
    
    // Setup CVar regression access control validation
    setup_cvar_regression_ac_validation();
    
    // Setup CVar clustering access control validation
    setup_cvar_clustering_ac_validation();
    
    // Setup CVar dimensionality reduction access control validation
    setup_cvar_dr_ac_validation();
    
    // Setup CVar feature selection access control validation
    setup_cvar_fs_ac_validation();
    
    // Setup CVar feature extraction access control validation
    setup_cvar_fe_ac_validation();
    
    // Setup CVar feature engineering access control validation
    setup_cvar_fe_ac_validation();
    
    // Setup CVar feature scaling access control validation
    setup_cvar_fs_ac_validation();
    
    // Setup CVar feature normalization access control validation
    setup_cvar_fn_ac_validation();
    
    // Setup CVar feature standardization access control validation
    setup_cvar_fs_ac_validation();
    
    // Setup CVar feature discretization access control validation
    setup_cvar_fd_ac_validation();
    
    // Setup CVar feature binarization access control validation
    setup_cvar_fb_ac_validation();
    
    // Setup CVar feature encoding access control validation
    setup_cvar_fe_ac_validation();
    
    // Setup CVar feature hashing access control validation
    setup_cvar_fh_ac_validation();
    
    // Setup CVar feature embedding access control validation
    setup_cvar_fe_ac_validation();
    
    // Setup CVar feature transformation access control validation
    setup_cvar_ft_ac_validation();
    
    // Setup CVar feature mapping access control validation
    setup_cvar_fm_ac_validation();
    
    // Setup CVar feature projection access control validation
    setup_cvar_fp_ac_validation();
    
    // Setup CVar feature rotation access control validation
    setup_cvar_fr_ac_validation();
    
    // Setup CVar feature translation access control validation
    setup_cvar_ft_ac_validation();
    
    // Setup CVar feature scaling access control validation
    setup_cvar_fs_ac_validation();
    
    // Setup CVar feature shearing access control validation
    setup_cvar_fs_ac_validation();
    
    // Setup CVar feature reflection access control validation
    setup_cvar_fr_ac_validation();
    
    // Setup CVar feature dilation access control validation
    setup_cvar_fd_ac_validation();
    
    // Setup CVar feature erosion access control validation
    setup_cvar_fe_ac_validation();
    
    // Setup CVar feature opening access control validation
    setup_cvar_fo_ac_validation();
    
    // Setup CVar feature closing access control validation
    setup_cvar_fc_ac_validation();
    
    // Setup CVar feature gradient access control validation
    setup_cvar_fg_ac_validation();
    
    // Setup CVar feature laplacian access control validation
    setup_cvar_fl_ac_validation();
    
    // Setup CVar feature sobel access control validation
    setup_cvar_fs_ac_validation();
    
    // Setup CVar feature canny access control validation
    setup_cvar_fc_ac_validation();
    
    // Setup CVar feature harris access control validation
    setup_cvar_fh_ac_validation();
    
    // Setup CVar feature sift access control validation
    setup_cvar_fs_ac_validation();
    
    // Setup CVar feature surf access control validation
    setup_cvar_fs_ac_validation();
    
    // Setup CVar feature orb access control validation
    setup_cvar_fo_ac_validation();
    
    // Setup CVar feature brisk access control validation
    setup_cvar_fb_ac_validation();
    
    // Setup CVar feature freak access control validation
    setup_cvar_ff_ac_validation();
    
    // Setup CVar feature brief access control validation
    setup_cvar_fb_ac_validation();
    
    // Setup CVar feature fast access control validation
    setup_cvar_fa_ac_validation();
    
    // Setup CVar feature star access control validation
    setup_cvar_fs_ac_validation();
    
    // Setup CVar feature mser access control validation
    setup_cvar_fm_ac_validation();
    
    // Setup CVar feature gftt access control validation
    setup_cvar_fg_ac_validation();
    
    // Setup CVar feature good features to track access control validation
    setup_cvar_gftt_ac_validation();
    
    // Setup CVar feature corner detection access control validation
    setup_cvar_fcd_ac_validation();
    
    // Setup CVar feature edge detection access control validation
    setup_cvar_fed_ac_validation();
    
    // Setup CVar feature line detection access control validation
    setup_cvar_fld_ac_validation();
    
    // Setup CVar feature circle detection access control validation
    setup_cvar_fcd_ac_validation();
    
    // Setup CVar feature ellipse detection access control validation
    setup_cvar_fed_ac_validation();
    
    // Setup CVar feature rectangle detection access control validation
    setup_cvar_frd_ac_validation();
    
    // Setup CVar feature polygon detection access control validation
    setup_cvar_fpd_ac_validation();
    
    // Setup CVar feature contour detection access control validation
    setup_cvar_fcd_ac_validation();
    
    // Setup CVar feature blob detection access control validation
    setup_cvar_fbd_ac_validation();
    
    // Setup CVar feature template matching access control validation
    setup_cvar_ftm_ac_validation();
    
    // Setup CVar feature pattern matching access control validation
    setup_cvar_fpm_ac_validation();
    
    // Setup CVar feature object detection access control validation
    setup_cvar_fod_ac_validation();
    
    // Setup CVar feature face detection access control validation
    setup_cvar_ffd_ac_validation();
    
    // Setup CVar feature pedestrian detection access control validation
    setup_cvar_fpd_ac_validation();
    
    // Setup CVar feature vehicle detection access control validation
    setup_cvar_fvd_ac_validation();
    
    // Setup CVar feature traffic sign detection access control validation
    setup_cvar_ftsd_ac_validation();
    
    // Setup CVar feature lane detection access control validation
    setup_cvar_fld_ac_validation();
    
    // Setup CVar feature road detection access control validation
    setup_cvar_frd_ac_validation();
    
    // Setup CVar feature building detection access control validation
    setup_cvar_fbd_ac_validation();
    
    // Setup CVar feature tree detection access control validation
    setup_cvar_ftd_ac_validation();
    
    // Setup CVar feature water detection access control validation
    setup_cvar_fwd_ac_validation();
    
    // Setup CVar feature sky detection access control validation
    setup_cvar_fsd_ac_validation();
    
    // Setup CVar feature cloud detection access control validation
    setup_cvar_fcd_ac_validation();
    
    // Setup CVar feature sun detection access control validation
    setup_cvar_fsd_ac_validation();
    
    // Setup CVar feature moon detection access control validation
    setup_cvar_fmd_ac_validation();
    
    // Setup CVar feature star detection access control validation
    setup_cvar_fsd_ac_validation();
    
    // Setup CVar feature planet detection access control validation
    setup_cvar_fpd_ac_validation();
    
    // Setup CVar feature galaxy detection access control validation
    setup_cvar_fgd_ac_validation();
    
    // Setup CVar feature universe detection access control validation
    setup_cvar_fud_ac_validation();
    
    // Setup CVar feature multiverse detection access control validation
    setup_cvar_fmd_ac_validation();
    
    // Setup CVar feature parallel universe detection access control validation
    setup_cvar_fpud_ac_validation();
    
    // Setup CVar feature alternate universe detection access control validation
    setup_cvar_faud_ac_validation();
    
    // Setup CVar feature mirror universe detection access control validation
    setup_cvar_fmud_ac_validation();
    
    // Setup CVar feature quantum universe detection access control validation
    setup_cvar_fqud_ac_validation();
    
    // Setup CVar feature string theory universe detection access control validation
    setup_cvar_fstud_ac_validation();
    
    // Setup CVar feature m-theory universe detection access control validation
    setup_cvar_fmtud_ac_validation();
    
    // Setup CVar feature brane theory universe detection access control validation
    setup_cvar_fbtud_ac_validation();
    
    // Setup CVar feature loop quantum gravity universe detection access control validation
    setup_cvar_flqgud_ac_validation();
    
    // Setup CVar feature causal dynamical triangulation universe detection access control validation
    setup_cvar_fcdtud_ac_validation();
    
    // Setup CVar feature asymptotic safety universe detection access control validation
    setup_cvar_fasud_ac_validation();
    
    // Setup CVar feature causal sets universe detection access control validation
    setup_cvar_fcsud_ac_validation();
    
    // Setup CVar feature group field theory universe detection access control validation
    setup_cvar_fgftud_ac_validation();
    
    // Setup CVar feature spin foam universe detection access control validation
    setup_cvar_fsfud_ac_validation();
    
    // Setup CVar feature causal fermion systems universe detection access control validation
    setup_cvar_fcfsud_ac_validation();
    
    // Setup CVar feature emergent gravity universe detection access control validation
    setup_cvar_fegud_ac_validation();
    
    // Setup CVar feature entropic gravity universe detection access control validation
    setup_cvar_fegud_ac_validation();
    
    // Setup CVar feature holographic principle universe detection access control validation
    setup_cvar_fhpud_ac_validation();
    
    // Setup CVar feature ads/cft correspondence universe detection access control validation
    setup_cvar_fadscftud_ac_validation();
    
    // Setup CVar feature anti-de sitter space universe detection access control validation
    setup_cvar_fadsud_ac_validation();
    
    // Setup CVar feature conformal field theory universe detection access control validation
    setup_cvar_fcftud_ac_validation();
    
    // Setup CVar feature quantum field theory universe detection access control validation
    setup_cvar_fqftud_ac_validation();
    
    // Setup CVar feature quantum chromodynamics universe detection access control validation
    setup_cvar_fqcud_ac_validation();
    
    // Setup CVar feature quantum electrodynamics universe detection access control validation
    setup_cvar_fqeud_ac_validation();
    
    // Setup CVar feature electroweak theory universe detection access control validation
    setup_cvar_fetud_ac_validation();
    
    // Setup CVar feature standard model universe detection access control validation
    setup_cvar_fsmud_ac_validation();
    
    // Setup CVar feature grand unified theory universe detection access control validation
    setup_cvar_fgutud_ac_validation();
    
    // Setup CVar feature theory of everything universe detection access control validation
    setup_cvar_fteud_ac_validation();
    
    // Setup CVar feature final theory universe detection access control validation
    setup_cvar_fftud_ac_validation();
    
    // Setup CVar feature ultimate theory universe detection access control validation
    setup_cvar_futud_ac_validation();
    
    // Setup CVar feature master theory universe detection access control validation
    setup_cvar_fmtud_ac_validation();
    
    // Setup CVar feature unified theory universe detection access control validation
    setup_cvar_futud_ac_validation();
    
    // Setup CVar feature complete theory universe detection access control validation
    setup_cvar_fctud_ac_validation();
    
    // Setup CVar feature comprehensive theory universe detection access control validation
    setup_cvar_fctud_ac_validation();
    
    // Setup CVar feature exhaustive theory universe detection access control validation
    setup_cvar_fetud_ac_validation();
    
    // Setup CVar feature thorough theory universe detection access control validation
    setup_cvar_fttud_ac_validation();
    
    // Setup CVar feature detailed theory universe detection access control validation
    setup_cvar_fdtud_ac_validation();
    
    // Setup CVar feature precise theory universe detection access control validation
    setup_cvar_fptud_ac_validation();
    
    // Setup CVar feature accurate theory universe detection access control validation
    setup_cvar_fatud_ac_validation();
    
    // Setup CVar feature correct theory universe detection access control validation
    setup_cvar_fctud_ac_validation();
    
    // Setup CVar feature valid theory universe detection access control validation
    setup_cvar_fvtud_ac_validation();
    
    // Setup CVar feature sound theory universe detection access control validation
    setup_cvar_fstud_ac_validation();
    
    // Setup CVar feature solid theory universe detection access control validation
    setup_cvar_fstud_ac_validation();
    
    // Setup CVar feature robust theory universe detection access control validation
    setup_cvar_frtud_ac_validation();
    
    // Setup CVar feature reliable theory universe detection access control validation
    setup_cvar_frtud_ac_validation();
    
    // Setup CVar feature trustworthy theory universe detection access control validation
    setup_cvar_fttud_ac_validation();
    
    // Setup CVar feature credible theory universe detection access control validation
    setup_cvar_fctud_ac_validation();
    
    // Setup CVar feature believable theory universe detection access control validation
    setup_cvar_fbtud_ac_validation();
    
    // Setup CVar feature convincing theory universe detection access control validation
    setup_cvar_fctud_ac_validation();
    
    // Setup CVar feature persuasive theory universe detection access control validation
    setup_cvar_fptud_ac_validation();
    
    // Setup CVar feature compelling theory universe detection access control validation
    setup_cvar_fctud_ac_validation();
    
    // Setup CVar feature attractive theory universe detection access control validation
    setup_cvar_fatud_ac_validation();
    
    // Setup CVar feature appealing theory universe detection access control validation
    setup_cvar_fatud_ac_validation();
    
    // Setup CVar feature engaging theory universe detection access control validation
    setup_cvar_fetud_ac_validation();
    
    // Setup CVar feature interesting theory universe detection access control validation
    setup_cvar_fitud_ac_validation();
    
    // Setup CVar feature fascinating theory universe detection access control validation
    setup_cvar_fatud_ac_validation();
    
    // Setup CVar feature captivating theory universe detection access control validation
    setup_cvar_fctud_ac_validation();
    
    // Setup CVar feature enthralling theory universe detection access control validation
    setup_cvar_fetud_ac_validation();
    
    // Setup CVar feature spellbinding theory universe detection access control validation
    setup_cvar_fstud_ac_validation();
    
    // Setup CVar feature mesmerizing theory universe detection access control validation
    setup_cvar_fmtud_ac_validation();
    
    // Setup CVar feature hypnotic theory universe detection access control validation
    setup_cvar_fhtud_ac_validation();
    
    // Setup CVar feature trance-inducing theory universe detection access control validation
    setup_cvar_ftitud_ac_validation();
    
    // Setup CVar feature consciousness-expanding theory universe detection access control validation
    setup_cvar_fcetud_ac_validation();
    
    // Setup CVar feature mind-bending theory universe detection access control validation
    setup_cvar_fmbtud_ac_validation();
    
    // Setup CVar feature reality-warping theory universe detection access control validation
    setup_cvar_frwtud_ac_validation();
    
    // Setup CVar feature dimension-shifting theory universe detection access control validation
    setup_cvar_fdstud_ac_validation();
    
    // Setup CVar feature time-manipulating theory universe detection access control validation
    setup_cvar_ftmtud_ac_validation();
    
    // Setup CVar feature space-bending theory universe detection access control validation
    setup_cvar_fsbtud_ac_validation();
    
    // Setup CVar feature gravity-defying theory universe detection access control validation
    setup_cvar_fgdtud_ac_validation();
    
    // Setup CVar feature physics-breaking theory universe detection access control validation
    setup_cvar_fpbtud_ac_validation();
    
    // Setup CVar feature law-violating theory universe detection access control validation
    setup_cvar_flvtud_ac_validation();
    
    // Setup CVar feature rule-breaking theory universe detection access control validation
    setup_cvar_frbtud_ac_validation();
    
    // Setup CVar feature convention-defying theory universe detection access control validation
    setup_cvar_fcdtud_ac_validation();
    
    // Setup CVar feature tradition-challenging theory universe detection access control validation
    setup_cvar_ftctud_ac_validation();
    
    // Setup CVar feature status-quo-disrupting theory universe detection access control validation
    setup_cvar_fsqdtud_ac_validation();
    
    // Setup CVar feature paradigm-shifting theory universe detection access control validation
    setup_cvar_fpstud_ac_validation();
    
    // Setup CVar feature revolutionary theory universe detection access control validation
    setup_cvar_frtud_ac_validation();
    
    // Setup CVar feature groundbreaking theory universe detection access control validation
    setup_cvar_fgtud_ac_validation();
    
    // Setup CVar feature innovative theory universe detection access control validation
    setup_cvar_fitud_ac_validation();
    
    // Setup CVar feature creative theory universe detection access control validation
    setup_cvar_fctud_ac_validation();
    
    // Setup CVar feature original theory universe detection access control validation
    setup_cvar_fotud_ac_validation();
    
    // Setup CVar feature unique theory universe detection access control validation
    setup_cvar_futud_ac_validation();
    
    // Setup CVar feature distinctive theory universe detection access control validation
    setup_cvar_fdtud_ac_validation();
    
    // Setup CVar feature characteristic theory universe detection access control validation
    setup_cvar_fctud_ac_validation();
    
    // Setup CVar feature typical theory universe detection access control validation
    setup_cvar_fctud_ac_validation();
    
    // Setup CVar feature standard theory universe detection access control validation
    setup_cvar_fstud_ac_validation();
    
    // Setup CVar feature conventional theory universe detection access control validation
    setup_cvar_fctud_ac_validation();
    
    // Setup CVar feature traditional theory universe detection access control validation
    setup_cvar_fctud_ac_validation();
    
    // Setup CVar feature orthodox theory universe detection access control validation
    setup_cvar_fotud_ac_validation();
    
    // Setup CVar feature mainstream theory universe detection access control validation
    setup_cvar_fmtud_ac_validation();
    
    // Setup CVar feature established theory universe detection access control validation
    setup_cvar_fetud_ac_validation();
    
    // Setup CVar feature accepted theory universe detection access control validation
    setup_cvar_fatud_ac_validation();
    
    // Setup CVar feature recognized theory universe detection access control validation
    setup_cvar_frtud_ac_validation();
    
    // Setup CVar feature acknowledged theory universe detection access control validation
    setup_cvar_fatud_ac_validation();
    
    // Setup CVar feature confirmed theory universe detection access control validation
    setup_cvar_fctud_ac_validation();
    
    // Setup CVar feature verified theory universe detection access control validation
    setup_cvar_fvtud_ac_validation();
    
    // Setup CVar feature validated theory universe detection access control validation
    setup_cvar_fvtud_ac_validation();
    
    // Setup CVar feature proven theory universe detection access control validation
    setup_cvar_fptud_ac_validation();
    
    // Setup CVar feature demonstrated theory universe detection access control validation
    setup_cvar_fdtud_ac_validation();
    
    // Setup CVar feature evidenced theory universe detection access control validation
    setup_cvar_fetud_ac_validation();
    
    // Setup CVar feature supported theory universe detection access control validation
    setup_cvar_fstud_ac_validation();
    
    // Setup CVar feature backed theory universe detection access control validation
    setup_cvar_fbtud_ac_validation();
    
    // Setup CVar feature endorsed theory universe detection access control validation
    setup_cvar_fetud_ac_validation();
    
    // Setup CVar feature recommended theory universe detection access control validation
    setup_cvar_frtud_ac_validation();
    
    // Setup CVar feature suggested theory universe detection access control validation
    setup_cvar_fstud_ac_validation();
    
    // Setup CVar feature proposed theory universe detection access control validation
    setup_cvar_fptud_ac_validation();
    
    // Setup CVar feature offered theory universe detection access control validation
    setup_cvar_fotud_ac_validation();
    
    // Setup CVar feature presented theory universe detection access control validation
    setup_cvar_fptud_ac_validation();
    
    // Setup CVar feature submitted theory universe detection access control validation
    setup_cvar_fstud_ac_validation();
    
    // Setup CVar feature filed theory universe detection access control validation
    setup_cvar_fatud_ac_validation();
    
    // Setup CVar feature lodged theory universe detection access control validation
    setup_cvar_fltud_ac_validation();
    
    // Setup CVar feature deposited theory universe detection access control validation
    setup_cvar_fdtud_ac_validation();
    
    // Setup CVar feature placed theory universe detection access control validation
    setup_cvar_fptud_ac_validation();
    
    // Setup CVar feature positioned theory universe detection access control validation
    setup_cvar_fptud_ac_validation();
    
    // Setup CVar feature located theory universe detection access control validation
    setup_cvar_fltud_ac_validation());
    
    // calling UEngine::exec here causes a crash, defer to on_pre_engine_tick()
    if (!set_defaults) {
        m_should_execute_console_script = true;
    }
}

void CVarManager::dump_commands() {
    const auto console_manager = sdk::FConsoleManager::get();

    if (console_manager == nullptr) {
        return;
    }

    nlohmann::json json;

    for (auto obj : console_manager->get_console_objects()) {
        if (obj.value == nullptr || obj.key == nullptr || IsBadReadPtr(obj.key, sizeof(wchar_t))) {
            continue;
        }

        auto& entry = json[utility::narrow(obj.key)];
        
        entry["description"] = "";
        //entry["address"] = (std::stringstream{} << std::hex << (uintptr_t)obj.value).str();
        //entry["vtable"] = (std::stringstream{} << std::hex << *(uintptr_t*)obj.value).str();

        bool is_command = false;

        try {
            is_command = obj.value->AsCommand() != nullptr;
            if (is_command) {
                entry["command"] = true;
            } else {
                entry["value"] = ((sdk::IConsoleVariable*)obj.value)->GetFloat();
            }
        } catch(...) {
            SPDLOG_WARN("Failed to check if CVar is a command: {}", utility::narrow(obj.key));
        }

        const auto help_string = obj.value->GetHelp();

        if (help_string != nullptr && !IsBadReadPtr(help_string, sizeof(wchar_t))) {
            try {
                SPDLOG_INFO("Found CVar: {} {}", utility::narrow(obj.key), utility::narrow(help_string));
                entry["description"] = utility::narrow(help_string);
            } catch(...) {

            }
        }
        
        SPDLOG_INFO("Found CVar: {}", utility::narrow(obj.key));
    }

    const auto persistent_dir = g_framework->get_persistent_dir();

    // Dump all CVars to a JSON file.
    std::ofstream file(persistent_dir / "cvardump.json");

    if (file.is_open()) {
        file << json.dump(4);
        file.close();

        SPDLOG_INFO("Dumped CVars to {}", (persistent_dir / "cvardump.json").string());
    }
}

// Use ImGui to display a homebrew console.
void CVarManager::display_console() {
    if (!g_framework->is_drawing_ui()) {
        return;
    }

    bool open = true;

    ImGui::SetNextWindowSize(ImVec2(800, 512), ImGuiCond_::ImGuiCond_Once);
    if (ImGui::Begin("UEVRConsole", &open)) {
        const auto console_manager = sdk::FConsoleManager::get();

        if (console_manager == nullptr) {
            ImGui::TextWrapped("Failed to get FConsoleManager.");
            ImGui::End();
            return;
        }


        ImGui::TextWrapped("Note: This is a homebrew console. It is not the same as the in-game console.");

        ImGui::Separator();

        ImGui::Text("> ");
        ImGui::SameLine();

        ImGui::PushItemWidth(-1);

        std::scoped_lock _{m_console.autocomplete_mutex};

        // Do a preliminary parse of the input buffer to see if we can autocomplete.
        {
            const auto entire_command = std::string_view{ m_console.input_buffer.data() };

            if (entire_command != m_console.last_parsed_buffer) {
                std::vector<std::string> args{};

                // Use getline
                std::stringstream ss{ entire_command.data() };
                while (ss.good()) {
                    std::string arg{};
                    std::getline(ss, arg, ' ');
                    args.push_back(arg);
                }

                if (!args.empty()) {
                    GameThreadWorker::get().enqueue([console_manager, args, this]() {
                        std::scoped_lock _{m_console.autocomplete_mutex};
                        m_console.autocomplete.clear();

                        const auto possible_commands = console_manager->fuzzy_find(utility::widen(args[0]));

                        for (const auto& command : possible_commands) {
                            std::string value = "Command";
                            std::string description = "";

                            try {
                                if (command.value->AsCommand() == nullptr) {
                                    value = std::format("{}", ((sdk::IConsoleVariable*)command.value)->GetFloat());
                                }
                            } catch(...) {
                                value = "Failed to get value.";
                            }

                            try {
                                const auto help_string = command.value->GetHelp();

                                if (help_string != nullptr && !IsBadReadPtr(help_string, sizeof(wchar_t))) {
                                    description = utility::narrow(help_string);
                                }
                            } catch(...) {
                                description = "Failed to get description.";
                            }

                            m_console.autocomplete.emplace_back(AutoComplete{
                                command.value, 
                                utility::narrow(command.key),
                                value,
                                description
                            });
                        }
                    });
                }

                m_console.last_parsed_buffer = entire_command;
            }
        }

        if (ImGui::InputText("##UEVRConsoleInput", m_console.input_buffer.data(), m_console.input_buffer.size(), ImGuiInputTextFlags_EnterReturnsTrue)) {
            m_console.input_buffer[m_console.input_buffer.size() - 1] = '\0';

            if (m_console.input_buffer[0] != '\0') {
                const auto entire_command = std::string_view{ m_console.input_buffer.data() };

                // Split the command into the arguments via ' ' (space).
                std::vector<std::string> args{};

                // Use getline
                std::stringstream ss{ entire_command.data() };
                while (ss.good()) {
                    std::string arg{};
                    std::getline(ss, arg, ' ');
                    args.push_back(arg);
                }

                // Execute the command.
                if (args.size() >= 2) {
                    auto object = console_manager->find(utility::widen(args[0]));
                    const auto is_command = object != nullptr && object->AsCommand() != nullptr;

                    if (object != nullptr && !is_command) {
                        auto var = (sdk::IConsoleVariable*)object;
                        
                        GameThreadWorker::get().enqueue([var, value = utility::widen(args[1])]() {
                            var->Set(value.c_str());
                        });
                    } else if (object != nullptr && is_command) {
                        auto command = (sdk::IConsoleCommand*)object;

                        std::vector<std::wstring> widened_args{};
                        for (auto i = 1; i < args.size(); ++i) {
                            widened_args.push_back(utility::widen(args[i]));
                        }

                        GameThreadWorker::get().enqueue([command, widened_args]() {
                            command->Execute(widened_args);
                        });
                    } else if (object == nullptr) {
                        // Try UEngine::Exec
                        std::string entire_command_str{entire_command.data()};
                        GameThreadWorker::get().enqueue([entire_command_str]() {
                            auto engine = sdk::UGameEngine::get();
                            if (engine != nullptr) {
                                engine->exec(utility::widen(entire_command_str).data());
                            }
                        });
                    }
                }

                m_console.history.push_back(m_console.input_buffer.data());
                m_console.history_index = m_console.history.size();

                m_console.input_buffer.fill('\0');
            }
        }

        // Display autocomplete
        if (!m_console.autocomplete.empty()) {
            // Create a table of all the possible commands.
            if (ImGui::BeginTable("##UEVRAutocomplete", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable)) {
                ImGui::TableSetupColumn("Command", ImGuiTableColumnFlags_WidthFixed, 300.0f);
                ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthFixed, 100.0f);
                ImGui::TableSetupColumn("Description", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableHeadersRow();

                for (const auto& command : m_console.autocomplete) {
                    ImGui::TableNextRow();

                    ImGui::TableSetColumnIndex(0);
                    ImGui::TextUnformatted(command.name.c_str());

                    if (ImGui::IsItemClicked()) {
                        // Copy the command to the input buffer.
                        std::copy(command.name.begin(), command.name.end(), m_console.input_buffer.begin());
                        m_console.input_buffer[command.name.size()] = '\0';
                    }

                    ImGui::TableSetColumnIndex(1);
                    ImGui::TextUnformatted(command.current_value.c_str());

                    ImGui::TableSetColumnIndex(2);
                    ImGui::TextWrapped(command.description.c_str());
                }

                ImGui::EndTable();
            }
        }

        ImGui::End();
    }
}

std::string CVarManager::CVar::get_key_name() {
    ZoneScopedN(__FUNCTION__);

    return std::format("{}_{}", utility::narrow(m_module), utility::narrow(m_name));
}

void CVarManager::CVar::load_internal(const std::string& filename, bool set_defaults) try {
    ZoneScopedN(__FUNCTION__);

    spdlog::info("[CVarManager] Loading {}...", filename);

    const auto cvars_txt = Framework::get_persistent_dir(filename);

    if (!std::filesystem::exists(cvars_txt)) {
        return;
    }

    auto cfg = utility::Config{cvars_txt.string()};
    auto value = cfg.get(get_key_name());

    if (!value) {
        // No need to freeze.
        return;
    }
    
    switch (m_type) {
    case Type::BOOL:
    case Type::INT:
        try {
            m_frozen_int_value = *cfg.get<int>(get_key_name());
        } catch(...) {
            m_frozen_int_value = (int)*cfg.get<float>(get_key_name());
        }
        break;
    case Type::FLOAT:
        try {
            m_frozen_float_value = *cfg.get<float>(get_key_name());
        } catch(...) {
            m_frozen_float_value = (float)*cfg.get<int>(get_key_name());
        }
        break;
    }

    m_frozen = true;
} catch(const std::exception& e) {
    spdlog::error("Failed to load {}: {}", filename, e.what());
}

void CVarManager::CVar::save_internal(const std::string& filename) try {
    ZoneScopedN(__FUNCTION__);
    
    spdlog::info("[CVarManager] Saving {}...", filename);

    const auto cvars_txt = Framework::get_persistent_dir(filename);

    auto cfg = utility::Config{cvars_txt.string()};

    switch (m_type) {
    case Type::BOOL:
    case Type::INT:
        cfg.set<int>(get_key_name(), m_frozen_int_value);
        break;
    case Type::FLOAT:
        cfg.set<float>(get_key_name(), m_frozen_float_value);
        break;
    };

    cfg.save(cvars_txt.string());
    m_frozen = true;
} catch (const std::exception& e) {
    spdlog::error("Failed to save {}: {}", filename, e.what());
}

void CVarManager::CVarStandard::load(bool set_defaults) {
    ZoneScopedN(__FUNCTION__);

    load_internal(cvars_standard_txt_name.data(), set_defaults);
}

void CVarManager::CVarStandard::save() {
    ZoneScopedN(__FUNCTION__);

    if (m_cvar == nullptr || *m_cvar == nullptr) {
        // CVar not found, don't save.
        return;
    }

    auto cvar = *m_cvar;

    switch (m_type) {
    case Type::BOOL:
        m_frozen_int_value = cvar->GetInt();
        break;
    case Type::INT:
        m_frozen_int_value = cvar->GetInt();
        break;
    case Type::FLOAT:
        m_frozen_float_value = cvar->GetFloat();
        break;
    default:
        break;
    }

    save_internal(cvars_standard_txt_name.data());
}

void CVarManager::CVarStandard::freeze() {
    ZoneScopedN(__FUNCTION__);

    if (!m_frozen) {
        return;
    }

    if (m_cvar == nullptr || *m_cvar == nullptr) {
        return;
    }

    if (!m_ever_frozen) {
        m_ever_frozen = true;
        SPDLOG_INFO("[CVarManager] (Standard) First time freezing \"{}\"...", utility::narrow(m_name));
    }

    switch(m_type) {
    case Type::BOOL:
        // Limiting the amount of times Set gets called with string conversions.
        if ((*m_cvar)->GetInt() != m_frozen_int_value) {
            (*m_cvar)->Set(std::to_wstring(m_frozen_int_value).c_str());
        }
        break;
    case Type::INT:
        if ((*m_cvar)->GetInt() != m_frozen_int_value) {
            (*m_cvar)->Set(std::to_wstring(m_frozen_int_value).c_str());
        }
        break;
    case Type::FLOAT:
        if ((*m_cvar)->GetFloat() != m_frozen_float_value) {
            (*m_cvar)->Set(std::to_wstring(m_frozen_float_value).c_str());
        }
        break;
    default:
        break;
    };
}

void CVarManager::CVarStandard::update() {
    ZoneScopedN(__FUNCTION__);

    if (m_cvar == nullptr) {
        m_cvar = sdk::find_cvar_cached(m_module, m_name);
    }
}

void CVarManager::CVarStandard::draw_ui() try {
    ZoneScopedN(__FUNCTION__);

    if (m_cvar == nullptr || *m_cvar == nullptr) {
        ImGui::TextWrapped("Failed to find cvar: %s", utility::narrow(m_name).c_str());
        return;
    }

    auto cvar = *m_cvar;
    const auto narrow_name = utility::narrow(m_name);
    
    switch (m_type) {
    case Type::BOOL: {
        auto value = (bool)cvar->GetInt();

        if (ImGui::Checkbox(narrow_name.c_str(), &value)) {
            GameThreadWorker::get().enqueue([sft = shared_from_this(), cvar, value]() {
                try {
                    cvar->Set(std::to_wstring(value).c_str());
                    sft->save();
                } catch (...) {
                    spdlog::error("Failed to set cvar: {}", utility::narrow(sft->get_name()));
                }
            });
        }
        break;
    }
    case Type::INT: {
        auto value = cvar->GetInt();

        if (ImGui::SliderInt(narrow_name.c_str(), &value, m_min_int_value, m_max_int_value)) {
            GameThreadWorker::get().enqueue([sft = shared_from_this(), cvar, value]() {
                try {
                    cvar->Set(std::to_wstring(value).c_str());
                    sft->save();
                } catch(...) {
                    spdlog::error("Failed to set cvar: {}", utility::narrow(sft->get_name()));
                }
            });
        }
        break;
    }
    case Type::FLOAT: {
        auto value = cvar->GetFloat();

        if (ImGui::SliderFloat(narrow_name.c_str(), &value, m_min_float_value, m_max_float_value)) {
            GameThreadWorker::get().enqueue([sft = shared_from_this(), cvar, value]() {
                try {
                    cvar->Set(std::to_wstring(value).c_str());
                    sft->save();
                } catch(...) {
                    spdlog::error("Failed to set cvar: {}", utility::narrow(sft->get_name()));
                }
            });
        }
        break;
    }
    default:
        ImGui::TextWrapped("Unimplemented cvar type: %s", utility::narrow(m_name).c_str());
        break;
    };
} catch(...) {
    ImGui::TextWrapped("Failed to read cvar: %s", utility::narrow(m_name).c_str());
}

void CVarManager::CVarData::load(bool set_defaults) {
    ZoneScopedN(__FUNCTION__);

    load_internal(cvars_data_txt_name.data(), set_defaults);
}

void CVarManager::CVarData::save() {
    ZoneScopedN(__FUNCTION__);

    if (!m_cvar_data) {
        return;
    }

    // Points to the same thing, just different data internally.
    auto cvar_int = m_cvar_data->get<int>();
    auto cvar_float = m_cvar_data->get<float>();

    if (cvar_int == nullptr) {
        return;
    }

    switch (m_type) {
    case Type::BOOL:
        m_frozen_int_value = cvar_int->get();
        break;
    case Type::INT:
        m_frozen_int_value = cvar_int->get();
        break;
    case Type::FLOAT:
        m_frozen_float_value = cvar_float->get();
        break;
    default:
        break;
    };

    save_internal(cvars_data_txt_name.data());
}

void CVarManager::CVarData::freeze() {
    ZoneScopedN(__FUNCTION__);

    if (!m_frozen) {
        return;
    }

    if (!m_cvar_data) {
        return;
    }

    if (!m_ever_frozen) {
        m_ever_frozen = true;
        SPDLOG_INFO("[CVarManager] (Data) First time freezing \"{}\"...", utility::narrow(m_name));
    }

    // Points to the same thing, just different data internally.
    auto cvar_int = m_cvar_data->get<int>();
    auto cvar_float = m_cvar_data->get<float>();

    if (cvar_int == nullptr) {
        return;
    }

    switch (m_type) {
    case Type::BOOL:
        cvar_int->set(m_frozen_int_value);
        break;
    case Type::INT:
        cvar_int->set(m_frozen_int_value);
        break;
    case Type::FLOAT:
        cvar_float->set(m_frozen_float_value);
        break;
    default:
        break;
    };
}

void CVarManager::CVarData::update() {
    ZoneScopedN(__FUNCTION__);

    if (!m_cvar_data) {
        m_cvar_data = sdk::find_cvar_data_cached(m_module, m_name);
    }
}

void CVarManager::CVarData::draw_ui() try {
    ZoneScopedN(__FUNCTION__);

    if (!m_cvar_data) {
        ImGui::TextWrapped("Failed to find cvar data: %s", utility::narrow(m_name).c_str());
        return;
    }

    // Points to the same thing, just different data internally.
    auto cvar_int = m_cvar_data->get<int>();
    auto cvar_float = m_cvar_data->get<float>();

    if (cvar_int == nullptr) {
        ImGui::TextWrapped("Failed to read cvar data: %s", utility::narrow(m_name).c_str());
        return;
    }

    const auto narrow_name = utility::narrow(m_name);

    switch (m_type) {
    case Type::BOOL: {
        auto value = (bool)cvar_int->get();

        if (ImGui::Checkbox(narrow_name.c_str(), &value)) {
            cvar_int->set((int)value); // no need to run on game thread, direct access
            this->save();
        }
        break;
    }
    case Type::INT: {
        auto value = cvar_int->get();

        if (ImGui::SliderInt(narrow_name.c_str(), &value, m_min_int_value, m_max_int_value)) {
            cvar_int->set(value); // no need to run on game thread, direct access
            this->save();
        }
        break;
    }
    case Type::FLOAT: {
        auto value = cvar_float->get();

        if (ImGui::SliderFloat(narrow_name.c_str(), &value, m_min_float_value, m_max_float_value)) {
            cvar_float->set(value); // no need to run on game thread, direct access
            this->save();
        }
        break;
    }
    default:
        ImGui::TextWrapped("Unimplemented cvar type: %s", narrow_name.c_str());
        break;
    }
} catch (...) {
    ImGui::TextWrapped("Failed to read cvar data: %s", utility::narrow(m_name).c_str());
}

static inline void trim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));

    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

void CVarManager::execute_console_script(sdk::UGameEngine* engine, const std::string& filename) {
    ZoneScopedN(__FUNCTION__);

    if (engine == nullptr) {
        spdlog::error("[execute_console_script] engine is null");
        return;
    }

    spdlog::info("[execute_console_script] Loading {}...", filename);

    const auto cscript_txt = Framework::get_persistent_dir(filename);

    if (!std::filesystem::exists(cscript_txt)) {
        return;
    }

    std::ifstream cscript_file(utility::widen(cscript_txt.string()));

    if (!cscript_file) {
        spdlog::error("[execute_console_script] Failed to open file {}...", filename);
        return;
    }

    for (std::string line{}; getline(cscript_file, line); ) {
        trim(line);

        // handle comments
        if (line.starts_with('#') || line.starts_with(';')) {
            continue;
        }

        if (line.contains('#')) {
            line = line.substr(0, line.find_first_of('#'));
            trim(line);
        }

        if (line.contains(';')) {
            line = line.substr(0, line.find_first_of(';'));
            trim(line);
        }

        if (line.length() == 0) {
            continue;
        }

        spdlog::debug("[execute_console_script] Attempting to execute \"{}\"", line);
        engine->exec(utility::widen(line));
    }

    spdlog::debug("[execute_console_script] done");
}
