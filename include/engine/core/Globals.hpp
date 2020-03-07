#ifndef CAELUS_GLOBALS_HPP
#define CAELUS_GLOBALS_HPP

#include <engine/core/components/Camera.hpp>

namespace caelus::core {
    inline components::Camera camera{};

    inline f32 delta_time = 0;
    inline f32 last_frame = 0;
}

#endif //CAELUS_GLOBALS_HPP
