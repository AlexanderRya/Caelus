#ifndef CAELUS_GLOBALS_HPP
#define CAELUS_GLOBALS_HPP

#include <engine/Forwards.hpp>

namespace caelus::core {
    extern components::Camera camera;

    inline f32 delta_time = 0;
    inline f32 last_frame = 0;
}

#endif //CAELUS_GLOBALS_HPP
