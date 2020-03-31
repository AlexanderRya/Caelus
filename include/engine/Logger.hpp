#ifndef CAELUS_LOGGER_HPP
#define CAELUS_LOGGER_HPP

#include <engine/Util.hpp>

namespace caelus::logger {
    template <typename ...Args>
    void info(Args&& ...args) {
        util::print(util::format(
            "[{}] [Logger] [Info]: ",
            util::get_current_timestamp()));

        (((util::print(args)), ...), util::print("\n"));
    }

    template <typename ...Args>
    void warning(Args&& ...args) {
        util::print(util::format(
            "[{}] [Logger] [Warning]: ",
            util::get_current_timestamp()));

        (((util::print(args)), ...), util::print("\n"));
    }

    template <typename ...Args>
    void error(Args&& ...args) {
        util::print(util::format(
            "[{}] [Logger] [Error]: ",
            util::get_current_timestamp()));

        (((util::print(args)), ...), util::print("\n"));
    }
} // namespace caelus::logger

#endif //CAELUS_LOGGER_HPP
