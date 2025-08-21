#pragma once

namespace uevrLCX {

class UEVRLCX_Core {
public:
    UEVRLCX_Core();
    ~UEVRLCX_Core();

    bool initialize();
    void shutdown();
    void update();
};

} // namespace uevrLCX
