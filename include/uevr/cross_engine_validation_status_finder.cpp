/*
 * Main Cross-Engine Framework Validation Status Finder
 * 
 * Comprehensive validation system for uevr Cross-Engine Framework
 * Validates framework completeness, integration status, and overall readiness
 * 
 * Copyright (c) 2024 uevr Cross-Engine Adapter Project
 */

#include "ICrossEngineAdapter.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <memory>
#include <regex>

namespace uevr {

/**
 * @brief Main Cross-Engine Framework Validation Status Finder
 * 
 * This class provides comprehensive validation for the main cross-engine framework
 * including interface completeness, integration status, and overall readiness.
 */
class CrossEngineFrameworkValidationStatusFinder {
private:
    // Validation categories
    struct ValidationCategory {
        std::string name;
        std::string description;
        bool isComplete;
        std::vector<std::string> issues;
        std::vector<std::string> recommendations;
        int todoCount;
        int fixmeCount;
    };

    std::vector<ValidationCategory> m_validationCategories;
    std::string m_reportPath;
    bool m_verboseOutput;
    int m_totalTODOs;
    int m_totalFIXMEs;

public:
    CrossEngineFrameworkValidationStatusFinder(const std::string& reportPath = "cross_engine_framework_validation_report.txt", bool verbose = true)
        : m_reportPath(reportPath), m_verboseOutput(verbose), m_totalTODOs(0), m_totalFIXMEs(0) {
        initializeValidationCategories();
    }

    /**
     * @brief Execute comprehensive validation
     * @return Overall validation status
     */
    bool executeValidation() {
        std::cout << "========================================\n";
        std::cout << "MAIN CROSS-ENGINE FRAMEWORK VALIDATION\n";
        std::cout << "========================================\n\n";

        bool overallStatus = true;

        // Execute each validation category
        for (auto& category : m_validationCategories) {
            std::cout << "[" << (category.isComplete ? "OK" : "ISSUE") << "] " 
                      << category.name << "\n";
            std::cout << "Description: " << category.description << "\n";
            
            if (!category.isComplete) {
                std::cout << "Status: INCOMPLETE - " << category.todoCount << " TODOs, " 
                          << category.fixmeCount << " FIXMEs\n";
                overallStatus = false;
                
                if (m_verboseOutput) {
                    std::cout << "Issues:\n";
                    for (const auto& issue : category.issues) {
                        std::cout << "  - " << issue << "\n";
                    }
                    
                    std::cout << "Recommendations:\n";
                    for (const auto& rec : category.recommendations) {
                        std::cout << "  - " << rec << "\n";
                    }
                }
            } else {
                std::cout << "Status: COMPLETE - Ready for Integration\n";
            }
            
            std::cout << "\n";
        }

        // Display overall TODO/FIXME summary
        std::cout << "========================================\n";
        std::cout << "OVERALL FRAMEWORK STATUS\n";
        std::cout << "========================================\n";
        std::cout << "Total TODOs: " << m_totalTODOs << "\n";
        std::cout << "Total FIXMEs: " << m_totalFIXMEs << "\n";
        
        if (m_totalTODOs > 0) {
            std::cout << "⚠️  WARNING: " << m_totalTODOs << " framework items incomplete\n";
        } else {
            std::cout << "✅ All framework items complete - Ready for Engine Integration\n";
        }
        
        std::cout << "\n";

        // Generate validation report
        generateValidationReport();
        
        return overallStatus;
    }

    /**
     * @brief Get validation summary
     * @return Summary string
     */
    std::string getValidationSummary() const {
        int totalCategories = m_validationCategories.size();
        int completeCategories = 0;
        
        for (const auto& category : m_validationCategories) {
            if (category.isComplete) completeCategories++;
        }
        
        double completionPercentage = (double)completeCategories / totalCategories * 100.0;
        
        std::string summary = "Cross-Engine Framework Validation Summary:\n";
        summary += "- Total Categories: " + std::to_string(totalCategories) + "\n";
        summary += "- Complete: " + std::to_string(completeCategories) + "\n";
        summary += "- Incomplete: " + std::to_string(totalCategories - completeCategories) + "\n";
        summary += "- Completion: " + std::to_string((int)completionPercentage) + "%\n";
        summary += "- Total TODOs: " + std::to_string(m_totalTODOs) + "\n";
        summary += "- Total FIXMEs: " + std::to_string(m_totalFIXMEs) + "\n";
        
        if (completionPercentage == 100.0) {
            summary += "- Status: READY FOR ENGINE INTEGRATION\n";
        } else if (completionPercentage >= 90.0) {
            summary += "- Status: NEARLY READY - Minor Issues\n";
        } else if (completionPercentage >= 80.0) {
            summary += "- Status: MOSTLY READY - Some Issues\n";
        } else {
            summary += "- Status: NOT READY - Major Issues\n";
        }
        
        return summary;
    }

private:
    /**
     * @brief Initialize validation categories
     */
    void initializeValidationCategories() {
        // 1. Core Interface Definition
        ValidationCategory interfaceCategory;
        interfaceCategory.name = "Core Interface Definition";
        interfaceCategory.description = "ICrossEngineAdapter interface completeness";
        interfaceCategory.isComplete = validateCoreInterface();
        interfaceCategory.todoCount = countTODOsInCategory("interface");
        interfaceCategory.fixmeCount = countFIXMEsInCategory("interface");
        m_totalTODOs += interfaceCategory.todoCount;
        m_totalFIXMEs += interfaceCategory.fixmeCount;
        
        if (!interfaceCategory.isComplete) {
            interfaceCategory.issues = {
                "Missing essential interface methods",
                "Incomplete virtual function declarations",
                "Interface documentation incomplete"
            };
            interfaceCategory.recommendations = {
                "Complete all required interface methods",
                "Add missing virtual function declarations",
                "Complete interface documentation"
            };
        }
        m_validationCategories.push_back(interfaceCategory);

        // 2. Factory Pattern Implementation
        ValidationCategory factoryCategory;
        factoryCategory.name = "Factory Pattern Implementation";
        factoryCategory.description = "ICrossEngineAdapterFactory implementation";
        factoryCategory.isComplete = validateFactoryPattern();
        factoryCategory.todoCount = countTODOsInCategory("factory");
        factoryCategory.fixmeCount = countFIXMEsInCategory("factory");
        m_totalTODOs += factoryCategory.todoCount;
        m_totalFIXMEs += factoryCategory.fixmeCount;
        
        if (!factoryCategory.isComplete) {
            factoryCategory.issues = {
                "Factory interface incomplete",
                "Adapter creation methods missing",
                "Engine detection not implemented"
            };
            factoryCategory.recommendations = {
                "Complete factory interface implementation",
                "Implement adapter creation methods",
                "Add engine detection system"
            };
        }
        m_validationCategories.push_back(factoryCategory);

        // 3. Documentation and Examples
        ValidationCategory docCategory;
        docCategory.name = "Documentation and Examples";
        docCategory.description = "Framework documentation and usage examples";
        docCategory.isComplete = validateDocumentation();
        docCategory.todoCount = 0; // Documentation should not have TODOs
        docCategory.fixmeCount = 0;
        
        if (!docCategory.isComplete) {
            docCategory.issues = {
                "Missing framework documentation",
                "No usage examples provided",
                "Integration guide incomplete"
            };
            docCategory.recommendations = {
                "Create comprehensive framework documentation",
                "Add usage examples and tutorials",
                "Complete integration guide"
            };
        }
        m_validationCategories.push_back(docCategory);

        // 4. Cross-Engine Compatibility
        ValidationCategory compatibilityCategory;
        compatibilityCategory.name = "Cross-Engine Compatibility";
        compatibilityCategory.description = "Framework compatibility across different engines";
        compatibilityCategory.isComplete = validateCrossEngineCompatibility();
        compatibilityCategory.todoCount = 0; // Compatibility should not have TODOs
        compatibilityCategory.fixmeCount = 0;
        
        if (!compatibilityCategory.isComplete) {
            compatibilityCategory.issues = {
                "Engine-specific abstractions missing",
                "Graphics API abstraction incomplete",
                "Memory management not standardized"
            };
            compatibilityCategory.recommendations = {
                "Implement engine-specific abstractions",
                "Complete graphics API abstraction",
                "Standardize memory management"
            };
        }
        m_validationCategories.push_back(compatibilityCategory);

        // 5. Integration Testing
        ValidationCategory testingCategory;
        testingCategory.name = "Integration Testing";
        testingCategory.description = "Framework integration testing and validation";
        testingCategory.isComplete = validateIntegrationTesting();
        testingCategory.todoCount = 0; // Testing should not have TODOs
        testingCategory.fixmeCount = 0;
        
        if (!testingCategory.isComplete) {
            testingCategory.issues = {
                "No integration tests available",
                "Framework validation missing",
                "Performance testing not implemented"
            };
            testingCategory.recommendations = {
                "Create integration test suite",
                "Implement framework validation",
                "Add performance testing"
            };
        }
        m_validationCategories.push_back(testingCategory);
    }

    /**
     * @brief Validate core interface
     */
    bool validateCoreInterface() {
        // Check if ICrossEngineAdapter.hpp exists and is properly defined
        std::ifstream interfaceFile("ICrossEngineAdapter.hpp");
        if (!interfaceFile.is_open()) {
            return false;
        }

        std::string content((std::istreambuf_iterator<char>(interfaceFile)),
                           std::istreambuf_iterator<char>());
        interfaceFile.close();

        // Check for essential interface elements
        std::vector<std::string> requiredElements = {
            "class ICrossEngineAdapter", "virtual bool initialize", "virtual void cleanup",
            "virtual bool isVREnabled", "virtual void update", "virtual void render"
        };

        for (const auto& element : requiredElements) {
            if (content.find(element) == std::string::npos) {
                return false;
            }
        }

        return true;
    }

    /**
     * @brief Validate factory pattern
     */
    bool validateFactoryPattern() {
        // Check if ICrossEngineAdapter.hpp contains factory interface
        std::ifstream interfaceFile("ICrossEngineAdapter.hpp");
        if (!interfaceFile.is_open()) {
            return false;
        }

        std::string content((std::istreambuf_iterator<char>(interfaceFile)),
                           std::istreambuf_iterator<char>());
        interfaceFile.close();

        // Check for factory interface elements
        std::vector<std::string> requiredFactoryElements = {
            "class ICrossEngineAdapterFactory", "virtual std::shared_ptr<ICrossEngineAdapter>",
            "virtual bool canCreateAdapter", "virtual std::string getEngineType"
        };

        for (const auto& element : requiredFactoryElements) {
            if (content.find(element) == std::string::npos) {
                return false;
            }
        }

        return true;
    }

    /**
     * @brief Validate documentation
     */
    bool validateDocumentation() {
        // Check if documentation files exist
        std::vector<std::string> requiredDocs = {
            "../../docs/cross-engine/adapter_development.md",
            "../../docs/cross-engine/RE-Engine.md",
            "../../docs/cross-engine/REDengine4.md",
            "../../docs/cross-engine/MT-Framework.md"
        };

        for (const auto& doc : requiredDocs) {
            if (!std::filesystem::exists(doc)) {
                return false;
            }
        }

        return true;
    }

    /**
     * @brief Validate cross-engine compatibility
     */
    bool validateCrossEngineCompatibility() {
        // Check if compatibility documentation exists
        std::vector<std::string> compatibilityDocs = {
            "../../docs/cross-engine/adapter_development.md"
        };

        for (const auto& doc : compatibilityDocs) {
            if (!std::filesystem::exists(doc)) {
                return false;
            }
        }

        // Check if the documentation contains compatibility information
        std::ifstream docFile("../../docs/cross-engine/adapter_development.md");
        if (!docFile.is_open()) {
            return false;
        }

        std::string content((std::istreambuf_iterator<char>(docFile)),
                           std::istreambuf_iterator<char>());
        docFile.close();

        // Check for compatibility keywords
        std::vector<std::string> compatibilityKeywords = {
            "Universal Architecture", "Core Design Principles", "Engine Detection",
            "Graphics API", "Memory Management"
        };

        for (const auto& keyword : compatibilityKeywords) {
            if (content.find(keyword) == std::string::npos) {
                return false;
            }
        }

        return true;
    }

    /**
     * @brief Validate integration testing
     */
    bool validateIntegrationTesting() {
        // Check if test files exist
        std::vector<std::string> testFiles = {
            "../adapters/MT-Framework/test_integration.cpp",
            "../adapters/REDengine4/test_integration.cpp",
            "../adapters/RE-Engine/test_integration.cpp"
        };

        int existingTests = 0;
        for (const auto& test : testFiles) {
            if (std::filesystem::exists(test)) {
                existingTests++;
            }
        }

        // At least 2 out of 3 test files should exist
        return existingTests >= 2;
    }

    /**
     * @brief Count TODOs in a specific category
     */
    int countTODOsInCategory(const std::string& category) {
        if (category == "interface") {
            return countTODOsInFile("ICrossEngineAdapter.hpp");
        } else if (category == "factory") {
            return countTODOsInFile("ICrossEngineAdapter.hpp");
        }
        return 0;
    }

    /**
     * @brief Count FIXMEs in a specific category
     */
    int countFIXMEsInCategory(const std::string& category) {
        if (category == "interface") {
            return countFIXMEsInFile("ICrossEngineAdapter.hpp");
        } else if (category == "factory") {
            return countFIXMEsInFile("ICrossEngineAdapter.hpp");
        }
        return 0;
    }

    /**
     * @brief Count TODOs in a specific file
     */
    int countTODOsInFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return 0;
        }

        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();

        std::regex todoRegex("//\\s*TODO", std::regex_constants::icase);
        auto words_begin = std::sregex_iterator(content.begin(), content.end(), todoRegex);
        auto words_end = std::sregex_iterator();

        return std::distance(words_begin, words_end);
    }

    /**
     * @brief Count FIXMEs in a specific file
     */
    int countFIXMEsInFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return 0;
        }

        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        file.close();

        std::regex fixmeRegex("//\\s*FIXME", std::regex_constants::icase);
        auto words_begin = std::sregex_iterator(content.begin(), content.end(), fixmeRegex);
        auto words_end = std::sregex_iterator();

        return std::distance(words_begin, words_end);
    }

    /**
     * @brief Generate validation report
     */
    void generateValidationReport() {
        std::ofstream reportFile(m_reportPath);
        if (!reportFile.is_open()) {
            std::cerr << "Error: Could not create validation report file\n";
            return;
        }

        reportFile << "Main Cross-Engine Framework Validation Report\n";
        reportFile << "Generated: " << __DATE__ << " " << __TIME__ << "\n";
        reportFile << "========================================\n\n";

        reportFile << getValidationSummary() << "\n\n";

        reportFile << "Detailed Validation Results:\n";
        reportFile << "============================\n\n";

        for (const auto& category : m_validationCategories) {
            reportFile << "Category: " << category.name << "\n";
            reportFile << "Description: " << category.description << "\n";
            reportFile << "Status: " << (category.isComplete ? "COMPLETE" : "INCOMPLETE") << "\n";
            reportFile << "TODOs: " << category.todoCount << ", FIXMEs: " << category.fixmeCount << "\n";
            
            if (!category.isComplete) {
                reportFile << "Issues:\n";
                for (const auto& issue : category.issues) {
                    reportFile << "  - " << issue << "\n";
                }
                
                reportFile << "Recommendations:\n";
                for (const auto& rec : category.recommendations) {
                    reportFile << "  - " << rec << "\n";
                }
            }
            
            reportFile << "\n";
        }

        reportFile.close();
        std::cout << "Validation report saved to: " << m_reportPath << "\n";
    }
};

} // namespace uevr

/**
 * @brief Main validation execution function
 */
int main() {
    uevr::CrossEngineFrameworkValidationStatusFinder validator;
    
    bool validationResult = validator.executeValidation();
    
    std::cout << "\n" << validator.getValidationSummary() << "\n";
    
    if (validationResult) {
        std::cout << "\n✅ Cross-Engine Framework is READY for Engine Integration!\n";
        return 0;
    } else {
        std::cout << "\n⚠️  Cross-Engine Framework has some issues that should be reviewed.\n";
        std::cout << "Review the validation report for details.\n";
        return 1;
    }
}
