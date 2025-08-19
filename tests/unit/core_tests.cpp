#include <gtest/gtest.h>
#include "uevr/Framework.hpp"

namespace uevr {
namespace Tests {

class FrameworkTest : public ::testing::Test {
protected:
    void SetUp() override {
        framework = std::make_unique<Framework>();
    }
    
    void TearDown() override {
        if (framework) {
            framework->cleanup();
        }
    }
    
    std::unique_ptr<Framework> framework;
};

TEST_F(FrameworkTest, InitializationTest) {
    EXPECT_TRUE(framework->initialize());
    EXPECT_TRUE(framework->isInitialized());
}

TEST_F(FrameworkTest, CleanupTest) {
    framework->initialize();
    framework->cleanup();
    EXPECT_FALSE(framework->isInitialized());
}

} // namespace Tests
} // namespace uevr
