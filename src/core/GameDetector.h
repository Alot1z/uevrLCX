#pragma once

#include <string>
#include <vector>
#include <memory>

namespace uevrLCX {
namespace core {

class GameDetector {
public:
    GameDetector();
    ~GameDetector();

    // Core game detection functionality
    bool detectGame(const std::string& gamePath);
    std::string detectEngine(const std::string& gamePath);
    std::vector<std::string> scanForGames();
    bool isGameRunning(const std::string& gamePath);

private:
    class GameDetectorImpl;
    std::unique_ptr<GameDetectorImpl> impl;
};

} // namespace core
} // namespace uevrLCX
